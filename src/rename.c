#include <stdio.h>

int main(int ac, char *av[])
{
	do_rename(av[1], av[2]);
}

void do_rename(char * name, char *change)
{
	rename(name, change);
}
