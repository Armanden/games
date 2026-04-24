#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/*
 * HANGMAN GAME
 * A classic word guessing game with visual hangman drawing.
 *
 * Controls:
 * - A-Z: Guess a letter
 * - R: Restart game (after game ends)
 */

#define MAX_WRONG  6
#define MAX_WORD_LEN 64

/* Game state structure */
typedef struct {
    char word[MAX_WORD_LEN];
    char visible[MAX_WORD_LEN];
    bool guessed[26];
    int wrong_guesses;
    bool game_over;
    bool win;
} GameState;

/* Word list - cities and animals */
const char *words[] = {
    "istanbul", "samsun", "antalya", "rize", "ankara", "corum",
    "konya", "izmir", "trabzon", "gaziantep", "mugla", "bolu",
    "kocaeli", "eskisehir", "diyarbakir", "sanliurfa", "van",
    "cat", "dog", "elephant", "giraffe", "penguin", "dolphin",
    "mountain", "river", "ocean", "forest", "desert", "valley",
    "puzzle", "mystery", "secret", "hidden", "locked", "coded",
};
const int WORD_COUNT = sizeof(words) / sizeof(words[0]);

/* Initialize game with a random word */
void init_game(GameState *state) {
    /* Select random word */
    strcpy(state->word, words[rand() % WORD_COUNT]);
    int len = strlen(state->word);

    /* Initialize visible word with underscores */
    for (int i = 0; i < len; i++) {
        state->visible[i] = '_';
    }
    state->visible[len] = '\0';

    /* Reset guessed letters */
    for (int i = 0; i < 26; i++) {
        state->guessed[i] = false;
    }

    state->wrong_guesses = 0;
    state->game_over = false;
    state->win = false;
}

/* Draw the hangman figure based on wrong guess count */
void draw_hangman(int wrong) {
    /* Gallows structure (static) */
    DrawLine(150, 400, 350, 400, BLACK);    /* Base */
    DrawLine(200, 400, 200, 150, BLACK);    /* Pole */
    DrawLine(200, 150, 300, 150, BLACK);    /* Top beam */
    DrawLine(300, 150, 300, 180, BLACK);    /* Rope */

    /* Body parts - drawn progressively with each wrong guess */
    if (wrong >= 1) {  /* Head */
        DrawCircleLines(300, 210, 30, BLACK);
    }
    if (wrong >= 2) {  /* Body */
        DrawLine(300, 240, 300, 320, BLACK);
    }
    if (wrong >= 3) {  /* Left arm */
        DrawLine(300, 260, 260, 300, BLACK);
    }
    if (wrong >= 4) {  /* Right arm */
        DrawLine(300, 260, 340, 300, BLACK);
    }
    if (wrong >= 5) {  /* Left leg */
        DrawLine(300, 320, 260, 370, BLACK);
    }
    if (wrong >= 6) {  /* Right leg - game over */
        DrawLine(300, 320, 340, 370, BLACK);
    }
}

int main(void) {
    InitWindow(800, 600, "Hangman");
    SetTargetFPS(60);

    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    /* Initialize game state */
    GameState state;
    init_game(&state);

    /* Main game loop */
    while (!WindowShouldClose()) {
        /* Handle input when game is active */
        if (!state.game_over) {
            /* Check each letter key A-Z */
            for (int i = 0; i < 26; i++) {
                int key = KEY_A + i;
                if (IsKeyPressed(key)) {
                    char letter = 'a' + i;

                    /* Skip if already guessed */
                    if (state.guessed[i]) continue;
                    state.guessed[i] = true;

                    /* Check if letter exists in word */
                    bool found = false;
                    for (int j = 0; j < strlen(state.word); j++) {
                        if (state.word[j] == letter) {
                            state.visible[j] = letter;
                            found = true;
                        }
                    }

                    /* Wrong guess - increment counter */
                    if (!found) {
                        state.wrong_guesses++;
                    }

                    /* Check win condition - all letters revealed */
                    state.win = true;
                    for (int j = 0; j < strlen(state.word); j++) {
                        if (state.visible[j] == '_') {
                            state.win = false;
                            break;
                        }
                    }

                    /* Check lose condition */
                    if (state.win || state.wrong_guesses >= MAX_WRONG) {
                        state.game_over = true;
                    }
                }
            }
        }

        /* Restart game with R key */
        if (state.game_over && IsKeyPressed(KEY_R)) {
            init_game(&state);
        }

        /* ============ RENDERING ============ */
        BeginDrawing();
        ClearBackground(WHITE);

        /* Draw hangman figure */
        draw_hangman(state.wrong_guesses);

        /* Draw the word with spacing */
        DrawText(state.visible, 420, 200, 50, DARKGRAY);

        /* Draw underscores for visual spacing */
        for (int i = 0; i < strlen(state.visible); i++) {
            DrawText("_", 420 + i * 35, 260, 50, LIGHTGRAY);
        }

        /* Draw attempts remaining */
        int remaining = MAX_WRONG - state.wrong_guesses;
        char attempts_text[32];
        sprintf(attempts_text, "Remaining: %d/%d", remaining, MAX_WRONG);
        DrawText(attempts_text, 20, 20, 25, remaining <= 2 ? RED : DARKGRAY);

        /* Draw instructions */
        DrawText("Press A-Z to guess a letter", 20, 450, 20, GRAY);

        /* Draw guessed letters */
        DrawText("Guessed: ", 20, 480, 20, GRAY);
        int x_pos = 140;
        for (int i = 0; i < 26; i++) {
            if (state.guessed[i]) {
                char letter_str[2] = {'a' + i, '\0'};
                DrawText(letter_str, x_pos, 480, 20, DARKGRAY);
                x_pos += 20;
            }
        }

        /* Game over overlay */
        if (state.game_over) {
            DrawRectangle(0, 0, 800, 600, (Color){0, 0, 0, 127});  /* Semi-transparent black */

            if (state.win) {
                DrawText("YOU WIN!", 300, 250, 50, GREEN);
            } else {
                DrawText("GAME OVER!", 260, 250, 50, RED);
            }

            DrawText("The word was:", 260, 320, 25, WHITE);
            DrawText(state.word, 280, 355, 35, GOLD);
            DrawText("Press R to play again", 260, 420, 25, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}