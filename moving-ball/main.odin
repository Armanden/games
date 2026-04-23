package main

import "core:fmt"
import "core:math/rand"
import "core:time"
import "core:os"

import rl "vendor:raylib"

SQUARE_SIZE :: 50
MAX_SPEED :: 3
MAX_LEVEL :: 10
SQUARES_PER_LEVEL :: 10
MAX_SQUARES :: MAX_LEVEL * SQUARES_PER_LEVEL

scX : i32 = 1000
scY : i32 = 1000
speed : i32 = 5

squares : [MAX_SQUARES]rl.Vector2
velocities : [MAX_SQUARES]rl.Vector2

init_squares :: proc(count: int) {
	for i := 0; i < count; i++ {
		squares[i].x = f32(rand.int31() % int(GetScreenWidth() - SQUARE_SIZE))
		squares[i].y = f32(rand.int31() % int(GetScreenHeight() - SQUARE_SIZE))

		velocities[i].x = f32((rand.int31() % (MAX_SPEED * 2 + 1)) - MAX_SPEED)
		velocities[i].y = f32((rand.int31() % (MAX_SPEED * 2 + 1)) - MAX_SPEED)

		if velocities[i].x == 0 do velocities[i].x = 1
		if velocities[i].y == 0 do velocities[i].y = 1
	}
}

get_screen_width :: proc() -> i32 {
	return rl.get_render_width()
}

get_screen_height :: proc() -> i32 {
	return rl.get_render_height()
}

