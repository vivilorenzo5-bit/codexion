/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 10:29:27 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/23 12:03:30 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Retorna o elemento de maior prioridade sem remover da fila.
*/
int	heap_peek(const t_heap *heap, t_node *min_out)
{
	if (!heap || heap->size == 0 || !min_out)
		return (1);
	*min_out = heap->data[0];
	return (0);
}

/*
** Determina se o nó 'a' deve ser atendido antes do nó 'b'.
** Retorna 1 se 'a' tiver maior prioridade que 'b', ou 0 caso contrário.
** Para LIFO, tudo o que tenho de fazer e mudar o < para > 
*/
int	should_serve_first(t_node a, t_node b, t_sched_type type)
{
	if (type == SCHED_EDF)
	{
		if (a.deadline != b.deadline)
			return (a.deadline < b.deadline);
		return (a.coder_id < b.coder_id);
	}
	if (type == SCHED_FIFO)
	{
		if (a.arrival_time != b.arrival_time)
			return (a.arrival_time < b.arrival_time);
		return (a.coder_id < b.coder_id);
	}
	return (0);
}
