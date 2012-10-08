#include <x86/x86.h>
#include SYSCALL_H
#include ISR_H
#include PCI_H
#include V86_H
#include <kernel/mm.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/schedule.h>
#include <kernel/info.h>
#include <kernel/multiboot.h>
#include <error.h>
#include <string.h>
#include <stdio.h>
#include <drivers/rtc.h>
#include <drivers/floppy.h>
#include <fs/fs.h>
#include <setjmp.h>

jmp_buf g_oops;

/* Abre um descritor de arquivo. */
int open(const char *path, unsigned access)
{
    fs_node_t *fs_node = search_path(fs_root, (char *)path);

    if (fs_node == NULL)
        return -1;

    if (fs_node->flags & FS_DIRECTORY)
        return -1;

    fs_node_t *fs_new_node = fs_clone_node(fs_node);

    curr_task->files = fs_link_node(curr_task->files, fs_new_node);

    open_fs(fs_new_node, 0, 0);

    return (int)fs_new_node;
}

/* Fecha um descritor de arquivos. */
int close(uintptr_t handle){
	fs_node_t *fs_node = (fs_node_t *)handle;

	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

    /* Certifica se pode ou n�o chamar a fun��o write. */
	if((fs_node->close == NULL)){
		return -1;
	}

// Fecha o nodo
	close_fs(fs_node);

// Destroi o nodo
    if (fs_node == curr_task->files)
        curr_task->files = fs_node->fs_next_node;

    curr_task->files = fs_destroy_node(fs_node);

	return 0;
}

/* Escreve em um descritor. */
int write(uintptr_t handle, void *buf, unsigned len){
    fs_node_t *fs_node;

    switch(handle)
    {
        case 0:
        case 1:
        case 2:
            fs_node = curr_task->files;
            if (fs_node)
                break;

            return 0;
        default:
            fs_node = (fs_node_t *)handle;
    }

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	/* Certifica se pode ou n�o chamar a fun��o write. */
	if((fs_node->write == NULL)){
		return -1;
	}

	/* Chama a fun��o write do descritor. */
	return fs_node->write(fs_node, buf, len);
}

/* L� de um descritor. */
int read(uintptr_t handle, void *buf, unsigned len){
    fs_node_t *fs_node;

    switch(handle)
    {
        case 0:
        case 1:
        case 2:
            fs_node = curr_task->files;
            if (fs_node)
                break;

            return 0;
        default:
            fs_node = (fs_node_t *)handle;
    }

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

    //printf("#FILENAME: %s\n", fs_node->name);

	/* Certifica se pode ou n�o chamar a fun��o read. */
	if(fs_node->read == NULL){
		return -1;
	}

	/* Chama a fun��o read do descritor. */
	return fs_node->read(fs_node, buf, len);
}


/* Altera o modo de acesso do descritor. */
int select(uintptr_t handle, unsigned access, unsigned *timeout){
	static wait_queue_t delay_wq;

	fs_node_t *fs_node = (fs_node_t *)handle;

	if( access == 0 ){
		sleep_on(&delay_wq, timeout);
		return 0;
	}

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	/* Certifica se pode ou n�o chamar a fun��o read. */
	if(fs_node->select == NULL){
		return -1;
	}

	/* Chama a fun��o select do descritor. */
	return fs_node->select(fs_node, access, timeout);
}

/* Limpa o console indicando por handle. */
int conclear(uintptr_t handle){
	fs_node_t *fs_node = (fs_node_t *)handle;

	int con;

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	con = (int )fs_node->inode;
	set_con(con);
	/* Limpa o console. */
	clscr();

	return 0;
}

/* Seta a color do console indicando por handle. */
int concolor(uintptr_t handle, uint32_t text, uint32_t background){
	fs_node_t *fs_node = (fs_node_t *)handle;

	int con;

    /* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	con = fs_node->inode;
	set_con(con);
	/* Seta a cor do console. */
	set_color(text, background);

	return 0;
}

/* Seta a posi��o do cursor. */
uint32_t congotoxy(uintptr_t handle, uint32_t pos){
	fs_node_t *fs_node = (fs_node_t *)handle;
	int con;
	bool res;

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	con = fs_node->inode;
	set_con(con);
	/* Seta a posi��o do cursor do consoles. */
	res = gotoxy(pos%80, pos/80);

	return res;
}

/* Pega a posi��o atual do cursor. */
uint32_t congetxy(uintptr_t handle){
	fs_node_t *fs_node = (fs_node_t *)handle;
	int con;
	uint32_t     res;

	/* Verifica se o handle � v�lido. */
	if (fs_node->magic != FS_MAGIC)
        return -ERROR_BAD_ARG;

	con = fs_node->inode;
	set_con(con);
	/* Seta a cor do console. */

	/* Pega a posi��o atual do cursor do console 'con'. */
	res = getxy();

	return res;
}

