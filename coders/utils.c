/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 18:13:31 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/24 14:05:45 by abouzkra         ###   ########.fr       */
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
	long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < ms)
	{
		if (!sim_is_over(data))
			break ;
		usleep(500);
	}
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

static int	get_sign(char c, size_t *i)
{
	if (c == '-' || c == '+')
	{
		*i += 1;
		if (c == '-')
			return (-1);
	}
	return (1);
}

int	ft_atoi(const char *nptr)
{
	long	res;
	int		sign;
	int		digit;
	size_t	i;

	i = 0;
	while (nptr[i] == ' ' || (nptr[i] <= 13 && nptr[i] >= 9))
		i++;
	sign = get_sign(nptr[i], &i);
	if (nptr[i] == '\0')
		return (-1);
	res = 0;
	while (nptr[i])
	{
		if (nptr[i] < '0' || nptr[i] > '9')
			return (-1);
		digit = nptr[i] - '0';
		if ((sign == 1 && res > (2147483647 - digit) / 10)
			|| (sign == -1 && -(res) < (-2147483648 + digit) / 10))
			return (-1);
		res = res * 10 + digit;
		i++;
	}
	return ((int)(sign * res));
}
