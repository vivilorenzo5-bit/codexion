/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 11:25:33 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 11:54:34 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Regista o evento fatal de burnout e sinaliza a paragem da simulacao.
*/
static void	report_burnout(t_coder *coder)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->sim->log_mutex);
	stop_simulation(coder->sim);
	timestamp = get_current_time_ms() - coder->sim->start_time;
	printf("%lld %d burned out\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}

/*
** Inspeciona se o prazo limite de time_to_burnout foi estritamente ultrapassado.
*/
static int	check_coder_burnout(t_coder *coder)
{
	long long	now;
	long long	last_compile;
	long long	time_since_compile;

	now = get_current_time_ms();
	pthread_mutex_lock(&coder->coder_muted);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->coder_muted);
	time_since_compile = now - last_compile;
	if (time_since_compile > coder->sim->time_to_burnout)
	{
		report_burnout(coder);
		return (1);
	}
	return (0);
}

/*
** Avalia se todos os coders da mesa ja atingiram a meta minima de compilacoes.
*/
static int	check_all_compiled(t_simulation *sim)
{
	int	i;
	int	done_count;

	if (sim->compiles_required <= 0)
		return (0);
	i = 0;
	done_count = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].coder_muted);
		if (sim->coders[i].compile_count >= sim->compiles_required)
			done_count++;
		pthread_mutex_unlock(&sim->coders[i].coder_muted);
		i++;
	}
	if (done_count == sim->num_coders)
	{
		stop_simulation(sim);
		return (1);
	}
	return (0);
}

/*
** Rotina independente da thread de monitorizacao.
*/
void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				i;

	sim = (t_simulation *)arg;
	while (!is_simulation_over(sim))
	{
		i = 0;
		while (i < sim->num_coders)
		{
			if (check_coder_burnout(&sim->coders[i]))
				return (NULL);
			i++;
		}
		if (check_all_compiled(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
