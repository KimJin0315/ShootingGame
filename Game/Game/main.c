#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 35
#define HEIGHT 29
#define EnemyMAX 1
#define PlayerX 19
#define PlayerY 29
#define Maxbullet 5
#define Maxitem 1
#define Itembullet 10

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

int itemb = 0;
int itemc = 15;
int items = 10;
int itemh = 1;
int ix[Maxitem] = { 0 };
int iy[Maxitem] = { 0 };
bool item[Maxitem] = { FALSE };

enum Item
{
	Mbullet = 1,

	Drange = 2,

	Speedup = 3,

	Healthup = 4

};

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
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
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

void CreateBullet()
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

						score += 10;
						
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

	// 아이템과 플레이어의 충돌 판정
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == TRUE)
		{
			if (abs(ix[i] - x) <= 1 && abs(iy[i] - y) <= 1)
			{
				// 아이템이 플레이어와 충돌한 경우
				GotoXY(ix[i], iy[i]);
				printf("  ");
				item[i] = FALSE;

				itemb = (rand() % 4) + 1;
				

			}
			else if (iy[i] > HEIGHT)
			{
				// 아이템이 화면을 벗어난 경우
				GotoXY(ix[i],iy[i]);
				printf("  ");
				item[i] = FALSE;
			}
			
		}
	}
}

void Score()
{
	GotoXY(0,0);
	printf("점수 : %d", score);
	if (score == 300)
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
		GotoXY(25, 15);
		printf("게임종료");
		
	}
}

void ItemSpawn()
{
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == FALSE)
		{
			ix[i] =rand() % 37 + 1;
			iy[i] = rand() % 30 + 10;
			item[i] = TRUE;
		}
	}
}

void ItemMove()
{
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == FALSE)
			continue;

		if (item[i] == TRUE)
		{
			GotoXY(ix[i], iy[i]);
			printf("★");
			
		}
	}
}

void Item()
{


	switch(itemb)
	{
		case Mbullet:	// 최대 총알 수 증가
		
			while (itemc > 0)
			{
				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				{
					for (int i = 0; i < itemc; i++)
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

				for (int i = 0; i < itemc; i++)
				{
					if (bullet[i] == TRUE)
					{
						GotoXY(bx[i], by[i]);
						SetColor(15,10);
						printf("↑");
						by[i]--;
						itemc--;
					}
				}
			}
			
			break;

		case Drange: // 공격 범위 증가
			

			
			break;

		case Speedup: // 이동범위 증가
			
			while (items > 0)
			{
				
				if (GetAsyncKeyState(VK_LEFT) & 0x8000)
				{
					x -= 3;
					if (x < 0) x = 0;
					
				}

				if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
				{
					x += 3;
					if (x > WIDTH) x = WIDTH;
					
				}

				if (GetAsyncKeyState(VK_UP) & 0x8000)
				{
					y -= 3;
					if (y < 0) y = 0;
					
				}

				if (GetAsyncKeyState(VK_DOWN) & 0x8000)
				{
					y += 3;
					if (y > HEIGHT) y = HEIGHT;
					
				}

				items--;

			}
	

			break;

		case Healthup: // 목숨 +1
	
			while (itemb == Healthup)
			{
				health++;
				itemb = 0;
			}
			break;

	}
		
	
	
}

void Itemshow()
{
	GotoXY(13, 0);
	printf("아이템 : %d", itemb);
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
		CreateBullet();
		
		SetColor(15, 5);
		EnemySpawn();
		EnemyMove();

		ItemSpawn();
		ItemMove();
		Item();
		SetColor(15, 0);
		Itemshow();
		

		Hit();

		SetColor(15, 0);
		Health();
		Score();
		
		

	
		Sleep(75);
		
	}



	return 0;
}