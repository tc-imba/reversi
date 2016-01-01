#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 8
#ifndef __cplusplus
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
typedef enum{EMPTY, WHITE_, BLACK_, BOTH, WHITE, BLACK }STATE;
#define AI_COLOR_BLACK
//#define AI_COLOR_WHITE
#define DISPLAY_CHESS '`'
#define MODE_EASY 0
#define MODE_NORM 1
#define MODE_HARD 2
#define MODE_HELL 3
#define AI_MODE MODE_HARD


#ifdef AI_COLOR_BLACK
char symbol[6] = { ' ',DISPLAY_CHESS,' ',DISPLAY_CHESS,'O','X' };
#endif
#ifdef AI_COLOR_WHITE
char symbol[6] = { ' ',' ',DISPLAY_CHESS,DISPLAY_CHESS,'O','X' };
#endif
int dir[8][2] = { -1, 0, 0,-1,-1, 1, 1, 1,
				  -1,-1, 1,-1, 0, 1, 1, 0 };

char chessboard[SIZE][SIZE];
int chess_num[2];
int num_now[2];

int arg[SIZE][SIZE] =
{ 
	100, -5, 10,  5,  5, 10, -5,100,
	-5,-45,  1,  1,  1,  1,-45, -5,
	10,  1,  3,  2,  2,  3,  1, 10,
	5,  1,  2,  1,  1,  2,  1,  5,
	5,  1,  2,  1,  1,  2,  1,  5,
	10,  1,  3,  2,  2,  3,  1, 10,
	-5,-45,  1,  1,  1,  1,-45, -5,
	100, -5, 10,  5,  5, 10, -5,100
};

int IsChessAround(int x, int y, char board[][SIZE], STATE s)
{
	int i;
	for (i = 0;i<8;i++)
	{
		int xx = x + dir[i][0], yy = y + dir[i][1];
		if (xx >= 0 && xx < SIZE&&yy >= 0 && yy < SIZE)
		{
			if (board[xx][yy] == s)
			{
				return 1;
			}
		}
	}
	return 0;
}

int revnum(int x, int y, int d, char board[][SIZE], STATE s, int f)
{
	int x0 = x, y0 = y;
	int a = dir[d][0], b = dir[d][1];
	int num = 0, flag = 0;
	STATE rs = (s == WHITE ? BLACK : WHITE);
	x += a;y += b;
	while (x >= 0 && x<SIZE&&y >= 0 && y<SIZE)
	{
		if (board[x][y] == rs)
		{
			num++;
		}
		else if (board[x][y] == s)
		{
			flag = 1;
			break;
		}
		else
		{
			break;
		}
		x += a;y += b;
	}
	if (flag)
	{
		if (f)
		{
			int k;
			for (k = 1;k <= num;k++)
			{
				x = x0 + a*k;
				y = y0 + b*k;
				board[x][y] = s;
			}
		}
		return num;
	}
	return 0;
}

int canrev(int x, int y, char board[][SIZE] ,STATE s ,int f)
{
	int i, sum = 0;
	if (board[x][y] == WHITE || board[x][y] == BLACK)
	{
		return 0;
	}	
	if (x<0 || x >= SIZE || y<0 || y >= SIZE)
	{
		return 0;
	}
	STATE rs = (s == WHITE ? BLACK : WHITE);
	if (IsChessAround(x, y, board, rs))
	{
		for (i = 0;i<8;i++)
		{
			sum += revnum(x, y, i, board, s, f);
		}
	}
	return sum;
}

