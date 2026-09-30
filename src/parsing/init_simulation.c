/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 09:52:18 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 11:21:23 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

/*
** Inicializa os mutexes globais de sincronização (logs e término da simulação).
*/
static int	init_global_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->finish_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->finish_mutex);
		return (1);
	}
	sim->is_finished = 0;
	return (0);
}

/*
** Inicializa o estado de um dongle individual: mutex, variável de condição
** e a respetiva fila de espera em heap (capacidade para os 2 coders vizinhos).
*/
static int	init_single_dongle(t_dongle *d, int id, t_sched_type type)
{
	d->id = id;
	d->is_in_use = 0;
	d->cooldown_until = 0;
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mutex);
		return (1);
	}
	if (heap_init(&d->wait_queue, 10, type) != 0)
	{
		pthread_cond_destroy(&d->cond);
		pthread_mutex_destroy(&d->mutex);
		return (1);
	}
	return (0);
}

/*
** Aloca o array de dongles e inicializa cada um sequencialmente.
*/
static int	init_dongles(t_simulation *sim)
{
	int	i;

	sim->dongles = (t_dongle *)malloc(sizeof(t_dongle) * sim->num_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		if (init_single_dongle(&sim->dongles[i], i, sim->scheduler) != 0)
			return (1);
		i++;
	}
	return (0);
}

/*
** Configura os atributos individuais de cada coder e mapeia a mesa circular:
** coder N fica com dongles vizinhos à esquerda e à direita.
*/
static int	init_coders(t_simulation *sim)
{
	int	i;

	sim->coders = (t_coder *)malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].coder_muted, NULL) != 0)
			return (1);
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->num_coders];
		i++;
	}
	return (0);
}

/*
** Ponto de entrada da inicialização completa da simulação.
*/
int	init_simulation(t_simulation *sim)
{
	if (init_global_mutexes(sim) != 0)
		return (1);
	if (init_dongles(sim) != 0)
		return (1);
	if (init_coders(sim) != 0)
		return (1);
	return (0);
}
