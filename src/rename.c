#include <stdio.h>

void do_rename(char * name, char *change)
{
	rename(name, change);
}