void refresh()
{
	int i, j;
	num_now[0] = 0;
	num_now[1] = 0;
	for (i = 0;i<SIZE;i++)
	{
		for (j = 0;j<SIZE;j++)
		{
			if (chessboard[i][j] != WHITE&&chessboard[i][j] != BLACK)
			{
				chessboard[i][j] = 0;
				if (canrev(i, j, chessboard, WHITE, 0)>0)
				{
					chessboard[i][j] += WHITE_;
					num_now[0]++;
				}
				if (canrev(i, j, chessboard, BLACK, 0)>0)
				{
					chessboard[i][j] += BLACK_;
					num_now[1]++;
				}
			}
		}
	}
	for (i = 0;i<SIZE;i++)
	{
		printf("   %c", 'a' + i);
	}
	for (i = 0;i<SIZE;i++)
	{
		printf("\n ");
		for (j = 0;j<SIZE;j++)
		{
			printf("+---");
		}
		printf("+\n%c", '1' + i);
		for (j = 0;j<SIZE;j++)
		{
			printf("| %c ", symbol[chessboard[i][j]]);
		}
		printf("|");
	}
	printf("\n ");
	for (i = 0;i<SIZE;i++)
	{
		printf("+---");
	}
	printf("+\n");
}

void putchess(int x, int y, STATE s)
{
	canrev(x, y, chessboard, s, 1);
	chessboard[x][y] = s;
	chess_num[s - WHITE]++;
}

void init()
{
	int i, j;
	for (i = 0;i < SIZE;i++)
	{
		for (j = 0;j < SIZE;j++)
		{
			chessboard[i][j] = 0;
		}
	}
	chess_num[0] = chess_num[1] = 1;
	num_now[0] = num_now[1] = 1;

	chessboard[3][3] = WHITE;
	chessboard[3][4] = BLACK;
	chessboard[4][3] = BLACK;
	chessboard[4][4] = WHITE;
	chess_num[0] = 2;
	chess_num[1] = 2;
	refresh();
}

long getarg(int x, int y, STATE s)
{
#ifdef AI_COLOR_BLACK
	return (s == BLACK ? arg[x][y] : -arg[x][y]);
#endif
#ifdef AI_COLOR_WHITE
	return (s == WHITE ? arg[x][y] : -arg[x][y]);
#endif
}

int boyi_max_x, boyi_max_y;

