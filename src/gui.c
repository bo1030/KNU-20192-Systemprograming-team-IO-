#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>
#include <unistd.h>

void underdock(char **, int);
void center_left();
void ls(char *);
void do_ls(char*);
void dostat(char *);
void save_info(char *, struct stat *);
void set_crmode(int);
void set_echomode(int);
void tty_mode(int);
int isDir(char *);

struct cur_info{
	int col_pos;
	int row_pos;
	char sel[100][100];
};

struct file_info{
	char name[100];
	char fsize[10];
	char moditime[20];
};

struct scroll_pos{
	int up;
	int down;
};

struct cur_info cur;
struct file_info ifile[100];
struct scroll_pos sc;
int fsize = 0;
int selected = 0;
int change = 1;

int main(void)
{
	char underlist[][] = {"1. sel", "2. paste", "3. move", "4. rename", "5. quit", "6. give", "7. get"};
	char path[] = ".";
	int menu;
	char temp[100];

	tty_mode(0);
	set_crmode(0);
	set_echomode(2);
	initscr();
	clear();
	while (1)
	{
		if(change == 1)
		{
			fsize = 0;
			ls(path);
			change = 0;
			cur.col_pos = 0;
			cur.row_pos = 1;
			sc.down = LINE -3;
			sc.up = 1;
		}
		clear();
		underdock(underlist, 5);
		center_left();
		move(cur.row_pos, 0);
		addstr("*");
		refresh();
		menu = getchar();
		
		switch(menu)
		{
			case '1':
				getcwd(temp, 100);
				strcat(temp, ifile[cur.row_pos-1].name)
				strcpy(cur.sel[selected++], temp);
				break;
			case '2':
				for(int i = 0; i < selected; i++)
				{
					getcwd(temp, 100);
					copy(sel[i], temp);
				}
				break;
			case '3':
				set_crmode(1);
				set_echomode(1);
				move(LINE-2, 0);
				addstr("target dir: ");
				scanf("%s", temp);
				set_crmode(0);
				set_echomode(2);
				do_rename(ifile[cur.row_pos-1], temp);
				addstr("							");
				move(cur.row_pos, cur.col_pos);
				change = 1;
				break;
			case '4':
				set_crmode(1);
				set_echomode(1);
				move(LINE-2, 0);
				addstr("rename: ");
				scanf("%s", temp);
				set_crmode(0);
				set_echomode(2);
				do_rename(ifile[cur.row_pos-1], temp);
				addstr("							");
				move(cur.row_pos, cur.col_pos);
				change = 1;
				break;
			case '5':
				tty_mode(1);
				return;
			case '\n':
				if(isDir == 1)
				{
					chdir(ifile[cur.row_pos-1].name);
					strncpy(path, ifile[cur.row_pos-1].name, strlen(ifile[cur.row_pos-1].name));
					change = 1;
				}
				else
				{
					exec(ifile[cur.row_pos-1].name);
				}
				break;
	
			case 224:
				menu = getchar();
				switch(menu)
				{
					case 72:
						if(cur.row_pos != 1)
						{
							cur.row_pos--;
							if(cur.row_pos < sc.up)
								sc.up = cur.row_pos;
						}
						break;
					case 75:
						if(cur.row_pos != fsize)
						{
							cur.row_pos++;
							if(cur.row_pos > sc.down)
								sc.down = cur.row_pos;
						}
						break;
					case 77:
						cur.col_pos--;
						break;
					case 80:
						cur.col_pos++;
						break;
				}
			break;
	}

}

void underdock(char ** under, int fsize)
{
	move(LINES - 2, 0);
	for (int i = 0; i < fsize; i++)
	{
		addstr(under[i]);
		addstr(" ");
	}
}

void center_left()
{
	move(0, 2);
	addstr("name	  ");
	addstr("fsize	  ");
	addstr("moditime");
	move(1, 2);
	
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

void set_echomode(int mode)
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	if (mode == 2)
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