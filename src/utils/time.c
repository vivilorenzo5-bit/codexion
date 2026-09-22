/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 10:02:08 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/22 10:14:20 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Retorna o timestamp atual em milissegundos.
** Usamos gettimeofday conforme explicitamente recomendado no subject.
*/
long long	get_current_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

/*
** Adormece a thread durante 'ms' milissegundos com alta precisão.
** Fragmenta o sono em pequenos intervalos para:
** 1. Evitar atrasos excessivos do scheduler do SO.
** 2. Despertar imediatamente caso a simulação seja cancelada (morte de outro coder).
*/
void	precise_sleep(long long ms, t_simulation *sim)
{
	long long	start;

	start = get_current_time_ms();
	while (!is_simulation_over(sim))
	{
		if (get_current_time_ms() - start >= ms)
			break ;
		usleep(200);
	}
}
