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
		destroy_mutex(&data->dongles[i].mut, 1);
		destroy_cond(&data->dongles[i].cond, i < data->t.dongle_cond_init);
		i++;
	}
}

static void	cleanup(t_data *data)
{
	if (!data)
		return ;
	destroy_dongles(data);
	destroy_mutex(&data->sim_mut, data->t.sim_mut_init);
	destroy_cond(&data->sim_cond, data->t.sim_cond_init);
	destroy_mutex(&data->logger_mut, data->t.logger_mut_init);
	destroy_cond(&data->sleep_cond, data->t.sleep_cond_init);
	if (data->dongles)
		free(data->dongles);
	if (data->coders)
		free(data->coders);
	free(data);
}

int g_spawned_threads = 0;
int active_mutexes;
int active_conds;

void check_resource_cleanup(void)
{
  if (active_mutexes == 0)
    printf("SUCCESS: No mutexes were leaked!\n");
  else
    printf("FAILURE: You've leaked %d mutexes!\n", active_mutexes);
  if (active_conds == 0)
    printf("SUCCESS: No conds were leaked!\n");
  else
    printf("FAILURE: You've leaked %d conds!\n", active_conds);
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
	else if (!start_sim(data))
		exit_status = 2;
	if (exit_status != 1)
		end_sim(data);
	printf("%d\n", g_spawned_threads);
	cleanup(data);
	check_resource_cleanup();
	return (exit_status);
}
