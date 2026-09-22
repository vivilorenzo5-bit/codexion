/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 10:20:05 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/22 10:40:37 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Verifica de forma thread-safe se a simulação já foi dada como encerrada.
*/
int	is_simulation_over(t_simulation *sim)
{
	int	over;

	pthread_mutex_lock(&sim->finish_mutex);
	over = sim->is_finished;
	pthread_mutex_unlock(&sim->finish_mutex);
	return (over);
}

/*
** Marca a simulação como terminada de forma atómica.
*/
void	stop_simulation(t_simulation *sim)
{
	pthread_mutex_lock(&sim->finish_mutex);
	sim->is_finished = 1;
	pthread_mutex_unlock(&sim->finish_mutex);
}

/*
** Imprime o estado do coder garantindo:
** 1. Exclusão mútua na saída padrão (stdout) via log_mutex.
** 2. Formatação exata exigida pelo subject.
** 3. Supressão de mensagens após o encerramento da simulação.
*/
void	log_state(t_coder *coder, const char *msg)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->sim->log_mutex);
	if (!is_simulation_over(coder->sim))
	{
		timestamp = get_current_time_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