/* Espera at� o fim da execu��o de um pid(esse pid deve ser de um filho). */
int  wait4pid(uint32_t pid){
	int i;

	/* Procura o filho indicado por pid. */
	for (i = 0;i < NUM_TASKS; i++){
		if (tasks[i].pid == pid){
			// Se o pid do wait4pid for mesmo o pid de um filho
			// ent�o coloca o processo para dormir.
			if (tasks[i].ppid == curr_task->pid){
				curr_task->status		= TS_BLOCKED;
				curr_task->wait4pid	= true;
				/* Escalona o processo para deixar o processo atual dormindo. */
				schedule();
			}
			else{
				return -1;
			}
		}
	}

	return -1;
}

/* Executa uma aplica��o identificada pelo "id". */
int execid(uint32_t id, char **argp, char **envp, unsigned uvirt_to_kvirt){
	module_t *mod;
	char        *image;
	int 		 r;
	char         *name;

	/* Verifica se h� m�dulos carregados. */
	if(!(CHECK_FLAG(multiboot_info, MBF_MODS))){
		return -1;
	}
	/* Verifica se o id do m�dulo � valido. */
	if (id >= multiboot_info->mods_count){
		return -1;
	}
	/* Pega o endere�o do m�dulo. */
	mod	  = (module_t *)(multiboot_info->mods_addr) + id;

	/* Endere�o do inicio do m�dulo. */
	image = (char *)(mod->mod_start);
	name  = (char *)(mod->string);

	/* Cria a tarefa. */
	if((r=task_create(name, image, curr_task->pid)) > 0){
		return r;
	}

	return -1;
}

/* Executa uma aplica��o identificada pelo "id". */
int execlist(char* cmd, uint32_t id){
	module_t *mod;
	char        *name;
	uint32_t i;

	/* Verifica se h� m�dulos carregados. */
	if(!(CHECK_FLAG(multiboot_info, MBF_MODS))){
		return -1;
	}

	/* Verifica se o id do m�dulo � valido. */
	if (id >= multiboot_info->mods_count){
		return -1;
	}

	/* Pega o endere�o do m�dulo. */
	mod	  = (module_t *)(multiboot_info->mods_addr) + id;

	/* Endere�o do inicio do m�dulo. */
	name  = (char *)(mod->string);
	name++;

	for(i=0; *name; i++, name++)
	{
	    if (*name == '.')
	    {
	        cmd[i] = 0;
	        break;
	    }

            cmd[i] = *name;
	}

	return multiboot_info->mods_count;
}

int exec(char *name, char **argp, char **envp, unsigned uvirt_to_kvirt)
{
	char lista[31];
	unsigned i=1, j;

	for (j=0;j < i; j++)
	{
		i=execlist(lista,j);
		if (!strcmp(name, lista))
            return execid(j, argp, envp, uvirt_to_kvirt);
	}
	return -1;
}

/* Pega os processos atuais. */
int getprocess(ps_t *buffer, uint32_t n){
	int i, num;
	ps_t  *ps;
	num = 0;
	ps  = buffer;
	/* Come�a do 1 para n�o pega a task IDLE. */
	for (i=1;i<NUM_TASKS;i++){
		if (tasks[i].used==1){
			ps->pid    = tasks[i].pid;
			ps->ppid   = tasks[i].ppid;
			ps->status = tasks[i].status;
			strncpy(ps->cmd, tasks[i].cmd, 16);
			num++;
			ps++;
			if (num>=n){
				break;
			}
		}
	}
	return num;
}

int mount(){
	return floppy_init();
}

