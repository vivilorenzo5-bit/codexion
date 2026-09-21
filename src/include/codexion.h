/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 12:23:45 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/21 12:30:57 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include "heap.h"

typedef struct s_simulation t_simulation;

/*
** Representação do USB Dongle:
** - id: identificador para ordenar locks e prevenir deadlocks
** - mutex: protege o estado deste dongle
** - cond: variável de condição onde os coders esperam pela sua vez
** - is_in_use: 1 se estiver na mão de alguém, 0 se na mesa
** - cooldown_until: timestamp ms até quando o dongle está inutilizável
** - wait_queue: Priority Queue com os pedidos pendentes
*/

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_in_use;
	long long		cooldown_until;
	t_heap			wait_queue;
}	t_dongle;

/*
** Representação de cada Coder (Thread individual):
** - id: número de 1 a N
** - compile_count: vezes que já compilou com sucesso
** - last_compile_start: início da última compilação (para cálculo de burnout)
** - coder_mutex: protege as leituras do monitor a last_compile_start e compile_count
** - left_dongle / right_dongle: apontadores para os dongles adjacentes
*/

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compile_count;
	long long		last_compile_start;
	pthread_mutex_t	coder_muted;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_simulation	*sim;
}	t_coder;


struct s_simulation
{
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				compiles_required;
	long long		dongle_cooldown;
	t_sched_type	scheduler;

	long long		start_time;
	int				is_finished;
	pthread_mutex_t	finish_mutex;
	pthread_mutex_t	log_mutex;

	t_dongle		*dongles;
	t_coder			*coders;
	pthread_t		monitor_thread;
};