#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int exec(char* fname, int * newpid)
{
    char *array[3];
    array[0] = (char *)malloc(4);	    
    array[1] = (char *)malloc(strlen(fname)+1);
    array[2] = 0;    
    strcpy(array[0], "vim");
    strcpy(array[1], fname);
    if((*newpid = fork())==-1)
        perror("fork");
    else if(*newpid == 0)
    {
       if( execvp("vim", array)==-1)
	       perror("execvp");
    }
    else
        wait(NULL);
}