long boyi_search(char n_w, char n_b, char board[][SIZE], char s, char f, long a, long b)
{
	char n = n_w + n_b;
	char i, j;
	if (n == SIZE*SIZE)
	{
#ifdef AI_COLOR_BLACK
		return (n_w > n_b ? -1000000 : (n_w < n_b ? 1000000 : 0));
#endif
#ifdef AI_COLOR_WHITE
		return (n_w < n_b ? -1000000 : (n_w > n_b ? 1000000 : 0));
#endif
	}
#if (AI_MODE==MODE_EASY)
	if (n >= chess_num[0] + chess_num[1] + 2 && chess_num[0] + chess_num[1] <= SIZE*SIZE - 3)
#elif (AI_MODE==MODE_NORM)
	if (n >= chess_num[0] + chess_num[1] + 5 && chess_num[0] + chess_num[1] <= SIZE*SIZE - 6)
#elif (AI_MODE==MODE_HARD)
	if (n >= chess_num[0] + chess_num[1] + 8 && chess_num[0] + chess_num[1] <= SIZE*SIZE - 12)
#elif (AI_MODE==MODE_HELL)
	if (n >= chess_num[0] + chess_num[1] + 10 && chess_num[0] + chess_num[1] <= SIZE*SIZE - 12)
#endif
	{
		return 0;
	}
	char num = 0;
	if (f != 1)
	{
		for (i = 0;i < SIZE;i++)
		{
			for (j = 0;j < SIZE;j++)
			{
				if (board[i][j] != WHITE&&board[i][j] != BLACK)
				{
					if (canrev(i, j, board, s, 0)>0)
					{
						board[i][j] = s - 3;
						num++;
					}
					else
					{
						board[i][j] = EMPTY;
					}
				}
			}
		}
	}
	else
	{
#ifdef AI_COLOR_BLACK
		num = num_now[1];
#endif
#ifdef AI_COLOR_WHITE
		num = num_now[0];
#endif
	}
	long arg_m;
	char rs = (s == WHITE ? BLACK : WHITE);
	char new_board[SIZE][SIZE];
	memcpy(new_board, board, SIZE*SIZE);
	if (num == 0)
	{
		if (f == 2)
		{
#ifdef AI_COLOR_BLACK
			return (n_w > n_b ? -1000000 : (n_w < n_b ? 1000000 : 0));
#endif
#ifdef AI_COLOR_WHITE
			return (n_w < n_b ? -1000000 : (n_w > n_b ? 1000000 : 0));
#endif
		}
		else
		{
			return boyi_search(n_w, n_b, new_board, rs, 2, a, b);
		}
	}
	num = 0;
	for (i = 0;i < SIZE;i++)
	{
		for (j = 0;j < SIZE;j++)
		{
			if (f==1)
			{
#ifdef AI_COLOR_BLACK
				if (chessboard[i][j] != BLACK_&&chessboard[i][j] != BOTH)
#endif
#ifdef AI_COLOR_WHITE
				if (chessboard[i][j] != WHITE_&&chessboard[i][j] != BOTH)
#endif
				{
					continue;
				}
			}
			else
			{
				if (board[i][j] != s - 3)
				{
					continue;
				}
			}
			char rev = canrev(i, j, new_board, s, 1);
			new_board[i][j] = s;
			long arg_now;
			if (num != 0)
			{
#ifdef AI_COLOR_BLACK
				if (s == BLACK)
#endif
#ifdef AI_COLOR_WHITE
				if (s == WHITE)
#endif
				{
					a = max(a, arg_now);
				}
				else
				{
					b = min(b, arg_now);
				}
			}
			long arg_d = getarg(i, j, s);
			if (s == BLACK)
			{
				arg_now = boyi_search(n_w - rev, n_b + rev + 1, new_board, rs, 0, a - arg_d, b - arg_d) + arg_d;
			}
			else
			{
				arg_now = boyi_search(n_w + rev + 1, n_b - rev, new_board, rs, 0, a - arg_d, b - arg_d) + arg_d;
			}
#ifdef AI_COLOR_BLACK
			if (s == WHITE&&arg_now <= a)//alpha
#endif
#ifdef AI_COLOR_WHITE
			if (s == BLACK&&arg_now <= a)//alpha
#endif
			{
				return a;
			}
#ifdef AI_COLOR_BLACK
			if (s == BLACK&&arg_now >= b)//beita
#endif
#ifdef AI_COLOR_WHITE
			if (s == WHITE&&arg_now >= b)//beita
#endif
			{
				return b;
			}

#ifdef AI_COLOR_BLACK
			if (num == 0 || s == BLACK &&arg_now > arg_m || s == WHITE&&arg_now < arg_m)
#endif
#ifdef AI_COLOR_WHITE
			if (num == 0 || s == BLACK &&arg_now < arg_m || s == WHITE&&arg_now > arg_m)
#endif
			{
				arg_m = arg_now;
				if (f==1)
				{
					boyi_max_x = i;
					boyi_max_y = j;
				}
			}
			num++;
			memcpy(new_board, board, SIZE*SIZE);
		}
	}
	return arg_m;
}

int runAI()
{
	boyi_max_x = -1;
	boyi_max_y = -1;
#ifdef AI_COLOR_BLACK
	boyi_search(chess_num[0], chess_num[1], chessboard, BLACK, 1, -2000000, 2000000);
#endif
#ifdef AI_COLOR_WHITE
	boyi_search(chess_num[0], chess_num[1], chessboard, WHITE, 1, -2000000, 2000000);
#endif
	if (boyi_max_x == -1)
	{
		return 0;//AI无法行动 
	}
	printf("%c%c\n", boyi_max_x + '1', boyi_max_y + 'a');
#ifdef AI_COLOR_BLACK
	putchess(boyi_max_x, boyi_max_y, BLACK);
#endif
#ifdef AI_COLOR_WHITE
	putchess(boyi_max_x, boyi_max_y, WHITE);
#endif
	refresh();
	return 1;
}

