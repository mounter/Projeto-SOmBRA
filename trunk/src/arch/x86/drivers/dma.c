/*****************************************************************************
**                                                                          **
**	Arquivo: dma.c                                                          **
**	Descrição:                                                              **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <dma.h>
#include <system.h>
#include <kernel/mm.h>
#include <stdio.h>

static uint8_t free_frames[NUM_FRAMES];

static dmachannel_t dmainfo[]={
	{ 0x87, 0x00, 0x01 },
	{ 0x83, 0x02, 0x03 },
	{ 0x81, 0x04, 0x05 },
	{ 0x82, 0x06, 0x07 }
};

static uint8_t *dma_mm = NULL;

void init_dma(){
	uint16_t i;

	dma_mm = (unsigned char*)malloc(NUM_FRAMES*SIZE_FRAME);
	printf("DMA_MM: %X a %X.\n", dma_mm, ((int)dma_mm)+(NUM_FRAMES*SIZE_FRAME));

	for (i=0;i<NUM_FRAMES;i++){
		free_frames[i] = true;
	}

}

void* dma_phys_alloc(){
	uint16_t i;

	for (i=0;i<NUM_FRAMES;i++){
		if (free_frames[i]){
			break;
		}
	}

	if (i>=NUM_FRAMES){
		return NULL;
	}

	free_frames[i] = false;

	return ((void*)(dma_mm+(i*SIZE_FRAME)));
}

void dma_phys_free(void *mm){
	int32_t m;
	m  = (int32_t)mm;
	m -= (int32_t)dma_mm;
	m /= SIZE_FRAME;
	if ((m<0)||(m>(NUM_FRAMES-1))){
		panic("Desalocacao incorreta de frames do DMA.");
	}
	free_frames[m] = true;
}

void dma_xfer(unsigned channel, unsigned int physaddr, unsigned int length, bool read){
	int page, offset;

	if (channel > 3){
		return;
	}

	page    = physaddr >> 16;
	offset  = physaddr & 0xFFFF;
	length -= 1;

	disable();

	outportb(0x0A, channel | 0x04);
	outportb(0x0C, 0x00);

	outportb(0x0B, (read ? 0x48 : 0x44) + channel);

	outportb(dmainfo[channel].page, page);

	outportb(dmainfo[channel].offset, offset & 0xFF);
	outportb(dmainfo[channel].offset, offset >> 8);

	outportb(dmainfo[channel].length, length & 0xFF);
	outportb(dmainfo[channel].length, length >> 8);

	outportb(0x0A, channel);

	enable();
}
