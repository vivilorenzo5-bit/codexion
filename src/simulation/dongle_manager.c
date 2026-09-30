/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 10:58:54 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 11:04:44 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	order_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

static int	is_ready(t_dongle *d, t_coder *coder, long long now)
{
	t_node	top;

	if (d->is_in_use || now < d->cooldown_until)
		return (0);
	if (!heap_peek(&d->wait_queue, &top) || top.coder_id != coder->id)
		return (0);
	return (1);
}

static int	try_lock_both(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_node		top;
	long long	now;

	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	now = get_current_time_ms();
	if (is_ready(first, coder, now) && is_ready(second, coder, now))
	{
		heap_pop(&first->wait_queue, &top);
		heap_pop(&second->wait_queue, &top);
		first->is_in_use = 1;
		second->is_in_use = 1;
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		return (1);
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_cond_wait(&first->cond, &first->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (0);
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	t_node		req;

	order_dongles(coder, &first, &second);
	req.coder_id = coder->id;
	req.arrival_time = get_current_time_ms();
	pthread_mutex_lock(&coder->coder_muted);
	req.deadline = coder->last_compile_start + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->coder_muted);
	pthread_mutex_lock(&first->mutex);
	heap_push(&first->wait_queue, req);
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(&second->wait_queue, req);
	pthread_mutex_unlock(&second->mutex);
	while (!is_simulation_over(coder->sim))
	{
		if (try_lock_both(coder, first, second))
			break ;
	}
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
}

void	drop_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long long	cd;

	order_dongles(coder, &first, &second);
	cd = get_current_time_ms() + coder->sim->dongle_cooldown;
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	first->is_in_use = 0;
	second->is_in_use = 0;
	first->cooldown_until = cd;
	second->cooldown_until = cd;
	pthread_cond_broadcast(&second->cond);
	pthread_cond_broadcast(&first->cond);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}
