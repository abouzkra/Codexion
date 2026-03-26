/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 07:08:13 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/22 08:28:35 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	run_coders(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_create(&data->coders[i].th_id, NULL, &coder,
			(void *)data) != 0)
			return (i);
		i++;
	}
	if (pthread_create(&data->monitor_th, NULL, &monitor,
		(void *)data) != 0)
		return (data->number_of_coders);
	if (pthread_create(&data->logger_th, NULL, &logger,
		(void *)data) != 0)
		return (data->number_of_coders);
	return (data->number_of_coders + 2);
}

int	join_coders(t_data *data)
{
	int	i;

	if (pthread_join(data->logger_th, NULL) != 0)
		return (0);
	if (pthread_join(data->monitor_th, NULL) != 0)
		return (0);
	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_join(data->coders[i].th_id, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}
