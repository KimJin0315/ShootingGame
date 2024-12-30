#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 35
#define HEIGHT 29
#define EnemyMAX 2
#define PlayerX 19
#define PlayerY 29
#define Maxbullet 5


int ex[EnemyMAX] = { 0 };
int	ey[EnemyMAX] = { 0 };
bool enemy[EnemyMAX] = { FALSE };
int enemynum = EnemyMAX;

int x = PlayerX, y = PlayerY;
int bx[Maxbullet] = { 0 };
int by[Maxbullet] = { 0 };
bool bullet[Maxbullet] = { FALSE };

int score = 0;
int health = 5;
int item = 0;

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

void CursorView()
{
	CONSOLE_CURSOR_INFO curInfo = {0, };
	curInfo.dwSize = 1;
	curInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void Player()
{
	GotoXY(x, y);
	printf("●");
}

void EnemySpawn()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == FALSE)
		{
			ex[i] = 7+ (rand() % 10) * 2;
			ey[i] = 0;
			enemy[i] = TRUE;
		}
	}
}

void PlayerMove()
{
	if (GetAsyncKeyState(VK_LEFT) )
	{
		x -= 1;
		if (x < 0) x = 0;
	}
                
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		x += 1;
		if (x > WIDTH) x = WIDTH;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		y -= 1;
		if (y < 0) y = 0;
	}
	
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		y += 1;
		if (y > HEIGHT ) y = HEIGHT;
	}
	
}

void CreatBullet()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		for (int i = 0; i < Maxbullet; i++)
		{
			if (bullet[i] == FALSE)
			{
				bx[i] = x;
				by[i] = y - 1;
				bullet[i] = TRUE;
				break;
			}
		}
	}
	for (int i = 0; i < Maxbullet; i++)
	{
		if (bullet[i] == TRUE)
		{
			GotoXY(bx[i], by[i]);
			printf("↑");
			by[i]--;
		}
	}

}

void EnemyMove()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == FALSE)
			continue;

		if (enemy[i] == TRUE)
		{
			GotoXY(ex[i], ey[i]);
			printf("■");
			ey[i]++;
				
		}
	}

}

void Hit()
{
	// 총알 충돌 판정
	for (int i = 0; i < Maxbullet; i++)
	{
		if (bullet[i] == TRUE)
		{
			for (int j = 0; j < EnemyMAX; j++)
			{
				if (enemy[j] == TRUE)
				{
					if (abs(bx[i] - ex[j]) <= 2 && by[i] == ey[j])
					{
						// 총알과 적이 충돌한 경우
						GotoXY(bx[i], by[i]);
						printf(" ");
						bullet[i] = FALSE;

						GotoXY(ex[j], ey[j]);
						printf(" ");
						enemy[j] = FALSE;

						score++;

						item = rand() % 15 + 1;
						break; // 충돌 시 반복문 종료
					}
				}
			}

			if (bullet[i] == TRUE && by[i] < 0)
			{
				// 총알이 화면을 벗어난 경우
				GotoXY(bx[i], by[i]);
				printf(" ");
				bullet[i] = FALSE;
			}
		}
	}

	// 적과 플레이어의 충돌 판정
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == TRUE)
		{
			if (abs(ex[i] - x) <= 1 && ey[i] == y)
			{
				// 적이 플레이어와 충돌한 경우
				GotoXY(ex[i], ey[i]);
				printf(" ");
				enemy[i] = FALSE;

				health--;
			}
			else if (ey[i] > HEIGHT)
			{
				// 적이 화면을 벗어난 경우
				GotoXY(ex[i], ey[i]);
				printf(" ");
				enemy[i] = FALSE;

				score--;
			}
		}
	}
}

void Score()
{
	GotoXY(0,0);
	printf("점수 : %d", score);
	if (score == 10)
	{
		system("cls");
		GotoXY(20, 15);
		printf("게임종료");

		
	}
	if (score <= 0) score = 0;

}

void Health()
{
	GotoXY(29, 0);
	printf("목숨 : %d", health);
	if (health <= 0)
	{
		system("cls");
		GotoXY(20, 15);
		printf("게임종료");
		
	}
}

void Item()
{

	switch (item)
	{
		case 1:
		break;

		case 3: 
		break;

		case 5: 
		break;

		case 7: 
		break;
	}
}

int main()
{
	
	CursorView();
	system("mode con cols=38 lines=30");
 	srand(time(NULL));
	SetTitle("Shooting game");

	while (1)
	{
		Clear();

		SetColor(15, 10);
		Player();
		PlayerMove();
		CreatBullet();
		
		SetColor(15, 5);
		EnemySpawn();
		EnemyMove();
		
		Hit();

		SetColor(15, 0);
		Health();
		Score();
		
		

	
		Sleep(75);
		
	}



	return 0;
}