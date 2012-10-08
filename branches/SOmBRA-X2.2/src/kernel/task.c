#include <task.h>
#include <mm.h>
#include <aspace.h>
#include <thread.h>
#include <kernel.h>
#include <x86-common/elf.h>
#include <x86-common/memory.h>
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <x86-common/page.h>

extern char *g_code;
extern void isr_to_user();

#define STACK_ALLOC(stack, sz) (void*)(stack  -= sz)

static void *lpush(uintptr_t *stack, uint_arch_t value)
{
    *stack -= sizeof(uint_arch_t);

    *(uint_arch_t *)(*stack) = value;

    return (void*)*stack;
}

static unsigned vector_count(char **vector)
{
    if (vector == NULL) return 0;

    unsigned i, count = 0;

    for (i = 0; vector[i] != NULL; i++)
        count++;

    return count;
}

#if 0
static void vector_print(char **vector)
{
	if (vector == NULL) return;

	unsigned i = 0;

	for (; vector[i] != NULL; i++)
		printf("vector[%i]: %X => %s\n", i, vector[i], vector[i]);
}
#endif

static void *vector_copy(uintptr_t *stack, char **vector, uint_arch_t kvirt_to_uvirt, uint_arch_t uvirt_to_kvirt, char *first_item)
{
    if (vector == NULL && first_item == NULL) return NULL;

    unsigned i = 0, count = vector_count(vector);
    char c = 0;

    if (first_item != NULL) i = 1;

    char **vect = STACK_ALLOC(*stack, sizeof(char *) * (count + 1 + i));

    if (i)
    {
        vect[0] = STACK_ALLOC(*stack, strlen(first_item)+1);
        strcpy(vect[0], first_item);
        vect[0] -= kvirt_to_uvirt;
        count++;
	c = 1;
    }

    if (vector != NULL)
    for (; i < count; i++)
    {
        char *fp = vector[i-c] + uvirt_to_kvirt;

        vect[i] = STACK_ALLOC(*stack, strlen(fp)+1);
        strcpy(vect[i], fp);
        vect[i] -= kvirt_to_uvirt;
    }
    vect[i] = NULL;

    return (void*)vect;
}

//#define page_map(addr, toaddr, tbprot, pgprot) printf("PAGE_MAP(%X, %X, %X, %X)\n", addr, toaddr, tbprot, pgprot); page_map(addr, toaddr, tbprot, pgprot)

