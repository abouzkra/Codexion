/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:59:40 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/22 08:38:00 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_data(t_data *data)
{
	printf("%i\n", data->number_of_coders);
	printf("%ld\n", data->time_to_burnout);
	printf("%ld\n", data->time_to_compile);
	printf("%ld\n", data->time_to_debug);
	printf("%ld\n", data->time_to_refactor);
	printf("%i\n", data->number_of_compiles_required);
	printf("%ld\n", data->dongle_cooldown);
	printf("%ld\n", data->start_time);
	printf("%s\n", (data->scheduler == FIFO) ? "fifo" : "edf");
}

int	main(int ac, char **av)
{
	t_data	*data;
	int		created_threads;

	data = parse_args(ac, av);
	if (!data)
		return (1);
	created_threads = run_coders(data);
	if (created_threads != data->number_of_coders + 2)
	{
		cleanup(data);
		return (1);
	}
	if (!join_coders(data))
		return (1);
	cleanup(data);
	return (0);
}
