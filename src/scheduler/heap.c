/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 10:51:37 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/23 11:09:35 by vlourenc         ###   ########.fr       */
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
** Troca a posição de dois nós dentro do array.
*/
static void	swap_nodes(t_node *a, t_node *b)
{
	t_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
** Restabelece a propriedade de Min-Heap de baixo para cima (bubble-up).
*/
static void	heapify_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (should_serve_first(heap->data[index], heap->data[parent],
				heap->type))
		{
			
		}
	}
}