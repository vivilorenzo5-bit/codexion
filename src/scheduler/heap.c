/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 10:51:37 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/23 12:02:25 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Inicializa a estrutura do Heap alocando espaço para 'capacity' nós.
** Retorna 0 em caso de sucesso ou 1 se o malloc falhar.
*/
int	heap_init(t_heap *heap, int capacity, t_sched_type type)
{
	heap->data = (t_node *)malloc(sizeof(t_node) * capacity);
	if (!heap->data)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	heap->type = type;
	return (0);
}

/*
** Liberta a memória alocada para os nós e repõe os contadores a zero.
*/
void	heap_destroy(t_heap *heap)
{
	if (heap && heap->data)
	{
		free(heap->data);
		heap->data = NULL;
	}
	if (heap)
	{
		heap->size = 0;
		heap->capacity = 0;
	}
}

/*
** Insere um no no fim do heap e reposiciona-o chamando heapify_up.
*/
int	heap_push(t_heap *heap, t_node node)
{
	if (!heap || heap->size >= heap->capacity)
		return (1);
	heap->data[heap->size] = node;
	heapify_up(heap, heap->size);
	heap->size++;
	return (0);
}

/*
** Extrai a raiz (maior prioridade) e reorganiza o heap com heapify_down.
*/
int	heap_pop(t_heap *heap, t_node *min_out)
{
	if (!heap || heap->size == 0)
		return (1);
	if (min_out)
		*min_out = heap->data[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->data[0] = heap->data[heap->size];
		heapify_down(heap, 0);
	}
	return (0);
}
