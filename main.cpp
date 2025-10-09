#include "raylib.h"
#include <cmath>

int main() {
    InitWindow(960, 540, "Tetris Clone");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
