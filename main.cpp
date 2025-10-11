// Includes

#include "raylib.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>
#include <vector>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Constants

constexpr Vector2 screen {636, 700}; 
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

// Global variables

std::vector<std::vector<Tile>> tiles;
std::vector<std::vector<Tile>> next_tiles;

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

std::vector<Piece> bag;

std::vector<Color> colors {
    RED, ORANGE, YELLOW, GREEN, GOLD, PINK, MAROON, LIME, DARKGREEN,
    SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE
};

// Save Load functions

void save_hi_score(int hi_score) {
    std::ofstream file ("save.data");
    file << std::to_string(hi_score) << '\n';
    file.close();
}

int load_hi_score() {
    std::fstream file ("save.data");
    std::string line;
    file >> line;
    file.close();

    try {
        return std::stoi(line);
    } catch (...) {
        return 0;
    }
}

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

void draw_next(Piece& piece, const Color& color) {
    for (int y = 1; y < 5; ++y) {
        for (int x = 1; x < 5; ++x) {
            next_tiles[y][x].on = false;
        }
    }
    int oy = (piece.tiles.size() == 4 ? 1 : 2);
    int ox = (piece.tiles[0].size() == 2 ? 2 : 1);

    for (int y = oy; y < piece.tiles.size() + oy; ++y) {
        for (int x = ox; x < piece.tiles[0].size() + ox; ++x) {
            if (piece.tiles[y - oy][x - ox]) {
                next_tiles[y][x].on = true;
                next_tiles[y][x].color = color;
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
        for (int xx = x; xx < tile_size.x and xx < x + (int)piece.tiles[0].size(); ++xx) {
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

void clear_cleared_rows(int& score) {
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

    if (cleared_rows.size() == 1) {
        score += 100;
    } else if (cleared_rows.size() == 2) {
        score += 300;
    } else if (cleared_rows.size() == 3) {
        score += 500;
    } else if (cleared_rows.size() == 4) {
        score += 800;
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

Piece get_random_piece() {
    if (bag.empty()) {
        std::random_device rd;
        std::mt19937 device(rd());
        std::shuffle(pieces.begin(), pieces.end(), device);

        for (const auto& piece : pieces) {
            bag.push_back(piece);
        }
    }
    auto piece = bag.back();
    bag.pop_back();
    return piece;
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

    for (int y = 0; y < 6; ++y) {
        std::vector<Tile> row;
        for (int x = 0; x < 6; ++x) {
            if (y == 0 or y == 5 or x == 0 or x == 5) {
                Tile tile {true, true, GRAY};
                row.push_back(tile);
            } else {
                row.push_back({});
            }
        }
        next_tiles.push_back(row);
    }

    Piece piece = get_random_piece();
    Color color = get_random_color();
    Piece next_piece = get_random_piece();
    Color next_color = get_random_color();
    draw_next(next_piece, next_color);

    int x = 4;
    int y = 1;
    bool make_next_piece = false;

    float down_timer = 0.f;
    float down_after = 1.f;

    int score = 0;
    int best_score = load_hi_score();

    while (!WindowShouldClose()) {
        // Controls

        if (make_next_piece) {
            clear_cleared_rows(score);
            piece = next_piece;
            color = next_color;
            next_piece = get_random_piece();
            next_color = get_random_color();
            draw_next(next_piece, next_color);

            x = 4;
            y = 1;
            make_next_piece = false;
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

        if (key_pressed(KEY_SPACE)) {
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
                make_next_piece = true;
            }
        }
        draw(piece, x, y, color);

        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
            break;
        }

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

            for (int y = 0; y < 6; ++y) {
                for (int x = 0; x < 6; ++x) {
                    if (next_tiles[y][x].on) {
                        Vector2 pos {(x + 13) * tile_texture.width * tile_scale, (y + 2) * tile_texture.height * tile_scale};
                        DrawTextureEx(tile_texture, pos, 0.f, tile_scale, next_tiles[y][x].color);
                    }
                }
            }

            Piece copy = piece;
            int copy_y = y;

            clear(piece, x, y);
            while (can_move(copy, x, copy_y, DOWN)) {
                copy_y++;
            }
            draw(piece, x, y, color);

            if (copy_y != y) {
                for (int yy = copy_y; yy < copy_y + (int)copy.tiles.size() and yy < tile_size.y; ++yy) {
                    for (int xx = x; xx < x + (int)copy.tiles.size() and xx < tile_size.x; ++xx) {
                        if (copy.tiles[yy - copy_y][xx - x]) {
                            Vector2 pos {xx * tile_texture.width * tile_scale, yy * tile_texture.height * tile_scale};
                            DrawRectangleLines(pos.x, pos.y, tile_texture.width * tile_scale, tile_texture.height * tile_scale, color);
                        }
                    }
                }
            }

            DrawText("NEXT:", 13 * tile_texture.width * tile_scale, tile_texture.height * tile_scale, 20, WHITE);
            DrawText(("SCORE: "s + std::to_string(score)).c_str(), 13 * tile_texture.width * tile_scale, 9 * tile_texture.height * tile_scale, 20, WHITE);
            DrawText(("HI-SCORE: "s + std::to_string(best_score)).c_str(), 13 * tile_texture.width * tile_scale, 11 * tile_texture.height * tile_scale, 20, WHITE);
        EndDrawing();
    }

    if (score >= best_score) {
        save_hi_score(score);
    }
    CloseWindow();
    return 0;
}
