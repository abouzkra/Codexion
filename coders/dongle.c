/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 10:10:29 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/06 11:50:36 by abouzkra         ###   ########.fr       */
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

static int	dongle_exit(t_dongle *dongle, int coder_id)
{
	if (is_head(dongle, coder_id))
		dequeue(dongle);
	else
		dongle->queue_size--;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static void	dongle_wait(t_dongle *dongle, int coder_id)
{
	struct timespec	timeout;

	if (is_head(dongle, coder_id) && dongle->held_by == -1
		&& dongle->cooldown_timestamp > get_time_in_ms())
	{
		timeout = ms_to_ts(dongle->cooldown_timestamp);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &timeout);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
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
			return (dongle_exit(dongle, coder->id));
		dongle_wait(dongle, coder->id);
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
