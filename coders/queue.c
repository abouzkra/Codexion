/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 12:40:39 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/11 15:15:18 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_priority(t_waiter w1, t_waiter w2, enum e_scheduler sched)
{
	if (sched == FIFO)
		return (w1.arrival_time < w2.arrival_time);
	return (w1.deadline < w2.deadline);
}

void	enqueue(t_waiter w, t_dongle *dongle, enum e_scheduler sched)
{
	if (dongle->queue_size == 0)
	{
		dongle->queue[0] = w;
		dongle->queue_size = 1;
		return ;
	}
	if (has_priority(w, dongle->queue[0], sched))
	{
		dongle->queue[1] = dongle->queue[0];
		dongle->queue[0] = w;
	}
	else
		dongle->queue[1] = w;
	dongle->queue_size = 2;
}

void	dequeue(t_dongle *dongle)
{
	if (dongle->queue_size == 0)
		return ;
	dongle->queue[0] = dongle->queue[1];
	dongle->queue_size--;
}

int	is_head(t_dongle *dongle, int coder_id)
{
	return (dongle->queue_size > 0 && dongle->queue[0].coder == coder_id);
}
