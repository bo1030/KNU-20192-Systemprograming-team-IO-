#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>₩
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUFFERSIZE 4096

void copy(char *, char *, char *);
int mode(char *);
int is_read(int);
char* exist(char*, char *);

void copy(char * dirname, char * targetname, char *fname)
{
	DIR *dir_ptr;
	struct dirent *direntp;

	int in_fd, out_fd, n_chars, info;
	char buf[BUFFERSIZE];
	char in_name[200];
	char out_name[200];
	if(isDir(dirname) == 1)
	{
		
		if((dir_ptr = opendir(dirname)) == NULL)
			fprintf(stderr, "cannot open %s\n", dirname);
		else{
			while((direntp = readdir(dir_ptr)) != NULL)
			{
				if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name,"..") == 0)
					continue;

				strcpy(in_name, dirname);
				strcpy(out_name, targetname);

				strcat(in_name, "/");
				strcat(in_name, direntp->d_name);
				strcat(out_name, "/");
				strcat(out_name, direntp->d_name);
			
				info = mode(in_name);

				if(S_ISDIR(info))
				{
					mkdir(out_name, info);
					copy(in_name, out_name);
					continue;
				}

				if(is_read(info)){

					if((in_fd = open(in_name, O_RDONLY)) == -1)	
						fprintf(stderr, "cannot open %s\n", in_name);
					if((out_fd = creat(out_name, info)) == -1)
						fprintf(stderr, "cannot creat %s\n", out_name);

					while((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
						if(write(out_fd, buf, n_chars) != n_chars)
							fprintf(stderr, "write error to %s\n", out_name);

					if(n_chars == -1)
						fprintf(stderr, "Read error from %s\n", in_name);

					if(close(in_fd) == -1 || close(out_fd) == -1)
						fprintf(stderr, "Error closing files\n");
				}

			}
		closedir(dir_ptr);
		}
	}
	else
	{
		stat(dirname, &info);
		strcpy(in_name, dirname);
                strcpy(out_name, targetname);


                strcat(out_name, "/");
                strcat(out_name, fname);

                info = mode(in_name);

                if(is_read(info)){
			if((in_fd = open(in_name, O_RDONLY)) == -1)
				fprintf(stderr, "cannot open %s\n", in_name);
                                if((out_fd = creat(out_name, info)) == -1)
                                        fprintf(stderr, "cannot creat %s\n", out_name);

                                while((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
                                        if(write(out_fd, buf, n_chars) != n_chars)
						fprintf(stderr, "write error to %s\n", out_name);

                                if(n_chars == -1)
                                       	fprintf(stderr, "Read error from %s\n", in_name);

                                if(close(in_fd) == -1 || close(out_fd) == -1)
                                       	fprintf(stderr, "Error closing files\n");
        	}
	}
}

int mode(char *filename)
{
	struct stat info;

	if(stat(filename, &info) == -2)
		perror(filename);

	return (info.st_mode);
}

int is_read(int mode)
{
	if((mode & S_IRUSR) || (mode & S_IRGRP) || (mode & S_IROTH))
		return 1;
	else 
		return 0;
}			

char * exist(char* dir_name, char * target)
{
	if(chdir(target) == -1)
	{
		mkdir(target, mode(dir_name));
		return target;
	}
}
