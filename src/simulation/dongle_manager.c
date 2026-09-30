/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 10:58:54 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 10:52:41 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#include "codexion.h"

/*
** Aguarda passivamente enquanto o dongle estiver em cooldown.
*/
static void	wait_for_cooldown(t_dongle *d, t_simulation *sim)
{
	long long	now;

	now = get_current_time_ms();
	while (!is_simulation_over(sim) && now < d->cooldown_until)
	{
		pthread_mutex_unlock(&d->mutex);
		precise_sleep(d->cooldown_until - now, sim);
		pthread_mutex_lock(&d->mutex);
		now = get_current_time_ms();
	}
}

/*
** Regista o pedido no heap e aguarda pela sua vez e pelo fim do cooldown.
*/
static void	acquire_single_dongle(t_coder *coder, t_dongle *d)
{
	t_node	req;
	t_node	top;

	pthread_mutex_lock(&d->mutex);
	req.coder_id = coder->id;
	req.arrival_time = get_current_time_ms();
	pthread_mutex_lock(&coder->coder_muted);
	req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->coder_muted);
	heap_push(&d->wait_queue, req);
	while (!is_simulation_over(coder->sim))
	{
		heap_peek(&d->wait_queue, &top);
		wait_for_cooldown(d, coder->sim);
		if (!d->is_in_use && top.coder_id == coder->id)
			break ;
		pthread_cond_wait(&d->cond, &d->mutex);
	}
	heap_pop(&d->wait_queue, &top);
	d->is_in_use = 1;
	pthread_mutex_unlock(&d->mutex);
}

/*
** Liberta um unico dongle, marca o cooldown e acorda threads a espera.
*/
static void	release_single_dongle(t_coder *coder, t_dongle *d)
{
	pthread_mutex_lock(&d->mutex);
	d->is_in_use = 0;
	d->cooldown_until = get_current_time_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}

/*
** Adquire os dois dongles ordenados por ID (prevencao absoluta de deadlock).
** Emite ambos os logs sequencialmente assim que detem a posse de ambos.
*/
void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	acquire_single_dongle(coder, first);
	acquire_single_dongle(coder, second);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
}

/*
** Liberta ambos os dongles apos a compilacao.
*/
void	drop_dongles(t_coder *coder)
{
	release_single_dongle(coder, coder->left_dongle);
	release_single_dongle(coder, coder->right_dongle);
}
