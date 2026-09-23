/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_internal.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:56:47 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/23 11:58:53 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
			swap_nodes(&heap->data[index], &heap->data[parent]);
			index = parent;
		}
	}
}

/*
** Restabelece a propriedade de Min-Heap de cima para baixo (bubble-down).
*/
static void	heapify_down(t_heap *heap, int index)
{
	int	smallest;
	int	left;
	int	right;

	while (index < heap->size)
	{
		smallest = index;
		left = (2 * index) + 1;
		right = (2 * index) + 2;
		if (left < heap->size
			&& should_serve_first(heap->data[left], heap->data[smallest],
				heap->type))
			smallest = left;
		if (right < heap->size
			&& should_serve_first(heap->data[right], heap->data[smallest],
				heap->type))
			smallest = right;
		if (smallest != index)
		{
			swap_nodes(&heap->data[index], &heap->data[smallest]);
			index = smallest;
		}
		else
			break ;
	}
}
