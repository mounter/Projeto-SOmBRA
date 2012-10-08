#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel.h>

/* Syscall para manipulação de descritores de arquivos e consoles. */
#define SYSCALL_OPEN		100
#define SYSCALL_CLOSE		101
#define SYSCALL_READ		102
#define SYSCALL_WRITE		103
#define SYSCALL_SELECT		104

/* Syscall para manipulação especifica de consoles. */
#define SYSCALL_GOTOXY		200
#define SYSCALL_CLEAR		201
#define SYSCALL_SETCOLOR	202
#define SYSCALL_GETXY		203
#define SYSCALL_CONWRITE	204
#define SYSCALL_CONREAD		205

/* Syscall referente a tempo. */
#define SYSCALL_GETTIME		300
#define SYSCALL_GETDAY		301
#define	SYSCALL_SLEEP		302

/* Syscall para manipulação de processos. */
#define	SYSCALL_GETPID		400
#define SYSCALL_EXECID		401
#define	SYSCALL_EXIT		402
#define SYSCALL_GETPPID		403
#define SYSCALL_WAIT4PID	404
#define SYSCALL_PROCESS		405
#define SYSCALL_EXECLIST	406
#define SYSCALL_EXEC		407
#define SYSCALL_EXECP		408

/* Syscall para sistema de arquivos. */
#define SYSCALL_MOUNT		500
#define SYSCALL_UNMOUNT		501
#define SYSCALL_FREAD		502
#define SYSCALL_FWRITE		503

/* Syscall para versão do sistema. */
#define SYSCALL_VERSION		600
#define SYSCALL_GETVER		601

/* Syscall para manipulação de hardwares */
#define	SYSCALL_PCICOUNT	700
#define	SYSCALL_PCIDEV		701

/* Syscall para manipulação de modo virtual */
#define SYSCALL_V86_INIT	800
#define SYSCALL_V86_CALL	801
#define SYSCALL_V86_MEMREAD	802
#define SYSCALL_V86_VIDEOWR	803

// Syscall para gerenciamento de memória do processo
#define SYSCALL_BRK		900

#define	SYSCALL_INT	0x30

typedef struct{
	uint32_t pid;
	uint32_t ppid;
	uint32_t status;
	char     cmd[16];
} ps_t;

int syscall(uregs_t *regs);
void init_syscall();

#endif
