#pragma region �������

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#pragma endregion

#pragma region ����

#define WIDTH 35
#define HEIGHT 29
#define EnemyMAX 5
#define PlayerX 19
#define PlayerY 29
#define Maxbullet 5
#define Maxitem 2
#define MaxbulletDoubleShot 20 // ���� Ȱ��ȭ �� �ִ� �Ѿ� ��
#define MaxbulletMbullet 10  // Mbullet ������ Ȱ��ȭ �� �ִ� �Ѿ� ��

#pragma endregion

#pragma region ��������

int ex[EnemyMAX] = { 0 };
int ey[EnemyMAX] = { 0 };
bool enemy[EnemyMAX] = { FALSE };
int enemynum = EnemyMAX;

int x = PlayerX, y = PlayerY; // �÷��̾� ���� ��ǥ

// ����, ���
int score = 0;
int health = 5;

#pragma endregion

#pragma region ������

int itemb = 0;

int ix[Maxitem] = { 0 };
int iy[Maxitem] = { 0 };
bool item[Maxitem] = { FALSE };

int bulletCount = Maxbullet; // �Ѿ� ��

int maxBulletTemp = Maxbullet;  // �Ͻ������� �Ѿ� ���� �ø��� ���� ����
clock_t MbulletStartTime = 0;  // Mbullet ������ Ȱ��ȭ ���� �ð�
bool MbulletActive = FALSE;  // Mbullet ������ Ȱ��ȭ ����

int bx[MaxbulletDoubleShot] = { 0 };
int by[MaxbulletDoubleShot] = { 0 };
bool bullet[MaxbulletDoubleShot] = { FALSE };

bool doubleShotActive = FALSE; // doubleShot ������ Ȱ��ȭ ����
int doubleShotStage = 0; // ���� �߻� �ܰ� (0: �߾�, 1: ����, 2: ������)
clock_t lastShotTime = 0;      // ������ �Ѿ� �߻� �ð�

unsigned long doubleShotStartTime = 0; // ���� �������� Ȱ��ȭ�� �ð�
unsigned long doubleShotDuration = 5000; // ���� ȿ�� �ð� (5��)

int speedUpDuration = 0;     // Speedup ������ ���� �ð�
bool speedUpActive = FALSE;  // Speedup ������ Ȱ��ȭ ����
clock_t speedUpStartTime = 0;    // Speedup ������ ȿ�� ���� �ð�
const int SPEEDUP_DURATION_MS = 5000;  // 5�� (10000ms)

enum Item
{
	Mbullet = 1,
	DoubleShot = 2,
	Speedup = 3,
	Healthup = 4
};

#pragma endregion

