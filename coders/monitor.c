/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 22:14:28 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/18 20:34:19 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_finished(t_data *data)
{
	int		i;

	i = 0;
	pthread_mutex_lock(&data->sim_mut);
	while (i < data->n_coders)
	{
		if (data->coders[i].compile_count < data->n_compiles)
		{
			pthread_mutex_unlock(&data->sim_mut);
			return (0);
		}
		i++;
	}
	pthread_mutex_unlock(&data->sim_mut);
	return (1);
}

static int	burnout_check(t_data *data)
{
	int		i;
	long	time_since_lc;

	i = 0;
	pthread_mutex_lock(&data->sim_mut);
	while (i < data->n_coders)
	{
		time_since_lc = get_time_in_ms() - data->coders[i].last_compile;
		if (time_since_lc >= data->t_burnout)
		{
			pthread_mutex_unlock(&data->sim_mut);
			return (i + 1);
		}
		i++;
	}
	pthread_mutex_unlock(&data->sim_mut);
	return (-1);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		burned_out;

	data = (t_data *)arg;
	while (1)
	{
		burned_out = burnout_check(data);
		if (burned_out > -1 || all_finished(data))
		{
			pthread_mutex_lock(&data->sim_mut);
			data->sim_over = 1;
			pthread_mutex_unlock(&data->sim_mut);
			pthread_mutex_lock(&data->logger_mut);
			if (burned_out > -1)
				printf("%ld %d %s\n",
					get_time_in_ms() - data->start_time, burned_out,
					"burned out");
			pthread_mutex_unlock(&data->logger_mut);
			broadcast_dongles(data);
			break ;
		}
		usleep(500);
	}
	return (NULL);
}
