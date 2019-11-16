#include <stdio.h>

int main(int ac, char *av[])
{
	rename(av[1], av[2]);
}

