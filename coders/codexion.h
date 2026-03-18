/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:59:52 by abouzkra          #+#    #+#             */
/*   Updated: 2026/03/17 00:34:58 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <string.h>
# include <sys/time.h>

enum e_scheduler
{
	FIFO,
	EDF
};

typedef struct s_waiter
{
	int		coder;
	long	arrival_time;
	long	deadline;
}	t_waiter;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				held_by;
	long			cooldown_timestamp;
	t_waiter		queue[2];
	int				queue_size;
}	t_dongle;

typedef struct s_coder
{
	pthread_t	th_id;
	int			id;
	int			compile_count;
	long		last_compile_start;
	t_dongle	*l_dongle;
	t_dongle	*r_dongle;
	void		*data;
}	t_coder;

typedef struct s_data
{
	int					number_of_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					number_of_compiles_required;
	long				dongle_cooldown;
	enum e_scheduler	scheduler;

	long				start_time;
	int					sim_over;
	pthread_mutex_t		sim_mutex;
	pthread_mutex_t		print_mutex;
	t_dongle			*dongles;
	t_coder				*coders;
}	t_data;

long	get_time_in_ms(void);
t_data	*parse_args(int ac, char **av);
int		init_args(t_data *data);
void	cleanup(t_data *data);

#endif
