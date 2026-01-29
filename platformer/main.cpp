//Welcomo to this fuckton of a mess this is code that I wrote and than gave gpt and copilot to review and bug fix but as ai can't write code it has turned into this fuckton of a mess that took me 4 hours to debug becaus ai is so good, I no longer know what this code does I just know it works.
#include <raylib.h>
#include <vector>
#include <cmath>

// Helper: clamp value between a and b
static float Clamp(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}
 const int screenW = 1200;
    const int screenH = 900;

class Plat {
public:
    Rectangle plat;
    Color platColor = GREEN;

    // Refactored collision handling with better side collision detection
    void collision(float &px, float &py, float &vy, bool &onGround, const float radius, const float dt) {
        // --- Platform collision (circle vs rect) ---
        float closestX = Clamp(px, plat.x, plat.x + plat.width);
        float closestY = Clamp(py, plat.y, plat.y + plat.height);

        float dx = px - closestX;
        float dy = py - closestY;
        float distSq = dx * dx + dy * dy;
        float rSq = radius * radius;

        if (distSq < rSq) {
            float dist = sqrtf(distSq);
            float nx = 0.0f, ny = -1.0f;
            if (dist > 0.0001f) {
                nx = dx / dist;
                ny = dy / dist;
            }

            float penetration = radius - dist;
            // Push circle out along normal by penetration amount
            px += nx * penetration;
            py += ny * penetration;

            // Interpret normal to determine contact type
            if (ny < -0.5f) {  // Landed on top of platform
                vy = 0.0f;
                onGround = true;
                py = plat.y - radius;
            }
            else if (ny > 0.5f) {  // Hit head (bottom of the platform)
                vy = 0.0f;
                py = plat.y + plat.height + radius;
            }
            else if (nx < -0.5f) {  // Left side collision
                px = plat.x - radius;            
            }
            else if (nx > 0.5f) {  // Right side collision
                px = plat.x + plat.width + radius;
            }
        } else {
            if (py < (float)screenH - radius - 0.5f) {
                onGround = false;
            }
        }
    }
};

int main(void) {
    const int screenW = 1200;
    const int screenH = 900;
    InitWindow(screenW, screenH, "Platformer");
    SetTargetFPS(120);

    // Player (circle)
    float px = 10.0f;
    const float radius = 10.0f;
    float py = (float)screenH - radius; // start on ground (center y)

    // Movement / physics
    const float speed = 300.0f;
    const float gravity = 1500.0f;
    const float jumpForce = 650.0f;
    float vy = 0.0f;
    bool onGround = true;

    // Platforms initialization
    std::vector<Plat> platforms;
    platforms.push_back({{200, 600, 200, 20}, GREEN}); // First platform
    platforms.push_back({{500, 700, 200, 20}, GREEN}); // Second platform

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Horizontal input
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) px += speed * dt;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) px -= speed * dt;

        // Keep inside screen horizontally
        px = Clamp(px, radius, screenW - radius);

        // Jump (only if on ground)
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && onGround) {
            vy = -jumpForce * 1.5f;
            onGround = false;
        }

        // Apply gravity (removed extra multiplier)
        if (!onGround) {
            vy += gravity * dt;
        }

        // Update player position vertically based on velocity
        py += vy * dt;

        // Collision with platforms
        for (auto& plat : platforms) {
            plat.collision(px, py, vy, onGround, radius, dt);
        }

        // Ground collision (bottom of screen)
        float groundY = (float)screenH - radius;
        if (py > groundY) {
            py = groundY;
            vy = 0.0f;
            onGround = true;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw each platform
        for (auto& plat : platforms) {
            DrawRectangleRec(plat.plat, plat.platColor);
        }

        // Draw player (circle)
        DrawCircle((int)roundf(px), (int)roundf(py), (int)radius, WHITE);

        // Display whether the player is on the ground or not
        DrawText(onGround ? "Grounded" : "Airborne", 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
