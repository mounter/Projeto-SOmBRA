#include <kernel/mm.h>
#include <stdio.h>
#include <kernel.h>
#include PAGE_H

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

void* malloc_g(uint32_t size, uint32_t gran){
	mmblock_t *pn,*pt;
	uint32_t total_size, n;
	uint32_t g;

	// Calcula o espaço de memória que será necessário para armazenar o bloco e os dados.
	total_size = sizeof(mmblock_t)+(size * gran);

	// Se o tamanho for zero retorna NULL.
	if (size == 0){
		return NULL;
	}

	// Percorrer a lista até encontrar um espaço capaz de armazenar o bloco e os dados.
	pn = mmpi;
	while (pn->next != NULL){
		g = ((uint32_t)pn+sizeof(mmblock_t)+pn->size);
		n = ((uint32_t)pn->next) - g;

		if ((g = ((g + sizeof(mmblock_t)) % gran)))
			g = gran - g;
		
		// Em caso de uma lacuna na memória.
		if ((total_size+g) <= n)
		{
			pt = (mmblock_t*)((uint32_t)pn + (pn->size) + sizeof(mmblock_t) + g);
			pt->magic = NM_MEMORY;
			pt->size  = size * gran;
			pt->next  = pn->next;
			pn->next  = pt;
			
			return (void*)((uint32_t)pt+sizeof(mmblock_t));
		}

		if (pn->magic!=NM_MEMORY){
			panic("O Heap do kernel está corrompido.");
			return NULL;
		}
		pn = pn->next;
	}

	if ((g = (((uint32_t)pn + (2 * sizeof(mmblock_t)) + pn->size) % gran)))
		g = gran - g;

	// Armazena no fim da memória.
	pt = (mmblock_t*)brk(total_size+g);

	if (pt == NULL)
        	return NULL;

	pt = (mmblock_t *)((uint32_t)pt+g);

	pt->magic = NM_MEMORY;
	pt->size  = size * gran;
	pt->next  = NULL;
	pn->next  = pt;

	//printf("MALLOC_ADDR: %X\n", (int)pt+sizeof(mmblock_t));
	return (void*)((int)pt+sizeof(mmblock_t));
}
/*
void* malloc(uint32_t size){
	mmblock_t *pn,*pt;
	int total_size, n;

	// Calcula o espaço de memória que será necessário para armazenar o bloco e os dados.
	total_size = sizeof(mmblock_t)+size;

	// Se o tamanho for zero retorna NULL.
	if (size==0){
		return NULL;
	}

	// Percorrer a lista até encontrar um espaço capaz de armazenar o bloco e os dados.
	pn = mmpi;
	while (pn->next!=NULL){
		n = ((int)pn->next)-((int)pn+sizeof(mmblock_t)+pn->size);
		// Em caso de uma lacuna na memória.
		if (total_size<=n){
			pt = (mmblock_t*)((int)pn+(pn->size)+sizeof(mmblock_t));
			pt->magic = NM_MEMORY;
			pt->size  = size;
			pt->next  = pn->next;
			pn->next  = pt;

			g_curr_heap_size += total_size;

			return (void*)((int)pt+sizeof(mmblock_t));
		}
		if (pn->magic!=NM_MEMORY){
			panic("O Heap do kernel está corrompido.");
			return NULL;
		}
		pn = pn->next;
	}

	// Armazena no fim da memória.
	pt = (mmblock_t*)brk(sizeof(mmblock_t)+size);

	if (pt == NULL)
        	return NULL;

	pt->magic = NM_MEMORY;
	pt->size  = size;
	pt->next  = NULL;
	pn->next  = pt;

	g_curr_heap_size += total_size;

	//printf("MALLOC_ADDR: %X\n", (int)pt+sizeof(mmblock_t));
	return (void*)((int)pt+sizeof(mmblock_t));
}*/

void *malloc(uint32_t size)
{
	return malloc_g(size, 1);
}

// Libera memória
void free(void *mm){
	mmblock_t *pt, *pn;

	// Pega o bloco referente aquele endereço de memória.
	pt = (mmblock_t*)((int)mm-sizeof(mmblock_t));

	// Verifica se realmente é um bloco de memória.
	if (pt->magic!=NM_MEMORY){
		panic("Endereco de memoria incorreto(kfree %X %X).", pt->magic, (int)mm);
		return;
	}

	// Mantém a lista encadeada e consistente.
	pn = mmpi;
	while(pn->next!=NULL){
		if (pn->next==pt){
			pn->next = pt->next;

			if (pt->next == NULL)
				brk(-(((uint32_t)pt + sizeof(mmblock_t)+pt->size) -
					((uint32_t)pn + sizeof(mmblock_t)+pn->size)));

			return;
		}
		pn = pn->next;
  	}
}

// Alloca memória e zera o conteúdo.
void* zmalloc(uint32_t size){
	void *mmnew;
	mmnew = malloc(size);
	if (mmnew!=NULL){
		memset(mmnew, 0, size);
	}

	return mmnew;
}

// Realoca memória e copia o conteúdo se mm for válido.
void* realloc(void *mm, uint32_t size){
	void *mmnew;
	mmblock_t *pt = (mmblock_t*)((int)mm-sizeof(mmblock_t));

	mmnew = zmalloc(size);
	if ((mmnew!=NULL)&&(mm!=NULL)){
		memcpy(mmnew, mm, pt->size);
		free(mm);
	}
	return mmnew;
}


// Mostra a lista encadeada de memória.
void mmdump(){
  mmblock_t *pn;
  int i;
  pn = mmpi;
  i = 0;
  printf("-------------------MMDUMP-------------------\n");
  while (pn!=NULL){
    printf("[%d]pn=%d, size=%d, next=%d\n", i, pn, pn->size, pn->next);
    printf("Data:|%s|\n", (int)pn+sizeof(mmblock_t));
    i++;
    pn = pn->next;
  }
}
