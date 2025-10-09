// Includes

#include "raylib.h"
#include <vector>

// Constants

constexpr Vector2 screen {540, 636}; 
constexpr Vector2 tile_size {10, 20};
constexpr float tile_scale = .5f;

// Tiles

struct Piece {
    std::vector<std::vector<bool>> tiles;
};

struct Tile {
    bool border = false, on = false;
    Color color;
};

Tile tiles[(unsigned)tile_size.y][(unsigned)tile_size.x];

std::vector<Piece> pieces {
    {{{1, 1}, {1, 1}}}, {{{1, 1, 1}, {0, 0, 1}}}
};

// Draw functions

void clear(Piece& piece, int x, int y) {
    for (int yy = y; yy < tile_size.y and yy < y + piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + piece.tiles[0].size(); ++xx) {
            if (piece.tiles[yy - y][xx - x] and not tiles[yy][xx].border) {
                tiles[yy][xx].on = false;
            }
        }
    }
}

void draw(Piece& piece, int x, int y, const Color& color) {
    for (int yy = y; yy < tile_size.y and yy < y + piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + piece.tiles[0].size(); ++xx) {
            if (piece.tiles[yy - y][xx - x] and not tiles[yy][xx].border) {
                tiles[yy][xx].on = true;
                tiles[yy][xx].color = color;
            }
        }
    }
}

// Collision functions

constexpr int LEFT = 0;
constexpr int RIGHT = 1;
constexpr int DOWN = 2;

bool can_move(Piece& piece, int x, int y, int direction) {
    for (int yy = y; yy < tile_size.y and yy < y + piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + piece.tiles[0].size(); ++xx) {
            if (not piece.tiles[yy - y][xx - x]) {
                continue;
            }

            if (direction == LEFT and (xx - 1 < 0 or tiles[yy][xx - 1].on)) {
                return false;
            } else if (direction == RIGHT and (xx + 1 >= tile_size.x or tiles[yy][xx + 1].on)) {
                return false;
            } else if (direction == DOWN and (yy + 1 >= tile_size.y or tiles[yy + 1][xx].on)) {
                return false;
            }
        }
    }
    return true;
}

// Other helper functions

bool key_pressed(int key) {
    return IsKeyPressed(key) or IsKeyPressedRepeat(key);
}

// Main function

int main() {
    InitWindow(screen.x, screen.y, "Tetris Clone");
    SetTargetFPS(60);

    Texture tile_texture = LoadTexture("assets/tile.png");

    for (int y = 0; y < tile_size.y; ++y) {
        for (int x = 0; x < tile_size.x; ++x) {
            if (y == 0 or y == tile_size.y - 1 or x == 0 or x == tile_size.x - 1) {
                tiles[y][x].border = true;
                tiles[y][x].on = true;
                tiles[y][x].color = GRAY;
            }
        }
    }

    Piece piece = pieces[1];
    int x = 5;
    int y = 1;
    draw(piece, x, y, GREEN);

    while (!WindowShouldClose()) {
        // Controls

        clear(piece, x, y);
        if ((key_pressed(KEY_DOWN) or key_pressed(KEY_S)) and can_move(piece, x, y, DOWN)) {
            y++;
        }

        if ((key_pressed(KEY_RIGHT) or key_pressed(KEY_D)) and can_move(piece, x, y, RIGHT)) {
            x++;
        }

        if ((key_pressed(KEY_LEFT) or key_pressed(KEY_A)) and can_move(piece, x, y, LEFT)) {
            x--;
        }

        if (key_pressed(KEY_UP) or key_pressed(KEY_W)) {
            while (can_move(piece, x, y, DOWN)) {
                ++y;
            }
        }
        draw(piece, x, y, GREEN);

        // Render
        
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
