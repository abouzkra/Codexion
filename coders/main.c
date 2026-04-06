/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:59:40 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/05 16:59:40 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_data	*data;

	data = parse_args(ac, av);
	if (!data)
		return (1);
	run_threads(data);
	join_threads(data);
	cleanup(data);
	return (0);
}
