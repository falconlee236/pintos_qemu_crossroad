
#include "projects/crossroads/map.h"


#define ANSI_NONE "\033[0m"
#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

#define ON_ANSI_BLACK "\033[40m"
#define ON_ANSI_RED "\033[41m"
#define ON_ANSI_GREEN "\033[42m"
#define ON_ANSI_YELLOW "\033[43m"
#define ON_ANSI_BLUE "\033[44m"
#define ON_ANSI_MAGENTA "\033[45m"
#define ON_ANSI_CYAN "\033[46m"
#define ON_ANSI_WHITE "\033[47m"

#define clear() printf("\033[H\033[J")
#define gotoxy(y,x) printf("\033[%d;%dH", (y), (x))


const char map_draw_default[7][7] = {
	{'X', 'X', ' ', 'X', ' ', 'X', 'X'}, 
	{'X', 'X', ' ', 'X', ' ', 'X', 'X'}, 
	{' ', ' ', ' ', '-', ' ', ' ', ' '}, 
	{'-', '-', '-', '-', '-', '-', '-'}, 
	{' ', ' ', ' ', '-', ' ', ' ', ' '}, 
	{'X', 'X', ' ', '-', ' ', 'X', 'X'}, 
	{'X', 'X', ' ', '-', ' ', 'X', 'X'}, 
};


void map_draw(void)
{
	int i, j;

	clear();

	for (i=0; i<7; i++) {
		for (j=0; j<7; j++) {
			printf("%c ", map_draw_default[i][j]);
		}
		printf("\n");
	}
	printf("unit step: %d\n", crossroads_step);
	gotoxy(0, 0);
}

void map_draw_vehicle(char id, int row, int col)
{
	if (row >= 0 && col >= 0) {
		gotoxy(row + 1, col * 2 + 1);
		printf("%c ", id);
		gotoxy(0, 0);
	}
}

void map_draw_reset(void)
{
	clear();
}