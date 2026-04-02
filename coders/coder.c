/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 08:39:49 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/01 11:30:10 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	compile(t_coder *coder, t_data *data)
{
	if (!acquire_dongle(coder, coder->first_dongle))
		return (0);
	log_state(data, coder->id, "has taken a dongle");
	if (!acquire_dongle(coder, coder->second_dongle))
	{
		release_dongle(coder->first_dongle, data->dongle_cooldown);
		return (0);
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

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	pthread_mutex_lock(&data->sim_mutex);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&data->sim_mutex);
	while (coder->compile_count != data->number_of_compiles_required
		&& !sim_is_over(data))
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
