// 처음으로 한줄 이상 완성되면, 다음 1개의 블록만 2배의 속도로 떨어진다.
// 한개의 블록만 빠르게 떨어지고, 그 다음무터 정상 속도로 떨어진다.

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include "block.h"

#define LEFT 75
#define RIGHT 77
#define UP 72
#define SPACE 32

/* Size of gameboard */
#define  GBOARD_WIDTH    10
#define  GBOARD_HEIGHT   20

/* Starting point of gameboard */
#define  GBOARD_ORIGIN_X  4
#define  GBOARD_ORIGIN_Y  2

int curPosX = 0;
int curPosY = 0;
int block_id;
int speed = 50;	// speed는 수가 작아질수록 빨라짐
int gameBoardInfo[GBOARD_HEIGHT+1][GBOARD_WIDTH+2] = {0};
int cnt = 0;

void SetCurrentCursorPos(int x, int y);
COORD GetCurrentCursorPos(void);
void RemoveCursor(void);
void ShowBlock(char blockInfo[4][4]);
void DeleteBlock(char blockInfo[4][4]);
int ShiftRight();
int ShiftLeft();
int RotateBlock();
int BlockDown();
void SpaceDown();
void ProcessKeyInput();
void DrawGameBoard();
int DetectCollision(int posX, int posY, char blockInfo[4][4]);
void AddBlockToBoard(void);
int IsGameOver(void);
void RemoveFillUpLine(void);
void RedrawBlocks(void);
void RevProcessKeyInput();

int main()
{
	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;
		gameBoardInfo[y][GBOARD_WIDTH+1] = 1;
	}

	for (int x = 0; x < GBOARD_WIDTH+2; x++)
		gameBoardInfo[GBOARD_HEIGHT][x] = 1;

	srand((unsigned int)time(NULL));
	block_id = (rand()%7)*4;

	RemoveCursor();
	DrawGameBoard();

	curPosX = 16;
	curPosY = 2;

	// _kbhit : 키보드 입력을 겁사, 입력 발생 : 1, 없으면 : 0
	// _getch() : 입력된 값의 아스키코드 반환

	while (1)
	{
		if (IsGameOver())
			break;

		while (1)
		{
			if (BlockDown() == 0)
			{
				AddBlockToBoard();

				srand((unsigned int)time(NULL));
				block_id = (rand()%7)*4;

				curPosX = 16;
				curPosY = 2;
				SetCurrentCursorPos(curPosX, curPosY);

				ShowBlock(blockModel[block_id]);

				break;
			}
			if (cnt >= 1)
				speed = 20;
			else
				speed = 50;;
			ProcessKeyInput();
		}
		RemoveFillUpLine();
	}

	SetCurrentCursorPos(14, 2);
	puts("Game Over!!");

	getchar();


	return 0;
}

void SetCurrentCursorPos(int x, int y)	// 현재 위치 설정
{
	COORD pos = {x, y};

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetCurrentCursorPos(void)	// 현재 위치 반환
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}

void RemoveCursor(void)	// 커서 지우기
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void ShowBlock(char blockInfo[4][4])	// 블록 보여주기
{
	COORD curPos = GetCurrentCursorPos();

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + x*2, curPos.Y + y);

			if (blockInfo[y][x] == 1)
				printf("■");
		}
	}

	SetCurrentCursorPos(curPos.X, curPos.Y);	// 원래 위치로
}

void DeleteBlock(char blockInfo[4][4])	// 블록 지우기
{
	COORD curPos = GetCurrentCursorPos();

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + x*2, curPos.Y + y);

			if (blockInfo[y][x] == 1)
				printf(" ");
		}
	}

	SetCurrentCursorPos(curPos.X, curPos.Y);	// 원래 위치로
}

int ShiftRight()	// 오른쪽 이동
{
	if (!DetectCollision(curPosX+2, curPosY, blockModel[block_id]))
		return 0;

	DeleteBlock(blockModel[block_id]);
	curPosX += 2;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);

	return 1;
}

int ShiftLeft()	// 왼쪽 이동
{
	if (!DetectCollision(curPosX-2, curPosY, blockModel[block_id]))
		return 0;

	DeleteBlock(blockModel[block_id]);
	curPosX -= 2;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);

	return 1;
}

int RotateBlock()	// 90도 회전
{
	int block_base = block_id - block_id%4;
	int block_rotated = block_base+(block_id+1)%4;

	if (!DetectCollision(curPosX, curPosY, blockModel[block_rotated]))
		return 0;

	DeleteBlock(blockModel[block_id]);
	block_id = block_rotated;
	ShowBlock(blockModel[block_id]);

	return 1;
}

