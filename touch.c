/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#include "util.h"

static void touch(const char *);

static bool cflag = false;
static time_t t;

int
main(int argc, char *argv[])
{
	char *end, c;

	t = time(NULL);
	while((c = getopt(argc, argv, "ct:")) != -1)
		switch(c) {
		case 'c':
			cflag = true;
			break;
		case 't':
			t = strtol(optarg, &end, 0);
			if(*end != '\0')
				eprintf("%s: not a number\n", optarg);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	for(; optind < argc; optind++)
		touch(argv[optind]);
	return EXIT_SUCCESS;
}

void
touch(const char *str)
{
	int fd;
	struct stat st;
	struct utimbuf ut;

	if(stat(str, &st) != 0) {
		if(errno != ENOENT)
			eprintf("stat %s:", str);
		if(cflag)
			return;
		if((fd = creat(str, O_RDONLY|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) < 0)
			eprintf("creat %s:", str);
		close(fd);
	}
	ut.actime = st.st_atime;
	ut.modtime = t;
	if(utime(str, &ut) != 0)
		eprintf("utime %s:", str);
}
