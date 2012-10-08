#include <task.h>
#include <mm.h>
#include <aspace.h>
#include <thread.h>
#include <kernel.h>
#include ELF_H
#include MEMORY_H
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <x86/page.h>

extern char *g_code;
extern void isr_to_user();

#define STACK_ALLOC(stack, sz) (void*)(stack  -= sz)

static void *lpush(uintptr_t *stack, unsigned value)
{
    *stack -= sizeof(unsigned);

    *(unsigned *)(*stack) = value;

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

static void vector_print(char **vector)
{
	if (vector == NULL) return;

	unsigned i = 0;

	for (; vector[i] != NULL; i++)
		printf("vector[%i]: %X => %s\n", i, vector[i], vector[i]);
}

static void *vector_copy(uintptr_t *stack, char **vector, unsigned kvirt_to_uvirt, unsigned uvirt_to_kvirt, char *first_item)
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
	unsigned entry, lowest, highest, i, uvirt_to_kvirt, uvirt_to_kvirt2;
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
	unsigned pd;
	char *stack_mem;
	uintptr_t stack_ptr; 

// Pilha tempor�ria para armazenar os argumentos
	stack_mem = zmalloc(USERMEM_STACK_SIZE);
	if (stack_mem == NULL) return -ERROR_NO_MEM;
	stack_ptr = (unsigned)stack_mem + USERMEM_STACK_SIZE;

	/* Cria um aspace para alocar os dados sobre as se��es do arquivo execut�vel. */
	as = aspace_create();
	if(as == NULL){
		printf("Nao foi possivel alocar memoria para aspace.\n");
		return -ERROR_NO_MEM;
	}

	/* L� e valida um arquivo exeut�vel do formato ELF. */
	err = load_elf_exec(image, &entry, as);
	if (err)
	{
	    printf("N�o foi poss�vel carregar executavel!\n");
		aspace_destroy(as);
		return +1;
	}

	/* Valida��o e coleta de informa��es sobre as se��es. */
	lowest = -1u;
	highest = 0;
	for(i = 0; i < as->num_sects; i++){
		sect = &as->sect[i];
		/* Verifica se a se��o est� dentro da area de usu�rio(USER_BASE). */
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
		/* Encontra o maior e o menor endere�o virtual. */
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
	/* Seta o tamanho do processo na mem�ria. */
	//as->size = (highest - lowest) + USER_STACK_SIZE;
	as->size = 0xFF800000 - USERMEM_BASE_ADDR - 1;

/* STEP 4: allocate memory for user task, including user stack */
// xxx - not for demand-load paging

	/* Aloca mem�ria para carrregar o processo. */
	as->user_mem = (char *)USERMEM_BASE_ADDR;

	/* Pega o endere�o virtual do processo. */
	uvirt_to_kvirt = (unsigned)as->user_mem - as->virt_adr;

	uvirt_to_kvirt2 = 0;
	if (task_curr->as != NULL)
		uvirt_to_kvirt2 = (unsigned)task_curr->as->user_mem - task_curr->as->virt_adr;

// Copiando os argumentos

	env = vector_copy(&stack_ptr, envp, 0, uvirt_to_kvirt2, NULL);
    	arg = vector_copy(&stack_ptr, argp, 0, uvirt_to_kvirt2, NULL);
	argc = vector_count(argp)+1;

	pd = (unsigned)page_new();
	page_dir_exchange(&pd);

	// Adicionando a pilha do programa
	aspace_section_add(as, (USERMEM_END_ADDR - USERMEM_STACK_SIZE) - uvirt_to_kvirt, USERMEM_STACK_SIZE - 1, SF_BSS, 0);

	/* Carrega as se��es, no caso do BSS(SF_ZERO) zera o conte�do. */
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

	lpush(&user_stack, (unsigned)env - uvirt_to_kvirt);
	lpush(&user_stack, (unsigned)arg - uvirt_to_kvirt);
	lpush(&user_stack, argc);

	diff -= (unsigned)user_stack;

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
	t->files = NULL;

	/* Seta o ASPACE da tarefa. */
	t->as = as;

	/* Seta a pilha(kernel) que ser� utilizada quando a tarefa sair do ring 3 para o ring 0. */
	t->kstack = t->kstack_mem + KRNL_STACK_SIZE - sizeof(uregs_t) - sizeof(kregs_t);

	t->console = get_con();

	/* Seta o contexto do Ring3. */
	uregs = (uregs_t *)(t->kstack + sizeof(kregs_t));

	uregs->ds		= uregs->es = uregs->user_ss = uregs->fs = uregs->gs = GDT_UDATA;
	uregs->cs		= GDT_UCODE;
	uregs->eip		= entry;
	uregs->eflags	= 0x200; /* Habilita as interrup��es. */
	uregs->user_esp	= user_stack - uvirt_to_kvirt;

	/* Seta regs em modo kernel para que a primeira troca de contexto fique condicionada
	a execu��o da fun��o switch_to(), entao por meio da fun��o to_user() a tarefa vai para o ring 3. */
	kregs			= (kregs_t *)t->kstack;
	kregs->eip		= (unsigned)isr_to_user;
	kregs->eflags	= 0; /* interrupts off until ring 3 */

	/* Retorna o pid da tarefa. */
	return t->pid;
}

/* Destroi uma tarefa. */
void task_destroy(task_t *t)
{
	asm ("cli");

	/* Fecha todos os descritores abertos. */
	while(t->files != NULL)
	{
	    t->files->close(t->files);
		t->files = fs_destroy_node(t->files);
	}

	/* Destroi o aspace. */
	if(t->as != NULL){
		sect_t *sect;
		unsigned i, addr, uvirt_to_kvirt;
		uvirt_to_kvirt = (unsigned)t->as->user_mem - t->as->virt_adr;
		unsigned pd = t->pagedir;

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
