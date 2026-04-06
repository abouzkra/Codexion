/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 12:16:17 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/06 10:37:48 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	burnout_check(t_data *data)
{
	int		i;
	long	time_since_lc;

	i = 0;
	while (i < data->number_of_coders)
	{
		time_since_lc = get_time_in_ms() - data->coders[i].last_compile_start;
		if (time_since_lc >= data->time_to_burnout)
			return (data->coders[i].id);
		i++;
	}
	return (-1);
}

static int	all_done(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		if (data->coders[i].compile_count < data->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	check_sim_state(t_data *data)
{
	int	burned_id;

	pthread_mutex_lock(&data->sim_mutex);
	if (data->sim_over)
	{
		pthread_mutex_unlock(&data->sim_mutex);
		return (1);
	}
	burned_id = burnout_check(data);
	if (burned_id != -1 || all_done(data))
	{
		data->sim_over = 1;
		pthread_mutex_unlock(&data->sim_mutex);
		if (burned_id != -1)
			log_state(data, burned_id, "burned out");
		broadcast_all_dongles(data);
		return (1);
	}
	pthread_mutex_unlock(&data->sim_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (!check_sim_state(data))
		usleep(1000);
	return (NULL);
}
