#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define SQUARE_SIZE 50
#define MAX_SPEED 3
#define MAX_LEVEL 10
#define SQUARES_PER_LEVEL 10
#define MAX_SQUARES (MAX_LEVEL * SQUARES_PER_LEVEL)

const int scX = 1000;
const int scY = 1000;
int speed = 5;

Vector2 squares[MAX_SQUARES];
Vector2 velocities[MAX_SQUARES];

void InitSquares(int count) {
    for (int i = 0; i < count; i++) {
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
    SetTargetFPS(500);

    srand((unsigned)time(NULL));

    // Game states
    bool gameStarted = false;
    bool gameOver = false;

    // Level and squares count
    int currentLevel = 2; // default level (2 -> 20 squares). Change as desired.
    if (currentLevel < 1) currentLevel = 1;
    if (currentLevel > MAX_LEVEL) currentLevel = MAX_LEVEL;
    int squaresCount = currentLevel * SQUARES_PER_LEVEL;

    // Score/time
    int score = 0;
    double lastScoreTime = 0.0; // using GetTime()
    int finalScore = 0;

    // Circle player
    Vector2 playerPos = { scX / 2.0f, scY / 2.0f };
    float playerRadius = 10.0f;

    // NOTE: Do NOT initialize squares here. They will be initialized only when the game actually starts.
    // This ensures no squares are spawned or shown on the level (start) screen.

    while (!WindowShouldClose()) {
        // Input handling for screens
        if (!gameStarted) {
            // Level selector on the start screen (no squares are initialized or drawn here)
            if (IsKeyPressed(KEY_RIGHT)) {
                if (currentLevel < MAX_LEVEL) {
                    currentLevel++;
                    squaresCount = currentLevel * SQUARES_PER_LEVEL;
                    // DO NOT call InitSquares here (no preview on level screen)
                }
            }
            if (IsKeyPressed(KEY_LEFT)) {
                if (currentLevel > 1) {
                    currentLevel--;
                    squaresCount = currentLevel * SQUARES_PER_LEVEL;
                    // DO NOT call InitSquares here (no preview on level screen)
                }
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)) {
                // Start the game: initialize squares for the chosen level
                gameStarted = true;
                gameOver = false;
                score = 0;
                lastScoreTime = GetTime();
                // Reset player to center
                playerPos.x = scX / 2.0f;
                playerPos.y = scY / 2.0f;
                // Initialize squares for the chosen level (actually spawn them now)
                squaresCount = currentLevel * SQUARES_PER_LEVEL;
                InitSquares(squaresCount);
            }
            if (IsKeyPressed(KEY_ESCAPE)) break;
        } else if (gameOver) {
            // On game over, allow restart or quit
            if (IsKeyPressed(KEY_R)) {
                // Return to start screen (keeps last selected level)
                // Do NOT initialize squares here so the level screen has no squares.
                gameStarted = false;
                gameOver = false;
                score = 0;
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
            for (int i = 0; i < squaresCount; i++) {
                squares[i].x += velocities[i].x;
                squares[i].y += velocities[i].y;

                // Bounce off walls (clamp position and reverse)
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
            for (int i = 0; i < squaresCount; i++) {
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
            // Start screen with level selector (no squares shown)
            const char *title = "DODGE THE SQUARES";
            int titleFontSize = 40;
            int infoFontSize = 20;

            DrawText(title, scX/2 - MeasureText(title, titleFontSize)/2, scY/6, titleFontSize, GOLD);

            // Level display
            char levelText[64];
            snprintf(levelText, sizeof(levelText), "Level: %d   (Squares: %d)", currentLevel, currentLevel * SQUARES_PER_LEVEL);
            DrawText(levelText, scX/2 - MeasureText(levelText, infoFontSize)/2, scY/6 + 80, infoFontSize, LIGHTGRAY);

            const char *instr1 = "Use LEFT / RIGHT to change level";
            const char *instr2 = "Press ENTER or SPACE to start";
            const char *instr3 = "Arrow keys to move. Press ESC to quit.";
            const char *instr4 = "Press R during Game Over to return here.";

            DrawText(instr1, scX/2 - MeasureText(instr1, infoFontSize)/2, scY/6 + 110, infoFontSize, LIGHTGRAY);
            DrawText(instr2, scX/2 - MeasureText(instr2, infoFontSize)/2, scY/6 + 140, infoFontSize, LIGHTGRAY);
            DrawText(instr3, scX/2 - MeasureText(instr3, infoFontSize)/2, scY/6 + 170, infoFontSize, LIGHTGRAY);
            DrawText(instr4, scX/2 - MeasureText(instr4, infoFontSize)/2, scY/6 + 200, infoFontSize, LIGHTGRAY);

            // Draw player in center (so user can see starting position)
            DrawCircleV(playerPos, playerRadius, GREEN);
        } else if (gameOver) {
            // Draw squares frozen (from the game that just ended)
            for (int i = 0; i < squaresCount; i++) {
                DrawRectangleV(squares[i], (Vector2){SQUARE_SIZE, SQUARE_SIZE}, BLUE);
            }
            // Draw player as red (since collision happened)
            DrawCircleV(playerPos, playerRadius, RED);

            // Game over text and final score
            char scoreText[64];
            snprintf(scoreText, sizeof(scoreText), "GAME OVER! Final score: %d", finalScore);
            DrawText(scoreText, scX/2 - MeasureText(scoreText, 24)/2, scY/3, 24, RED);

            const char *restartText = "Press R to return to start screen (change level) or ESC to quit.";
            DrawText(restartText, scX/2 - MeasureText(restartText, 20)/2, scY/3 + 50, 20, LIGHTGRAY);
        } else {
            // Normal gameplay draw
            // Draw squares
            for (int i = 0; i < squaresCount; i++) {
                DrawRectangleV(squares[i], (Vector2){SQUARE_SIZE, SQUARE_SIZE}, BLUE);
            }

            // Draw player
            DrawCircleV(playerPos, playerRadius, GREEN);

            // Draw score (current, not yet multiplied)
            char scoreText[32];
            snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
            DrawText(scoreText, 10, 10, 20, WHITE);

            // Draw level in corner during play
            char levelHud[32];
            snprintf(levelHud, sizeof(levelHud), "Level: %d", currentLevel);
            DrawText(levelHud, scX - MeasureText(levelHud, 20) - 10, 10, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    // Close the window
    CloseWindow();
    return 0;
}
