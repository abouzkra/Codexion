/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 16:32:18 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/20 11:26:02 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_is_over(t_data *data)
{
	int	state;

	pthread_mutex_lock(&data->sim_mut);
	state = data->sim_state;
	pthread_mutex_unlock(&data->sim_mut);
	return (state == OVER);
}

void	log_state(t_data *data, int coder_id, char *msg)
{
	pthread_mutex_lock(&data->logger_mut);
	if (!sim_is_over(data))
		printf("%ld %d %s\n",
			get_time_in_ms() - data->start_time,
			coder_id,
			msg
			);
	pthread_mutex_unlock(&data->logger_mut);
}

static void	set_sim_state(t_data *data, enum e_sim_state state)
{
	pthread_mutex_lock(&data->sim_mut);
	data->sim_state = state;
	pthread_cond_broadcast(&data->sim_cond);
	pthread_mutex_unlock(&data->sim_mut);
}

int	start_sim(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_coders)
	{
		if (pthread_create(&data->coders[i].th_id, NULL, coder_routine,
				(void *)&data->coders[i]) != 0)
		{
			pthread_mutex_lock(&data->sim_mut);
			data->sim_state = OVER;
			data->n_coders = i;
			pthread_cond_broadcast(&data->sim_cond);
			pthread_mutex_unlock(&data->sim_mut);
			return (0);
		}
		i++;
	}
	if (pthread_create(&data->monitor_th,
			NULL, monitor_routine, (void *)data) != 0)
	{
		set_sim_state(data, OVER);
		return (0);
	}
	data->t.monitor_thread = 1;
	set_sim_state(data, STARTED);
	return (1);
}

void	end_sim(t_data *data)
{
	int	i;

	if (data->t.monitor_thread)
		pthread_join(data->monitor_th, NULL);
	i = 0;
	while (i < data->n_coders)
	{
		pthread_join(data->coders[i].th_id, NULL);
		i++;
	}
}
