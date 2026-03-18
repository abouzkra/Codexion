/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 10:03:06 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/17 00:38:28 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->sim_mutex);
		return (0);
	}
	return (1);
}

static int	init_dongle(t_dongle *d)
{
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
		return (0);
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mutex);
		return (0);
	}
	d->held_by = -1;
	d->cooldown_timestamp = 0;
	d->queue_size = 0;
	memset(d->queue, 0, sizeof(t_waiter) * 2);
	return (1);
}

static void	init_dongles(t_data *data)
{
	int			i;

	data->dongles = (t_dongle *)malloc(sizeof(t_dongle) * data->number_of_coders);
	if (!data->dongles)
		return ;
	i = 0;
	while (i < data->number_of_coders)
	{
		if (!init_dongle(data->dongles + i))
		{
			while (--i >= 0)
			{
				pthread_mutex_destroy(&data->dongles[i].mutex);
				pthread_cond_destroy(&data->dongles[i].cond);
			}
			free(data->dongles);
			data->dongles = NULL;
		}
		i++;
	}
}

static void	init_coders(t_data *data)
{
	int	i;
	int	n;

	data->coders = (t_coder *)malloc(sizeof(t_coder) * data->number_of_coders);
	if (!data->coders)
		return ;
	n = data->number_of_coders;
	i = 0;
	while (i < data->number_of_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].compile_count = 0;
		data->coders[i].l_dongle = data->dongles + i;
		data->coders[i].r_dongle = data->dongles + (i + 1) % n;
		data->coders[i].last_compile_start = data->start_time;
		data->coders[i].data = data;
		i++;
	}
}

int	init_args(t_data *data)
{
	data->sim_over = 0;
	data->start_time = get_time_in_ms();
	if (!init_mutexes(data))
		return (0);
	init_dongles(data);
	if (!data->dongles)
		return (0);
	init_coders(data);
	if (!data->coders)
	{
		cleanup(data);
		return (0);
	}
	return (1);
}
