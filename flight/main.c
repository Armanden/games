#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * INFINITE FLIGHT SIMULATOR
 * Procedurally generated terrain that extends infinitely
 *
 * Controls:
 * - W/S: Accelerate/Decelerate
 * - A/D: Turn left/right
 * - Q/E: Pitch up/down
 * - SPACE/SHIFT: Ascend/Descend
 * - R: Reset position
 * - ESC: Pause
 */

#define SCREEN_W 1200
#define SCREEN_H 800

#define CHUNK_SIZE  50
#define CHUNK_SCALE 20.0f
#define VIEW_CHUNKS 5

typedef struct {
    Vector3 pos;
    float yaw;
    float pitch;
    float speed;
} Plane;

Plane plane = {0};

float noise(float x, float z) {
    int ix = (int)floorf(x);
    int iz = (int)floorf(z);
    float fx = x - ix;
    float fz = z - iz;
    
    float a = sinf(ix * 0.1f) * cosf(iz * 0.1f);
    float b = sinf((ix + 1) * 0.1f) * cosf(iz * 0.1f);
    float c = sinf(ix * 0.1f) * cosf((iz + 1) * 0.1f);
    float d = sinf((ix + 1) * 0.1f) * cosf((iz + 1) * 0.1f);
    
    float ab = a * (1 - fx) + b * fx;
    float cd = c * (1 - fx) + d * fx;
    return ab * (1 - fz) + cd * fz;
}

float terrainHeight(float x, float z) {
    float h = noise(x, z) * 30.0f;
    h += noise(x * 0.3f, z * 0.3f) * 50.0f;
    h += noise(x * 0.05f, z * 0.05f) * 20.0f;
    return h;
}

void drawGroundChunk(int cx, int cz) {
    float baseX = cx * CHUNK_SIZE;
    float baseZ = cz * CHUNK_SIZE;
    
    for (int x = 0; x < CHUNK_SIZE; x += 2) {
        for (int z = 0; z < CHUNK_SIZE; z += 2) {
            float h1 = terrainHeight(baseX + x, baseZ + z);
            float h2 = terrainHeight(baseX + x + 2, baseZ + z);
            float h3 = terrainHeight(baseX + x, baseZ + z + 2);
            float h4 = terrainHeight(baseX + x + 2, baseZ + z + 2);
            
            Vector3 v1 = {baseX + x, h1, baseZ + z};
            Vector3 v2 = {baseX + x + 2, h2, baseZ + z};
            Vector3 v3 = {baseX + x, h3, baseZ + z + 2};
            Vector3 v4 = {baseX + x + 2, h4, baseZ + z + 2};
            
            float colorVar = (h1 + 40.0f) / 100.0f;
            unsigned char g = (unsigned char)(80 + colorVar * 120);
            Color col1 = {34, (int)g, 23, 255};
            Color col2 = {45, (int)(g * 0.9f), 30, 255};
            Color col3 = {56, (int)(g * 1.1f), 28, 255};
            
            DrawTriangle3D(v1, v2, v3, col1);
            DrawTriangle3D(v2, v4, v3, col2);
            
            DrawLine3D(v1, v2, (Color){20, 60, 15, 255});
            DrawLine3D(v1, v3, (Color){20, 60, 15, 255});
        }
    }
}

void drawPlane(void) {
    DrawTriangle3D(
        (Vector3){0, 0, -3},
        (Vector3){-2.5f, 0, 2},
        (Vector3){2.5f, 0, 2},
        RED
    );
    
    DrawTriangle3D(
        (Vector3){0, 0, 2},
        (Vector3){-2.5f, 0, 2},
        (Vector3){-4.0f, 0, 3},
        DARKBLUE
    );
    
    DrawTriangle3D(
        (Vector3){0, 0, 2},
        (Vector3){2.5f, 0, 2},
        (Vector3){4.0f, 0, 3},
        DARKBLUE
    );
    
    DrawTriangle3D(
        (Vector3){0, 1.0f, -2},
        (Vector3){0, 0, 1},
        (Vector3){0, -1.0f, -2},
        ORANGE
    );
}

Camera3D createFollowCamera(Plane *p) {
    Camera3D cam = {0};
    
    float camDist = 25.0f;
    float camHeight = 8.0f;
    
    cam.position = (Vector3){
        p->pos.x - sinf(p->yaw) * camDist,
        p->pos.y + camHeight,
        p->pos.z - cosf(p->yaw) * camDist
    };
    
    cam.target = (Vector3){
        p->pos.x + sinf(p->yaw) * 10.0f,
        p->pos.y,
        p->pos.z + cosf(p->yaw) * 10.0f
    };
    
    cam.up = (Vector3){0, 1, 0};
    cam.fovy = 60.0f;
    cam.projection = CAMERA_PERSPECTIVE;
    
    return cam;
}

