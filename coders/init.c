/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 17:57:18 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/24 15:10:13 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->sim_mut, NULL) != 0)
		return (0);
	data->t.sim_mut_init = 1;
	if (pthread_mutex_init(&data->logger_mut, NULL) != 0)
		return (0);
	data->t.logger_mut_init = 1;
	return (1);
}

static int	init_dongles(t_data *data)
{
	t_dongle	*dongle;
	int			i;

	data->dongles = (t_dongle *)malloc(sizeof(t_dongle) * data->n_coders);
	if (!data->dongles)
		return (0);
	memset(data->dongles, 0, sizeof(t_dongle) * data->n_coders);
	i = 0;
	while (i < data->n_coders)
	{
		dongle = data->dongles + i;
		if (pthread_mutex_init(&dongle->mut, NULL) != 0)
			return (0);
		data->t.dongle_mut_init++;
		dongle->held_by = -1;
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
		data->coders[i].data = data;
		i++;
	}
	data->t.spawned_coders = data->n_coders;
	return (1);
}

int	init_sim(t_data *data)
{
	data->sim_state = -1;
	data->start_time = get_time_in_ms();
	if (!init_mutexes(data) || !init_dongles(data) || !init_coders(data))
		return (0);
	if (pthread_cond_init(&data->sim_cond, NULL) != 0)
		return (0);
	data->t.sim_cond_init = 1;
	return (1);
}
