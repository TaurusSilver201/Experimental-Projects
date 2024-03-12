#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_WIDTH 1
#define PADDLE_HEIGHT 5
#define MAX_SCORE 10

// Function prototypes
void initGame(int *player1, int *player2, int *ballX, int *ballY, int *ballDirX, int *ballDirY, int *player1Score, int *player2Score);
void drawGame(int player1, int player2, int ballX, int ballY, int player1Score, int player2Score);
void movePaddles(int *player1, int *player2);
void moveBall(int *ballX, int *ballY, int *ballDirX, int *ballDirY, int player1, int player2);
int collisionDetection(int ballX, int ballY, int player1, int player2);
void resetBall(int *ballX, int *ballY, int *ballDirX, int *ballDirY);
void playSound(int soundType);

int main() {
    int player1, player2, ballX, ballY, ballDirX, ballDirY, player1Score = 0, player2Score = 0;
    char key;

    initGame(&player1, &player2, &ballX, &ballY, &ballDirX, &ballDirY, &player1Score, &player2Score);

    while (player1Score < MAX_SCORE && player2Score < MAX_SCORE) {
        drawGame(player1, player2, ballX, ballY, player1Score, player2Score);
        movePaddles(&player1, &player2);
        moveBall(&ballX, &ballY, &ballDirX, &ballDirY, player1, player2);

        // Check for collision with paddles or walls
        int collision = collisionDetection(ballX, ballY, player1, player2);
        if (collision == 1 || collision == 2) {
            ballDirX = -ballDirX;
            playSound(1);  // Play paddle hit sound
        } else if (collision == 3 || collision == 4) {
            ballDirY = -ballDirY;
            playSound(1);  // Play wall hit sound
        } else if (collision == 5) {
            player2Score++;
            resetBall(&ballX, &ballY, &ballDirX, &ballDirY);
            playSound(2);  // Play score sound
        } else if (collision == 6) {
            player1Score++;
            resetBall(&ballX, &ballY, &ballDirX, &ballDirY);
            playSound(2);  // Play score sound
        }

        Sleep(50);  // Delay for smooth animation

        // Check if 'q' is pressed to quit the game
        if (kbhit()) {
            key = getch();
            if (key == 'q' || key == 'Q') {
                break;
            }
        }
    }

    // Game over
    system("cls");
    printf("Game Over!\n");
    printf("Player 1 Score: %d\n", player1Score);
    printf("Player 2 Score: %d\n", player2Score);

    if (player1Score == MAX_SCORE) {
        printf("Player 1 wins!\n");
    } else {
        printf("Player 2 wins!\n");
    }

    return 0;
}

void initGame(int *player1, int *player2, int *ballX, int *ballY, int *ballDirX, int *ballDirY, int *player1Score, int *player2Score) {
    *player1 = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    *player2 = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    *ballX = WIDTH / 2;
    *ballY = HEIGHT / 2;
    *ballDirX = 1;
    *ballDirY = 1;
    *player1Score = 0;
    *player2Score = 0;
}

void drawGame(int player1, int player2, int ballX, int ballY, int player1Score, int player2Score) {
    system("cls");  // Clear the console

    // Draw the scores
    printf("Player 1: %d   Player 2: %d\n", player1Score, player2Score);

    // Draw the playing field
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1) {
                printf("|");
            } else if (y == player1 || (y >= player1 && y < player1 + PADDLE_HEIGHT)) {
                printf("#");
            } else if (y == player2 || (y >= player2 && y < player2 + PADDLE_HEIGHT)) {
                printf("#");
            } else if (x == ballX && y == ballY) {
                printf("O");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void movePaddles(int *player1, int *player2) {
    char key;

    if (kbhit()) {
        key = getch();

        switch (key) {
            case 'w':
            case 'W':
                if (*player1 > 0) {
                    (*player1)--;
                }
                break;
            case 's':
            case 'S':
                if (*player1 < HEIGHT - PADDLE_HEIGHT) {
                    (*player1)++;
                }
                break;
            case 'i':
            case 'I':
                if (*player2 > 0) {
                    (*player2)--;
                }
                break;
            case 'k':
            case 'K':
                if (*player2 < HEIGHT - PADDLE_HEIGHT) {
                    (*player2)++;
                }
                break;
        }
    }
}

void moveBall(int *ballX, int *ballY, int *ballDirX, int *ballDirY, int player1, int player2) {
    *ballX += *ballDirX;
    *ballY += *ballDirY;
}

int collisionDetection(int ballX, int ballY, int player1, int player2) {
    // Check for collision with paddles
    if (ballX == 1 && (ballY >= player1 && ballY < player1 + PADDLE_HEIGHT)) {
        return 1;  // Left paddle
    } else if (ballX == WIDTH - 2 && (ballY >= player2 && ballY < player2 + PADDLE_HEIGHT)) {
        return 2;  // Right paddle
    }

    // Check for collision with top and bottom walls
    if (ballY == 0 || ballY == HEIGHT - 1) {
        return 3;  // Top or bottom wall
    }

    // Check if the ball goes out of bounds (left or right)
    if (ballX == 0) {
        return 5;  // Out of bounds (left)
    } else if (ballX == WIDTH - 1) {
        return 6;  // Out of bounds (right)
    }

    return 0;  // No collision
}

void resetBall(int *ballX, int *ballY, int *ballDirX, int *ballDirY) {
    *ballX = WIDTH / 2;
    *ballY = HEIGHT / 2;
    *ballDirX = (rand() % 2 == 0) ? 1 : -1;
    *ballDirY = (rand() % 2 == 0) ? 1 : -1;
}

void playSound(int soundType) {
    // Play sound effect based on the sound type
    // You can use the Windows Beep function or include a sound library
    // Example: Beep(frequency, duration)
    switch (soundType) {
        case 1:  // Paddle or wall hit
            Beep(500, 50);
            break;
        case 2:  // Score
            Beep(1000, 200);
            break;
    }
}