/* ISR para Syscall: */
int syscall(uregs_t *regs){
	unsigned uvirt_to_kvirt = 0;

	/* Para threads do kernel n�o � preciso fazer a tradu��o de endere�os. */
	if(curr_task->as != NULL){
		/* Valor do endere�o virtual das aplica��es (ring3), s� aplicado em EBX. */
		uvirt_to_kvirt = (unsigned)curr_task->as->user_mem - curr_task->as->virt_adr;
		/* Se EBX for 0, n�o � preciso fazer tradu��o nenhuma. */
		if(regs->ebx == 0){
			uvirt_to_kvirt = 0;
		}
	}

	switch(regs->eax){
		/* Syscalls para manipula��o de descritores. */
		case SYSCALL_OPEN:
			regs->eax = open((char *)(regs->ebx + uvirt_to_kvirt),regs->ecx);
		break;
		case SYSCALL_CLOSE:
			regs->eax = close(regs->edx);
		break;
		case SYSCALL_WRITE:
			regs->eax = write(regs->edx,(char *)(regs->ebx + uvirt_to_kvirt), regs->ecx);
		break;
		case SYSCALL_READ:
			regs->eax = read(regs->edx, (char *)(regs->ebx + uvirt_to_kvirt), regs->ecx);
		break;
		case SYSCALL_SELECT:
			regs->eax = select(regs->edx, regs->ecx,(unsigned *)(regs->ebx + uvirt_to_kvirt));
		break;

		/* Syscalls relacionadas a tempo. */
		case SYSCALL_GETTIME:
			regs->eax = get_time();
		break;
		case SYSCALL_GETDAY:
			regs->eax = get_date();
		break;

		/* Syscalls para manipula��o de consoles. */
		case SYSCALL_CLEAR:
			regs->eax = conclear(regs->ebx);
		break;
		case SYSCALL_SETCOLOR:
			regs->eax = concolor(regs->ebx, regs->ecx, regs->edx);
		break;
		case SYSCALL_GOTOXY:
			regs->eax = congotoxy(regs->ebx, regs->ecx);
		break;
		case SYSCALL_GETXY:
			regs->eax = congetxy(regs->ebx);
		break;
		case SYSCALL_CONWRITE:
		break;
		case SYSCALL_CONREAD:
		break;


		/* Syscalls para manipula��o de processo. */
		case SYSCALL_EXIT:
			taskexit(regs->ebx);
		break;
		case SYSCALL_GETPID:
			regs->eax = curr_task->pid;
		break;
		case SYSCALL_GETPPID:
			regs->eax = curr_task->ppid;
		break;
		case SYSCALL_WAIT4PID:
			regs->eax = wait4pid(regs->ebx);
		break;
		case SYSCALL_EXECID:
			regs->eax = execid(regs->ebx, NULL, NULL, 0);
		break;
		case SYSCALL_PROCESS:
			regs->eax = getprocess((ps_t*)(regs->ebx + uvirt_to_kvirt),regs->ecx);
		break;
		case SYSCALL_EXECLIST:
			regs->eax = execlist((char *)(regs->ebx + uvirt_to_kvirt),regs->ecx);
		break;
		case SYSCALL_EXEC:
			regs->eax = exec((char *)(regs->ebx + uvirt_to_kvirt), NULL, NULL, 0);
		break;
		case SYSCALL_EXECP:
		{
            char **argp = NULL;
            char **envp = NULL;

            if (regs->ecx)
                argp = (char **)(regs->ecx + uvirt_to_kvirt);

            if (regs->edx)
                envp = (char **)(regs->edx + uvirt_to_kvirt);

			regs->eax = exec((char *)(regs->ebx + uvirt_to_kvirt), argp, envp, uvirt_to_kvirt);
		}
		break;

		/* Syscall para sistema de arquivos. */
		case SYSCALL_MOUNT:
			regs->eax = mount();
		break;
		case SYSCALL_UNMOUNT:
			regs->eax = floppy_down();
		break;
		case SYSCALL_FREAD:
			regs->eax = fdc_read(regs->edx, (unsigned char*)(regs->ebx + uvirt_to_kvirt),regs->ecx);
		break;
		case SYSCALL_FWRITE:
			regs->eax = fdc_read(regs->edx, (unsigned char*)(regs->ebx + uvirt_to_kvirt),regs->ecx);
		break;

		case SYSCALL_VERSION:
			bem_vindo();
		break;

		case SYSCALL_PCICOUNT:
			regs->eax = pci_getmax();
		break;
		case SYSCALL_PCIDEV:
			regs->eax = pci_setdevice((pci_device_t*)(regs->ebx + uvirt_to_kvirt), regs->ecx);
		break;

		case SYSCALL_V86_INIT:
            v86_init_regs((uregs_t*)(regs->ebx + uvirt_to_kvirt));
		break;

		case SYSCALL_V86_CALL:
            if (!setjmp(g_oops))
                v86_call_int((uregs_t*)(regs->ebx + uvirt_to_kvirt), regs->ecx);
		break;

		case SYSCALL_V86_MEMREAD:
            regs->eax = (unsigned)v86_mem_read((void*)(regs->ebx + uvirt_to_kvirt), (void *)regs->ecx, regs->edx);
		break;

		case SYSCALL_V86_VIDEOWR:
            v86_video_write((void*)(regs->ebx + uvirt_to_kvirt), regs->ecx, regs->edx);
		break;

		default:
			printf("Syscall invalida(#%d), o processo %d sera finalizado.\n",regs->eax, curr_task->pid);
			/* Finaliza o processo. */
			taskexit(-1);
		break;
	}

	return 0;
}

void init_syscall()
{
	isr_vector_t syscall_vec = { 0xEE, syscall };
	isr_set_handler(SYSCALL_INT, &syscall_vec);
}