/* Cria uma task. */
int task_create(char *name, char **argp, char **envp, char *image, uint32_t ppid)
{
	uintptr_t entry, lowest, highest, i, uvirt_to_kvirt, uvirt_to_kvirt2;
	uregs_t *uregs;
	kregs_t *kregs;
	aspace_t *as;
	sect_t *sect;
	task_t *t;
	int err;
	void *arg, *env;
	int argc;
	uintptr_t address;
	uint16_t proto;
	uintptr_t pd;
	char *stack_mem;
	uintptr_t stack_ptr; 

// Pilha temporária para armazenar os argumentos
	stack_mem = zmalloc(USERMEM_STACK_SIZE);
	if (stack_mem == NULL) return -ERROR_NO_MEM;
	stack_ptr = (uintptr_t)stack_mem + USERMEM_STACK_SIZE;

	/* Cria um aspace para alocar os dados sobre as seções do arquivo executável. */
	as = aspace_create();
	if(as == NULL){
		printf("Nao foi possivel alocar memoria para aspace.\n");
		return -ERROR_NO_MEM;
	}

	/* Lê e valida um arquivo exeutável do formato ELF. */
	err = load_elf_exec(image, &entry, as);
	if (err)
	{
	    printf("Não foi possível carregar executavel!\n");
		aspace_destroy(as);
		return +1;
	}

	/* Validação e coleta de informações sobre as seções. */
	lowest = -1u;
	highest = 0;
	for(i = 0; i < as->num_sects; i++){
		sect = &as->sect[i];
		/* Verifica se a seção está dentro da area de usuário(USER_BASE). */
		if(sect->adr < USER_BASE){
			printf("A secao %d(addr%x) deve ter endereco acima de %x.\n", i, sect->adr, USER_BASE);
			/* Destroi o ASPACE. */
			aspace_destroy(as);
			return -ERROR_NO_EXEC;
		}
		// Precisa disso mesmo?
		//if(sect->adr + sect->size >= (unsigned)g_code){
		//	printf("A secao %d(addr%x) deve ter endereco abaixo de %x.\n", i, sect->adr + sect->size, g_code);
		//	/* Destroi o ASPACE. */
		//	aspace_destroy(as);
		//	return -ERROR_NO_EXEC;
		//}
		/* Encontra o maior e o menor endereço virtual. */
		if(sect->adr < lowest){
			lowest = sect->adr;
		}
		if((sect->adr + sect->size) > highest){
			highest = sect->adr + sect->size;
		}
	}
	as->virt_adr = lowest;

	// Ativando o heap do programa
	as->heap.adr = highest;
	as->heap.size = 0;
	as->heap.offset = 0;
	as->heap.flags = SF_BSS;

// xxx - separate sect_t for stack
// xxx - add sect_t for heap, too
/* convert highest virtual address to size */
	/* Seta o tamanho do processo na memória. */
	//as->size = (highest - lowest) + USER_STACK_SIZE;
	as->size = 0xFF800000 - USERMEM_BASE_ADDR - 1;

/* STEP 4: allocate memory for user task, including user stack */
// xxx - not for demand-load paging

	/* Aloca memória para carrregar o processo. */
	as->user_mem = (char *)USERMEM_BASE_ADDR;

	/* Pega o endereço virtual do processo. */
	uvirt_to_kvirt = (uintptr_t)as->user_mem - as->virt_adr;

	uvirt_to_kvirt2 = 0;
	if (task_curr->as != NULL)
		uvirt_to_kvirt2 = (uintptr_t)task_curr->as->user_mem - task_curr->as->virt_adr;

// Copiando os argumentos

	env = vector_copy(&stack_ptr, envp, 0, uvirt_to_kvirt2, NULL);
    	arg = vector_copy(&stack_ptr, argp, 0, uvirt_to_kvirt2, NULL);
	argc = vector_count(argp)+1;

	pd = (uintptr_t)page_new();
	page_dir_exchange(&pd);

	// Adicionando a pilha do programa
	aspace_section_add(as, (USERMEM_END_ADDR - USERMEM_STACK_SIZE) - uvirt_to_kvirt, USERMEM_STACK_SIZE - 1, SF_BSS, 0);

	/* Carrega as seções, no caso do BSS(SF_ZERO) zera o conteúdo. */
	//printf("num_sects: %u\n", as->num_sects);
	for(i = 0; i < as->num_sects; i++)
	{
		sect = &as->sect[i];

		//printf("sect_%i ", i);

		if (sect->flags & (SF_LOAD | SF_ZERO))
		{
			proto = USERMEM_DATAAREA_PROT;

			if (sect->flags & (SF_EXEC))
				proto = USERMEM_CODEAREA_PROT;

			address = sect->adr + uvirt_to_kvirt;

			page_map((void*)address,(void*)(address+sect->size), PAGE_PRESENT_WRITE_USER, proto);

			if(sect->flags & SF_LOAD)
			{
				/* Para Code e Data. */
				memcpy((char *)address, image + sect->offset, sect->size);
			}
			else if(sect->flags & SF_ZERO)
			{
				/* Para o BSS. */
				memset((char *)address, 0, sect->size);
			}
		}
	}

// Fazendo teste para ENVIRON e PARAMETERS
	uintptr_t user_stack, diff = user_stack = 0xBFFFFFFF;

	env = vector_copy(&user_stack, env, uvirt_to_kvirt, 0, NULL);
	arg = vector_copy(&user_stack, arg, uvirt_to_kvirt, 0, name);

	lpush(&user_stack, (uintptr_t)env - uvirt_to_kvirt);
	lpush(&user_stack, (uintptr_t)arg - uvirt_to_kvirt);
	lpush(&user_stack, argc);

	diff -= (uintptr_t)user_stack;

	free(stack_mem);

	page_dir_exchange(&pd);

	/* Cria a Thread(task) para alocar o contexto do processo. */
	t = thread_create(entry, 0, ppid, name);
	if(t == NULL){
		printf("Nao ha slot para colocar a nova tarefa.\n");
		aspace_destroy(as);
		return -ERROR_NO_MEM;
	}

	t->pagedir = pd;

	// Iniciando os arquivos
	//t->files = NULL;

	/* Seta o ASPACE da tarefa. */
	t->as = as;

	/* Seta a pilha(kernel) que será utilizada quando a tarefa sair do ring 3 para o ring 0. */
	t->kstack = t->kstack_mem + KRNL_STACK_SIZE - sizeof(uregs_t) - sizeof(kregs_t);

	t->console = get_con();

	/* Seta o contexto do Ring3. */
	uregs = (uregs_t *)(t->kstack + sizeof(kregs_t));

	#ifdef __x86_64__
	uregs->ss = uregs->fs = uregs->gs = GDT64_UDATA;
	uregs->cs		= GDT64_UCODE;
	uregs->rip		= entry;
	uregs->rflags	= 0x200; /* Habilita as interrupções. */
	uregs->rsp	= user_stack - uvirt_to_kvirt;
	#else
	uregs->ds		= uregs->es = uregs->user_ss = uregs->fs = uregs->gs = GDT_UDATA;
	uregs->cs		= GDT_UCODE;
	uregs->eip		= entry;
	uregs->eflags	= 0x200; /* Habilita as interrupções. */
	uregs->user_esp	= user_stack - uvirt_to_kvirt;
	#endif

	/* Seta regs em modo kernel para que a primeira troca de contexto fique condicionada
	a execução da função switch_to(), entao por meio da função to_user() a tarefa vai para o ring 3. */
	kregs			= (kregs_t *)t->kstack;
	
	#ifdef __x86_64__
	kregs->rip		= (uintptr_t)isr_to_user;
	kregs->rflags	= 0; /* interrupts off until ring 3 */
	#else
	kregs->eip		= (uintptr_t)isr_to_user;
	kregs->eflags	= 0; /* interrupts off until ring 3 */
	#endif

	/* Retorna o pid da tarefa. */
	return t->pid;
}

