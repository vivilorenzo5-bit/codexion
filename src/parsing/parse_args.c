/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlourenc <vlourenc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 10:57:18 by vlourenc          #+#    #+#             */
/*   Updated: 2026/09/22 11:07:57 by vlourenc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Converte uma string num inteiro positivo (long long defensivo).
** Rejeita caracteres inválidos, valores negativos e previne overflow de int.
*/
static long long	ft_parse_positive_int(const char *str)
{
	long long	res;
	int			i;

	res = 0;
	i = 0;
	if (!str || !str[0])
		return (-1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		res = (res * 10) + (str[i] - '0');
		if (res > 2147483647)
			return (-1);
		i++;
	}
	return (res);
}

/*
** Valida e atribui a política de escalonamento.
** Aceita estritamente "fifo" ou "edf".
*/
static int	parse_scheduler(const char *str, t_sched_type *scheduler)
{
	if (!str)
		return (-1);
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = SCHED_FIFO;
		return (0);
	}
	if (strcmp(str, "edf") == 0)
	{
		*scheduler = SCHED_EDF;
		return (0);
	}
	return (-1);
}

/*
** Valida a contagem e os valores dos 8 argumentos da linha de comandos.
** Retorna 0 se os parâmetros forem válidos, ou 1 se houver qualquer erro.
*/
int	parse_args(int argc, char **argv, t_simulation *sim)
{
	if (argc != 9)
		return (1);
	sim->num_coders = (int)ft_parse_positive_int(argv[1]);
	sim->time_to_burnout = ft_parse_positive_int(argv[2]);
	sim->time_to_compile = ft_parse_positive_int(argv[3]);
	sim->time_to_burnout = ft_parse_positive_int(argv[4]);
	sim->time_to_refactor = ft_parse_positive_int(argv[5]);
	sim->compiles_required = (int)ft_parse_positive_int(argv[6]);
	sim->dongle_cooldown = ft_parse_positive_int(argv[7]);
	if (sim->num_coders <= 0 || sim->time_to_burnout <= 0
		|| sim->time_to_compile <= 0 || sim->time_to_debug <= 0
		|| sim->time_to_refactor <= 0 || sim->compiles_required < 0
		|| sim->dongle_cooldown < 0)
		return (1);
	if (parse_scheduler(argv[8], &sim->scheduler) != 0)
		return (1);
	return (0);
}
