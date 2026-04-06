/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 09:04:57 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/06 09:10:21 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongle(t_dongle *d)
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
	return (1);
}

void	broadcast_all_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_lock(&(data->dongles[i].mutex));
		pthread_cond_broadcast(&(data->dongles[i].cond));
		pthread_mutex_unlock(&(data->dongles[i].mutex));
		i++;
	}
}
