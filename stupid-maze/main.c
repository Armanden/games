#include <raylib.h>
#include <stdlib.h>
#include <time.h>

/* ============================================================================
 * MAZE GENERATOR - Recursive Backtracking Algorithm
 * ============================================================================
 * 
 * This program generates and renders a random maze using the recursive 
 * backtracking (depth-first search) algorithm:
 * 
 * 1. Start with a grid where all cells have all 4 walls
 * 2. Pick a starting cell, mark it as visited
 * 3. While there are unvisited cells:
 *    a. If current cell has unvisited neighbors:
 *       - Choose random unvisited neighbor
 *       - Remove wall between current and chosen cell
 *       - Push current cell to stack
 *       - Make chosen cell the current cell
 *    b. Else if stack is not empty:
 *       - Pop cell from stack and make it current
 *    c. Else:
 *       - Pick new unvisited cell (for disconnected mazes)
 * 
 * This produces a "perfect maze" - one with exactly one path between any
 * two cells, with no loops.
 * ============================================================================*/

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 900

/* Number of cells in the maze grid */
#define CELL_COUNT_X  30
#define CELL_COUNT_Y  22

/* Size of each cell in pixels */
#define CELL_SIZE     40

/* Wall thickness in pixels */
#define WALL_THICKNESS 4

/* Colors */
#define BG_COLOR      DARKGRAY
#define WALL_COLOR  BLACK
#define PATH_COLOR  LIGHTGRAY
#define SOLVE_COLOR GOLD

/* Direction vectors for the 4 cardinal directions */
typedef enum {
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3,
    DIR_COUNT = 4
} Direction;

/* A single cell in the maze grid.
 * Each cell tracks which walls are still standing. */
typedef struct {
    bool walls[DIR_COUNT];  /* true = wall exists, false = passage opened */
    bool visited;           /* whether this cell has been processed */
} Cell;

/* A position in the maze grid */
typedef struct {
    int x, y;
} Position;

/* The maze grid */
static Cell maze[CELL_COUNT_X][CELL_COUNT_Y];

/* Stack for backtracking during maze generation */
static Position stack[CELL_COUNT_X * CELL_COUNT_Y];
static int stackTop = -1;

/* ============================================================================
 * STACK OPERATIONS
 * Used during maze generation to track our path for backtracking
 * ============================================================================*/

static void stackPush(int x, int y) {
    stackTop++;
    stack[stackTop].x = x;
    stack[stackTop].y = y;
}

static Position stackPop(void) {
    Position p = stack[stackTop];
    stackTop--;
    return p;
}

static bool stackIsEmpty(void) {
    return stackTop == -1;
}

/* ============================================================================
 * DIRECTION UTILITIES
 * ============================================================================*/

/* Check if a direction leads to a valid cell within the grid */
static bool isValidCell(int x, int y) {
    return x >= 0 && x < CELL_COUNT_X && y >= 0 && y < CELL_COUNT_Y;
}

/* Get the neighbor position in a given direction */
static Position getNeighbor(int x, int y, Direction dir) {
    Position p = {x, y};
    switch (dir) {
        case DIR_UP:    p.y--; break;
        case DIR_RIGHT: p.x++; break;
        case DIR_DOWN:  p.y++; break;
        case DIR_LEFT:  p.x--; break;
        default: break;
    }
    return p;
}

/* Get the opposite direction (for removing walls between cells) */
static Direction getOpposite(Direction dir) {
    return (dir + 2) % DIR_COUNT;
}

/* ============================================================================
 * MAZE GENERATION - Recursive Backtracking Algorithm
 * ============================================================================*/

