/********************************************************
** Nome: mm.h
** Data: 07/02/2009
** Autor: Mauro Joel Schütz
** Descrição: Controle de alocação de memória
********************************************************/
#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <kernel/types.h>

#define NM_MEMORY 0xBABA01

#pragma pack(1)
typedef struct{
	uint32_t  magic;
	uintptr_t  size;
	void   *next;
} mmblock_t;

int mm_init(void);
void* malloc(uint32_t size);
void *calloc(size_t num, size_t size);
void  free(void *blk_UNUSED);
void* realloc(void *blk, uint32_t size);
void* zmalloc(uint32_t size);
void mmdump(void);

#endif
