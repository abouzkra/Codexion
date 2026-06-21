/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:47:18 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/21 12:45:58 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	convert_args(char *av[], t_data *data)
{
	data->n_coders = ft_atoi(av[1]);
	data->t_burnout = ft_atoi(av[2]);
	data->t_compile = ft_atoi(av[3]);
	data->t_debug = ft_atoi(av[4]);
	data->t_refactor = ft_atoi(av[5]);
	data->n_compiles = ft_atoi(av[6]);
	data->cooldown = ft_atoi(av[7]);
	data->scheduler = -1;
	if (strcmp(av[8], "fifo") == 0)
		data->scheduler = FIFO;
	if (strcmp(av[8], "edf") == 0)
		data->scheduler = EDF;
}

static int	validate_args(t_data *data)
{
	return (data->n_coders >= 0
		&& data->t_burnout >= 0
		&& data->t_compile >= 0
		&& data->t_debug >= 0
		&& data->t_refactor >= 0
		&& data->n_compiles >= 1
		&& data->cooldown >= 0
		&& (data->scheduler == FIFO || data->scheduler == EDF)
	);
}

t_data	*parse_args(int ac, char *av[])
{
	t_data	*data;

	if (ac != 9)
		return (NULL);
	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
		return (NULL);
	memset(data, 0, sizeof(t_data));
	convert_args(av, data);
	if (!validate_args(data))
	{
		free(data);
		return (NULL);
	}
	return (data);
}
