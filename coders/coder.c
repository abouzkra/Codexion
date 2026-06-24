/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 18:21:07 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/24 13:25:38 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_priority(t_coder *c1, t_coder *c2)
{
	enum e_scheduler	sched;

	sched = c1->data->scheduler;
	if (sched == FIFO)
		return (0);
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

static int	compile(t_coder *coder)
{
	if (!acquire_dongles(coder))
		return (0);
	pthread_mutex_lock(&coder->data->sim_mut);
	coder->last_compile = get_time_in_ms();
	pthread_mutex_unlock(&coder->data->sim_mut);
	log_state(coder->data, coder->id, "has taken a dongle");
	log_state(coder->data, coder->id, "has taken a dongle");
	log_state(coder->data, coder->id, "is compiling");
	coder_sleep(coder->data, coder->data->t_compile);
	pthread_mutex_lock(&coder->data->sim_mut);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->data->sim_mut);
	release_dongle(coder->first_dongle, coder->data->cooldown);
	release_dongle(coder->second_dongle, coder->data->cooldown);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (!all_threads_started(coder->data))
		return (NULL);
	if (coder->id % 2 == 0)
		coder_sleep(coder->data,
			(coder->data->t_compile + coder->data->cooldown) / 4);
	while (!sim_is_over(coder->data))
	{
		if (!compile(coder))
			break ;
		if (sim_is_over(coder->data))
			break ;
		log_state(coder->data, coder->id, "is debugging");
		coder_sleep(coder->data, coder->data->t_debug);
		if (sim_is_over(coder->data))
			break ;
		log_state(coder->data, coder->id, "is refactoring");
		coder_sleep(coder->data, coder->data->t_refactor);
	}
	return (NULL);
}
