/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 07:08:13 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/01 09:42:28 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_is_over(t_data *data)
{
	int	over;

	pthread_mutex_lock(&data->sim_mutex);
	over = data->sim_over;
	pthread_mutex_unlock(&data->sim_mutex);
	return (over);
}

void	run_threads(t_data *data)
{
	int	i;

	if (pthread_create(&data->monitor_th, NULL, &monitor_routine,
			(void *)data) != 0)
		return ;
	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_create(&data->coders[i].th_id, NULL, &coder_routine,
				(void *)&data->coders[i]) != 0)
		{
			pthread_mutex_lock(&data->sim_mutex);
			data->sim_over = 1;
			pthread_mutex_unlock(&data->sim_mutex);
			broadcast_all_dongles(data);
			data->number_of_coders = i;
			return ;
		}
		i++;
	}
}

void	join_threads(t_data *data)
{
	int	i;

	if (pthread_join(data->monitor_th, NULL) != 0)
		return ;
	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_join(data->coders[i].th_id, NULL) != 0)
			return ;
		i++;
	}
}
