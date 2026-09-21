/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 11:57:08 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/21 12:13:40 by vlourenc         ###   ########.fr       */
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
