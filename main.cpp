// Includes

#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <vector>

// Constants

constexpr Vector2 screen {600, 700}; 
constexpr Vector2 tile_size {12, 22};
constexpr float tile_scale = .5f;

// Tiles

struct Piece {
    std::vector<std::vector<bool>> tiles;
};

struct Tile {
    bool border = false, on = false;
    Color color;
};

std::vector<std::vector<Tile>> tiles;

std::vector<Piece> pieces {
    {{{1, 1}, {1, 1}}},
    {{{0, 0, 1}, {1, 1, 1}, {0, 0, 0}}},
    {{{1, 0, 0}, {1, 1, 1}, {0, 0, 0}}},
    {{{0, 1, 1}, {1, 1, 0}, {0, 0, 0}}},
    {{{1, 1, 0}, {0, 1, 1}, {0, 0, 0}}},
    {{{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}},
    {{{0, 0, 0}, {1, 1, 1}, {0, 0, 0}}},
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
};

std::vector<Color> colors {
    RED, ORANGE, YELLOW, GREEN, GOLD, PINK, MAROON, LIME, DARKGREEN,
    SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE
};

// Draw functions

void clear(Piece& piece, int x, int y) {
    for (int yy = y; yy < tile_size.y and yy < y + (int)piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + (int)piece.tiles[0].size(); ++xx) {
            if (piece.tiles[yy - y][xx - x] and not tiles[yy][xx].border) {
                tiles[yy][xx].on = false;
            }
        }
    }
}

void draw(Piece& piece, int x, int y, const Color& color) {
    for (int yy = y; yy < tile_size.y and yy < y + (int)piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + (int)piece.tiles[0].size(); ++xx) {
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
constexpr int CURRENT = 3;

bool can_move(Piece& piece, int x, int y, int direction) {
    for (int yy = y; yy < tile_size.y and yy < y + (int)piece.tiles.size(); ++yy) {
        for (int xx = x; xx < tile_size.x and xx < x + (int)piece.tiles.size(); ++xx) {
            if (not piece.tiles[yy - y][xx - x]) {
                continue;
            }

            if (direction == LEFT and (xx - 1 < 0 or tiles[yy][xx - 1].on)) {
                return false;
            } else if (direction == RIGHT and (xx + 1 >= tile_size.x or tiles[yy][xx + 1].on)) {
                return false;
            } else if (direction == DOWN and (yy + 1 >= tile_size.y or tiles[yy + 1][xx].on)) {
                return false;
            } else if (direction == CURRENT and tiles[yy][xx].on) {
                return false;
            }
        }
    }
    return true;
}

bool rotate(Piece& piece, int x, int y) {
    Piece new_piece = piece;
    
    for (int y = 0; y < piece.tiles.size(); ++y) {
        for (int x = 0; x < piece.tiles[0].size(); ++x) {
            new_piece.tiles[y][x] = piece.tiles[piece.tiles.size() - x - 1][y];
        }
    }

    bool can_rotate = can_move(new_piece, x, y, CURRENT);
    if (can_rotate) {
        piece = new_piece;
    }
    return can_rotate;
}

// Clear

void clear_cleared_rows() {
    std::vector<int> cleared_rows;
    
    for (int y = 1; y < tile_size.y - 1; ++y) {
        for (int x = 1; x < tile_size.x - 1; ++x) {
            if (not tiles[y][x].on) {
                break;
            }

            if (x == tile_size.x - 2) {
                cleared_rows.push_back(y);
                for (int nx = 1; nx < tile_size.x - 1; ++nx) {
                    tiles[y][nx].on = false;
                }
            }
        }
    }

    if (cleared_rows.empty()) {
        return;
    }

    for (const auto& yy : cleared_rows) {
        for (int y = yy; y >= 2; --y) {
            for (int x = 1; x < tile_size.x - 1; ++x) {
                tiles[y][x] = tiles[y - 1][x];
            }
        }
    }
}

// Other helper functions

bool key_pressed(int key) {
    return IsKeyPressed(key) or IsKeyPressedRepeat(key);
}

Piece& get_random_piece() {
    return pieces[rand() % pieces.size()];
}

Color& get_random_color() {
    return colors[rand() % colors.size()];
}

// Main function

int main() {
    srand(time(nullptr));
    InitWindow(screen.x, screen.y, "Tetris Clone");
    SetTargetFPS(60);

    Texture tile_texture = LoadTexture("assets/tile.png");

    for (int y = 0; y < tile_size.y; ++y) {
        std::vector<Tile> row;
        for (int x = 0; x < tile_size.x; ++x) {
            if (y == 0 or y == tile_size.y - 1 or x == 0 or x == tile_size.x - 1) {
                Tile tile {true, true, GRAY};
                row.push_back(tile);
            } else {
                row.push_back({});
            }
        }
        tiles.push_back(row);
    }

    Piece piece = get_random_piece();
    Color color = get_random_color();
    int x = 4;
    int y = 1;
    bool next_piece = false;

    float down_timer = 0.f;
    float down_after = 1.f;

    while (!WindowShouldClose()) {
        // Controls

        if (next_piece) {
            clear_cleared_rows();
            piece = get_random_piece();
            color = get_random_color();
            x = 4;
            y = 1;
            next_piece = false;
            down_timer = 0.f;
        }

        clear(piece, x, y);
        if (key_pressed(KEY_UP) or key_pressed(KEY_W)) {
            bool rotated = rotate(piece, x, y);
            if (not rotated) {
                rotated = rotate(piece, x + 1, y);
                if (rotated) {
                    x++;
                } else {
                    rotated = rotate(piece, x - 1, y);
                    if (rotated) {
                        x--;
                    }
                }
            }
        }

        if ((key_pressed(KEY_DOWN) or key_pressed(KEY_S)) and can_move(piece, x, y, DOWN)) {
            y++;
            down_timer = 0.f;
        }

        if ((key_pressed(KEY_RIGHT) or key_pressed(KEY_D)) and can_move(piece, x, y, RIGHT)) {
            x++;
        }

        if ((key_pressed(KEY_LEFT) or key_pressed(KEY_A)) and can_move(piece, x, y, LEFT)) {
            x--;
        }

        if (key_pressed(KEY_UP) or key_pressed(KEY_SPACE)) {
            while (can_move(piece, x, y, DOWN)) {
                y++;
            }
            down_timer = down_after;
        }

        down_timer += GetFrameTime();
        if (down_timer >= down_after) {
            down_timer -= down_after;

            if (can_move(piece, x, y, DOWN)) {
                y++;
            } else {
                next_piece = true;
            }
        }
        draw(piece, x, y, color);

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