#pragma region ���ӽý����Լ�

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
	int moveSpeed = speedUpActive ? 2 : 1;

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
	int maxBullet = doubleShotActive ? MaxbulletDoubleShot : maxBulletTemp; // ���� �ִ� �Ѿ� �� ����

	// �����̽��� �Է� Ȯ��
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (doubleShotActive) // �� �� �Ѿ� Ȱ��ȭ ����
		{
			for (int i = 0; i < maxBullet - 1; i++) // �� �� �߻縦 ���� �� ĭ ���� Ȯ��
			{
				if (bullet[i] == FALSE && bullet[i + 1] == FALSE)
				{
					// ���� �Ѿ�
					bx[i] = x - 1; // �÷��̾� ���� �������� 1ĭ
					by[i] = y - 1;
					bullet[i] = TRUE;

					// ������ �Ѿ�
					bx[i + 1] = x + 1; // �÷��̾� ���� ���������� 1ĭ
					by[i + 1] = y - 1;
					bullet[i + 1] = TRUE;

					break; // 2���� �Ѿ��� �� ���� ���������Ƿ� �ݺ��� ����
				}
			}
		}
		else // �Ϲ� �Ѿ� �߻�
		{
			for (int i = 0; i < maxBullet; i++)
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

	// ���� �Ѿ� �̵� ó��
	for (int i = 0; i < maxBullet; i++)
	{
		if (bullet[i] == TRUE)
		{
			GotoXY(bx[i], by[i]);
			printf(" ");            // ���� ��ġ �����
			by[i]--;                // �Ѿ� �̵�

			if (by[i] < 0)          // ȭ�� ������ ���� ��� ��Ȱ��ȭ
			{
				bullet[i] = FALSE;
			}
			else
			{
				GotoXY(bx[i], by[i]);
				printf("��");        // �� ��ġ�� �Ѿ� ǥ��
			}
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

			itemb = (rand() % 4) + 1;  // ���ο� ������ �߰�
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
	if (score >= 300)
	{
		system("cls");
		GotoXY(15, 15);
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
		GotoXY(15, 15);
		printf("��������");
	}
}

void BulletCountDisplay()
{
	GotoXY(0, 1);  // �Ѿ� ���� ȭ���� ��� ���ʿ� ��µǵ��� ��ġ ����
	printf("�ִ� �Ѿ� �� : %d", bulletCount);  // ���� �ִ� �Ѿ� �� ���
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
		if (!MbulletActive)  // �̹� Ȱ��ȭ�� ���¿��� �ٽ� ȣ����� �ʵ���
		{
			MbulletActive = TRUE;
			maxBulletTemp = MaxbulletMbullet;  // �ִ� �Ѿ� ���� �ø�
			MbulletStartTime = clock();  // Ȱ��ȭ ���� �ð� ����
		}
		itemb = 0;
		break;

	case DoubleShot:  // 3�� ���� Ȱ��ȭ
		if (!doubleShotActive)  // �̹� Ȱ��ȭ�� ���¿��� �ٽ� ȣ������ �ʵ���
		{
			doubleShotActive = true;
			doubleShotStartTime = GetTickCount64(); // Ȱ��ȭ ���� �ð� ����

			// ������ �߻�� �Ѿ��� ������ ��� ��Ȱ��ȭ
			for (int i = 0; i < MaxbulletDoubleShot; i++)
			{
				bullet[i] = FALSE; // ���� �Ѿ� ��Ȱ��ȭ
			}
		}
		itemb = 0;
		break;

	case Speedup:  // �̵� �ӵ� ����
		speedUpActive = true;
		speedUpStartTime = clock();  // ���� �ð� ����
		itemb = 0;
		break;

	case Healthup:  // ü�� ����
		health++;
		itemb = 0;
		break;
	}


}

void CheckMbulletDuration()
{
	if (MbulletActive)
	{
		clock_t currentTime = clock();
		if ((currentTime - MbulletStartTime) * 1000 / CLOCKS_PER_SEC >= 5000)  // 5�� ��� �� ��Ȱ��ȭ
		{
			MbulletActive = FALSE;
			maxBulletTemp = Maxbullet;  // �Ѿ� ���� ������� ����
		}
	}
}

void ApplyDoubleShotItem()
{
	doubleShotActive = TRUE; // ���� ȿ�� Ȱ��ȭ
	doubleShotStartTime = GetTickCount64(); // ���� �ð� ���
}

void CheckDoubleShot()
{
	if (doubleShotActive)
	{
		if (GetTickCount64() - doubleShotStartTime >= doubleShotDuration)
		{
			doubleShotActive = FALSE; // 10�� �� ���� ��Ȱ��ȭ
		}
	}
}

void CheckSpeedupDuration()
{
	if (speedUpActive)
	{
		clock_t currentTime = clock();
		if ((currentTime - speedUpStartTime) * 1000 / CLOCKS_PER_SEC >= SPEEDUP_DURATION_MS)
		{
			// 10��(10000ms)�� ����� ��� ��Ȱ��ȭ
			speedUpActive = false;
		}
	}
}

#pragma endregion

#pragma region ���ӽ����Լ�

void GameStart()
{
	CursorView();

	srand(time(NULL));
	SetTitle(TEXT("Shooting game"));

	while (1)
	{
		Clear();

		CheckMbulletDuration();
		CheckDoubleShot();
		CheckSpeedupDuration();

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

		Hit();

		SetColor(15, 0);
		Health();
		Score();
		BulletCountDisplay();

		Sleep(75);
	}
}

void ShowHelp()
{
	Clear();

	GotoXY(0, 7);
	printf("����");
	GotoXY(0, 9);
	printf("���� : ����Ű�� �̵�");
	GotoXY(0, 10);
	printf("�߻� : �����̽���");
	GotoXY(0, 11);
	printf("�������� ���� �� 5�ʰ� �ڵ� ���");
	GotoXY(0, 12);
	printf("������ 1: �ִ� �Ѿ� ����");
	GotoXY(0, 13);
	printf("������ 2: �Ѿ� �� �� �߻�");
	GotoXY(0, 14);
	printf("������ 3: �̵� �ӵ� ����");
	GotoXY(0, 15);
	printf("������ 4: ��� 1 ����");
	GotoXY(0, 17);
	printf("Esc Ű�� ���� �޴��� ���ư���");

	while (1)
	{
		if (_kbhit())
		{
			char key = _getch();
			if (key == 27)  // Esc Ű
				return;  // �޴��� ���ư���
		}
	}
}

void ShowStartMenu()
{
	int choice = 0;

	while (1)
	{
		Clear();
		system("mode con cols=38 lines=30");


		GotoXY(12, 7);
		printf("���� ���� ȭ��");
		GotoXY(12, 9);
		printf("1. ���� ����");
		GotoXY(12, 10);
		printf("2. ����");
		GotoXY(12, 11);
		printf("3. ���� ����");

		GotoXY(12, 13);
		printf("���� : ");
		choice = _getch();  // Ű �Է� �ޱ�

		if (choice == '1')
		{
			break;  // ���� ����
		}
		else if (choice == '2')
		{
			ShowHelp();  // ���� ȭ��
		}
		else if (choice == '3')
		{
			exit(0);  // ���� ����
		}
	}
}

#pragma endregion

int main()
{
	ShowStartMenu();
	GameStart();

	return 0;
}
