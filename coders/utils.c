/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 18:13:31 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/21 11:26:33 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)(tv.tv_sec * 1000) + (long)(tv.tv_usec / 1000));
}

void	coder_sleep(t_data *data, long ms)
{
	struct timespec	ts;
	long			target_ms;

	target_ms = get_time_in_ms() + ms;
	ts.tv_sec = target_ms / 1000;
	ts.tv_nsec = (target_ms % 1000) * 1000000;
	pthread_mutex_lock(&data->sim_mut);
	while (data->sim_state != OVER && get_time_in_ms() < target_ms)
		pthread_cond_timedwait(&data->sleep_cond, &data->sim_mut, &ts);
	pthread_mutex_unlock(&data->sim_mut);
}

int	all_threads_started(t_data *data)
{
	pthread_mutex_lock(&data->sim_mut);
	while (data->sim_state != STARTED && data->sim_state != OVER)
		pthread_cond_wait(&data->sim_cond, &data->sim_mut);
	if (data->sim_state == OVER)
	{
		pthread_mutex_unlock(&data->sim_mut);
		return (0);
	}
	pthread_mutex_unlock(&data->sim_mut);
	return (1);
}
