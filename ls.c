#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <utf.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include "util.h"

enum {
	aFlag =  1,
	dFlag =  2,
	lFlag =  4,
	tFlag =  8,
	RFlag = 16,
	pFlag = 32,
};

unsigned int parseXD (char *s, int l) {
	int ii;
	unsigned int n = 0;
	for (ii = 0; ii < l; ii++) {
		n *= 16;
		if (s[ii] <= '9' && s[ii] >= '0') {
			n += s[ii] - '0';
			continue;
		}
		if (s[ii] & 0xDF <= 'Z' && s[ii] & 0xDF >= 'A') {
			n += s[ii] - 'A';
			continue;
		}
		eprintf ("ls: no parse: %s\n", s);
	}
	return n;
}

void showUnixMode (int mode) {
	int ii;
	char x[] = "----------";
	for (ii = 0; ii < 9; ii++) if (mode & (1 << ii)) x[9 - ii] = "xwr"[ii % 3];
	if (mode & S_IFDIR) x[0] = 'd';
	printf ("%s", x);
}

void ls1 (char *fmt, int flags, char *path) {
	struct stat s;
	if (lstat (path, &s) < 0) eprintf ("ls: %s:", path);
	if (!(flags & lFlag)) fmt = "%N%n";
	if (!fmt) fmt = "%P  %l %u %g %z %m %N%n";
	for (; fmt[0]; fmt++) {
		if (fmt[0] == '%') {
			int w = 0;
			char *p;
			if(!*fmt++) {
				fputs ("ls: bad format\n", stderr);
				exit (1);
			}
			if (fmt[0] >= '0' && fmt[0] <= '9') w = strtoul (fmt, &fmt, 10);
			switch (fmt[0]) {
			case '%':
				fputc ('%', stdout);
				break;
			case 'n':
				fputc ('\n', stdout);
				break;
			case 't':
				fputc ('\t', stdout);
				break;
			case 'd':
				printf ("%*d", w ? w : 8, s.st_dev);
				break;
			case 'i':
				printf ("%*d", w ? w : 16, s.st_ino);
				break;
			case 'p':
				printf ("%.4o", s.st_mode & 0x0FFF);
				break;
			case 'P':
				showUnixMode (s.st_mode);
				break;
			case 'l':
				printf ("%*d", w ? w : 4, s.st_nlink);
				break;
			case 'u':
				printf ("%*d", w ? w : 12, s.st_uid);
				break;
			case 'g':
				printf ("%*d", w ? w : 12, s.st_gid);
				break;
			case 'm':
				printf ("%*d", w ? w : 20, s.st_mtime);
				break;
			case 'v':
				printf ("%*d", w ? w : 4, minor (s.st_rdev));
				break;
			case 'V':
				printf ("%*d", w ? w : 4, major (s.st_rdev));
				break;
			case 'x':
				printf ("%c", parseXD (++fmt, 4));
				fmt += 3;
				break;
			case 'z':
				printf ("%*d", w ? w : 16, s.st_size);
				break;
			case 'N':
				p = flags & pFlag ? utfrrune (path, L'/') : 0;
				fputs (p ? p + runelen (L'/') : path, stdout);
				break;
			default:
				fprintf (stderr, "ls: unrecognized format spec: %c\n", fmt[0]);
				exit (1);
			}
		}
		else fputc (fmt[0], stdout);
	}
}

int noDot (const struct dirent *e) {
	return (e -> d_name[0] != '.');
}

int comparMTime (const char *x, const char *y) {
	struct stat s, t;
	if (lstat (x, &s) < 0) eprintf ("ls: %s:", x);
	if (lstat (y, &t) < 0) eprintf ("ls: %s:", y);
	return ((int)(t.st_mtime - s.st_mtime));
}

int comparPMTime (const char **p, const char **q) {
	return comparMTime (*p, *q);
}

int cdeMTime (const struct dirent **e, const struct dirent **f) {
	return comparMTime ((*e) -> d_name, (*f) -> d_name);
}

int cdeName (const struct dirent **e, const struct dirent **f) {
	return (strcmp ((*e) -> d_name, (*f) -> d_name));
}

void ls (char *fmt, int flags, char *path) {
	struct dirent **es;
	int ii, n;
	if (flags & dFlag) {
		ls1 (fmt, flags, path);
		return;
	}
	n = scandir (path, &es, flags & aFlag ? 0 : noDot, flags & tFlag ? cdeMTime : cdeName);
	if (n < 0) {
		if (errno == ENOTDIR) {
			ls1 (fmt, flags, path);
			return;
		}
		else eprintf ("ls: %s:", path);
	}

	if (flags & RFlag) ls1 (fmt, flags, path);
	for (ii = 0; ii < n; ii++) {
		char *subpath;
		if (asprintf (&subpath, "%s/%s", path, es[ii] -> d_name) < 0) eprintf ("ls:");
		(flags & RFlag ? ls : ls1) (fmt, flags, subpath);
		free (subpath);
	}
}

void lsn (char *fmt, int flags, char *paths[], int n) {
	int ii;
	if (flags & tFlag) qsort (paths, n, sizeof (char *), comparPMTime);
	for (ii = 0; ii < n; ii++) ls (fmt, flags, paths[ii]);
}

int main (int argc, char *argu[]) {
	int flags = 0;
	char *fmt;
	int ii;
	fmt = 0;
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
		case 'd':
			flags |= dFlag;
			break;
		case 'l':
			flags |= lFlag;
			break;
		case 't':
			flags |= tFlag;
			break;
		case 'f':
			fmt = argu[++ii];
			goto nextArgument;
		case 'R':
			flags |= RFlag;
			break;
		case 'p':
			flags |= pFlag;
			break;
		}
nextArgument:	;
	}

	if (ii < argc) lsn (fmt, flags, argu + ii, argc - ii);
	else ls (fmt, flags, ".");
	
	return 0;
}
