#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 50
#define HEIGHT 20
#define PLAYER_LIFE 5

typedef struct {
    int x, y;
    int is_alive;
} Player;

typedef struct {
    int x, y;
    int is_active;
} Bullet;

typedef struct {
    int x, y;
    int is_active;
    int is_invincible;
    clock_t invincible_start;
} Enemy;

typedef struct {
    int x, y;
    int type; // 1: 연사 속도 증가, 2: 적 총알 소멸, 3: 총알 범위 증가, 4: 목숨 +1, 5: 일시무적
    int is_active;
} Item;

Player player;
Bullet player_bullets[WIDTH];
Enemy enemies[WIDTH];
Bullet enemy_bullets[WIDTH];
Item items[WIDTH];

int player_life = PLAYER_LIFE;
int score = 0;
int player_bullet_speed = 1;
int player_bullet_range = 1;
int item_invincible = 0;
clock_t item_invincible_start;

void init_game() {
    player.x = WIDTH / 2;
    player.y = HEIGHT - 1;
    player.is_alive = 1;

    for (int i = 0; i < WIDTH; i++) {
        player_bullets[i].is_active = 0;
        enemies[i].is_active = 0;
        enemy_bullets[i].is_active = 0;
        items[i].is_active = 0;
    }
}

void draw_game() {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (player.is_alive && player.x == j && player.y == i) {
                printf("P");
            }
            else {
                int drawn = 0;
                for (int k = 0; k < WIDTH; k++) {
                    if (player_bullets[k].is_active && player_bullets[k].x == j && player_bullets[k].y == i) {
                        printf("^");
                        drawn = 1;
                        break;
                    }
                    else if (enemies[k].is_active && enemies[k].x == j && enemies[k].y == i) {
                        printf("E");
                        drawn = 1;
                        break;
                    }
                    else if (enemy_bullets[k].is_active && enemy_bullets[k].x == j && enemy_bullets[k].y == i) {
                        printf("v");
                        drawn = 1;
                        break;
                    }
                    else if (items[k].is_active && items[k].x == j && items[k].y == i) {
                        printf("*");
                        drawn = 1;
                        break;
                    }
                }
                if (!drawn) printf(" ");
            }
        }
        printf("\n");
    }
    printf("Life: %d, Score: %d\n", player_life, score);
}

void update_player() {
    if (_kbhit()) {
        char input = _getch();
        if (input == 75 && player.x > 0) player.x--; // Left arrow
        else if (input == 77 && player.x < WIDTH - 1) player.x++; // Right arrow
        else if (input == 72 && player.y > 0) player.y--; // Up arrow
        else if (input == 80 && player.y < HEIGHT - 1) player.y++; // Down arrow
        else if (input == 32) { // Space bar
            for (int i = 0; i < WIDTH; i++) {
                if (!player_bullets[i].is_active) {
                    player_bullets[i].x = player.x;
                    player_bullets[i].y = player.y - 1;
                    player_bullets[i].is_active = 1;
                    break;
                }
            }
        }
    }
}

void update_bullets() {
    for (int i = 0; i < WIDTH; i++) {
        if (player_bullets[i].is_active) {
            player_bullets[i].y -= player_bullet_speed;
            if (player_bullets[i].y < 0) player_bullets[i].is_active = 0;
        }
    }
}

void update_enemies() {
    for (int i = 0; i < WIDTH; i++) {
        if (enemies[i].is_active) {
            if (clock() - enemies[i].invincible_start > 1000) {
                enemies[i].is_invincible = 0;
            }
            enemies[i].y++;
            if (enemies[i].y >= HEIGHT) {
                enemies[i].is_active = 0;
                score--;
            }
        }
        else {
            if (rand() % 20 == 0) {
                enemies[i].x = rand() % WIDTH;
                enemies[i].y = 0;
                enemies[i].is_active = 1;
                enemies[i].is_invincible = 1;
                enemies[i].invincible_start = clock();
            }
        }
    }
}

void check_collisions() {
    for (int i = 0; i < WIDTH; i++) {
        if (player_bullets[i].is_active) {
            for (int j = 0; j < WIDTH; j++) {
                if (enemies[j].is_active && !enemies[j].is_invincible && player_bullets[i].x == enemies[j].x && player_bullets[i].y == enemies[j].y) {
                    player_bullets[i].is_active = 0;
                    enemies[j].is_active = 0;
                    score++;
                    if (rand() % 10 == 0) {
                        for (int k = 0; k < WIDTH; k++) {
                            if (!items[k].is_active) {
                                items[k].x = enemies[j].x;
                                items[k].y = enemies[j].y;
                                items[k].type = rand() % 5 + 1;
                                items[k].is_active = 1;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void update_items() {
    for (int i = 0; i < WIDTH; i++) {
        if (items[i].is_active) {
            items[i].y++;
            if (items[i].y >= HEIGHT) {
                items[i].is_active = 0;
            }
            else if (items[i].x == player.x && items[i].y == player.y) {
                switch (items[i].type) {
                case 1:
                    player_bullet_speed++;
                    break;
                case 2:
                    for (int j = 0; j < WIDTH; j++) {
                        enemy_bullets[j].is_active = 0;
                    }
                    break;
                case 3:
                    player_bullet_range++;
                    break;
                case 4:
                    player_life++;
                    break;
                case 5:
                    item_invincible = 1;
                    item_invincible_start = clock();
                    break;
                }
                items[i].is_active = 0;
            }
        }
    }
    if (item_invincible && clock() - item_invincible_start > 5000) {
        item_invincible = 0;
    }
}

void update_enemy_bullets() {
    for (int i = 0; i < WIDTH; i++) {
        if (enemy_bullets[i].is_active) {
            enemy_bullets[i].y++;
            if (enemy_bullets[i].y >= HEIGHT) {
                enemy_bullets[i].is_active = 0;
            }
            else if (enemy_bullets[i].x == player.x && enemy_bullets[i].y == player.y) {
                enemy_bullets[i].is_active = 0;
                if (!item_invincible) player_life--;
            }
        }
    }
}

void spawn_enemy_bullets() {
    for (int i = 0; i < WIDTH; i++) {
        if (enemies[i].is_active && !enemies[i].is_invincible) {
            if (rand() % 20 == 0) {
                for (int j = 0; j < WIDTH; j++) {
                    if (!enemy_bullets[j].is_active) {
                        enemy_bullets[j].x = enemies[i].x;
                        enemy_bullets[j].y = enemies[i].y + 1;
                        enemy_bullets[j].is_active = 1;
                        break;
                    }
                }
            }
        }
    }
}

int main() {
    srand(time(NULL));
    init_game();

    while (player_life > 0 && score < 100) {
        draw_game();
        update_player();
        update_bullets();
        update_enemies();
        check_collisions();
        update_items();
        update_enemy_bullets();
        spawn_enemy_bullets();
        Sleep(100);
    }

    printf("Game Over! Final Score: %d\n", score);
    return 0;
}
