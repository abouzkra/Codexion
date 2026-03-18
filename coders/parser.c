/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:59:29 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/17 00:22:21 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

static int	validate_nbr(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	validate_args(char **av)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (!validate_nbr(av[i]))
			return (0);
		i++;
	}
	return (strcmp("fifo", av[8]) == 0 || strcmp("edf", av[8]) == 0);
}

static void	convert_args(char **av, t_data *data)
{
	data->number_of_coders = atoi(av[1]);
	data->time_to_burnout = atoi(av[2]);
	data->time_to_compile = atoi(av[3]);
	data->time_to_debug = atoi(av[4]);
	data->time_to_refactor = atoi(av[5]);
	data->number_of_compiles_required = atoi(av[6]);
	data->dongle_cooldown = atoi(av[7]);
	if (strcmp("fifo", av[8]) == 0)
		data->scheduler = FIFO;
	else if (strcmp("edf", av[8]) == 0)
		data->scheduler = EDF;
	data->dongles = NULL;
	data->coders = NULL;
}

t_data	*parse_args(int ac, char **av)
{
	t_data	*data;

	if (ac != 9)
		return (NULL);
	if (!validate_args(av))
		return (NULL);
	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
		return (NULL);
	memset(data, 0, sizeof(t_data));
	convert_args(av, data);
	if (!init_args(data))
	{
		perror("Error while initializing args\n");
		return (NULL);
	}
	return (data);
}
