/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:21:37 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/21 23:47:46 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cleanup(t_data *data)
{
	if (data && data->dongles)
		free(data->dongles);
	if (data && data->coders)
		free(data->coders);
	if (data)
		free(data);
}

int	main(int ac, char *av[])
{
	t_data	*data;

	data = parse_args(ac, av);
	if (!data)
	{
		cleanup(data);
		return (1);
	}
	if (!init_sim(data))
	{
		cleanup(data);
		return (1);
	}
	start_sim(data);
	end_sim(data);
	cleanup(data);
	return (0);
}
