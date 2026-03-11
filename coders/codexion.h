#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <string.h>

# define TRUE 1
# define FALSE 0

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_data
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_data;

t_data	*parse_args(int ac, char **av);

#endif
