/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 18:13:31 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/21 22:14:11 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)(tv.tv_sec * 1000) + (long)(tv.tv_usec / 1000));
}

void	coder_sleep(long ms)
{
	usleep((useconds_t)(ms * 1000));
}

struct timespec	ms_to_ts(long ms)
{
	return ((struct timespec){
		.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000
	});
}
