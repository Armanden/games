#include "raylib.h"

const int screenW = 540;
const int screenH = 540;

const int gridSize = 9;
const float cellSize = 60.0f; // Size of each cell in pixels

// Structure to represent the cell's state
typedef struct {
    int value; // Value of the cell, 0 means empty
} Cell;

int main(void) {
    InitWindow(screenW, screenH, "Sudoku");
    SetTargetFPS(300);

    Cell cells[9][9]; // Fixed-size array for cells
    // Initialize cells
    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize; col++) {
            cells[row][col].value = 0; // All cells start empty
        }
    }

    int selectedRow = -1;
    int selectedCol = -1;

    while (!WindowShouldClose()) {
        // Start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // Draw the grid
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                float x = col * cellSize;
                float y = row * cellSize;

                // Determine color of cell
                Color cellColor = LIGHTGRAY;

                // If the cell is selected, change the color to green
                if (selectedRow == row && selectedCol == col) {
                    cellColor = GREEN; // Highlight selected cell
                }

                DrawRectangle(x, y, cellSize, cellSize, cellColor);
                DrawRectangleLines(x, y, cellSize, cellSize, BLACK);

                // Draw the cell value
                if (cells[row][col].value > 0) {
                    DrawText(TextFormat("%d", cells[row][col].value), 
                             x + cellSize / 2 - 10, 
                             y + cellSize / 2 - 10, 
                             20, BLACK);
                }
            }
        }

        // Check for mouse click to select cell
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            for (int row = 0; row < gridSize; row++) {
                for (int col = 0; col < gridSize; col++) {
                    float x = col * cellSize;
                    float y = row * cellSize;
                    if (mousePosition.x >= x && mousePosition.x <= x + cellSize &&
                        mousePosition.y >= y && mousePosition.y <= y + cellSize) {
                        selectedRow = row;
                        selectedCol = col;
                    }
                }
            }
        }

        // Keyboard input for numbers 1-9
        for (int num = 1; num <= 9; num++) {
            if (IsKeyPressed(num + KEY_ONE - 1)) { // Adjust for key mapping
                if (selectedRow >= 0 && selectedRow < gridSize &&
                    selectedCol >= 0 && selectedCol < gridSize) {
                    cells[selectedRow][selectedCol].value = num; // Set cell value
                }
            }
        }

        // Draw vertical lines
        DrawRectangle(screenH / 3, 0, 5, screenH, BLACK);
        DrawRectangle(2 * screenH / 3, 0, 5, screenH, BLACK);

        // Draw horizontal lines
        DrawRectangle(0, screenH / 3, screenW, 5, BLACK);
        DrawRectangle(0, 2 * screenH / 3, screenW, 5, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

