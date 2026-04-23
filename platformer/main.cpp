#include <raylib.h>
#include <vector>
#include <cmath>

static float Clamp(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

class Plat {
public:
    Rectangle plat;
    Color platColor = GREEN;

    void collision(float &px, float &py, float &vy, bool &onGround, const float radius) {
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
            px += nx * penetration;
            py += ny * penetration;

            if (ny < -0.5f) {
                vy = 0.0f;
                onGround = true;
                py = plat.y - radius;
            } else if (ny > 0.5f) {
                vy = 0.0f;
                py = plat.y + plat.height + radius;
            } else if (nx < -0.5f) {
                px = plat.x - radius;
            } else if (nx > 0.5f) {
                px = plat.x + plat.width + radius;
            }
        }
    }
};

int main(void) {
    const int screenW = 1200;
    const int screenH = 900;
    InitWindow(screenW, screenH, "Platformer");
    SetTargetFPS(120);

    float px = 10.0f;
    const float radius = 10.0f;
    float py = (float)screenH - radius;

    const float speed = 300.0f;
    const float gravity = 1500.0f;
    const float jumpForce = 650.0f;
    float vy = 0.0f;
    bool onGround = true;

    std::vector<Plat> platforms;
    platforms.push_back({{200, 600, 200, 20}, GREEN});
    platforms.push_back({{500, 700, 200, 20}, GREEN});

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) px += speed * dt;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) px -= speed * dt;

        px = Clamp(px, radius, (float)screenW - radius);

        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && onGround) {
            vy = -jumpForce;
            onGround = false;
        }

        if (!onGround) {
            vy += gravity * dt;
        }

        py += vy * dt;
        onGround = false;

        for (auto &plat : platforms) {
            plat.collision(px, py, vy, onGround, radius);
        }

        float groundY = (float)screenH - radius;
        if (py > groundY) {
            py = groundY;
            vy = 0.0f;
            onGround = true;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto &plat : platforms) {
            DrawRectangleRec(plat.plat, plat.platColor);
        }

        DrawCircle((int)roundf(px), (int)roundf(py), (int)radius, WHITE);
        DrawText(onGround ? "Grounded" : "Airborne", 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}