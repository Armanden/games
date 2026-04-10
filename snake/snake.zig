const std = @import("std");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

const board_size = 20;
const cell_size = 20;
const window_size = board_size * cell_size;

const Vec = struct {
    x: i32,
    y: i32,
};

var snake: [board_size * board_size]Vec = undefined;
var snake_len: usize = 1;

var direction = Vec{ .x = 0, .y = 0 };
var food = Vec{ .x = 5, .y = 5 };

// Generate a random position not overlapping the snake
fn randPos() Vec {
    while (true) {
        const pos = Vec{
            .x = @as(i32)(std.crypto.random.int(u32) % board_size),
            .y = @as(i32)(std.crypto.random.int(u32) % board_size),
        };
        var collision = false;
        for (snake[0..snake_len]) |seg| {
            if (seg.x == pos.x and seg.y == pos.y) {
                collision = true;
                break;
            }
        }
        if (!collision) return pos;
    }
}

fn resetGame() void {
    snake_len = 1;
    snake[0] = Vec{ .x = 10, .y = 10 };
    direction = Vec{ .x = 0, .y = 0 };
    food = randPos();
}

fn update() void {
    if (direction.x == 0 and direction.y == 0) return;

    var head = snake[0];

    head.x = (head.x + direction.x + board_size) % board_size;
    head.y = (head.y + direction.y + board_size) % board_size;

    // collision with body
    for (snake[1..snake_len]) |seg| {
        if (seg.x == head.x and seg.y == head.y) {
            resetGame();
            return;
        }
    }

    // move body
    var i: usize = snake_len;
    while (i > 0) : (i -= 1) {
        snake[i] = snake[i - 1];
    }

    snake[0] = head;

    // eat food
    if (head.x == food.x and head.y == food.y) {
        snake_len += 1;
        food = randPos();
    }
}

fn draw(renderer: *c.SDL_Renderer) void {
    // clear background
    _ = c.SDL_SetRenderDrawColor(renderer, 30, 125, 50, 255);
    _ = c.SDL_RenderClear(renderer);

    // draw snake
    _ = c.SDL_SetRenderDrawColor(renderer, 21, 101, 192, 255);
    for (snake[0..snake_len]) |seg| {
        var rect = c.SDL_Rect{
            .x = seg.x * cell_size,
            .y = seg.y * cell_size,
            .w = cell_size,
            .h = cell_size,
        };
        _ = c.SDL_RenderFillRect(renderer, &rect);
    }

    // draw food
    _ = c.SDL_SetRenderDrawColor(renderer, 198, 40, 40, 255);
    var rect = c.SDL_Rect{
        .x = food.x * cell_size,
        .y = food.y * cell_size,
        .w = cell_size,
        .h = cell_size,
    };
    _ = c.SDL_RenderFillRect(renderer, &rect);

    c.SDL_RenderPresent(renderer);
}

pub fn main() !void {
    if (c.SDL_Init(c.SDL_INIT_VIDEO) != 0) return error.SDLInitFail;

    const window = c.SDL_CreateWindow(
        "Snake",
        c.SDL_WINDOWPOS_CENTERED,
        c.SDL_WINDOWPOS_CENTERED,
        window_size,
        window_size,
        0,
    );

    const renderer = c.SDL_CreateRenderer(window, -1, 0);

    resetGame();

    var event: c.SDL_Event = undefined;

    while (true) {
        while (c.SDL_PollEvent(&event) != 0) {
            if (event.type == c.SDL_QUIT) return;

            if (event.type == c.SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                   c.SDLK_UP => {
                       if (direction.y == 0) direction = Vec{ .x = 0, .y = -1 };
                   },
                   c.SDLK_DOWN => {
                       if (direction.y == 0) direction = Vec{ .x = 0, .y = 1 };
                   },
                   c.SDLK_LEFT => {
                       if (direction.x == 0) direction = Vec{ .x = -1, .y = 0 };
                   },
                   c.SDLK_RIGHT => {
                       if (direction.x == 0) direction = Vec{ .x = 1, .y = 0 };
                   },
                    else => {},
                }
            }
        }

        update();
        draw(renderer);

        c.SDL_Delay(120);
    }
}
