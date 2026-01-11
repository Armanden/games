#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define SQUARE_SIZE 50
#define NUMBER_OF_SQUARES 20
#define MAX_SPEED 3

const int scX = 1000;
const int scY = 1000;
int speed = 5;

void IntToString(int value, char* buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "%d", value);
}

Vector2 squares[NUMBER_OF_SQUARES];
Vector2 velocities[NUMBER_OF_SQUARES];

void InitSquares(void) {
    for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
        squares[i].x = rand() % (GetScreenWidth() - SQUARE_SIZE);
        squares[i].y = rand() % (GetScreenHeight() - SQUARE_SIZE);

        velocities[i].x = (rand() % (MAX_SPEED * 2 + 1)) - MAX_SPEED; // -MAX..MAX
        velocities[i].y = (rand() % (MAX_SPEED * 2 + 1)) - MAX_SPEED; // -MAX..MAX

        // Ensure the squares have some movement
        if (velocities[i].x == 0) velocities[i].x = 1;
        if (velocities[i].y == 0) velocities[i].y = 1;
    }
}

int main() {
    // Initialize Raylib
    InitWindow(scX, scY, "Dodge the Squares");
    SetTargetFPS(60);

    srand((unsigned)time(NULL));

    // Game states
    bool gameStarted = false;
    bool gameOver = false;

    // Score/time
    int score = 0;
    double lastScoreTime = 0.0; // using GetTime()
    int finalScore = 0;

    // Circle player
    Vector2 playerPos = { scX / 2.0f, scY / 2.0f };
    float playerRadius = 10.0f;

    // Initialize squares
    InitSquares();

    while (!WindowShouldClose()) {
        // Input handling for screens
        if (!gameStarted) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)) {
                // Start the game
                gameStarted = true;
                gameOver = false;
                score = 0;
                lastScoreTime = GetTime();
                // Reset player to center
                playerPos.x = scX / 2.0f;
                playerPos.y = scY / 2.0f;
                // re-init squares for fresh start
                InitSquares();
            }
            if (IsKeyPressed(KEY_ESCAPE)) break;
        } else if (gameOver) {
            // On game over, allow restart or quit
            if (IsKeyPressed(KEY_R)) {
                // Restart: go back to start screen (or you can set gameStarted=true to restart immediately)
                gameStarted = false;
                gameOver = false;
                score = 0;
                // re-init squares
                InitSquares();
            }
            if (IsKeyPressed(KEY_ESCAPE)) break;
        } else {
            // Gameplay input (only when game started and not over)
            if (IsKeyDown(KEY_RIGHT) && (playerPos.x + playerRadius + speed <= GetScreenWidth())) {
                playerPos.x += speed;
            }
            if (IsKeyDown(KEY_LEFT) && (playerPos.x - playerRadius - speed >= 0)) {
                playerPos.x -= speed;
            }
            if (IsKeyDown(KEY_UP) && (playerPos.y - playerRadius - speed >= 0)) {
                playerPos.y -= speed;
            }
            if (IsKeyDown(KEY_DOWN) && (playerPos.y + playerRadius + speed <= GetScreenHeight())) {
                playerPos.y += speed;
            }

            // Update score every 1 second
            double now = GetTime();
            if (now - lastScoreTime >= 1.0) {
                score++;
                lastScoreTime = now;
            }

            // Update square positions and check for wall collisions
            for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
                squares[i].x += velocities[i].x;
                squares[i].y += velocities[i].y;

                // Bounce off walls
                if (squares[i].x <= 0) {
                    squares[i].x = 0;
                    velocities[i].x *= -1;
                }
                if (squares[i].x >= scX - SQUARE_SIZE) {
                    squares[i].x = scX - SQUARE_SIZE;
                    velocities[i].x *= -1;
                }
                if (squares[i].y <= 0) {
                    squares[i].y = 0;
                    velocities[i].y *= -1;
                }
                if (squares[i].y >= scY - SQUARE_SIZE) {
                    squares[i].y = scY - SQUARE_SIZE;
                    velocities[i].y *= -1;
                }
            }

            // Collision detection
            bool collisionDetected = false;
            for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
                Rectangle square = { squares[i].x, squares[i].y, SQUARE_SIZE, SQUARE_SIZE };
                if (CheckCollisionCircleRec(playerPos, playerRadius, square)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (collisionDetected) {
                gameOver = true;
                finalScore = score * 10; // apply multiplier at game over
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameStarted) {
            // Start screen
            const char *title = "DODGE THE SQUARES";
            const char *instr1 = "Press ENTER or SPACE to start";
            const char *instr2 = "Arrow keys to move. Press ESC to quit.";
            const char *instr3 = "Press R after game over to return here.";
            int titleFontSize = 40;
            int infoFontSize = 20;
            DrawText(title, scX/2 - MeasureText(title, titleFontSize)/2, scY/3, titleFontSize, GOLD);
            DrawText(instr1, scX/2 - MeasureText(instr1, infoFontSize)/2, scY/3 + 80, infoFontSize, LIGHTGRAY);
            DrawText(instr2, scX/2 - MeasureText(instr2, infoFontSize)/2, scY/3 + 110, infoFontSize, LIGHTGRAY);
            DrawText(instr3, scX/2 - MeasureText(instr3, infoFontSize)/2, scY/3 + 140, infoFontSize, LIGHTGRAY);
        } else if (gameOver) {
            // Draw squares frozen
            for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
                DrawRectangleV(squares[i], (Vector2){SQUARE_SIZE, SQUARE_SIZE}, BLUE);
            }
            // Draw player as red (since collision happened)
            DrawCircleV(playerPos, playerRadius, RED);

            // Game over text and final score
            char scoreText[64];
            snprintf(scoreText, sizeof(scoreText), "GAME OVER! Final score: %d", finalScore);
            DrawText(scoreText, scX/2 - MeasureText(scoreText, 24)/2, scY/3, 24, RED);

            const char *restartText = "Press R to restart (go to start screen) or ESC to quit.";
            DrawText(restartText, scX/2 - MeasureText(restartText, 20)/2, scY/3 + 50, 20, LIGHTGRAY);
        } else {
            // Normal gameplay draw
            // Draw squares
            for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
                DrawRectangleV(squares[i], (Vector2){SQUARE_SIZE, SQUARE_SIZE}, BLUE);
            }

            // Draw player
            DrawCircleV(playerPos, playerRadius, GREEN);

            // Draw score (current, not yet multiplied)
            char scoreText[32];
            IntToString(score, scoreText, sizeof(scoreText));
            DrawText(scoreText, 10, 10, 20, WHITE);
        }

        EndDrawing();
    }

    // Close the window
    CloseWindow();
    return 0;
}
