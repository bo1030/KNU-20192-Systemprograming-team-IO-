#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int exec(char* fname)
{
    int newpid;
    char array[3][100]={0};

    strcpy(array[0], "vim");
    strcpy(array[1], fname);
    if((newpid = fork())==-1)
        perror("fork");
    else if(newpid == 0)
    {
        execvp("vim", array);
    }
    else
        wait(newpid);
}