int gameover()
{
	int white_score = 0, black_score = 0;
	int i, j;
	for (i = 0;i < SIZE;i++)
	{
		for (j = 0;j < SIZE;j++)
		{
			if (canrev(i, j, chessboard, WHITE, 0)>0)
			{
				return 0;
			}
			if (canrev(i, j, chessboard, BLACK, 0)>0)
			{
				return 0;
			}
		}
	}
	for (i = 0;i<SIZE;i++)
	{
		for (j = 0;j<SIZE;j++)
		{
			if (chessboard[i][j] == WHITE)
			{
				white_score++;
			}
			else if (chessboard[i][j] == BLACK)
			{
				black_score++;
			}
		}
	}
#ifdef AI_COLOR_BLACK
	printf("You have %d chess.\n", white_score);
	printf("I have %d chess.\n", black_score);
	if (white_score>black_score)
	{
		printf("You win! Thanks to teach me a lot.\n");
}
	else if (white_score<black_score)
	{
		printf("You lose! Don\'t give up!\n");
	}
#endif
#ifdef AI_COLOR_WHITE
	printf("You have %d chess.\n", black_score);
	printf("I have %d chess.\n", white_score);
	if (white_score<black_score)
	{
		printf("You win! Thanks to teach me a lot.\n");
	}
	else if (white_score>black_score)
	{
		printf("You lose! Don\'t give up!\n");
	}
#endif
	else
	{
		printf("Duals!\n");
	}
	return 1;
}

main()
{
	printf("\nREVERSI\n");
#ifdef AI_COLOR_BLACK
	printf("You can go first on the first game, then we will take turns.\n");
#endif
#ifdef AI_COLOR_WHITE
	printf("I will go first on the first game, then we will take turns.\n");
#endif
	printf("You will be white - (O)\n");
	printf("I will be black - (X).\n");
	printf("Select a square for your move by typing a digit for the row\n");
	printf("and a letter for the column with no spaces between.");
	printf("\nGood luck! Press Enter to start.\n");
	char c;
	while (getchar()!='\n') {}
	fflush(stdin);

	while (1)
	{
		init();
		int i, j;
		while (1)
		{
			int player_move = 0, ai_move = 0;

#ifdef AI_COLOR_WHITE
			ai_move = runAI();
#endif

			for (i = 0;i < SIZE;i++)
			{
				for (j = 0;j < SIZE;j++)
				{
#ifdef AI_COLOR_BLACK
					if (chessboard[i][j] == WHITE_ || chessboard[i][j] == BOTH)
#endif
#ifdef AI_COLOR_WHITE
					if (chessboard[i][j] == BLACK_ || chessboard[i][j] == BOTH)
#endif
					{
						player_move = 1;
						break;
					}
				}
			}

			while (player_move)
			{

				char s[100] = { 0 };
				printf("Please enter your move (row column- no space):");
				scanf("%s", s);
				fflush(stdin);
				if (strlen(s) != 2)
				{
					printf("Not a valid move, try again.\n");
					continue;
				}
				int x = s[0] - '1';
				int y = s[1] - 'a';
				if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
				{
					printf("Not a valid move, try again.\n");
					continue;
				}
#ifdef AI_COLOR_BLACK
				if (chessboard[x][y] != WHITE_&&chessboard[x][y] != BOTH)
#endif
#ifdef AI_COLOR_WHITE
				if (chessboard[x][y] != BLACK_&&chessboard[x][y] != BOTH)
#endif
				{
					printf("Not a valid move, try again.\n");
					continue;
				}
#ifdef AI_COLOR_BLACK
				putchess(x, y, WHITE);
#endif
#ifdef AI_COLOR_WHITE
				putchess(x, y, BLACK);
#endif
				refresh();
				break;
			}

#ifdef AI_COLOR_BLACK
			ai_move = runAI();
#endif
			
			if (gameover()==1)
			{
				break;
			}
			else if (player_move == 0)
			{
				printf("You have to pass. Press enter.\n");
				while (getchar() != '\n') {}
			}
			else if (ai_move == 0)
			{
				printf("AI have to pass. You will go.\n");
			}
		}
		printf("\nDo you want to play again?(y/n)\n");
		while (1)
		{
			c = getchar();
			fflush(stdin);
			if (c == 'Y' || c == 'y')
			{
				break;
			}
			else if (c == 'N' || c == 'n')
			{
				printf("\nGoodbye!\n");
				getchar();
				return 0;
			}
		}
	}
	return 0;
}
