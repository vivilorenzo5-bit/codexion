/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 10:22:46 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/30 12:04:40 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

/*
** Destroi os mutexes individuais de cada coder e liberta o array de coders.
*/
static void	cleanup_coders(t_simulation *sim)
{
	int	i;

	if (!sim->coders)
		return ;
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].coder_muted);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

/*
** Destroi mutexes, variaveis de condicao e heaps dos dongles.
*/
static void	cleanup_dongles(t_simulation *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_destroy(&sim->dongles[i].wait_queue);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

/*
** Destroi as primitivas de sincronizacao globais.
*/
static void	cleanup_globals(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->finish_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
}

/*
** Funcao principal de encerramento: liberta todos os recursos do programa.
*/
void	cleanup_simulation(t_simulation *sim)
{
	if (!sim)
		return ;
	cleanup_coders(sim);
	cleanup_dongles(sim);
	cleanup_globals(sim);
}