main :: proc() {
	rand.seed(time.now()._nsec)

	rl.init_window(scX, scY, "Dodge the Squares")
	rl.set_target_fps(60)

	game_started := false
	game_over := false

	current_level : i32 = 2
	if current_level < 1 do current_level = 1
	if current_level > MAX_LEVEL do current_level = MAX_LEVEL
	squares_count := int(current_level * SQUARES_PER_LEVEL)

	score : i32 = 0
	last_score_time : f64 = 0.0
	final_score : i32 = 0

	player_pos := rl.Vector2{f32(scX) / 2.0, f32(scY) / 2.0}
	player_radius : f32 = 10.0

	for !rl.window_should_close() {
		if !game_started {
			if rl.is_key_pressed(rl.KEY_RIGHT) {
				if current_level < MAX_LEVEL {
					current_level += 1
					squares_count = int(current_level * SQUARES_PER_LEVEL)
				}
			}
			if rl.is_key_pressed(rl.KEY_LEFT) {
				if current_level > 1 {
					current_level -= 1
					squares_count = int(current_level * SQUARES_PER_LEVEL)
				}
			}
			if rl.is_key_pressed(rl.KEY_ENTER) || rl.is_key_pressed(rl.KEY_KP_ENTER) || rl.is_key_pressed(rl.KEY_SPACE) {
				game_started = true
				game_over = false
				score = 0
				last_score_time = rl.get_time()
				player_pos.x = f32(scX) / 2.0
				player_pos.y = f32(scY) / 2.0
				squares_count = int(current_level * SQUARES_PER_LEVEL)
				init_squares(squares_count)
			}
			if rl.is_key_pressed(rl.KEY_ESCAPE) do break
		} else if game_over {
			if rl.is_key_pressed(rl.KEY_R) {
				game_started = false
				game_over = false
				score = 0
			}
			if rl.is_key_pressed(rl.KEY_ESCAPE) do break
		} else {
			if rl.is_key_down(rl.KEY_RIGHT) && (player_pos.x + player_radius + f32(speed) <= f32(get_screen_width())) {
				player_pos.x += f32(speed)
			}
			if rl.is_key_down(rl.KEY_LEFT) && (player_pos.x - player_radius - f32(speed) >= 0) {
				player_pos.x -= f32(speed)
			}
			if rl.is_key_down(rl.KEY_UP) && (player_pos.y - player_radius - f32(speed) >= 0) {
				player_pos.y -= f32(speed)
			}
			if rl.is_key_down(rl.KEY_DOWN) && (player_pos.y + player_radius + f32(speed) <= f32(get_screen_height())) {
				player_pos.y += f32(speed)
			}

			now := rl.get_time()
			if now - last_score_time >= 1.0 {
				score += 1
				last_score_time = now
			}

			for i := 0; i < squares_count; i++ {
				squares[i].x += velocities[i].x
				squares[i].y += velocities[i].y

				if squares[i].x <= 0 {
					squares[i].x = 0
					velocities[i].x *= -1
				}
				if squares[i].x >= f32(scX - SQUARE_SIZE) {
					squares[i].x = f32(scX - SQUARE_SIZE)
					velocities[i].x *= -1
				}
				if squares[i].y <= 0 {
					squares[i].y = 0
					velocities[i].y *= -1
				}
				if squares[i].y >= f32(scY - SQUARE_SIZE) {
					squares[i].y = f32(scY - SQUARE_SIZE)
					velocities[i].y *= -1
				}
			}

			collision_detected := false
			for i := 0; i < squares_count; i++ {
				square := rl.Rectangle{squares[i].x, squares[i].y, f32(SQUARE_SIZE), f32(SQUARE_SIZE)}
				if rl.check_collision_circle_rec(player_pos, player_radius, square) {
					collision_detected = true
					break
				}
			}

			if collision_detected {
				game_over = true
				final_score = score * 10
			}
		}

		rl.begin_drawing()
		rl.clear_background(rl.BLACK)

		if !game_started {
			title := "DODGE THE SQUARES"
			title_font_size : i32 = 40
			info_font_size : i32 = 20

			rl.draw_text(title, scX/2 - rl.measure_text(title, title_font_size)/2, scY/6, title_font_size, rl.GOLD)

			level_text := fmt.aprintf("Level: %d   (Squares: %d)", current_level, current_level * SQUARES_PER_LEVEL)
			rl.draw_text(level_text, scX/2 - rl.measure_text(level_text, info_font_size)/2, scY/6 + 80, info_font_size, rl.LIGHTGRAY)

			instr1 := "Use LEFT / RIGHT to change level"
			instr2 := "Press ENTER or SPACE to start"
			instr3 := "Arrow keys to move. Press ESC to quit."
			instr4 := "Press R during Game Over to return here."

			rl.draw_text(instr1, scX/2 - rl.measure_text(instr1, info_font_size)/2, scY/6 + 110, info_font_size, rl.LIGHTGRAY)
			rl.draw_text(instr2, scX/2 - rl.measure_text(instr2, info_font_size)/2, scY/6 + 140, info_font_size, rl.LIGHTGRAY)
			rl.draw_text(instr3, scX/2 - rl.measure_text(instr3, info_font_size)/2, scY/6 + 170, info_font_size, rl.LIGHTGRAY)
			rl.draw_text(instr4, scX/2 - rl.measure_text(instr4, info_font_size)/2, scY/6 + 200, info_font_size, rl.LIGHTGRAY)

			rl.draw_circle_v(player_pos, player_radius, rl.GREEN)
		} else if game_over {
			for i := 0; i < squares_count; i++ {
				rl.draw_rectangle_v(squares[i], rl.Vector2{f32(SQUARE_SIZE), f32(SQUARE_SIZE)}, rl.BLUE)
			}
			rl.draw_circle_v(player_pos, player_radius, rl.RED)

			score_text := fmt.aprintf("GAME OVER! Final score: %d", final_score)
			rl.draw_text(score_text, scX/2 - rl.measure_text(score_text, 24)/2, scY/3, 24, rl.RED)

			restart_text := "Press R to return to start screen (change level) or ESC to quit."
			rl.draw_text(restart_text, scX/2 - rl.measure_text(restart_text, 20)/2, scY/3 + 50, 20, rl.LIGHTGRAY)
		} else {
			for i := 0; i < squares_count; i++ {
				rl.draw_rectangle_v(squares[i], rl.Vector2{f32(SQUARE_SIZE), f32(SQUARE_SIZE)}, rl.BLUE)
			}

			rl.draw_circle_v(player_pos, player_radius, rl.GREEN)

			score_text := fmt.aprintf("Score: %d", score)
			rl.draw_text(score_text, 10, 10, 20, rl.WHITE)

			level_hud := fmt.aprintf("Level: %d", current_level)
			rl.draw_text(level_hud, scX - rl.measure_text(level_hud, 20) - 10, 10, 20, rl.LIGHTGRAY)
		}

		rl.end_drawing()
	}

	rl.close_window()
}