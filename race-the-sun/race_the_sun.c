#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 20
#define OBSTACLE_WIDTH 30
#define OBSTACLE_HEIGHT 60
#define MAX_OBSTACLES 15
#define OBSTACLE_SPEED 3
#define SUN_RADIUS 40
#define SUN_SET_SPEED 0.3f

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

typedef struct {
    int x, y;
    int w, h;
} Player;

typedef struct {
    int x, y;
    int w, h;
    bool active;
} Obstacle;

typedef struct {
    int x, y;
    int radius;
} Sun;

Player player;
Obstacle obstacles[MAX_OBSTACLES];
Sun sun;
bool running = true;
bool game_over = false;
int score = 0;
Uint32 last_time = 0;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Race the Sun", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    player = (Player){SCREEN_WIDTH/2 - PLAYER_WIDTH/2, 
                      SCREEN_HEIGHT - PLAYER_HEIGHT - 30,
                      PLAYER_WIDTH, PLAYER_HEIGHT};
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = false;
    }

    sun = (Sun){SCREEN_WIDTH - SUN_RADIUS - 20, SUN_RADIUS + 20, SUN_RADIUS};
    srand(time(NULL));
    last_time = SDL_GetTicks();
    return true;
}

void spawn_obstacle() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            int x_pos = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
            obstacles[i] = (Obstacle){x_pos, -OBSTACLE_HEIGHT,
                                      OBSTACLE_WIDTH, OBSTACLE_HEIGHT, true};
            break;
        }
    }
}

void update(float dt) {
    if (game_over) return;

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT]) player.x -= 5;
    if (keys[SDL_SCANCODE_RIGHT]) player.x += 5;
    if (player.x < 0) player.x = 0;
    if (player.x + player.w > SCREEN_WIDTH) player.x = SCREEN_WIDTH - player.w;

    sun.y += SUN_SET_SPEED * dt * 60;
    if (sun.y > SCREEN_HEIGHT + sun.radius) {
        game_over = true;
    }

    if (rand() % 100 < 5) spawn_obstacle();

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            obstacles[i].y += OBSTACLE_SPEED;
            if (obstacles[i].y > SCREEN_HEIGHT) {
                obstacles[i].active = false;
                score += 10;
            }

            if (SDL_HasIntersection((SDL_Rect*)&player, (SDL_Rect*)&obstacles[i])) {
                game_over = true;
            }
        }
    }
}

void render() {
    int light = 255 - (sun.y / SCREEN_HEIGHT * 200);
    if (light < 50) light = 50;

    SDL_SetRenderDrawColor(renderer, light/2, light/2, light, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int y = -sun.radius; y <= sun.radius; y++) {
        for (int x = -sun.radius; x <= sun.radius; x++) {
            if (x*x + y*y <= sun.radius*sun.radius) {
                SDL_RenderDrawPoint(renderer, sun.x + x, sun.y + y);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_Rect player_rect = {player.x, player.y, player.w, player.h};
    SDL_RenderFillRect(renderer, &player_rect);

    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            SDL_Rect obs_rect = {obstacles[i].x, obstacles[i].y, 
                                 obstacles[i].w, obstacles[i].h};
            SDL_RenderFillRect(renderer, &obs_rect);
        }
    }

    SDL_RenderPresent(renderer);
}

void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (!init()) return 1;

    while (running) {
        Uint32 current_time = SDL_GetTicks();
        float dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_r && game_over) {
                    game_over = false;
                    score = 0;
                    player.x = SCREEN_WIDTH/2 - PLAYER_WIDTH/2;
                    for (int i=0; i<MAX_OBSTACLES; i++) obstacles[i].active = false;
                    sun.y = SUN_RADIUS + 20;
                }
            }
        }

        update(dt);
        render();
        SDL_Delay(16);
    }

    cleanup();
    printf("Final score: %d\n", score);
    return 0;
}