static void generateMaze(int startX, int startY) {
    int visitCount = 1;
    int totalCells = CELL_COUNT_X * CELL_COUNT_Y;
    int x = startX, y = startY;
    
    /* Initialize: all cells have 4 walls, none visited */
    for (int i = 0; i < CELL_COUNT_X; i++) {
        for (int j = 0; j < CELL_COUNT_Y; j++) {
            maze[i][j].walls[DIR_UP]    = true;
            maze[i][j].walls[DIR_RIGHT] = true;
            maze[i][j].walls[DIR_DOWN]  = true;
            maze[i][j].walls[DIR_LEFT]  = true;
            maze[i][j].visited = false;
        }
    }
    
    /* Start at the given position */
    maze[x][y].visited = true;
    stackPush(x, y);
    
    /* Carve passages until we've visited every cell */
    while (visitCount < totalCells) {
        /* Find all unvisited neighbors of current cell */
        Position neighbors[DIR_COUNT];
        Direction validDirs[DIR_COUNT];
        int neighborCount = 0;
        
        for (Direction d = 0; d < DIR_COUNT; d++) {
            Position n = getNeighbor(x, y, d);
            if (isValidCell(n.x, n.y) && !maze[n.x][n.y].visited) {
                neighbors[neighborCount] = n;
                validDirs[neighborCount] = d;
                neighborCount++;
            }
        }
        
        /* If there are unvisited neighbors, pick one at random */
        if (neighborCount > 0) {
            int choice = rand() % neighborCount;
            Position chosen = neighbors[choice];
            Direction dir = validDirs[choice];
            Direction opposite = getOpposite(dir);
            
            /* Remove walls between current cell and chosen neighbor */
            maze[x][y].walls[dir] = false;
            maze[chosen.x][chosen.y].walls[opposite] = false;
            
            /* Move to the chosen cell */
            x = chosen.x;
            y = chosen.y;
            maze[x][y].visited = true;
            visitCount++;
            
            /* Push current position to stack for potential backtracking */
            stackPush(x, y);
        }
        /* No unvisited neighbors - backtrack to previous cell */
        else if (!stackIsEmpty()) {
            Position prev = stackPop();
            x = prev.x;
            y = prev.y;
        }
    }
}

/* ============================================================================
 * RENDERING
 * ============================================================================*/

/* Draw a single wall segment */
static void drawWall(int cellX, int cellY, Direction dir) {
    int px = cellX * CELL_SIZE;
    int py = cellY * CELL_SIZE;
    
    switch (dir) {
        case DIR_UP:
            DrawRectangle(px, py, CELL_SIZE, WALL_THICKNESS, WALL_COLOR);
            break;
        case DIR_DOWN:
            DrawRectangle(px, py + CELL_SIZE - WALL_THICKNESS, CELL_SIZE, WALL_THICKNESS, WALL_COLOR);
            break;
        case DIR_LEFT:
            DrawRectangle(px, py, WALL_THICKNESS, CELL_SIZE, WALL_COLOR);
            break;
        case DIR_RIGHT:
            DrawRectangle(px + CELL_SIZE - WALL_THICKNESS, py, WALL_THICKNESS, CELL_SIZE, WALL_COLOR);
            break;
        default:
            break;
    }
}

static void drawMaze(void) {
    /* Fill background */
    ClearBackground(BG_COLOR);
    
    /* Draw each cell's walls */
    for (int x = 0; x < CELL_COUNT_X; x++) {
        for (int y = 0; y < CELL_COUNT_Y; y++) {
            for (Direction d = 0; d < DIR_COUNT; d++) {
                if (maze[x][y].walls[d]) {
                    drawWall(x, y, d);
                }
            }
        }
    }
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================*/

int main(void) {
    /* Initialize random number generator with time-based seed */
    srand((unsigned int)time(NULL));
    
    /* Calculate actual window size based on maze grid */
    int windowW = CELL_COUNT_X * CELL_SIZE;
    int windowH = CELL_COUNT_Y * CELL_SIZE;
    
    /* Create window and set target framerate */
    InitWindow(windowW, windowH, "Maze Generator");
    SetTargetFPS(60);
    
    /* Generate the maze using recursive backtracking */
    generateMaze(0, 0);
    
    /* Main render loop */
    while (!WindowShouldClose()) {
        /* Handle input: press SPACE or R to regenerate maze */
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_R)) {
            generateMaze(rand() % CELL_COUNT_X, rand() % CELL_COUNT_Y);
        }
        
        /* Draw the maze */
        BeginDrawing();
        drawMaze();
        
        /* Draw instructions at the bottom */
        DrawText("Press SPACE or R to generate new maze", 
                 10, windowH - 30, 20, LIGHTGRAY);
        
        EndDrawing();
    }
    
    /* Cleanup */
    CloseWindow();
    return 0;
}