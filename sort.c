/* See LICENSE file for copyright and license details. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "text.h"
#include "util.h"

static int linecmp(const char **, const char **);
static void getlines(FILE *, const char *);

static bool rflag = false;
static char **lines = NULL;
static long nlines = 0;

int
main(int argc, char *argv[])
{
	char c;
	long i;
	FILE *fp;

	while((c = getopt(argc, argv, "r")) != -1)
		switch(c) {
		case 'r':
			rflag = true;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	if(optind == argc)
		getlines(stdin, "<stdin>");
	else for(; optind < argc; optind++) {
		if(!(fp = fopen(argv[optind], "r")))
			eprintf("fopen %s:", argv[optind]);
		getlines(fp, argv[optind]);
		fclose(fp);
	}
	qsort(lines, nlines, sizeof *lines, (int (*)(const void *, const void *))linecmp);

	for(i = 0; i < nlines; i++) {
		fputs(lines[i], stdout);
		free(lines[i]);
	}
	free(lines);

	return EXIT_SUCCESS;
}

void
getlines(FILE *fp, const char *str)
{
	char *line = NULL;
	size_t size = 0;

	while(afgets(&line, &size, fp)) {
		if(!(lines = realloc(lines, ++nlines * sizeof *lines)))
			eprintf("realloc:");
		if(!(lines[nlines-1] = malloc(strlen(line)+1)))
			eprintf("malloc:");
		strcpy(lines[nlines-1], line);
	}
	free(line);
}

int
linecmp(const char **a, const char **b)
{
	return strcmp(*a, *b) * (rflag ? -1 : +1);
}
