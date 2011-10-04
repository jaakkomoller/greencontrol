#include <stdio.h>
#include <stdlib.h>




/*
 * TODO: proper random, see:
 * http://tools.ietf.org/html/rfc3550#appendix-A.6
 */

unsigned long random32() {
	unsigned long ret;
	int *random = (int *)&ret;
	
	srand(time(NULL));
	random[0] = rand();
	random[1] = rand();

	return ret;
}