/* Destroi uma tarefa. */
void task_destroy(task_t *t)
{
	asm ("cli");

	/* Fecha todos os descritores abertos. */
	//while(t->files != NULL)
	//{
	//    t->files->close(t->files);
	//	t->files = fs_destroy_node(t->files);
	//}

	/* Destroi o aspace. */
	if(t->as != NULL){
		sect_t *sect;
		unsigned i;
		uintptr_t		addr, uvirt_to_kvirt;
		uvirt_to_kvirt = (uintptr_t)t->as->user_mem - t->as->virt_adr;
		uintptr_t pd = t->pagedir;

		page_dir_exchange(&pd);

		for(i = 0; i < t->as->num_sects; i++){
			sect = t->as->sect + i;
			if(sect->flags & (SF_LOAD | SF_ZERO)){
				addr = sect->adr + uvirt_to_kvirt;

				page_unmap((void*)addr, (void*)(addr+sect->size));
			}
		}

		if (t->as->heap.size)
		{
			addr = t->as->heap.adr + uvirt_to_kvirt;
			page_unmap((void*)addr, (void*)(addr+t->as->heap.size));
		}

		page_dir_exchange(&pd);

		if (t->pagedir)
			page_free(t->pagedir);

		aspace_destroy(t->as);
	}

	/* Destroi a thread(task). */
	thread_destroy(t);

	asm ("sti");
}
