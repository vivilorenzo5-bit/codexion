/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 09:51:41 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/29 10:41:43 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Trata o caso especial em que existe apenas 1 coder e 1 dongle na simulacao.
** O coder apanha o dongle disponivel e aguarda ate o monitor registar o burnout.
*/
static void	handle_single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	log_state(coder, "has taken a dongle");
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	while (!is_simulation_over(coder->sim))
		usleep(1000);
}

/*
** Executa o ciclo de compilacao:
** 1. Atualiza a marca de tempo de inicio da compilacao e incrementa contador.
** 2. Regista o log "is compiling".
** 3. Adormece pelo tempo exato de compilacao.
** 4. Devolve ambos os dongles a mesa.
*/
static void	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_muted);
	coder->last_compile_start = get_current_time_ms();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->coder_muted);
	log_state(coder, "is compiling");
	precise_sleep(coder->sim->time_to_compile, coder->sim);
	drop_dongles(coder);
}

/*
** Executa as etapas de debugging e refactoring logo apos largar os recursos.
*/
static void	coder_debug_and_refactor(t_coder *coder)
{
	log_state(coder, "is debugging");
	precise_sleep(coder->sim->time_to_debug, coder->sim);
	log_state(coder, "is refactoring");
	precise_sleep(coder->sim->time_to_refactor, coder->sim);
}

/*
** Ciclo principal de execucao de cada thread de coder.
*/
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->num_coders == 1)
	{
		handle_single_coder(coder);
		return (NULL);
	}
	if (coder->id % 2 == 0)
		usleep(1000);
	while (!is_simulation_over(coder->sim))
	{
		take_dongles(coder);
		coder_compile(coder);
		coder_debug_and_refactor(coder);
	}
	return (NULL);
}
