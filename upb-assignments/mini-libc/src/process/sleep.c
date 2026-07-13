#include <unistd.h>
#include <time.h>
#include <stddef.h>

unsigned int sleep(unsigned int seconds)
{
	struct timespec req;

	req.tv_sec = (long)seconds;
	req.tv_nsec = 0;

	nanosleep(&req, NULL);

	return 0;
}
