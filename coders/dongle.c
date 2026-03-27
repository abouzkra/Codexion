/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 10:10:29 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/27 11:40:34 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongle_ready(t_dongle *dongle, int coder_id)
{
	return (
		dongle->held_by == -1
		&& get_time_in_ms() >= dongle->cooldown_timestamp
		&& is_head(dongle, coder_id)
	);
}

int	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_data		*data;
	t_waiter	coder_q;

	data = (t_data *) coder->data;
	coder_q.coder = coder->id;
	coder_q.arrival_time = get_time_in_ms();
	coder_q.deadline = coder->last_compile_start + data->time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	enqueue(coder_q, dongle, ((t_data *)coder->data)->scheduler);
	while (!dongle_ready(dongle, coder->id))
	{
		if (sim_is_over(data))
		{
			if (is_head(dongle, coder->id))
				dequeue(dongle);
			else
				dongle->queue_size--;
			pthread_mutex_unlock(&dongle->mutex);
			return (0);
		}
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dequeue(dongle);
	dongle->held_by = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

void	release_dongle(t_dongle *dongle, long cooldown)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->held_by = -1;
	dongle->cooldown_timestamp = get_time_in_ms() + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	broadcast_all_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_lock(&(data->dongles[i].mutex));
		pthread_cond_broadcast(&(data->dongles[i].cond));
		pthread_mutex_unlock(&(data->dongles[i].mutex));
		i++;
	}
}
