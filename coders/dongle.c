/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 19:19:30 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/19 18:15:52 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

static int	is_available(t_dongle *dongle)
{
	return (dongle->held_by == -1 && dongle->cooldown_ts <= get_time_in_ms());
}

static int	try_acquire_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->first_dongle->mut);
	if (!is_top(coder, coder->first_dongle)
		|| !is_available(coder->first_dongle))
	{
		pthread_mutex_unlock(&coder->first_dongle->mut);
		return (0);
	}
	pthread_mutex_lock(&coder->second_dongle->mut);
	if (is_top(coder, coder->second_dongle)
		&& is_available(coder->second_dongle))
	{
		coder->first_dongle->held_by = coder->id;
		coder->second_dongle->held_by = coder->id;
		pop(coder->first_dongle);
		pop(coder->second_dongle);
		pthread_mutex_unlock(&coder->first_dongle->mut);
		pthread_mutex_unlock(&coder->second_dongle->mut);
		return (1);
	}
	pthread_mutex_unlock(&coder->first_dongle->mut);
	pthread_mutex_unlock(&coder->second_dongle->mut);
	return (0);
}

int	acquire_dongles(t_coder *coder)
{
	if (coder->first_dongle == coder->second_dongle)
	{
		coder_sleep(coder->data, coder->data->t_burnout);
		return (0);
	}
	pthread_mutex_lock(&coder->first_dongle->mut);
	pthread_mutex_lock(&coder->second_dongle->mut);
	insert(coder->first_dongle, coder);
	insert(coder->second_dongle, coder);
	pthread_mutex_unlock(&coder->first_dongle->mut);
	pthread_mutex_unlock(&coder->second_dongle->mut);
	while (!sim_is_over(coder->data))
	{
		if (try_acquire_dongles(coder))
			return (1);
		usleep(500);
	}
	return (0);
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
