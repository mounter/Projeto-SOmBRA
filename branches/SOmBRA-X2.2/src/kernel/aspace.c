#include <aspace.h>
#include <mm.h>
#include <error.h>
#include <stdio.h>

// Cria uma aspace.
aspace_t *aspace_create(){
	aspace_t *as;

	// Aloca mem�ria para uma aspace.
	as = (aspace_t *)zmalloc(sizeof(aspace_t));
	if(as == NULL)
		return NULL;

	// Seta o flag indicando que o aspace � v�lido.
	as->magic = AS_MAGIC;
	return as;
}

// Destroi uma aspace.
void aspace_destroy(aspace_t *as){
	if(as->magic != AS_MAGIC){
		panic("destroy_aspace: 'as' inv�lida endereco: %x\n", as);
	}

	// Desaloca mem�ria alocado para a aplica��o.
	//if(as->user_mem != NULL){
	//	free(as->user_mem);
	//}

	if(as->sect != NULL)
	{
		memset(as->sect, 0, as->num_sects * sizeof(sect_t));
		free(as->sect);
	}
	
	memset(as, 0, sizeof(aspace_t));

	// Libera a mem�ria alocada para guardar as se��es.
	free(as);
}

// Insere uma nova se��o dentro de um aspace.
int aspace_section_add(aspace_t *as, uint_arch_t adr, uint_arch_t size, uint_arch_t flags, uint_arch_t offset){
	sect_t *new_sect;

	//printf("aspace_section_add(%X, %u, %X, %X)\n", adr, size, flags, offset);

	// Aloca mem�ria para mais uma se��o e faz a copia das se��es anteriores.
	new_sect = (sect_t *)realloc(as->sect,sizeof(sect_t) * (as->num_sects + 1));
	if(new_sect == NULL)
	{
		return -ERROR_NO_EXEC;
	}
	
	// Seta o novo ponteiro para as novas se��es.
	as->sect = new_sect;

	// new_sect aponta para a �tlima se��o(a nova se��o).
	new_sect = &as->sect[as->num_sects];
	as->num_sects++; // Incrementa o n�mero de se��es.

	// Seta os valores das novas se��es.
	new_sect->adr = adr;
	new_sect->size = size;
	new_sect->flags = flags;
	new_sect->offset = offset;

	return 0;
}
