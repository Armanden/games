package main

import rl "vendor:raylib"
import "core:fmt"
import "core:math/rand"
import "core:time"

main :: proc() {
    rl.InitWindow(800, 600, "Hangman (Odin + raylib)")
    rl.SetTargetFPS(60)

    rand.seed(time.now().unix)

    words := []string{
        "istanbul", "samsun", "antalya",
        "rize", "ankara", "corum",
    }

    current_word := words[rand.intn(len(words))]
    word_length := len(current_word)

    visible_word := make([]rune, word_length)
    for i in 0..<word_length {
        visible_word[i] = '_'
    }

    attempts := word_length + 3
    guessed_letters := make(map[rune]bool)

    game_over := false
    win := false

    for !rl.WindowShouldClose() {

        // 🔤 Keyboard input (A-Z)
        if !game_over {
            for key in rl.KEY_A..=rl.KEY_Z {
                if rl.IsKeyPressed(key) {
                    letter := rune('a' + (key - rl.KEY_A))

                    if !guessed_letters[letter] {
                        guessed_letters[letter] = true

                        found := false
                        for i in 0..<word_length {
                            if rune(current_word[i]) == letter {
                                visible_word[i] = letter
                                found = true
                            }
                        }

                        if !found {
                            attempts -= 1
                        }

                        // Check win
                        win = true
                        for c in visible_word {
                            if c == '_' {
                                win = false
                            }
                        }

                        if win || attempts <= 0 {
                            game_over = true
                        }
                    }
                }
            }
        }

        // 🔁 Restart with R
        if game_over && rl.IsKeyPressed(rl.KEY_R) {
            current_word = words[rand.intn(len(words))]
            word_length = len(current_word)

            visible_word = make([]rune, word_length)
            for i in 0..<word_length {
                visible_word[i] = '_'
            }

            attempts = word_length + 3
            guessed_letters = make(map[rune]bool)

            game_over = false
            win = false
        }

        rl.BeginDrawing()
        rl.ClearBackground(rl.RAYWHITE)

        // 🧩 Draw word
        word_display := ""
        for c in visible_word {
            word_display += string(c) + " "
        }
        rl.DrawText(word_display, 200, 150, 40, rl.BLACK)

        // ❤️ Attempts
        attempts_text := fmt.tprintf("Attempts: %d", attempts)
        rl.DrawText(attempts_text, 20, 20, 20, rl.RED)

        // 🔤 Instructions
        rl.DrawText("Press A-Z to guess letters", 200, 250, 20, rl.DARKGRAY)

        // 📜 Show guessed letters
        guessed_text := "Guessed: "
        for k in guessed_letters {
            guessed_text += string(k) + " "
        }
        rl.DrawText(guessed_text, 200, 300, 20, rl.GRAY)

        // 🏁 End screen
        if game_over {
            if win {
                rl.DrawText("YOU WIN!", 300, 400, 30, rl.GREEN)
            } else {
                lose_text := fmt.tprintf("YOU LOST! Word: %s", current_word)
                rl.DrawText(lose_text, 200, 400, 30, rl.RED)
            }

            rl.DrawText("Press R to restart", 260, 450, 20, rl.DARKGRAY)
        }

        rl.EndDrawing()
    }

    rl.CloseWindow()
}
