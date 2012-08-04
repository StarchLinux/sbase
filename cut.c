#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>
#include "text.h"
#include "util.h"

typedef struct {
	int min, max;
} range;

int inRange (range r, unsigned int n) {
	if ((0 == r.max || n <= r.max) && n >= r.min) return 1;
	else return 0;
}

int fputrune (Rune r, FILE *f) {
	char x[UTFmax];
	int n;
	n = runetochar (x, &r);
	fwrite (x, 1, n, f);
	return n;
}

void cutLineF (Rune d, unsigned int s, range *rs, char *x) {
	int ii, n, nMax;
	char **xs;
	if (!utfrune (x, d)) {
		if (!s) fputs (x, stdout);
		return;
	}
	xs = malloc (sizeof (char *) * (strlen (x) + 2));
	if (!xs) eprintf ("cut:");
	for (n = 1; x; n++) {
		xs[n] = x;
		x = utfrune (x, d);
		if (x) {
			*x = 0;
			x += runelen (d);
		}
	}
	xs[n] = xs[n-1] + strlen (xs[n-1]);
	nMax = n;
	for (ii = 0; rs[ii].min; ii++) {
		if (nMax <= rs[ii].min) continue;
		for (n = rs[ii].min; (rs[ii].max ? n <= rs[ii].max : 1) && n < nMax; n++) {
			fputs (xs[n], stdout);
			fputrune (d, stdout);
		}
	}
	free (xs);
}

void cutLineC (range *rs, char *x) {
	Rune _r;
	int ii, n;
	for (ii = 0; rs[ii].min; ii++) {
		char *y;
		y = x;
		for (n = 1; *y; n++) {
			int l = chartorune (&_r, y);
			if (inRange (rs[ii], n)) fwrite (y, 1, l, stdout);
			y += l;
		}
	}
}

void cutLineB (range *rs, char *x) {
	int ii, n;
	for (ii = 0; rs[ii].min; ii++) {
		for (n = rs[ii].min - 1; rs[ii].max ? n < rs[ii].max : x[n]; n++) {
			fputc (x[n], stdout);
		}
	}
}

void go (int mode, Rune d, unsigned int s, range *rs, FILE *f) {
	char *x;
	size_t size = 0;
	x = 0;

	while (getline (&x, &size, f) > 0) {
		int ii;
		/* must delete newline here, and redo later;
		   otherwise, unknown whether it was included in cut */
		for (ii = 0; x[ii]; ii++) if (x[ii] == '\n') x[ii] = 0;
		switch (mode) {
		case 'f':
			if (!utfrune (x, d)) {
				if (s) break;
				fputs (x, stdout);
			}
			else cutLineF (d, s, rs, x);
			fputc ('\n', stdout);
			break;
		case 'c': cutLineC (rs, x); fputc ('\n', stdout); break;
		case 'b': cutLineB (rs, x); fputc ('\n', stdout); break;
		}
	}
}

int main (int argc, char *argu[]) {
	int mode = 0;
	Rune d = '\t';
	unsigned int s = 0;
	range *rs = 0;
	int ii;
	
	/* parse options */
	for (ii = 1; ii < argc; ii++) {
		int jj;
		if (argu[ii][0] != '-' || argu[ii][1] == 0) break;
		if (argu[ii][1] == '-' && argu[ii][2] == 0) {
			ii++;
			break;
		}
		for (jj = 1; argu[ii][jj]; jj++) switch (argu[ii][jj]) {
		case 'b':
		case 'c':
		case 'f':
			mode = argu[ii][jj];
			
			if (++ii >= argc) {
				fputs ("cut: No range argument\n", stderr);
				return 1;
			}
			
			rs = malloc (sizeof (range) * (utflen (argu[ii]) + 1));
			if (!rs) eprintf ("cut:");
			
			/* ensure space delimitation for strtoul */
			for (jj = 0; argu[ii][jj]; jj++) if (argu[ii][jj] == ',') argu[ii][jj] = ' ';
			
			/* parse ranges */
			/* max = 0 to denote last */
			{
				char *p;
				p = argu[ii];
				jj = 0;
				while (*p) {
					rs[jj].min = *p == '-' ? 1 : strtoul (p, &p, 10);
					rs[jj].max = *p == '-' ? strtoul (++p, &p, 10) : rs[jj].min;
					switch (*p) {
					case '\0':
						rs[++jj].min = 0;
						break;
					case ' ':
					case '\f':
					case '\v':
					case '\t':
					case '\r':
					case '\n':
						jj++;
						break;
					default:
						fputs ("cut: Malformed ranges\n", stderr);
						return 1;
					}
				}
			}
			goto nextArgument;
		case 'd':
			chartorune (&d, argu[++ii]);
			goto nextArgument;
		case 's':
			s = 1;
			break;
		}
nextArgument:	;
	}

	if (!mode) {
		fputs ("cut: No mode given\n", stderr);
		return 1;
	}
	
	if (ii < argc) for (; ii < argc; ii++) {
		FILE *f;
		f = strcmp ("-", argu[ii]) == 0 ? stdin : fopen (argu[ii], "r");
		if (!f) eprintf ("cut: %s:", argu[ii]);
		go (mode, d, s, rs, f);
		if (f != stdin) fclose (f);
	}
	else go (mode, d, s, rs, stdin);
	
	return 0;
}
