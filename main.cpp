// Includes

#include "raylib.h"
#include <vector>

// Constants

constexpr Vector2 screen {540, 636}; 
constexpr Vector2 tile_size {10, 20};
constexpr float tile_scale = .5f;

static std::vector<std::vector<std::vector<bool>>> pieces {
    {{1, 1}, {1, 1}}, {{1, 1, 1}, {0, 0, 1}}
};

// Tile

struct Tile {
    bool border = false, on = false;
    Color color;
};

// Main function

int main() {
    InitWindow(screen.x, screen.y, "Tetris Clone");
    SetTargetFPS(60);

    Texture tile_texture = LoadTexture("assets/tile.png");
    Tile tiles[(unsigned)tile_size.y][(unsigned)tile_size.x];

    for (int y = 0; y < tile_size.y; ++y) {
        for (int x = 0; x < tile_size.x; ++x) {
            if (y == 0 or y == tile_size.y - 1 or x == 0 or x == tile_size.x - 1) {
                tiles[y][x].border = true;
                tiles[y][x].on = true;
                tiles[y][x].color = GRAY;
            }
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            for (int y = 0; y < tile_size.y; ++y) {
                for (int x = 0; x < tile_size.x; ++x) {
                    if (tiles[y][x].on) {
                        Vector2 pos {x * tile_texture.width * tile_scale, y * tile_texture.height * tile_scale};
                        DrawTextureEx(tile_texture, pos, 0.f, tile_scale, tiles[y][x].color);
                    }
                }
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
