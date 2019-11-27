#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>

void underdock(char **, int);
void center_left(char *);
void ls(char *);
void do_ls(char[]);
void dostat(char *);
void show_file_info(char *, struct stat *);
void set_crmode(int);
void set_echomode(char mode);
void tty_mode(int);

int pos;

int main(void)
{
	char underlist[][] = {"1. 복사", "2. 붙여넣기", "3. 잘라내기", "4. 이름바꾸기", "5. 공유"};
	char path[] = ".";
	int menu;
	initscr();
	clear();
	while (1)
	{
		underdock(underlist, 5);
		center_left(path);
		menu = getchar()
	}

}

void underdock(char ** under, int size)
{
	move(LINES - 1, 0);
	for (int i = 0; i < size; i++)
	{
		addstr(under[i]);
		addstr(" ");
		refresh();
	}
}

void center_left(char *list)
{
	move(0, 0);
	addstr("name	  ");
	addstr("size	  ");
	addstr("moditime");
	pos = 1;
	move(pos, 0);
	ls(list);
}

void do_ls(char dirname[])
{
	DIR *dir_ptr;
	struct dirent *direntp;
	if ((dir_ptr = opendir(dirname)) == NULL)
		fprintf(stderr, "ls2: cannot open %s\n", dirname);
	else
	{
		while ((direntp = readdir(dir_ptr)) != NULL)
			dostat(direntp->d_name);
		closedir(dir_ptr);
	}
}

void dostat(char *filename)
{
	struct stat info;

	if (stat(filename, &info) == -1)
		perror(filename);
	else
		show_file_info(filename, &info);
}

void show_file_info(char *filename, struct stat *info_p)
{
	char *ctime();

	addstr(filename);
	addstr("	");
	addstr((long)info_p->st_size);
	addstr("	");
	addstr(4 + ctime(&info_p->st_mtime));
	move(++pos, 0);
}


void set_crmode(int mode)
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	if (mode == 0)
		ttystate.c_lflag &= ~ICANON;
	else
		ttystate.c_lflag |= ICANON;
	ttystate.c_cc[VMIN] = 1;

	tcsetattr(0, TCSANOW, &ttystate);
}

void set_echomode(char mode)
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	if (mode == '2')
		ttystate.c_lflag &= ~ECHO;
	else
		ttystate.c_lflag |= ECHO;
	ttystate.c_cc[VMIN] = 1;

	tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode(int how)
{
	static struct termios original_mode;
	if (how == 0)
		tcgetattr(0, &original_mode);
	else
		tcsetattr(0, TCSANOW, &original_mode);
}
