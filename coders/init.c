/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:57:18 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/21 23:57:53 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->sim_mut, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->logger_mut, NULL) != 0)
	{
		pthread_mutex_destroy(&data->sim_mut);
		return (0);
	}
	return (1);
}

static int	init_dongle(t_dongle *d)
{
	if (pthread_mutex_init(&d->mut, NULL) != 0)
		return (0);
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mut);
		return (0);
	}
	d->held_by = -1;
	d->cooldown_ts = 0;
	d->queue_size = 0;
	memset(d->queue, 0, sizeof(d->queue));
	return (1);
}

static int	init_dongles(t_data *data)
{
	int	i;

	data->dongles = (t_dongle *)malloc(sizeof(t_dongle) * data->n_coders);
	if (!data->dongles)
		return (0);
	memset(data->dongles, 0, sizeof(t_dongle) * data->n_coders);
	i = 0;
	while (i < data->n_coders)
	{
		if (!init_dongle(data->dongles + i))
		{
			while (--i >= 0)
			{
				pthread_mutex_destroy(&data->dongles[i].mut);
				pthread_cond_destroy(&data->dongles[i].cond);
			}
			free(data->dongles);
			data->dongles = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}

static int	init_coders(t_data *data)
{
	int	i;

	data->coders = (t_coder *)malloc(sizeof(t_coder) * data->n_coders);
	if (!data->coders)
		return (0);
	memset(data->coders, 0, sizeof(t_coder) * data->n_coders);
	i = 0;
	while (i < data->n_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].first_dongle = data->dongles + i;
		data->coders[i].second_dongle = data->dongles
			+ (i + 1) % data->n_coders;
		if (i % 2 != 0)
		{
			data->coders[i].first_dongle = data->dongles
				+ (i + 1) % data->n_coders;
			data->coders[i].second_dongle = data->dongles + i;
		}
		data->coders[i].last_compile = data->start_time;
		data->coders[i].data = (void *)data;
		i++;
	}
	return (1);
}

int	init_sim(t_data *data)
{
	data->sim_over = 0;
	data->start_time = get_time_in_ms();
	return (
		init_mutexes(data)
		&& init_dongles(data)
		&& init_coders(data)
	);
}
