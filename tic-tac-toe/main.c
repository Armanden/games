//tic-tac-toe

#include <raylib.h>
#include <stdio.h>

const int scX = 540;
const int scY = 540;
const int gridSize = 3;
const float cellSize = 180.0f;

int main(void) {

    InitWindow(scX, scY, "Grid Example");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);
	
        // Draw grid cells
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                int x = col * cellSize;
                int y = row * cellSize;

                DrawRectangle(x, y, cellSize, cellSize, RAYWHITE);       // Fill
                DrawRectangleLines(x, y, cellSize, cellSize, BLACK);    // Outline



		
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

