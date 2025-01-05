#pragma region 헤더파일

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#pragma endregion

#pragma region 정의

#define WIDTH 35
#define HEIGHT 29
#define EnemyMAX 5
#define PlayerX 19
#define PlayerY 29
#define Maxbullet 5
#define Maxitem 2
#define MaxbulletDoubleShot 20 // 더블샷 활성화 시 최대 총알 수
#define MaxbulletMbullet 10  // Mbullet 아이템 활성화 시 최대 총알 수

#pragma endregion

#pragma region 전역변수

int ex[EnemyMAX] = { 0 };
int ey[EnemyMAX] = { 0 };
bool enemy[EnemyMAX] = { FALSE };
int enemynum = EnemyMAX;

int x = PlayerX, y = PlayerY; // 플레이어 시작 좌표

// 점수, 목숨
int score = 0;
int health = 5;

#pragma endregion

#pragma region 아이템

int itemb = 0;

int ix[Maxitem] = { 0 };
int iy[Maxitem] = { 0 };
bool item[Maxitem] = { FALSE };

int bulletCount = Maxbullet; // 총알 수

int maxBulletTemp = Maxbullet;  // 일시적으로 총알 수를 늘리기 위한 변수
clock_t MbulletStartTime = 0;  // Mbullet 아이템 활성화 시작 시간
bool MbulletActive = FALSE;  // Mbullet 아이템 활성화 여부

int bx[MaxbulletDoubleShot] = { 0 };
int by[MaxbulletDoubleShot] = { 0 };
bool bullet[MaxbulletDoubleShot] = { FALSE };

bool doubleShotActive = FALSE; // doubleShot 아이템 활성화 여부
int doubleShotStage = 0; // 현재 발사 단계 (0: 중앙, 1: 왼쪽, 2: 오른쪽)
clock_t lastShotTime = 0;      // 마지막 총알 발사 시간

unsigned long doubleShotStartTime = 0; // 더블샷 아이템을 활성화한 시간
unsigned long doubleShotDuration = 5000; // 더블샷 효과 시간 (5초)

int speedUpDuration = 0;     // Speedup 아이템 지속 시간
bool speedUpActive = FALSE;  // Speedup 아이템 활성화 여부
clock_t speedUpStartTime = 0;    // Speedup 아이템 효과 시작 시간
const int SPEEDUP_DURATION_MS = 5000;  // 5초 (10000ms)

enum Item
{
	Mbullet = 1,
	DoubleShot = 2,
	Speedup = 3,
	Healthup = 4
};

#pragma endregion

