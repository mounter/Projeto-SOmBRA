/********************************************************
** Nome: mm.c
** Data: 07/02/2009
** Autor: Mauro Joel Sch�tz
** Descri��o: Controle de aloca��o de mem�ria
********************************************************/
#include <kernel/mm.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <arch.h>
#include _PAGE_H_

static mmblock_t *mmpi = NULL;

int mm_init(void)
{
    if (page_init())
        return 1;

	mmpi = (mmblock_t*)brk(sizeof(mmblock_t)+1);
    mmpi->magic = NM_MEMORY;
    mmpi->size  = 1;
    mmpi->next  = NULL;

    return 0;
}

void* malloc_g(uint32_t size, uint32_t gran)
{
	mmblock_t *pn,*pt;
	uintptr_t total_size, n;
	uintptr_t g;

	// Calcula o espa�o de mem�ria que ser� necess�rio para armazenar o bloco e os dados.
	total_size = sizeof(mmblock_t)+(size * gran);

	// Se o tamanho for zero retorna NULL.
	if (size == 0)
	{
		return NULL;
	}

	// Percorrer a lista at� encontrar um espa�o capaz de armazenar o bloco e os dados.
	pn = mmpi;
	while (pn->next != NULL)
	{
		g = ((uintptr_t)pn+sizeof(mmblock_t)+pn->size);
		n = ((uintptr_t)pn->next) - g;

		if ((g = ((g + sizeof(mmblock_t)) % gran)))
			g = gran - g;
		
		// Em caso de uma lacuna na mem�ria.
		if ((total_size+g) <= n)
		{
			pt = (mmblock_t*)((uintptr_t)pn + (pn->size) + sizeof(mmblock_t) + g);
			pt->magic = NM_MEMORY;
			pt->size  = size * gran;
			pt->next  = pn->next;
			pn->next  = pt;
			
			return (void*)((uintptr_t)pt+sizeof(mmblock_t));
		}

		if (pn->magic!=NM_MEMORY){
			panic("O Heap do kernel est� corrompido.");
			return NULL;
		}
		pn = pn->next;
	}

	if ((g = (((uintptr_t)pn + (2 * sizeof(mmblock_t)) + pn->size) % gran)))
		g = gran - g;

	// Armazena no fim da mem�ria.
	pt = (mmblock_t*)brk(total_size+g);

	if (pt == NULL)
        	return NULL;

	pt = (mmblock_t *)((uintptr_t)pt+g);

	pt->magic = NM_MEMORY;
	pt->size  = size * gran;
	pt->next  = NULL;
	pn->next  = pt;

	//printf("MALLOC_ADDR: %X\n", (long)pt+sizeof(mmblock_t));
	return (void*)((intptr_t)pt+sizeof(mmblock_t));
}

void *malloc(uint32_t size)
{
	return malloc_g(size, 1);
}

void *calloc(size_t num, size_t size)
{
	return zmalloc(num*size);
}

// Libera mem�ria
void free(void *mm){
	if (mm == NULL)
		return;
	
	mmblock_t *pt, *pn;

	// Pega o bloco referente aquele endere�o de mem�ria.
	pt = (mmblock_t*)((intptr_t)mm-sizeof(mmblock_t));

	// Verifica se realmente � um bloco de mem�ria.
	if (pt->magic != NM_MEMORY)
	{
		panic("Endereco de memoria incorreto(kfree %X %lX).", pt->magic, (intptr_t)mm);
		return;
	}

	// Mant�m a lista encadeada e consistente.
	pn = mmpi;
	while(pn->next!=NULL){
		if (pn->next==pt){
			pn->next = pt->next;

			if (pt->next == NULL)
				brk(-(((uintptr_t)pt + sizeof(mmblock_t)+pt->size) -
					((uintptr_t)pn + sizeof(mmblock_t)+pn->size)));

			return;
		}
		pn = pn->next;
  	}
}

// Alloca mem�ria e zera o conte�do.
void* zmalloc(uint32_t size){
	void *mmnew;
	mmnew = malloc(size);
	if (mmnew!=NULL){
		memset(mmnew, 0, size);
	}

	return mmnew;
}

// Realoca mem�ria e copia o conte�do se mm for v�lido.
void* realloc(void *mm, uint32_t size){
	void *mmnew;
	mmblock_t *pt = (mmblock_t*)((intptr_t)mm-sizeof(mmblock_t));

	mmnew = zmalloc(size);
	if ((mmnew!=NULL)&&(mm!=NULL)){
		memcpy(mmnew, mm, pt->size);
		free(mm);
	}
	return mmnew;
}


// Mostra a lista encadeada de mem�ria.
void mmdump(){
  mmblock_t *pn;
  int i;
  pn = mmpi;
  i = 0;
  printf("-------------------MMDUMP-------------------\n");
  while (pn!=NULL){
    printf("[%d]pn=%d, size=%d, next=%d\n", i, pn, pn->size, pn->next);
    printf("Data:|%s|\n", (intptr_t)pn+sizeof(mmblock_t));
    i++;
    pn = pn->next;
  }
}
