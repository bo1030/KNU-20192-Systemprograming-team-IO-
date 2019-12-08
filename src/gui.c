#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>

void underdock();
void center_left();
void do_ls(char*);
void dostat(char *);
void save_info(char *, struct stat *);
void set_crmode(int);
void set_echomode(int);
int isDir(char *);

struct cur_info{
	int col_pos;
	int row_pos;
	char sel_dir[100][100];
	char sel[100][100];
};

struct file_info{
	char name[100];
	char fsize[10];
	char moditime[30];
};

struct scroll_pos{
	int up;
	int down;
};

char underlist[7][20] = {"1. sel\0", "2. paste\0", "3. move\0", "4. rename\0", "5. quit\0", "6. give\0", "7. get\0"};
struct cur_info cur;
struct file_info ifile[100];
struct scroll_pos sc;
int fsize = 0;
int selected = 0;
int change = 1;

int main(void)
{
	char path[100] = ".";
	int menu;
	int i;
	char temp[100];

	set_crmode(0);
	set_echomode(2);
	init();
	while (1)
	{
		if(change == 1)
		{
			fsize = 0;
			do_ls(path);
			change = 0;
			cur.col_pos = 0;
			cur.row_pos = 1;
			sc.down = LINES -4;
			sc.up = 1;
		}
		clear();
		underdock();
		center_left();
		move(cur.row_pos, 0);
		addstr("*");
		refresh();
		menu = getch();
		
		switch(menu)
		{
			case '1':
				strcpy(cur.sel[selected], ifile[cur.row_pos-1].name);
				getcwd(temp, 100);
				strcat(temp, "/");
				strcat(temp, ifile[cur.row_pos-1].name);
				strcpy(cur.sel_dir[selected++], temp);
				break;
			case '2':
				for(int i = 0; i < selected; i++)
				{
					getcwd(temp, 100);
					copy(cur.sel_dir[i], temp, cur.sel[i]);
				}
				selected = 0;
				break;
			case '3':
				endwin();
				set_crmode(1);
				set_echomode(1);
				printf("target dir: ");
				scanf("%s", temp);
				init();
				set_crmode(0);
				set_echomode(2);
				do_rename(ifile[cur.row_pos-1].name, temp);
				refresh();
				move(cur.row_pos, cur.col_pos);
				change = 1;
				break;
			case '4':
				endwin();
				set_crmode(1);
				set_echomode(1);
				printf("rename: ");
				scanf("%s", temp);
				init();
				set_crmode(0);
				set_echomode(2);
				do_rename(ifile[cur.row_pos-1].name, temp);
				refresh();
				move(cur.row_pos, cur.col_pos);
				change = 1;
				break;
			case '5':
				endwin();
				set_crmode(1);
                                set_echomode(1);
				return 0;
			case '\n':
				if(isDir(ifile[cur.row_pos-1].name) == 1)
				{
					getcwd(temp, 100);
					strcat(temp, "/");
					strcat(temp, ifile[cur.row_pos-1].name);
					chdir(temp);
					strcpy(path, temp);		
					change = 1;
				}
				else
				{
					strcat(ifile[cur.row_pos-1].name,"\0");
					exec(ifile[cur.row_pos-1].name);
				}
				break;
	
			case 'w':
				if(cur.row_pos != 1)
				{
					cur.row_pos--;
					if(cur.row_pos < sc.up)
						sc.up = cur.row_pos;
				}
				break;
			case 's':
				if(cur.row_pos != fsize)
				{
					cur.row_pos++;
					if(cur.row_pos > sc.down)
						sc.down = cur.row_pos;
				}
				break;
			case 'a':
				cur.col_pos--;
				break;
			case 'd':
				cur.col_pos++;
				break;
		}
	}

}

void underdock()
{
	move(LINES - 3, 0);
	for (int i = 0; i < fsize; i++)
	{
		addstr(underlist[i]);
		addstr(" ");
	}
}

void center_left()
{
	move(0, 2);
	addstr("name                      ");
	addstr("fsize		");
	addstr("moditime");
	move(1, 2);
	
	for(int i = sc.up-1; i < sc.down; i++)
	{
		if(i == fsize)
			break;
		addstr(ifile[i].name);
		move((i+1)%(LINES-4), 28);
		addstr(ifile[i].fsize);
		move((i+1)%(LINES-4), 42);
		addstr(ifile[i].moditime);
		move((i+2)%(LINES-4), 2);
	}
}

void do_ls(char * dirname)
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
	sprintf(ifile[fsize].fsize, "%ld", info_p->st_size);
	strcpy(ifile[fsize].moditime, 4 + ctime(&info_p->st_mtime));
	strcat(ifile[fsize].moditime, "\0");
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

int isDir(char * fname)
{
	struct stat info;

	if (stat(fname, &info) == -1)
		perror(fname);
	else
	{
		if(S_ISDIR(info.st_mode))
			return 1;
		else
			return 0;
	}
}
