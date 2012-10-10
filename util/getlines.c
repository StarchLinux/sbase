/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../text.h"
#include "../util.h"

void
getlines(FILE *fp, struct linebuf *b)
{
	getdelims (fp, b, '\n');
}

void
getdelims(FILE *fp, struct linebuf *b, int delim)
{
	char *line = NULL;
	size_t size = 0;

	while(getdelim(&line, &size, delim, fp) >= 0) {
		if(++b->nlines > b->capacity && !(b->lines = realloc(b->lines, (b->capacity+=512) * sizeof *b->lines)))
			eprintf("realloc:");
		if(!(b->lines[b->nlines-1] = malloc(strlen(line)+1)))
			eprintf("malloc:");
		strcpy(b->lines[b->nlines-1], line);
	}
	free(line);
}
