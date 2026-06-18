/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 18:21:07 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/18 20:31:16 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_priority(t_coder *c1, t_coder *c2)
{
	enum e_scheduler	sched;

	sched = ((t_data *)c1->data)->scheduler;
	if (sched == FIFO && c1->arrival_time != c2->arrival_time)
		return (c1->arrival_time < c2->arrival_time);
	else if (sched == EDF && c1->last_compile != c2->last_compile)
		return (c1->last_compile < c2->last_compile);
	else if (c1->compile_count != c2->compile_count)
		return (c1->compile_count < c2->compile_count);
	return (c1->id < c2->id);
}

int	is_top(t_coder *coder, t_dongle *dongle)
{
	return (dongle->queue_size > 0 && coder == dongle->queue[0]);
}

int	compile(t_coder *coder)
{
	if (!acquire_dongles(coder))
		return (0);
	log_state((t_data *)coder->data, coder->id, "has taken a dongle");
	log_state((t_data *)coder->data, coder->id, "has taken a dongle");
	pthread_mutex_lock(&((t_data *)coder->data)->sim_mut);
	coder->last_compile = get_time_in_ms();
	coder->deadline = coder->last_compile + coder->data->t_burnout;
	pthread_mutex_unlock(&((t_data *)coder->data)->sim_mut);
	log_state((t_data *)coder->data, coder->id, "is compiling");
	coder_sleep(((t_data *)coder->data)->t_compile);
	pthread_mutex_lock(&((t_data *)coder->data)->sim_mut);
	coder->compile_count++;
	pthread_mutex_unlock(&((t_data *)coder->data)->sim_mut);
	release_dongle(coder->first_dongle, ((t_data *)coder->data)->cooldown);
	release_dongle(coder->second_dongle, ((t_data *)coder->data)->cooldown);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = (t_data *)coder->data; // check if simulation started
	if (coder->id % 2 == 0)
		coder_sleep((data->t_compile + data->cooldown) / 4);
	while (!sim_is_over(data))
	{
		if (!compile(coder))
			break ;
		if (sim_is_over(data))
			break ;
		log_state(data, coder->id, "is debugging");
		coder_sleep(data->t_debug);
		if (sim_is_over(data))
			break ;
		log_state(data, coder->id, "is refactoring");
		coder_sleep(data->t_refactor);
	}
	return (NULL);
}
