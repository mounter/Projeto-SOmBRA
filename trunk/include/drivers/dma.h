/*****************************************************************************
**                                                                          **
**	Arquivo: dma.h                                                          **
**	Descrição:                                                              **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#ifndef DMA_H
#define DMA_H

#include <kernel/types.h>

#define NUM_FRAMES   16
#define SIZE_FRAME   (64*1024)

typedef struct{
  uint8_t page;     /* Page register.   */
  uint8_t offset;   /* Offset register. */
  uint8_t length;   /* Length register. */
} dmachannel_t;

void  init_dma();
void* dma_phys_alloc();
void  dma_phys_free(void *mm);
void  dma_xfer(unsigned channel, unsigned int physaddr, unsigned int length, bool read);


#endif
