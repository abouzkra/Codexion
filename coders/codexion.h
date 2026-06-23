/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:27:58 by abouzkra          #+#    #+#             */
/*   Updated: 2026/06/22 15:45:09 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

enum	e_scheduler
{
	FIFO,
	EDF
};

enum	e_sim_state
{
	STARTED,
	OVER
};

typedef struct s_data	t_data;
typedef struct s_dongle	t_dongle;

typedef struct s_coder
{
	pthread_t	th_id;
	int			id;
	int			compile_count;
	long		last_compile;
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
	t_data		*data;

	long		arrival_time;
}	t_coder;

typedef struct s_dongle
{
	pthread_mutex_t	mut;
	pthread_cond_t	cond;
	int				held_by;
	long			cooldown_ts;
	t_coder			*queue[2];
	int				queue_size;
}	t_dongle;

typedef struct s_tracker
{
	int	spawned_coders;
	int	monitor_th_spawned;
	int	sim_mut_init;
	int	sim_cond_init;
	int	dongle_mut_init;
	int	dongle_cond_init;
	int	logger_mut_init;
	int	sleep_cond_init;
}	t_tracker;

typedef struct s_data
{
	int					n_coders;
	int					t_burnout;
	int					t_compile;
	int					t_debug;
	int					t_refactor;
	int					n_compiles;
	int					cooldown;
	int					scheduler;

	long				start_time;
	t_dongle			*dongles;
	t_coder				*coders;
	pthread_cond_t		sleep_cond;
	enum e_sim_state	sim_state;
	pthread_t			monitor_th;
	pthread_mutex_t		sim_mut;
	pthread_cond_t		sim_cond;
	pthread_mutex_t		logger_mut;
	t_tracker			t;
}	t_data;

int		ft_atoi(const char *nptr);
long	get_time_in_ms(void);
int		all_threads_started(t_data *data);
void	coder_sleep(t_data *data, long ms);
void	log_state(t_data *data, int coder_id, char *msg);

void	insert(t_dongle *dg, t_coder *coder);
int		pop(t_dongle *dg);

t_data	*parse_args(int ac, char *av[]);
int		init_sim(t_data *data);

int		has_priority(t_coder *c1, t_coder *c2);
int		is_top(t_coder *coder, t_dongle *dongle);
int		acquire_dongles(t_coder *coder);
void	release_dongle(t_dongle *dongle, long cooldown);
void	broadcast_dongles(t_data *data);

void	*monitor_routine(void *arg);
void	*coder_routine(void	*arg);

int		sim_is_over(t_data *data);
int		start_sim(t_data *data);
void	end_sim(t_data *data);

#endif
