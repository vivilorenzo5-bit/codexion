/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 11:57:08 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/23 11:58:14 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_H
# define HEAP_H

# include <stddef.h>

typedef enum e_sched_type
{
	SCHED_FIFO,
	SCHED_EDF
}	t_sched_type;

/*
** Nó da fila de espera de um dongle.
** - coder_id: identificador do coder (desempate determinístico)
** - arrival_time: timestamp de quando o coder pediu o dongle
** - deadline: timestamp limite (last_compile + time_to_burnout)
*/

typedef struct s_node
{
	int			coder_id;
	long long	arrival_time;
	long long	deadline;
}	t_node;

/*
** Min-Heap binário.
** Como cada dongle só é disputado pelos seus 2 vizinhos, a capacidade máxima
** necessária por dongle é pequena, mas definimos uma capacidade dinâmica/fixa segura.
*/

typedef struct s_heap
{
	t_node			*data;
	int				size;
	int				capacity;
	t_sched_type	type;
}	t_heap;

/* Funções Públicas do Heap */
int		heap_init(t_heap *heap, int capacity, t_sched_type type);
void	heap_destroy(t_heap *heap);
int		heap_push(t_heap *heap, t_node node);
int		heap_pop(t_heap *heap, t_node *min_out);
int		heap_peek(const t_heap *heap, t_node *min_out);

/* Helpers Internos de Rebalanceamento */
void	swap_nodes(t_node *a, t_node *b);
void	heapify_up(t_heap *heap, int index);
void	heapify_down(t_heap *heap, int index);

/* Comparador de Prioridade */
int		should_serve_first(t_node a, t_node b, t_sched_type type);

#endif
