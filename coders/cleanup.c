/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 00:15:13 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/22 07:45:51 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup(t_data *data)
{
	int	i;

	if (!data)
		return ;
	if (data->dongles)
	{
		i = 0;
		while (i < data->number_of_coders)
		{
			pthread_mutex_destroy(&data->dongles[i].mutex);
			pthread_cond_destroy(&data->dongles[i].cond);
			i++;
		}
		free(data->dongles);
	}
	free(data->coders);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->log_mutex);
	free(data);
}
