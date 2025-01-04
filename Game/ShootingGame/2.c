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
#define Maxitem 2
#define Itembullet 10

int ex[EnemyMAX] = { 0 };
int ey[EnemyMAX] = { 0 };
bool enemy[EnemyMAX] = { FALSE };
int enemynum = EnemyMAX;

int x = PlayerX, y = PlayerY;
int bx[Maxbullet] = { 0 };
int by[Maxbullet] = { 0 };
bool bullet[Maxbullet] = { FALSE };

int score = 0;
int health = 5;

int itemb = 0;

int ix[Maxitem] = { 0 };
int iy[Maxitem] = { 0 };
bool item[Maxitem] = { FALSE };

int bulletCount = Maxbullet; // �Ѿ� ��
bool speedUpActive = false;  // Speedup ������ Ȱ��ȭ ����
int speedUpDuration = 0;     // Speedup ������ ���� �ð�
bool tripleShotActive = false; // TripleShot ������ Ȱ��ȭ ����

enum Item
{
	Mbullet = 1,
	Drange = 2,
	Speedup = 3,
	Healthup = 4,
	TripleShot = 5 // ���ο� ������ �߰�
};

void GotoXY(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
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
	CONSOLE_CURSOR_INFO curInfo = { 0, };
	curInfo.dwSize = 1;
	curInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void Player()
{
	GotoXY(x, y);
	printf("��");
}

void EnemySpawn()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == FALSE)
		{
			ex[i] = 7 + (rand() % 10) * 2;
			ey[i] = 0;
			enemy[i] = TRUE;
		}
	}
}

void PlayerMove()
{
	int moveSpeed = speedUpActive ? 3 : 1;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		x -= moveSpeed;
		if (x < 0) x = 0;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		x += moveSpeed;
		if (x > WIDTH) x = WIDTH;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		y -= moveSpeed;
		if (y < 0) y = 0;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		y += moveSpeed;
		if (y > HEIGHT) y = HEIGHT;
	}
}

void CreateBullet()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (tripleShotActive)
		{
			// 3���� �Ѿ��� �߻� (����, �߾�, ������)
			for (int i = 0; i < bulletCount; i++)
			{
				if (bullet[i] == FALSE)
				{
					bx[i] = x - 1;  // ���� �Ѿ�
					by[i] = y - 1;
					bullet[i] = TRUE;

					bx[i + 1] = x;  // �߾� �Ѿ�
					by[i + 1] = y - 1;
					bullet[i + 1] = TRUE;

					bx[i + 2] = x + 1;  // ������ �Ѿ�
					by[i + 2] = y - 1;
					bullet[i + 2] = TRUE;

					break;
				}
			}
		}
		else
		{
			for (int i = 0; i < bulletCount; i++)
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
	}

	// �Ѿ� �̵�
	for (int i = 0; i < bulletCount; i++)
	{
		if (bullet[i] == TRUE)
		{
			GotoXY(bx[i], by[i]);
			printf("��");
			by[i]--;
		}
	}
}

void EnemyMove()
{
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == FALSE) continue;
		GotoXY(ex[i], ey[i]);
		printf("��");
		ey[i]++;
	}
}

void Hit()
{
	// �Ѿ� �浹 ����
	for (int i = 0; i < Maxbullet; i++)
	{
		if (bullet[i] == TRUE)
		{
			for (int j = 0; j < EnemyMAX; j++)
			{
				if (enemy[j] == TRUE && abs(bx[i] - ex[j]) <= 2 && by[i] == ey[j])
				{
					// �浹 �� ó��
					GotoXY(bx[i], by[i]);
					printf(" ");
					bullet[i] = FALSE;

					GotoXY(ex[j], ey[j]);
					printf(" ");
					enemy[j] = FALSE;

					score += 10;
					break;
				}
			}

			if (bullet[i] == TRUE && by[i] < 0)
			{
				// �Ѿ��� ȭ���� ��� ���
				GotoXY(bx[i], by[i]);
				printf(" ");
				bullet[i] = FALSE;
			}
		}
	}

	// ���� �÷��̾��� �浹 ����
	for (int i = 0; i < EnemyMAX; i++)
	{
		if (enemy[i] == TRUE && abs(ex[i] - x) <= 1 && ey[i] == y)
		{
			GotoXY(ex[i], ey[i]);
			printf(" ");
			enemy[i] = FALSE;

			health--;
		}
		else if (ey[i] > HEIGHT)
		{
			GotoXY(ex[i], ey[i]);
			printf(" ");
			enemy[i] = FALSE;

			score--;
		}
	}

	// �����۰� �÷��̾��� �浹 ����
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == TRUE && abs(ix[i] - x) <= 1 && abs(iy[i] - y) <= 1)
		{
			GotoXY(ix[i], iy[i]);
			printf("  ");
			item[i] = FALSE;

			itemb = (rand() % 5) + 1;  // ���ο� ������ �߰�
		}
		else if (iy[i] > HEIGHT)
		{
			GotoXY(ix[i], iy[i]);
			printf("  ");
			item[i] = FALSE;
		}
	}
}

void Score()
{
	GotoXY(0, 0);
	printf("���� : %d", score);
	if (score == 300)
	{
		system("cls");
		GotoXY(20, 15);
		printf("��������");
	}
	if (score <= 0) score = 0;
}

void Health()
{
	GotoXY(29, 0);
	printf("��� : %d", health);
	if (health <= 0)
	{
		system("cls");
		GotoXY(25, 15);
		printf("��������");
	}
}

void ItemSpawn()
{
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == FALSE)
		{
			ix[i] = rand() % 37 + 1;
			iy[i] = rand() % 30 + 10;
			item[i] = TRUE;
		}
	}
}

void ItemMove()
{
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == TRUE)
		{
			GotoXY(ix[i], iy[i]);
			printf("��");
		}
	}
}

void Item()
{
	switch (itemb)
	{
	case Mbullet:  // �Ѿ� �� ����
		bulletCount++;
		break;

	case Speedup:  // �̵� �ӵ� ����
		speedUpActive = true;
		speedUpDuration = 200;  // 200���� ���� ���� ���� ȿ�� ����
		break;

	case Healthup:  // ü�� ����
		health++;
		break;

	case TripleShot:  // 3�� ���� Ȱ��ȭ
		tripleShotActive = true;
		break;
	}
}

void Itemshow()
{
	GotoXY(13, 0);
	printf("������ : %d", itemb);
}

void ResetGame()
{
	x = PlayerX;
	y = PlayerY;
	health = 5;
	score = 0;
	bulletCount = Maxbullet;
	speedUpActive = false;
	speedUpDuration = 0;
	tripleShotActive = false;  // TripleShot ������ ��Ȱ��ȭ
	for (int i = 0; i < Maxbullet; i++) bullet[i] = false;
	for (int i = 0; i < EnemyMAX; i++) enemy[i] = false;
	for (int i = 0; i < Maxitem; i++) item[i] = false;
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

		// Speedup ������ ȿ�� ����
		if (speedUpActive)
		{
			speedUpDuration--;
			if (speedUpDuration <= 0)
			{
				speedUpActive = false;
			}
		}

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

		if (health <= 0 || score >= 300)
		{
			printf("\n���� ����! ���͸� ���� �����.");
			while (!_kbhit());  // Ű �Է� ���
			char key = _getch();
			if (key == 13)  // ���� Ű �Է� �� ���� ����
			{
				ResetGame();
			}
		}

		Sleep(75);
	}

	return 0;
}
