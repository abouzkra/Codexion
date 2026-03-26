#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define N_THREADS 20

void *routine(void *arg)
{
	printf("Hello from thread %d!\n", (int)(long)arg);
}

int main(void)
{
	pthread_t	*th = (pthread_t *) malloc(sizeof(pthread_t) * N_THREADS);
	for (int i = 0; i < N_THREADS; i++)
	{
		if (pthread_create(&th[i], NULL, &routine, (void *)(long)i) != 0)
		{
			printf("Failed to create thread!\n");
			return 1;
		}
	}
	for (int i = 0; i < N_THREADS; i++)
	{
		if (pthread_join(th[i], NULL) != 0)
		{
			printf("Failed to join thread!\n");
			return 1;
		}
	}
	free(th);
	return 0;
}
