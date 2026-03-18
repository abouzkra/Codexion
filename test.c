#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(void)
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);

	printf("%ld\n", tv.tv_sec);
	printf("%ld\n", tv.tv_usec);
	printf("time in ms: %ld\n", tv.tv_sec * 1000 + tv.tv_usec / 1000);
	return 0;
}
