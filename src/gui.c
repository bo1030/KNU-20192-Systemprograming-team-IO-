#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>

void underdock(char **, int);
void center_left();
void ls(char *);
void do_ls(char[]);
void dostat(char *);
void save_info(char *, struct stat *);
void set_crmode(int);
void set_echomode(char mode);
void tty_mode(int);
int isDir(char *);
void exc(char *);


struct cur_info{
	int col_pos;
	int row_pos;
	char sel[100][100];
}

struct file_info{
	char name[100];
	char fsize[10];
	char moditime[20];
}

struct scroll{
	int up=1;
	int down=LINES -2;
}

struct cur_info cur;
struct file_info ifile[100];
struct scroll sc;
int fsize = 0;
int change = 1;

int main(void)
{
	char underlist[][] = {"1. copy", "2. paste", "3. move", "4. rename", "5. sel", "6. quit"};
	char path[] = ".";
	int menu;
	initscr();
	clear();
	cur.col_pos = 0;
	cur.row_pos = 1;
	while (1)
	{
		if(change == 1)
			ls(path);
		underdock(underlist, 5);
		center_left();
		refresh();
		menu = getchar();
		
		switch(menu)
		{
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				return;
			case '\n':
				break;
	
			case 224:
				menu = getchar();
				switch(menu)
				{
					case 72:
						break;
					case 75:
						break;
					case 77:
						break;
					case 80:
						break;
				}
			break;


	}

}

void underdock(char ** under, int fsize)
{
	move(LINES - 1, 0);
	for (int i = 0; i < fsize; i++)
	{
		addstr(under[i]);
		addstr(" ");
	}
}

void center_left()
{
	move(0, 0);
	addstr("name	  ");
	addstr("fsize	  ");
	addstr("moditime");
	pos = 1;
	move(pos, 0);
	
	for(int i = sc.up; i < sc.down; i++)
	{
		if(i == fsize)
			break;
		addstr(ifile[i].name);
		addstr("	");
		addstr(ifile[i].fsize);
		addstr("	");
		addstr(ifile[i].moditime);
	}
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
	{
	 	save_info(filename, &info);
		fsize++;
	}
}

void save_info(char *filename, struct stat *info_p)
{
	char *ctime();

	strcpy(ifile[fsize].name, filename);
	itoa(info_p->st_size, ifile[fsize].fsize, 10);
	strcpy(ifile[fsize].name, 4 + ctime(&info_p->st_mtime));
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
