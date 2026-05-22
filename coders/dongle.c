/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 19:19:30 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/22 00:12:42 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_available(t_dongle *dongle)
{
	return (dongle->held_by == -1 && dongle->cooldown_ts <= get_time_in_ms());
}

static void	wait_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	timeout;

	if (is_top(coder, dongle) && dongle->held_by == -1
		&& dongle->cooldown_ts > get_time_in_ms())
	{
		timeout = ms_to_ts(dongle->cooldown_ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->mut, &timeout);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mut);
}

int	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_data	*data;

	data = (t_data *)coder->data;
	pthread_mutex_lock(&data->sim_mut);
	coder->arrival_time = get_time_in_ms();
	coder->deadline = coder->last_compile + data->t_burnout;
	pthread_mutex_unlock(&data->sim_mut);
	pthread_mutex_lock(&dongle->mut);
	insert(dongle, coder);
	while (!is_top(coder, dongle) || !is_available(dongle))
	{
		if (sim_is_over(data))
		{
			pop(dongle);
			pthread_mutex_unlock(&dongle->mut);
			return (0);
		}
		wait_dongle(coder, dongle);
	}
	pop(dongle);
	dongle->held_by = coder->id;
	pthread_mutex_unlock(&dongle->mut);
	return (1);
}

void	release_dongle(t_dongle *dongle, long cooldown)
{
	pthread_mutex_lock(&dongle->mut);
	dongle->held_by = -1;
	dongle->cooldown_ts = get_time_in_ms() + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mut);
}

void	broadcast_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->n_coders)
	{
		pthread_mutex_lock(&(data->dongles[i].mut));
		pthread_cond_broadcast(&(data->dongles[i].cond));
		pthread_mutex_unlock(&(data->dongles[i].mut));
		i++;
	}
}
