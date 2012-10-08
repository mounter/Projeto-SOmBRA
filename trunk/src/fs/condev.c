#include "condev.h"
#include <stdio.h>
#include <drivers/kbd.h>
#include <kernel/thread.h>
#include <kernel/queue.h>

// Array contendo as structs dos consoles
extern console_t	consoles[MAX_CONSOLE];
static uint32_t     con_opened[MAX_CONSOLE];

/* Desabilita as interrupções e pega o valor do EFLAGS. */
static unsigned ndisable(){
	unsigned ret_val;
	asm volatile(
		"pushfl\n"
		"popl %0\n"
		"cli"
		: "=a"(ret_val):);
	return ret_val;
}

/* Atualiza o valor dos EFLAGS. */
static void crite(unsigned flags){
	__asm__ __volatile__(
		"pushl %0\n"
		"popfl"
		:
		: "m"(flags)
		);
}

//static int conwrite(int idcon, uint8_t *buffer, uint32_t len){
//	unsigned flags;
//	if ((idcon>=0)&&(idcon<MAX_CONSOLE)){
//		flags = ndisable();
//			idcon=set_con(idcon);
//			nputs((char *)buffer, len);
//			set_con(idcon);
//		crite(flags);
//		return len;
//	}
//
//	return -1;
//}

/* Fecha o descritor de console, como os consoles são estáticos
   não é preciso fazer nada. */
static void con_close(fs_node_t *fs_node)
{
    if (fs_node->impl < con_opened[fs_node->inode])
        con_opened[fs_node->inode]--;
}



/* Escreve no console. Um ponto importante a ser levantado é que devemos salvar EFLAGS. */
static uint32_t con_write(fs_node_t *fs_node,uint8_t *buf, uint32_t len)
{
	unsigned flags;
	int con;

    if (fs_node->inode >= MAX_CONSOLE)
        return 0;

    if (!con_opened[fs_node->inode])
        return 0;

	con = fs_node->inode;	/* Pega o console. */
	con = set_con(con);

// Poderá ocorrer spurious interrupts nestá area do código!!!
// Devido ao IRQ0 disparar várias vezes com strings muito grandes sendo processadas!
// Melhorar depois.
	flags = ndisable();			/* Salva EFLAGS. */

    nputs((char *)buf,  len);	/* Escreve no console. */

	set_con(con);

	crite(flags);				/* Restaura EFLAGS. */
	return len;
}


/* Lê do console. */
static uint32_t con_read(fs_node_t *fs_node, uint8_t *buf, uint32_t len)
{
	unsigned flags, i;
	console_t  *con;

    if (fs_node->inode >= MAX_CONSOLE)
        return 0;

    if (!con_opened[fs_node->inode])
        return 0;

	con = &consoles[fs_node->inode]; /* Pega o console. */

	flags = ndisable();			/* Salva o EFLAGS. */
	for(i = 0; i < len; i++){
		do{
			if(con->keys.id_first == con->keys.id_last){ /* No caso da fila estar fazia. */
				/* Coloca o processo para dormir, na fila do seu respectivo console. */
				sleep_on(&con->wait, NULL);
			}
		} while(!queue_get_data(&con->keys, buf));

		nputs((char *)buf, 1);

		if (*buf == '\n')
		{
		    len = ++i;
		    break;
		}

		buf++;
	}
	/* Restaura EFLAGS. */
	crite(flags);

	return len;
}

/* Altera o modo de acesso do console. */
//static int con_select(fs_node_t *fs_node, uint32_t access, unsigned *timeout)
//{
//	console_t *con;
//	int rv;
//
//	con = &consoles[(int )f->data];
//	while(1){
//		/* Console sempre está apto para escrever. */
//		rv = 0x02;
//		/* Checa se há algo para ler do console virtual. */
//		if(con->keys.id_first != con->keys.id_last){
//			rv |= 0x01;
//		}
//		/* Pronto para ler ou escrever. */
//		rv &= access;
//		if(rv){
//			break;
//		}
//		/* Sem timeout. */
//		if(timeout == NULL){
//			break;
//		}
//		/* Espera pelo timeout. */
//		if(sleep_on(&con->wait, timeout)){
//			break;
//		}
//	}
//	return rv;
//}

/* Seção temporária depois fazer algo mais descente !!! */
//int ncon = 0;

/* Abre um descritor de console virtual. */
static void con_open(fs_node_t *fs_node, uint32_t mask)
{
    fs_node->impl = con_opened[fs_node->inode]++;
}

/* Abre um descritor de console virtual, no caso o console do pai. */
static void con_opentty(fs_node_t *fs_node, uint32_t mask)
{
	task_t *task;
	uint32_t  ppid;
	int     i;

	task = NULL;
	ppid = task_curr->ppid;

	/* Procura o slot do pai. */
	for (i=0;i < NUM_TASKS;i++){
		if (tasks[i].pid == ppid){
			task = &tasks[i];
			break;
		}
	}

	/* Seleciona o console virtual. */
    if (task != NULL)
        if (task->files != NULL){
            fs_node->inode = task->files->inode;
            fs_node->impl = con_opened[fs_node->inode]++;
            return;
        }

	fs_node->inode = 0;
	fs_node->impl = con_opened[fs_node->inode]++;
}

// Parametro fs_node -> pasta /dev
int condev_init(fs_node_t *fs_node)
{
    // Instalar todos os inodes de tty
    // tty
    // tty1 - ttyX

    static char *tty_names[MAX_CONSOLE] = {
        "tty1", "tty2", "tty3", "tty4",
        "tty5", "tty6", "tty7", "tty8"
    };

    unsigned ino;

    // Atribuindo o primeiro nodo
    fs_node_t *tty_node = fs_create_node(
                        "tty",
                        FS_FILE,
                        fs_node,
                        con_read,
                        con_write,
                        con_opentty,
                        con_close,
                        NULL,
                        NULL
                );

    // Atribuindo os outros nodos
    for (ino = 0; ino < MAX_CONSOLE; ino++)
    {
        con_opened[ino] = 0;
        tty_node = fs_create_node(
                        tty_names[ino],
                        FS_FILE,
                        fs_node,
                        con_read,
                        con_write,
                        con_open,
                        con_close,
                        NULL,
                        NULL
                );

        fs_node_setINode(tty_node, ino);
    }

    return 1;
}
