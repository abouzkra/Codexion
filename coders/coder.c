/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 08:39:49 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/06 11:49:21 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_coder(t_data *data, int i)
{
	int	n;

	n = data->number_of_coders;
	data->coders[i].id = i + 1;
	data->coders[i].compile_count = 0;
	data->coders[i].last_compile_start = data->start_time;
	data->coders[i].data = data;
	if ((i + 1) % 2 != 0)
	{
		data->coders[i].first_dongle = data->dongles + i;
		data->coders[i].second_dongle = data->dongles + (i + 1) % n;
	}
	else
	{
		data->coders[i].first_dongle = data->dongles + (i + 1) % n;
		data->coders[i].second_dongle = data->dongles + i;
	}
}

static int	try_compile(t_coder *coder, t_data *data)
{
	if (!acquire_dongle(coder, coder->first_dongle))
		return (0);
	log_state(data, coder->id, "has taken a dongle");
	if (!acquire_dongle(coder, coder->second_dongle))
	{
		release_dongle(coder->first_dongle, data->dongle_cooldown);
		return (-1);
	}
	log_state(data, coder->id, "has taken a dongle");
	pthread_mutex_lock(&data->sim_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->compile_count++;
	pthread_mutex_unlock(&data->sim_mutex);
	log_state(data, coder->id, "is compiling");
	coder_sleep(data->time_to_compile);
	release_dongle(coder->first_dongle, data->dongle_cooldown);
	release_dongle(coder->second_dongle, data->dongle_cooldown);
	return (1);
}

static int	compile(t_coder *coder, t_data *data)
{
	int	res;

	res = -1;
	while (res == -1)
	{
		if (sim_is_over(data))
			return (0);
		res = try_compile(coder, data);
	}
	return (res);
}

// static int	compile(t_coder *coder, t_data *data)
// {
// 	if (!acquire_dongle(coder, coder->first_dongle))
// 		return (0);
// 	log_state(data, coder->id, "has taken a dongle");
// 	if (!acquire_dongle(coder, coder->second_dongle))
// 	{
// 		release_dongle(coder->first_dongle, data->dongle_cooldown);
// 		return (0);
// 	}
// 	log_state(data, coder->id, "has taken a dongle");
// 	pthread_mutex_lock(&data->sim_mutex);
// 	coder->last_compile_start = get_time_in_ms();
// 	coder->compile_count++;
// 	pthread_mutex_unlock(&data->sim_mutex);
// 	log_state(data, coder->id, "is compiling");
// 	coder_sleep(data->time_to_compile);
// 	release_dongle(coder->first_dongle, data->dongle_cooldown);
// 	release_dongle(coder->second_dongle, data->dongle_cooldown);
// 	return (1);
// }

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	pthread_mutex_lock(&data->sim_mutex);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&data->sim_mutex);
	while (!sim_is_over(data))
	{
		if (!compile(coder, data))
			break ;
		if (sim_is_over(data))
			break ;
		log_state(data, coder->id, "is debugging");
		coder_sleep(data->time_to_debug);
		if (sim_is_over(data))
			break ;
		log_state(data, coder->id, "is refactoring");
		coder_sleep(data->time_to_refactor);
	}
	return (NULL);
}