#pragma region 게임시스템함수

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
	printf("●");
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
	int maxBullet = doubleShotActive ? MaxbulletDoubleShot : maxBulletTemp; // 현재 최대 총알 수 결정

	// 스페이스바 입력 확인
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (doubleShotActive) // 두 줄 총알 활성화 상태
		{
			for (int i = 0; i < maxBullet - 1; i++) // 두 줄 발사를 위해 두 칸 여유 확인
			{
				if (bullet[i] == FALSE && bullet[i + 1] == FALSE)
				{
					// 왼쪽 총알
					bx[i] = x - 1; // 플레이어 기준 왼쪽으로 1칸
					by[i] = y - 1;
					bullet[i] = TRUE;

					// 오른쪽 총알
					bx[i + 1] = x + 1; // 플레이어 기준 오른쪽으로 1칸
					by[i + 1] = y - 1;
					bullet[i + 1] = TRUE;

					break; // 2개의 총알을 한 번에 생성했으므로 반복문 종료
				}
			}
		}
		else // 일반 총알 발사
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

	// 기존 총알 이동 처리
	for (int i = 0; i < maxBullet; i++)
	{
		if (bullet[i] == TRUE)
		{
			GotoXY(bx[i], by[i]);
			printf(" ");            // 이전 위치 지우기
			by[i]--;                // 총알 이동

			if (by[i] < 0)          // 화면 밖으로 나간 경우 비활성화
			{
				bullet[i] = FALSE;
			}
			else
			{
				GotoXY(bx[i], by[i]);
				printf("↑");        // 새 위치에 총알 표시
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
		printf("■");
		ey[i]++;
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
				if (enemy[j] == TRUE && abs(bx[i] - ex[j]) <= 2 && by[i] == ey[j])
				{
					// 충돌 시 처리
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

	// 아이템과 플레이어의 충돌 판정
	for (int i = 0; i < Maxitem; i++)
	{
		if (item[i] == TRUE && abs(ix[i] - x) <= 1 && abs(iy[i] - y) <= 1)
		{
			GotoXY(ix[i], iy[i]);
			printf("  ");
			item[i] = FALSE;

			itemb = (rand() % 4) + 1;  // 새로운 아이템 추가
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
	printf("점수 : %d", score);
	if (score >= 300)
	{
		system("cls");
		GotoXY(15, 15);
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
		GotoXY(15, 15);
		printf("게임종료");
	}
}

void BulletCountDisplay()
{
	GotoXY(0, 1);  // 총알 수가 화면의 상단 왼쪽에 출력되도록 위치 설정
	printf("최대 총알 수 : %d", bulletCount);  // 현재 최대 총알 수 출력
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
			printf("★");
		}
	}
}

void Item()
{
	switch (itemb)
	{
	case Mbullet:  // 총알 수 증가
		if (!MbulletActive)  // 이미 활성화된 상태에서 다시 호출되지 않도록
		{
			MbulletActive = TRUE;
			maxBulletTemp = MaxbulletMbullet;  // 최대 총알 수를 늘림
			MbulletStartTime = clock();  // 활성화 시작 시간 저장
		}
		itemb = 0;
		break;

	case DoubleShot:  // 3줄 공격 활성화
		if (!doubleShotActive)  // 이미 활성화된 상태에서 다시 호출하지 않도록
		{
			doubleShotActive = true;
			doubleShotStartTime = GetTickCount64(); // 활성화 시작 시간 저장

			// 이전에 발사된 총알이 있으면 모두 비활성화
			for (int i = 0; i < MaxbulletDoubleShot; i++)
			{
				bullet[i] = FALSE; // 이전 총알 비활성화
			}
		}
		itemb = 0;
		break;

	case Speedup:  // 이동 속도 증가
		speedUpActive = true;
		speedUpStartTime = clock();  // 현재 시간 저장
		itemb = 0;
		break;

	case Healthup:  // 체력 증가
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
		if ((currentTime - MbulletStartTime) * 1000 / CLOCKS_PER_SEC >= 5000)  // 5초 경과 후 비활성화
		{
			MbulletActive = FALSE;
			maxBulletTemp = Maxbullet;  // 총알 수를 원래대로 복원
		}
	}
}

void ApplyDoubleShotItem()
{
	doubleShotActive = TRUE; // 더블샷 효과 활성화
	doubleShotStartTime = GetTickCount64(); // 현재 시간 기록
}

void CheckDoubleShot()
{
	if (doubleShotActive)
	{
		if (GetTickCount64() - doubleShotStartTime >= doubleShotDuration)
		{
			doubleShotActive = FALSE; // 10초 후 더블샷 비활성화
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
			// 10초(10000ms)가 경과한 경우 비활성화
			speedUpActive = false;
		}
	}
}

#pragma endregion

#pragma region 게임시작함수

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
	printf("도움말");
	GotoXY(0, 9);
	printf("조작 : 방향키로 이동");
	GotoXY(0, 10);
	printf("발사 : 스페이스바");
	GotoXY(0, 11);
	printf("아이템은 습득 시 5초간 자동 사용");
	GotoXY(0, 12);
	printf("아이템 1: 최대 총알 증가");
	GotoXY(0, 13);
	printf("아이템 2: 총알 두 줄 발사");
	GotoXY(0, 14);
	printf("아이템 3: 이동 속도 증가");
	GotoXY(0, 15);
	printf("아이템 4: 목숨 1 증가");
	GotoXY(0, 17);
	printf("Esc 키를 눌러 메뉴로 돌아가기");

	while (1)
	{
		if (_kbhit())
		{
			char key = _getch();
			if (key == 27)  // Esc 키
				return;  // 메뉴로 돌아가기
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
		printf("게임 시작 화면");
		GotoXY(12, 9);
		printf("1. 게임 시작");
		GotoXY(12, 10);
		printf("2. 도움말");
		GotoXY(12, 11);
		printf("3. 게임 종료");

		GotoXY(12, 13);
		printf("선택 : ");
		choice = _getch();  // 키 입력 받기

		if (choice == '1')
		{
			break;  // 게임 시작
		}
		else if (choice == '2')
		{
			ShowHelp();  // 도움말 화면
		}
		else if (choice == '3')
		{
			exit(0);  // 게임 종료
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