int BlockDown()	// 아래 이동
{
	if (!DetectCollision(curPosX, curPosY+1, blockModel[block_id]))
		return 0;	// 충돌하면 안내려가고 그냥 종료

	DeleteBlock(blockModel[block_id]);
	curPosY += 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);

	return 1;
}

void SpaceDown()	// 하단으로 한번에 이동
{
	while (BlockDown());
}

void ProcessKeyInput()	// 키 입력
{
	for (int i = 0; i < 10; i++)
	{
		if (_kbhit() != 0)
		{
			int key = _getch();

			switch (key)
			{
			case LEFT:	ShiftLeft();	break;
			case RIGHT:	ShiftRight();	break;
			case UP:	RotateBlock();	break;
			case SPACE: SpaceDown();	break;
			}
		}
		Sleep(speed);
	}
}

void DrawGameBoard()	// 게임판 그리기
{
	for (int y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y+y);
		
		if (y == GBOARD_HEIGHT)
			printf("└");
		else
			printf("│");
	}

	for (int y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X+(GBOARD_WIDTH+1)*2, GBOARD_ORIGIN_Y+y);
		
		if (y == GBOARD_HEIGHT)
			printf("┘");
		else
			printf("│");
	}

	for (int x = 1; x <= GBOARD_WIDTH; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X+x*2, GBOARD_ORIGIN_Y+GBOARD_HEIGHT);
		printf("─");
	}
	
	SetCurrentCursorPos(GBOARD_ORIGIN_X+GBOARD_WIDTH, GBOARD_ORIGIN_Y);
}

int DetectCollision(int posX, int posY, char blockInfo[4][4])	// 충돌 검사
{
	// posX, posY : 콘솔의 좌표
	// arrX, arrY : 블록의 좌표
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;	// 네모가 x는 2배 잡아먹어서
	int arrY = (posY - GBOARD_ORIGIN_Y);

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if ((blockInfo[y][x] == 1) && (gameBoardInfo[arrY+y][arrX+x] == 1))	// 2차원배열 인덱스
				return 0;	// 충돌하면 0
		}
	}

	return 1;	// 충돌 안하면 1
}

void AddBlockToBoard(void)	// 게임판에 블록 추가해서 업데이트
{
	int arrX = (curPosX - GBOARD_ORIGIN_X) / 2;
	int arrY = (curPosY - GBOARD_ORIGIN_Y);

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (blockModel[block_id][y][x] == 1)
				gameBoardInfo[arrY+y][arrX+x] = 1;
		}
	}
}

int IsGameOver(void)	// 게임 종료 검사
{
	if (!DetectCollision(curPosX, curPosY, blockModel[block_id])) {
		return 1;
	}
	return 0;
}

void RemoveFillUpLine(void)	// 줄 지우기
{
	int x;
	cnt = 0;

	for (int y = GBOARD_HEIGHT-1; y > 0; y--)
	{
		for (x = 1; x <= GBOARD_WIDTH; x++)
		{
			if (gameBoardInfo[y][x] == 0)	// if (gameBoardInfo[y][x] != 1)
				break;
		}

		if (x == GBOARD_WIDTH+1)
		{
			for (int line = 0; y-line > 0; line++)	// y번째 라인 완성
			{
				memcpy(&gameBoardInfo[y-line][1], &gameBoardInfo[(y-line)-1][1], GBOARD_WIDTH*sizeof(int));
				
			}
			cnt++;
			y++;	// 돌아가서 다시 검사
		}
	}

	RedrawBlocks();
}

void RedrawBlocks(void)	// 게임판 다시 그리기
{
	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (int x = 1; x <= GBOARD_WIDTH; x++)
		{
			int cursX = x*2 + GBOARD_ORIGIN_X;
			int cursY = y + GBOARD_ORIGIN_Y;
			
			SetCurrentCursorPos(cursX, cursY);
			
			if (gameBoardInfo[y][x] == 1)
			{
				printf("■");
			}
			
			else	printf("  ");
		}
	}
}

void RevProcessKeyInput()	// 키 입력
{
	for (int i = 0; i < 10; i++)
	{
		if (_kbhit() != 0)
		{
			int key = _getch();

			switch (key)
			{
			case LEFT:	ShiftRight();	break;
			case RIGHT:	ShiftLeft();	break;
			case UP:	RotateBlock();	break;
			case SPACE: SpaceDown();	break;
			}
		}
		Sleep(speed);
	}
}
