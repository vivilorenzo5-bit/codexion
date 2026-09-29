/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 11:55:25 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/29 12:12:19 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_threads(t_simulation *sim, pthread_t *monitor_thread)
{
	int	i;

	sim->start_time = get_current_time_ms();
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].coder_muted);
		sim->coders[i].last_compile_start = sim->start_time;
		pthread_mutex_unlock(&sim->coders[i].coder_muted);
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			return (1);
		i++;
	}
	if (pthread_create(monitor_thread, NULL, monitor_routine, sim) != 0)
		return (1);
	return (0);
}

static void	join_all_threads(t_simulation *sim, pthread_t monitor_thread)
{
	int	i;

	pthread_join(monitor_thread, NULL);
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		monitor_thread;

	memset(&sim, 0, sizeof(t_simulation));
	if (parse_args(argc, argv, &sim) != 0)
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (init_simulation(&sim) != 0)
	{
		cleanup_simulation(&sim);
		return (1);
	}
	if (start_threads(&sim, &monitor_thread) != 0)
	{
		stop_simulation(&sim);
		cleanup_simulation(&sim);
		return (1);
	}
	join_all_threads(&sim, monitor_thread);
	cleanup_simulation(&sim);
	return (0);
}
