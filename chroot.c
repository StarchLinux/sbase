#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

void main (int argc, char *argu[]) {
	if (argc < 2) {
		fputs ("No new root path given\n", stderr);
		exit (1);
	}
	if (chdir(argu[1]) || chroot (".")) eprintf ("chroot:");
	if (argc == 2) {
		char *x;
		x = getenv ("SHELL");
		if (!x) {
			fputs ("chroot: SHELL not set\n", stderr);
			exit (1);
		}
		if (execl (x, x, "-i", (char *)0) < 0) eprintf ("chroot: %s:", x);
	}
	else if (execv (argu[2], argu + 2) < 0) eprintf ("chroot: %s:", argu[2]);
}
