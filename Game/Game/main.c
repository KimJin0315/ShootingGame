#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 40
#define HEIGHT 29
#define EnemyMAX 5
#define PlayerX 21
#define PlayerY 29
#define Maxbullet 10

int ex[EnemyMAX] = { 0 };
int	ey[EnemyMAX] = { 0 };
bool enemy[EnemyMAX] = { FALSE };

int x = PlayerX, y = PlayerY;
int bx[Maxbullet] = { 0 };
int by[Maxbullet] = { 0 };
bool bullet[Maxbullet] = { FALSE };



//#pragma region buffer
//
//int screenIndex;
//HANDLE screen[2];
//
//void Buffer()
//{
//	screen[0] = CreateConsoleScreenBuffer
//	(
//		GENERIC_READ | GENERIC_WRITE,
//		0,
//		NULL,
//		CONSOLE_TEXTMODE_BUFFER,
//		NULL
//	);
//
//	screen[1] = CreateConsoleScreenBuffer
//	(
//		GENERIC_READ | GENERIC_WRITE,
//		0,
//		NULL,
//		CONSOLE_TEXTMODE_BUFFER,
//		NULL
//	);
//}
// 
// void Flip()
//{
//	SetConsoleActiveScreenBuffer(screen[screenIndex]);
//
//	screenIndex = !screenIndex;
//
//}
//
//void Clear()
//{
//	COORD position = { 0,0 };
//
//	DWORD dword;
//
//	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
//
//	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
//
//	GetConsoleScreenBufferInfo(handle, &consoleInfo);
//
//	int width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
//	int	height = consoleInfo.srWindow.Top - consoleInfo.srWindow.Bottom + 1;
//
//	FillConsoleOutputCharacter(screen[screenIndex], ' ', width * height, position, &dword);
//}
//
//void Release()
//{
//	CloseHandle(screen[0]);
//	CloseHandle(screen[1]);
//}
//
//void Render(int x, int y, const char* shape)
//{
//	DWORD dword;
//
//	// x축과 y축을 설정
//	COORD position = { x , y };
//
//	// 커서 위치를 이동하는 함수
//	SetConsoleCursorPosition(screen[screenIndex], position);
//
//	WriteFile(screen[screenIndex], shape, strlen(shape), &dword, NULL);
//}
//
//#pragma endregion


void GotoXY(int x,int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

COORD GetXY()
{
	COORD pos;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	pos.X = curInfo.dwCursorPosition.X;
	pos.Y = curInfo.dwCursorPosition.Y;

	return pos;

}

void Clear()
{
	system("cls");
}

void SetTitle(char* ConsoleName)
{
	SetConsoleTitle(ConsoleName);
}

void SetColor(unsigned char BgColor, unsigned char TextColor)
{
	if (BgColor > 15 || TextColor > 15) return;

	unsigned short ColorNum = (BgColor << 4) | TextColor;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ColorNum);
}

//void SetCursor()
//{
//	CONSOLE_CURSOR_INFO curInfo = {0, };
//	curInfo.dwSize = 1;
//	curInfo.bVisible = FALSE;
//	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
//}

void Player()
{
	GotoXY(x, y);
	printf("<p>");
}

void EnemySpawn()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (!enemy[i])
		{
			ex[i] = (rand() % 20) * 2;
			ey[i] = 0;
			enemy[i] = TRUE;
			break;
		}
	}
}

void PlayerMove()
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		x--;
		if (x < 0) x = 0;
	}
                
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		x++;
		if (x > WIDTH) x = WIDTH;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		y--;
		if (y < 0) y = 0;
	}
	
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		y++;
		if (y > HEIGHT ) y = HEIGHT;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		for (int i = 0; i < Maxbullet; i++)
		{
			if (!bullet[i])
			{
				bx[i] = x + 1;
				by[i] = y - 1;
				bullet[i] = TRUE;
				break;
			}

		}
	}

	for (int i = 0; i < Maxbullet; i++)
	{
		if (bullet[i])
		{
			GotoXY(bx[i], by[i]);
			printf("↑");
			by[i]--;

			if (abs(ex[i] - bx[i]) <= 2 && ey[i] - by[i])
			{

				bullet[i] = FALSE;
				break;
			}

			if (by[i] < 0)
			{
				bullet[i] = FALSE; 
			}

				
		}
	}
}

void EnemyMove()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i])
		{
			GotoXY(ex[i], ey[i]);
			printf("■");
			ey[i]++;
				
			if (abs(ex[i] - x) <=2 && ey[i] == y )
			{
				enemy[i] = FALSE;
			}

			else if (abs(ex[i] - bx[i]) <= 2 && ey[i] - by[i])
			{
				
				enemy[i] = FALSE;
				break;
			}

			else if (ey[i] > HEIGHT)
			{
				enemy[i] = FALSE;
			}

		}
	}
}

int main()
{
 	srand(time(NULL));
	SetTitle("Shooting game");

	while (1)
	{
		
		Clear();
		
		EnemySpawn();
		PlayerMove();
		EnemyMove();
		Player();


		

	
		Sleep(100);
		
	}


	system("pause");
	return 0;
}