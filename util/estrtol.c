/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include "../util.h"

long
estrtol(const char *s, int base)
{
	char *end;
	long n;
	
	n = strtol(s, &end, base);
	if(*end != '\0') {
		if(base == 0)
			eprintf("%s: not an integer\n", s);
		else
			eprintf("%s: not a base %d integer\n", s, base);
	}
	return n;
}
