#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "util.h"

enum {
	aFlag = 1,
	sFlag = 2,
};

int notAllDots (struct dirent *e) {
	int ii;
	for (ii = 0; e -> d_name[ii]; ii++) if (e -> d_name[ii] != '.') return 1;
	return 0;
}

int du (int top, int bs, int flags, char *path) {
	struct stat s;
	struct dirent **es;
	if (lstat (path, &s) < 0) eprintf ("du: %s:", path);
	if (s.st_mode & S_IFDIR) {
		int ii, n;
		n = scandir (path, &es, notAllDots, 0);
		if (n < 0) eprintf ("du: %s:", path);
		for (ii = 0; ii < n; ii++) {
			char *subpath;
			if (asprintf (&subpath, "%s/%s", path, es[ii] -> d_name) < 0) eprintf ("du:");
			s.st_size += du (0, bs, flags, subpath);
			free (subpath);
		}
	}
	if (top || (flags & aFlag || s.st_mode & S_IFDIR) && !(flags & sFlag)) printf ("%16d\t%s\n", s.st_size / bs, path);
	return s.st_size;
}
															
void main (int argc, char *argu[]) {
	int flags = 0;
	int bs = 512;
	int ii;
	for (ii = 1; ii < argc; ii++) {
		int jj;
		if (argu[ii][0] != '-' || argu[ii][1] == 0) break;
		if (argu[ii][1] == '-' && argu[ii][2] == 0) {
			ii++;
			break;
		}
		for (jj = 1; argu[ii][jj]; jj++) switch (argu[ii][jj]) {
		case 'a':
			flags |= aFlag;
			break;
		case 's':
			flags |= sFlag;
			break;
		case 'k':
			bs = 1024;
			break;
		}
	}
	
	if (ii < argc) for (; ii < argc; ii++) du (1, bs, flags, argu[ii]);
	else du (1, bs, flags, ".");
}
