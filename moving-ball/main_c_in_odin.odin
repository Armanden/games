package main

import "core:fmt"
import "core:math/rand"
import "core:time"

import rl "vendor:raylib"

SQUARE_SIZE :: 50
MAX_SPEED :: 3
MAX_LEVEL :: 10
SQUARES_PER_LEVEL :: 10
MAX_SQUARES :: MAX_LEVEL * SQUARES_PER_LEVEL

SCREEN_WIDTH  :: 1000
SCREEN_HEIGHT :: 1000
PLAYER_SPEED :: 5

squares:    [MAX_SQUARES]rl.Vector2
velocities: [MAX_SQUARES]rl.Vector2

init_squares :: proc(count: int) {
	for i := 0; i < count; i += 1 {
		squares[i].x = f32(rand.int31() % (i32(SCREEN_WIDTH) - SQUARE_SIZE))
		squares[i].y = f32(rand.int31() % (i32(SCREEN_HEIGHT) - SQUARE_SIZE))

		velocities[i].x = f32((rand.int31() % (MAX_SPEED * 2 + 1)) - MAX_SPEED)
		velocities[i].y = f32((rand.int31() % (MAX_SPEED * 2 + 1)) - MAX_SPEED)

		if velocities[i].x == 0 do velocities[i].x = 1
		if velocities[i].y == 0 do velocities[i].y = 1
	}
}

