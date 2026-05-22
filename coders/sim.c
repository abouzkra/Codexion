/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 16:32:18 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/21 23:22:35 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_is_over(t_data *data)
{
	int	over;

	pthread_mutex_lock(&data->sim_mut);
	over = data->sim_over;
	pthread_mutex_unlock(&data->sim_mut);
	return (over);
}

void	log_state(t_data *data, int coder_id, char *msg)
{
	if (sim_is_over(data))
		return ;
	pthread_mutex_lock(&data->logger_mut);
	printf("%ld %d %s\n", get_time_in_ms() - data->start_time, coder_id, msg);
	pthread_mutex_unlock(&data->logger_mut);
}

void	start_sim(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_coders)
	{
		if (pthread_create(&data->coders[i].th_id, NULL, coder_routine,
				(void *)&data->coders[i]) != 0)
		{
			pthread_mutex_lock(&data->sim_mut);
			data->sim_over = 1;
			pthread_mutex_unlock(&data->sim_mut);
			data->n_coders = i;
			return ;
		}
		i++;
	}
	if (pthread_create(&data->monitor_th,
			NULL, monitor_routine, (void *)data) != 0)
	{
		pthread_mutex_lock(&data->sim_mut);
		data->sim_over = 1;
		pthread_mutex_unlock(&data->sim_mut);
	}
}

void	end_sim(t_data *data)
{
	int	i;

	if (data->monitor_th)
		pthread_join(data->monitor_th, NULL);
	i = 0;
	while (i < data->n_coders)
	{
		if (pthread_join(data->coders[i].th_id, NULL) != 0)
			return ;
		i++;
	}
}
