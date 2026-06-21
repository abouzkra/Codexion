/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:21:37 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/21 11:25:19 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_mutex(pthread_mutex_t *mut, int flag)
{
	if (flag)
		pthread_mutex_destroy(mut);
}

static void	destroy_cond(pthread_cond_t *cond, int flag)
{
	if (flag)
		pthread_cond_destroy(cond);
}

static void	destroy_dongles(t_data *data)
{
	int	i;

	if (!data || !data->dongles)
		return ;
	i = 0;
	while (i < data->t.dongle_mut_init)
	{
		destroy_mutex(&data->dongles->mut, 1);
		destroy_cond(&data->dongles->cond, i < data->t.dongle_cond_init);
		i++;
	}
}

static void	cleanup(t_data *data)
{
	if (!data)
		return ;
	if (data->dongles)
		free(data->dongles);
	destroy_dongles(data);
	if (data->coders)
		free(data->coders);
	destroy_mutex(&data->sim_mut, data->t.sim_mut_init);
	destroy_cond(&data->sim_cond, data->t.sim_cond_init);
	destroy_mutex(&data->logger_mut, data->t.logger_mut_init);
	destroy_cond(&data->sleep_cond, data->t.sleep_cond_init);
	free(data);
}

int	main(int ac, char *av[])
{
	t_data	*data;
	int		exit_status;

	exit_status = 0;
	data = parse_args(ac, av);
	if (!data)
		return (1);
	if (!init_sim(data))
		exit_status = 1;
	if (!start_sim(data))
		exit_status = 1;
	end_sim(data);
	cleanup(data);
	return (exit_status);
}
