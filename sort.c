/* See LICENSE file for copyright and license details. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "text.h"
#include "util.h"

static int linecmp(const char **, const char **);

static bool rflag = false;
static bool uflag = false;

static char delim = '\n';

static struct linebuf linebuf = EMPTY_LINEBUF;

int
main(int argc, char *argv[])
{
	char c;
	long i;
	FILE *fp;

	while((c = getopt(argc, argv, "ruz")) != -1)
		switch(c) {
		case 'r':
			rflag = true;
			break;
		case 'u':
			uflag = true;
			break;
		case 'z':
			delim = '\0';
			break;
		default:
			exit(2);
		}
	if(optind == argc)
		getdelims(stdin, &linebuf, delim);
	else for(; optind < argc; optind++) {
		if(!(fp = fopen(argv[optind], "r")))
			eprintf("fopen %s:", argv[optind]);
		getdelims(fp, &linebuf, delim);
		fclose(fp);
	}
	qsort(linebuf.lines, linebuf.nlines, sizeof *linebuf.lines, (int (*)(const void *, const void *))linecmp);

	for(i = 0; i < linebuf.nlines; i++)
		if(!uflag || i == 0 || strcmp(linebuf.lines[i], linebuf.lines[i-1]) != 0)
			fputs(linebuf.lines[i], stdout);
	return EXIT_SUCCESS;
}

int
linecmp(const char **a, const char **b)
{
	return strcmp(*a, *b) * (rflag ? -1 : +1);
}
