/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 16:57:10 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/22 00:01:03 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap(t_coder **c1, t_coder **c2)
{
	t_coder	*tmp;

	tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}

static void	bubble_up(t_dongle *dg, int idx)
{
	int	parent;
	int	i;

	i = idx;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(dg->queue[i], dg->queue[parent]))
			swap(dg->queue + parent, dg->queue + i);
		else
			break ;
		i = parent;
	}
}

static void	bubble_down(t_dongle *dg, int idx)
{
	int	i;
	int	l_child;
	int	r_child;
	int	best_child;

	i = idx;
	while ((i * 2) + 1 < dg->queue_size)
	{
		l_child = i * 2 + 1;
		r_child = i * 2 + 2;
		best_child = l_child;
		if (r_child < dg->queue_size
			&& has_priority(dg->queue[r_child], dg->queue[l_child]))
			best_child = r_child;
		if (has_priority(dg->queue[best_child], dg->queue[i]))
		{
			swap(dg->queue + best_child, dg->queue + i);
			i = best_child;
		}
		else
			break ;
	}
}

void	insert(t_dongle *dg, t_coder *coder)
{
	if (dg->queue_size == 2)
		return ;
	dg->queue[dg->queue_size] = coder;
	dg->queue_size++;
	bubble_up(dg, dg->queue_size - 1);
}

int	pop(t_dongle *dg)
{
	int	top;

	if (dg->queue_size == 0)
		return (-1);
	top = dg->queue[0]->id;
	dg->queue[0] = dg->queue[dg->queue_size - 1];
	dg->queue_size--;
	if (dg->queue_size > 0)
		bubble_down(dg, 0);
	return (top);
}
