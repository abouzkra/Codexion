/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 08:15:41 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/26 09:39:02 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_state(t_data *data, int coder_id, char *action)
{
	long	timestamp;

	timestamp = get_time_in_ms() - data->start_time;
	pthread_mutex_lock(&data->log_mutex);
	printf("%ld %d %s\n", timestamp, coder_id, action);
	pthread_mutex_unlock(&data->log_mutex);
}