main :: proc() {
	rand.reset_u64(u64(time.now()._nsec))

	rl.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dodge the Squares")
	rl.SetTargetFPS(60)

	game_started := false
	game_over := false

	current_level: i32 = 2
	if current_level < 1 do current_level = 1
	if current_level > MAX_LEVEL do current_level = MAX_LEVEL
	squares_count := int(current_level * SQUARES_PER_LEVEL)

	score: i32
	last_score_time: f64
	final_score: i32

	player_pos := rl.Vector2{f32(SCREEN_WIDTH) / 2, f32(SCREEN_HEIGHT) / 2}
	player_radius: f32 = 10

	for !rl.WindowShouldClose() {
		if !game_started {
			if rl.IsKeyPressed(.RIGHT) {
				if current_level < MAX_LEVEL {
					current_level += 1
					squares_count = int(current_level * SQUARES_PER_LEVEL)
				}
			}
			if rl.IsKeyPressed(.LEFT) {
				if current_level > 1 {
					current_level -= 1
					squares_count = int(current_level * SQUARES_PER_LEVEL)
				}
			}
			if rl.IsKeyPressed(.ENTER) || rl.IsKeyPressed(.KP_ENTER) || rl.IsKeyPressed(.SPACE) {
				game_started = true
				game_over = false
				score = 0
				last_score_time = rl.GetTime()
				player_pos.x = f32(SCREEN_WIDTH) / 2
				player_pos.y = f32(SCREEN_HEIGHT) / 2
				squares_count = int(current_level * SQUARES_PER_LEVEL)
				init_squares(squares_count)
			}
			if rl.IsKeyPressed(.ESCAPE) do break
		} else if game_over {
			if rl.IsKeyPressed(.R) {
				game_started = false
				game_over = false
				score = 0
			}
			if rl.IsKeyPressed(.ESCAPE) do break
		} else {
			if rl.IsKeyDown(.RIGHT) && (player_pos.x + player_radius + f32(PLAYER_SPEED) <= f32(SCREEN_WIDTH)) {
				player_pos.x += f32(PLAYER_SPEED)
			}
			if rl.IsKeyDown(.LEFT) && (player_pos.x - player_radius - f32(PLAYER_SPEED) >= 0) {
				player_pos.x -= f32(PLAYER_SPEED)
			}
			if rl.IsKeyDown(.UP) && (player_pos.y - player_radius - f32(PLAYER_SPEED) >= 0) {
				player_pos.y -= f32(PLAYER_SPEED)
			}
			if rl.IsKeyDown(.DOWN) && (player_pos.y + player_radius + f32(PLAYER_SPEED) <= f32(SCREEN_HEIGHT)) {
				player_pos.y += f32(PLAYER_SPEED)
			}

			now := rl.GetTime()
			if now - last_score_time >= 1.0 {
				score += 1
				last_score_time = now
			}

			for i := 0; i < squares_count; i += 1 {
				squares[i].x += velocities[i].x
				squares[i].y += velocities[i].y

				if squares[i].x <= 0 {
					squares[i].x = 0
					velocities[i].x *= -1
				}
				if squares[i].x >= f32(SCREEN_WIDTH - SQUARE_SIZE) {
					squares[i].x = f32(SCREEN_WIDTH - SQUARE_SIZE)
					velocities[i].x *= -1
				}
				if squares[i].y <= 0 {
					squares[i].y = 0
					velocities[i].y *= -1
				}
				if squares[i].y >= f32(SCREEN_HEIGHT - SQUARE_SIZE) {
					squares[i].y = f32(SCREEN_HEIGHT - SQUARE_SIZE)
					velocities[i].y *= -1
				}
			}

			collision_detected := false
			for i := 0; i < squares_count; i += 1 {
				square := rl.Rectangle{squares[i].x, squares[i].y, f32(SQUARE_SIZE), f32(SQUARE_SIZE)}
				if rl.CheckCollisionCircleRec(player_pos, player_radius, square) {
					collision_detected = true
					break
				}
			}

			if collision_detected {
				game_over = true
				final_score = score * 10
			}
		}

		rl.BeginDrawing()
		rl.ClearBackground(rl.BLACK)

		if !game_started {
			title := fmt.ctprintf("DODGE THE SQUARES")
			title_size: i32 = 40
			info_size: i32 = 20

			rl.DrawText(title, SCREEN_WIDTH / 2 - rl.MeasureText(title, title_size) / 2, SCREEN_HEIGHT / 6, title_size, rl.GOLD)

			level_text := fmt.ctprintf("Level: %d   (Squares: %d)", current_level, current_level * SQUARES_PER_LEVEL)
			rl.DrawText(level_text, SCREEN_WIDTH / 2 - rl.MeasureText(level_text, info_size) / 2, SCREEN_HEIGHT / 6 + 80, info_size, rl.LIGHTGRAY)

			rl.DrawText("Use LEFT / RIGHT to change level", SCREEN_WIDTH / 2 - rl.MeasureText("Use LEFT / RIGHT to change level", info_size) / 2, SCREEN_HEIGHT / 6 + 110, info_size, rl.LIGHTGRAY)
			rl.DrawText("Press ENTER or SPACE to start", SCREEN_WIDTH / 2 - rl.MeasureText("Press ENTER or SPACE to start", info_size) / 2, SCREEN_HEIGHT / 6 + 140, info_size, rl.LIGHTGRAY)
			rl.DrawText("Arrow keys to move. Press ESC to quit.", SCREEN_WIDTH / 2 - rl.MeasureText("Arrow keys to move. Press ESC to quit.", info_size) / 2, SCREEN_HEIGHT / 6 + 170, info_size, rl.LIGHTGRAY)
			rl.DrawText("Press R during Game Over to return here.", SCREEN_WIDTH / 2 - rl.MeasureText("Press R during Game Over to return here.", info_size) / 2, SCREEN_HEIGHT / 6 + 200, info_size, rl.LIGHTGRAY)

			rl.DrawCircleV(player_pos, player_radius, rl.GREEN)
		} else if game_over {
			for i := 0; i < squares_count; i += 1 {
				rl.DrawRectangleV(squares[i], {f32(SQUARE_SIZE), f32(SQUARE_SIZE)}, rl.BLUE)
			}
			rl.DrawCircleV(player_pos, player_radius, rl.RED)

			score_text := fmt.ctprintf("GAME OVER! Final score: %d", final_score)
			rl.DrawText(score_text, SCREEN_WIDTH / 2 - rl.MeasureText(score_text, 24) / 2, SCREEN_HEIGHT / 3, 24, rl.RED)

			rl.DrawText("Press R to return to start screen (change level) or ESC to quit.", SCREEN_WIDTH / 2 - rl.MeasureText("Press R to return to start screen (change level) or ESC to quit.", 20) / 2, SCREEN_HEIGHT / 3 + 50, 20, rl.LIGHTGRAY)
		} else {
			for i := 0; i < squares_count; i += 1 {
				rl.DrawRectangleV(squares[i], {f32(SQUARE_SIZE), f32(SQUARE_SIZE)}, rl.BLUE)
			}

			rl.DrawCircleV(player_pos, player_radius, rl.GREEN)

			score_text := fmt.ctprintf("Score: %d", score)
			rl.DrawText(score_text, 10, 10, 20, rl.WHITE)

			level_hud := fmt.ctprintf("Level: %d", current_level)
			rl.DrawText(level_hud, SCREEN_WIDTH - rl.MeasureText(level_hud, 20) - 10, 10, 20, rl.LIGHTGRAY)
		}

		rl.EndDrawing()
	}

	rl.CloseWindow()
}