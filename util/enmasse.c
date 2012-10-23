/* See LICENSE file for copyright and license details. */
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "../util.h"

void
enmasse(int argc, char **argv, int (*fn)(const char *, const char *))
{
	char *buf, *dir;
	int i, pid, c, n = 0;
	long size;
	struct stat st;

	if(argc == 2 && !(stat(argv[1], &st) == 0 && S_ISDIR(st.st_mode))) {
		fnck(argv[0], argv[1], fn);
		return;
	}
	else
		dir = (argc == 1) ? "." : argv[--argc];

	pid = fork();
	if(pid) {
		waitpid(pid, &n, 0);
		if (n == 0) return;
		exit(n);
	}
	setpgrp();
	apathmax(&buf, &size);
	for(i = 0; i < argc; i++) {
		if(snprintf(buf, size, "%s/%s", dir, basename(argv[i])) > size)
			eprintf("%s/%s: filename too long\n", dir, basename(argv[i]));
		pid = fork();
		if(pid == 0) {
			fnck(argv[i], buf, fn);
			exit(0);
		}
	}
	free(buf);
	for(;;) {
		if(waitpid(0, &c, 0) < 0) switch(errno) {
		case EINTR:	continue;
		case ECHILD:	exit(n);
		default:	eprintf("Wait failed\n");
		}
		if(c != 0) n++;
	}
}
