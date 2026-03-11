#include "codexion.h"
#include <stdio.h>

void	print_data(t_data *data)
{
	printf("%i\n", data->number_of_coders);
	printf("%i\n", data->time_to_burnout);
	printf("%i\n", data->time_to_compile);
	printf("%i\n", data->time_to_debug);
	printf("%i\n", data->time_to_refactor);
	printf("%i\n", data->number_of_compiles_required);
	printf("%i\n", data->dongle_cooldown);
	printf("%s\n", (data->scheduler == FIFO) ? "fifo" : "edf");
}

int	main(int ac, char **av)
{
	t_data	*data;

	data = parse_args(ac, av);
	if (!data)
		return (-1);
	print_data(data);
	return (0);
}
