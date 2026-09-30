/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 10:58:54 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 11:55:09 by vlourenc         ###   ########.fr       */
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
static void	acquire_dongle(t_coder *coder, t_dongle *d)
{
	t_node	req;
	t_node	top;

	req.coder_id = coder->id;
	req.arrival_time = get_current_time_ms();
	pthread_mutex_lock(&coder->coder_muted);
	req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->coder_muted);
	pthread_mutex_lock(&d->mutex);
	heap_push(&d->wait_queue, req);
	while (!is_simulation_over(coder->sim))
	{
		wait_for_cooldown(d, coder->sim);
		if (!d->is_in_use && heap_peek(&d->wait_queue, &top) == 0
			&& top.coder_id == coder->id)
			break ;
		pthread_cond_wait(&d->cond, &d->mutex);
	}
	heap_pop(&d->wait_queue, &top);
	d->is_in_use = 1;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}

/*
** Ordena os dois dongles por ID para garantir prevencao total de deadlock.
*/
static void	order_dongles(t_coder *coder, t_dongle **f, t_dongle **s)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*f = coder->left_dongle;
		*s = coder->right_dongle;
	}
	else
	{
		*f = coder->right_dongle;
		*s = coder->left_dongle;
	}
}

/*
** Adquire ambos os dongles, renova imediatamente a deadline e emite os logs.
*/
void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	acquire_dongle(coder, first);
	acquire_dongle(coder, second);
	pthread_mutex_lock(&coder->coder_muted);
	coder->last_compile_start = get_current_time_ms();
	pthread_mutex_unlock(&coder->coder_muted);
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
}

/*
** Liberta ambos os dongles, marca o cooldown e acorda threads a espera.
*/
void	drop_dongles(t_coder *coder)
{
	long long	cd;

	cd = get_current_time_ms() + coder->sim->dongle_cooldown;
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->is_in_use = 0;
	coder->left_dongle->cooldown_until = cd;
	pthread_cond_broadcast(&coder->left_dongle->cond);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->is_in_use = 0;
	coder->right_dongle->cooldown_until = cd;
	pthread_cond_broadcast(&coder->right_dongle->cond);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}
