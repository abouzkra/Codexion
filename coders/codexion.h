/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:27:58 by abouzkra          #+#    #+#             */
/*   Updated: 2026/05/21 22:38:08 by abouzkra         ###   ########.fr       */
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

# define INT_MAX 2147483647

enum	e_scheduler
{
	FIFO,
	EDF
};

typedef struct s_dongle	t_dongle;

typedef struct s_coder
{
	pthread_t	th_id;
	int			id;
	int			compile_count;
	long		last_compile;
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
	void		*data;

	long		arrival_time;
	long		deadline;
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

typedef struct s_data
{
	int				n_coders;
	int				t_burnout;
	int				t_compile;
	int				t_debug;
	int				t_refactor;
	int				n_compiles;
	int				cooldown;
	int				scheduler;

	long			start_time;
	t_dongle		*dongles;
	t_coder			*coders;
	int				sim_over;
	pthread_t		monitor_th;
	pthread_mutex_t	sim_mut;
	pthread_mutex_t	logger_mut;
}	t_data;

int				ft_atoi(const char *nptr);
long			get_time_in_ms(void);
struct timespec	ms_to_ts(long ms);
void			coder_sleep(long ms);
void			log_state(t_data *data, int coder_id, char *msg);

void			insert(t_dongle *dg, t_coder *coder);
int				pop(t_dongle *dg);

t_data			*parse_args(int ac, char *av[]);
int				init_sim(t_data *data);

int				has_priority(t_coder *c1, t_coder *c2);
int				is_top(t_coder *coder, t_dongle *dongle);
int				acquire_dongle(t_coder *coder, t_dongle *dongle);
void			release_dongle(t_dongle *dongle, long cooldown);
void			broadcast_dongles(t_data *data);

void			*monitor_routine(void *arg);
void			*coder_routine(void	*arg);

int				sim_is_over(t_data *data);
void			start_sim(t_data *data);
void			end_sim(t_data *data);

#endif
