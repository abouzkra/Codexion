/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:58:14 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/09 17:31:34 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_atoi(const char *nptr)
{
	long long unsigned	res;
	int					digit;

	if (!nptr || !*nptr)
		return (-1);
	while (*nptr == ' ' || (*nptr <= 13 && *nptr >= 9))
		nptr++;
	if (*nptr == '+')
		nptr++;
	if (*nptr == '-')
		return (-1);
	res = 0;
	while (*nptr)
	{
		if (*nptr < '0' || *nptr > '9')
			return (-1);
		digit = *nptr - '0';
		res = res * 10 + digit;
		if (res > INT_MAX)
			return (-1);
		nptr++;
	}
	return ((int)(res));
}
