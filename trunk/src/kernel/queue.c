#include <kernel/queue.h>
#include <kernel/mm.h>

/* Pega e remove o primeiro elemento da lista. */
bool queue_get_data(queue_t *q, unsigned char *data)
{
	/* Caso da lista estar vazia. */
	if(q->id_first == q->id_last){
		return false;
	}

	/* Pega o elemento e incrementa o id do primeiro item. */
	(*data) = q->data[q->id_first++];

	/* Retorna para o começo do array, se id_first>=size. */
	if(q->id_first >= q->size){
		q->id_first = 0;
	}

	return true;
}

bool queue_insert_data(queue_t *q, unsigned char data)
{
	unsigned i;

	i = q->id_last + 1;
	if(i >= q->size){
		i = 0;
	}

	/* Caso a lista esteja cheia. */
	if(i == q->id_first){
		return false;
	}

	/* Coloca o dado na lista e seta o id(id_last) da próxima inserção. */
	q->data[q->id_last] = data;
	q->id_last = i;

	return true;
}

bool queue_init(queue_t *q, size_t sz)
{
	q->data = zmalloc(sz * sizeof(char));
	q->size = sz;

	return q->data != NULL;
}