void drawHUD(Plane *p) {
    DrawRectangle(10, 10, 250, 130, (Color){0, 0, 0, 200});
    DrawRectangleLines(10, 10, 250, 130, (Color){0, 200, 100, 255});
    
    DrawText("INFINITE FLIGHT SIM", 20, 20, 22, GREEN);
    DrawText(TextFormat("Speed: %.1f", p->speed * 100), 20, 55, 18, WHITE);
    DrawText(TextFormat("Altitude: %.1f m", p->pos.y), 20, 80, 18, WHITE);
    DrawText(TextFormat("X: %.0f  Z: %.0f", p->pos.x, p->pos.z), 20, 105, 18, WHITE);
    
    DrawRectangle(10, SCREEN_H - 60, 350, 50, (Color){0, 0, 0, 200});
    DrawText("W/S: Speed  A/D: Turn  Q/E: Pitch", 20, SCREEN_H - 50, 16, GRAY);
    DrawText("SPACE/SHIFT: Up/Down  R: Reset", 20, SCREEN_H - 30, 16, GRAY);
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Infinite Flight Simulator");
    SetTargetFPS(60);
    DisableCursor();
    
    plane.pos = (Vector3){0, 50, 0};
    plane.yaw = 0;
    plane.pitch = 0;
    plane.speed = 0.3f;
    
    float targetSpeed = plane.speed;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        
        if (IsKeyDown(KEY_W)) targetSpeed += dt * 1.5f;
        if (IsKeyDown(KEY_S)) targetSpeed -= dt * 1.5f;
        targetSpeed = fmaxf(0.05f, fminf(2.0f, targetSpeed));
        plane.speed += (targetSpeed - plane.speed) * dt * 3.0f;
        
        if (IsKeyDown(KEY_A)) plane.yaw += dt * 2.0f;
        if (IsKeyDown(KEY_D)) plane.yaw -= dt * 2.0f;
        
        if (IsKeyDown(KEY_Q)) plane.pitch += dt * 2.0f;
        if (IsKeyDown(KEY_E)) plane.pitch -= dt * 2.0f;
        plane.pitch = fmaxf(-1.0f, fminf(1.0f, plane.pitch));
        
        if (IsKeyDown(KEY_SPACE)) plane.pos.y += dt * 30.0f;
        if (IsKeyDown(KEY_LEFT_SHIFT)) plane.pos.y -= dt * 30.0f;
        
        if (IsKeyPressed(KEY_R)) {
            plane.pos = (Vector3){0, 50, 0};
            plane.yaw = 0;
            plane.pitch = 0;
            plane.speed = 0.3f;
            targetSpeed = plane.speed;
        }
        
        plane.pos.x += sinf(plane.yaw) * plane.speed * dt * 100.0f;
        plane.pos.z += cosf(plane.yaw) * plane.speed * dt * 100.0f;
        plane.pos.y -= plane.pitch * plane.speed * dt * 60.0f;
        plane.pos.y = fmaxf(10.0f, plane.pos.y);
        
        Camera3D cam = createFollowCamera(&plane);
        
        int playerChunkX = (int)floorf(plane.pos.x / CHUNK_SIZE);
        int playerChunkZ = (int)floorf(plane.pos.z / CHUNK_SIZE);
        
        BeginDrawing();
        ClearBackground((Color){135, 206, 235, 255});
        
        BeginMode3D(cam);
        
        Vector3 groundCenter = {plane.pos.x, 0, plane.pos.z};
        DrawPlane(groundCenter, (Vector2){20000, 20000}, (Color){135, 206, 235, 255});
        
        for (int cx = playerChunkX - VIEW_CHUNKS; cx <= playerChunkX + VIEW_CHUNKS; cx++) {
            for (int cz = playerChunkZ - VIEW_CHUNKS; cz <= playerChunkZ + VIEW_CHUNKS; cz++) {
                drawGroundChunk(cx, cz);
            }
        }
        
        DrawLine3D(
            (Vector3){plane.pos.x, plane.pos.y - 5, plane.pos.z},
            (Vector3){plane.pos.x, plane.pos.y - 200, plane.pos.z},
            RED
        );
        
        EndMode3D();
        
        drawHUD(&plane);
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            EnableCursor();
            while (!IsKeyPressed(KEY_SPACE) && !WindowShouldClose()) {
                BeginDrawing();
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 200});
                DrawText("PAUSED - Press SPACE to continue", SCREEN_W/2 - 200, SCREEN_H/2, 24, WHITE);
                EndDrawing();
            }
            DisableCursor();
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}