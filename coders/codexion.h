/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouzkra <abouzkra@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:59:52 by abouzkra          #+#    #+#             */
/*   Updated: 2026/04/06 09:09:22 by abouzkra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
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

enum e_coder_state
{
	COMPILING,
	REFACTORING,
	DEBUGGING,
	BURNED_OUT
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
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
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
	pthread_t			monitor_th;
	pthread_t			logger_th;
	pthread_mutex_t		sim_mutex;
	pthread_mutex_t		log_mutex;
	t_dongle			*dongles;
	t_coder				*coders;
}	t_data;

long			get_time_in_ms(void);
struct timespec	ms_to_ts(long ms);
void			coder_sleep(long ms);
t_data			*parse_args(int ac, char **av);
int				init_args(t_data *data);
void			log_state(t_data *data, int coder_id, char *action);

int				init_dongle(t_dongle *d);
int				acquire_dongle(t_coder *coder, t_dongle *dongle);
void			release_dongle(t_dongle *dongle, long cooldown);
void			broadcast_all_dongles(t_data *data);
void			print_dongle_queue(t_data *data, t_dongle *dongle);

int				has_priority(t_waiter w1, t_waiter w2, enum e_scheduler sched);
void			enqueue(t_waiter w, t_dongle *dongle, enum e_scheduler sched);
void			dequeue(t_dongle *dongle);
int				is_head(t_dongle *dongle, int coder_id);

void			init_coder(t_data *data, int i);
void			*coder_routine(void *arg);
void			*monitor_routine(void *arg);
void			*logger_routine(void *arg);
int				sim_is_over(t_data *data);

void			run_threads(t_data *data);
void			join_threads(t_data *data);

void			cleanup(t_data *data);

#endif
