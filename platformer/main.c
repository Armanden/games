#include <stdio.h>
#include <math.h>
#include <raylib.h>

int main(void)
{
    const int screenW = 1200;
    const int screenH = 900;
    InitWindow(screenW, screenH, "Platformer");
    SetTargetFPS(120);

    // Player
    float px = 10.0f;
    const float radius = 10.0f;
    float py = (float)screenH - radius; // start on ground

    // Movement / physics
    const float speed = 300.0f;     // horizontal speed (px/s)
    const float gravity = 1500.0f;  // gravity accel (px/s^2)
    const float jumpForce = 650.0f; // initial jump velocity (px/s)
    float vy = 0.0f;                // vertical velocity
    bool onGround = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Horizontal input
        if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && (px + radius + speed * dt <= screenW))
            px += speed * dt;
        if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && (px - radius - speed * dt >= 0))
            px -= speed * dt;

        // Jump (only if on ground)
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && onGround)
        {
            vy = -jumpForce *1.5 ;
            onGround = false;
        }

        // Apply gravity and integrate position
        vy += gravity * dt *1.2;
        py += vy * dt *1.2;

        // Ground collision
        float groundY = (float)screenH - radius;
        if (py > groundY)
        {
            py = groundY;
            vy = 0.0f;
            onGround = true;
        }

	 Rectangle plat = { 400, 650, 80, 20 };
	 Color platColor = GREEN;

	
        BeginDrawing();
        ClearBackground(BLACK);
	DrawRectangleRec(plat,platColor);
        DrawCircle((int)roundf(px), (int)roundf(py), (int)radius, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
