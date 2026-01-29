#include <stdio.h>
#include <raylib.h>

int main() {
  
  InitWindow(1200,900,"Stupid Maze");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
      
    BeginDrawing();
    ClearBackground(RED);
    DrawText("hello",10,10,30,BLACK);
    
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
