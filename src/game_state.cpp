#include "game_state.hpp"

// Includes

#include <algorithm>
#include <fstream>
#include <random>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Constants

namespace {
   // Tetromino width and height must be the same!
   static const std::vector<Tetromino> tetrominoes {
      {{1, 1}, {1, 1}},
      {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}},
      {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}},
      {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}},
      {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}},
      {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}},
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
   };

   static const std::vector<Color> colors {
      RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK,
   };

   static const std::vector<float> level_speeds {
      1.15f, 1.1f, 1.05f, 1.f, .95f, .9f, .85f, .8f, .75f, .7f, .65f, .55f, .45f, .35f, .25f, .175f
   };

   constexpr Vector2 starting_pos {4, 1};
   constexpr Vector2 grid {12, 22};
   constexpr Vector2 next_grid {6, 6};
   constexpr float tile_scale = .5f;
}

// Constructor

GameState::GameState() {
   tile_tx = LoadTexture("assets/tile.png");
   tile = {tile_tx.width * tile_scale, tile_tx.height * tile_scale};

   for (int y = 0; y < grid.y; ++y) {
      std::vector<Tile> row;
      for (int x = 0; x < grid.x; ++x) {
         if (y == 0 or y == grid.y - 1 or x == 0 or x == grid.x - 1) {
            Tile tile {Tile::border, GRAY};
            row.push_back(tile);
         } else {
            row.push_back({});
         }
      }
      tiles.push_back(row);
   }

   for (int y = 0; y < next_grid.y; ++y) {
      std::vector<Tile> row;
      for (int x = 0; x < next_grid.x; ++x) {
         if (y == 0 or y == next_grid.y - 1 or x == 0 or x == next_grid.x - 1) {
            Tile tile {Tile::border, GRAY};
            row.push_back(tile);
         } else {
            row.push_back({});
         }
      }
      next_tiles.push_back(row);
   }
   tetromino = get_random_tetromino();
   color = get_random_color();
   next_tetromino = get_random_tetromino();
   next_color = get_random_color();
   draw_next_tetromino();

   hi_score = load_hi_score();
   pos = starting_pos;
}

GameState::~GameState() {
   if (score >= hi_score) {
      save_hi_score(score);
   }
}

// Update functions

void GameState::update() {
   if (make_next_tetromino) {
      clear_cleared_rows();
      tetromino = next_tetromino;
      color = next_color;
      next_tetromino = get_random_tetromino();
      next_color = get_random_color();
      draw_next_tetromino();

      pos = starting_pos;
      make_next_tetromino = false;
      down_timer = 0.f;

      if (soft_drop or hard_drop) {
         add_drop_score(hard_drop);
      }
      soft_drop = hard_drop = false;
   }

   clear_tetromino();
   if ((IsKeyPressed(KEY_UP) or IsKeyPressed(KEY_W)) and not rotate()) {
      pos.x++;
      if (not rotate()) {
         pos.x -= 2;
         pos.x += (not rotate());
      }
   }

   if ((key_pressed(KEY_DOWN) or key_pressed(KEY_S)) and can_move(tetromino, Path::down)) {
      pos.y++;
      down_timer = 0.f;
      soft_drop = true;
   }

   pos.x += (key_pressed(KEY_RIGHT) or key_pressed(KEY_D)) and can_move(tetromino, Path::right);
   pos.x -= (key_pressed(KEY_LEFT) or key_pressed(KEY_A)) and can_move(tetromino, Path::left);

   if (IsKeyPressed(KEY_SPACE)) {
      while (can_move(tetromino, Path::down)) {
         pos.y++;
      }
      down_timer = down_after;
      hard_drop = true;
   }

   down_timer += GetFrameTime();
   if (down_timer >= down_after) {
      down_timer -= down_after;

      if (can_move(tetromino, Path::down)) {
         pos.y++;
      } else {
         make_next_tetromino = true;
      }
   }
   int original = pos.y;
   preview_y = pos.y;

   while (can_move(tetromino, Path::down)) {
      pos.y = preview_y = pos.y + 1;
   }
   pos.y = original;
   draw_tetromino();
}

// Render function

void GameState::render() {
   BeginDrawing();
      ClearBackground(BLACK);

      for (int y = 0; y < grid.y; ++y) {
         for (int x = 0; x < grid.x; ++x) {
            if (tiles[y][x].type) {
               DrawTextureEx(tile_tx, {x * tile.x, y * tile.y}, 0.f, tile_scale, tiles[y][x].color);
            }
         }
      }

      for (int y = 0; y < next_grid.y; ++y) {
         for (int x = 0; x < next_grid.x; ++x) {
            if (next_tiles[y][x].type) {
               DrawTextureEx(tile_tx, {(x + 13) * tile.x, (y + 2) * tile.y}, 0.f, tile_scale, next_tiles[y][x].color);
            }
         }
      }

      if (preview_y != pos.y) {
         for (int y = preview_y; y < preview_y + (int)tetromino.size() and y < grid.y; ++y) {
            for (int x = pos.x; x < pos.x + (int)tetromino.size() and x < grid.x; ++x) {
               if (tetromino[y - preview_y][x - pos.x]) {
                  DrawRectangleLines(x * tile.x, y * tile.y, tile.x, tile.y, color);
               }
            }
         }
      }

      DrawText("NEXT:", 13 * tile.x, tile.y, 20, WHITE);
      DrawText(("SCORE: "s + std::to_string(score)).c_str(), 13 * tile.x, 9 * tile.y, 20, WHITE);
      DrawText(("HI-SCORE: "s + std::to_string(hi_score)).c_str(), 13 * tile.x, 11 * tile.y, 20, WHITE);
      DrawText(("LEVEL: "s + std::to_string(level)).c_str(), 13 * tile.x, 13 * tile.y, 20, WHITE);
      DrawText(("COMBO: "s + std::to_string((combo_count == -1 ? 0 : combo_count))).c_str(), 13 * tile.x, 15 * tile.y, 20, WHITE);
   EndDrawing();
}

// Change states function

void GameState::change_state(States& states) {

}

// Draw functions

void GameState::clear_tetromino() {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.size(); ++x) {
         if (tetromino[y - pos.y][x - pos.x] and tiles[y][x].type != Tile::border) {
            tiles[y][x].type = Tile::off;
         }
      }
   }
}

void GameState::draw_tetromino() {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.size(); ++x) {
         if (tetromino[y - pos.y][x - pos.x] and tiles[y][x].type != Tile::border) {
            tiles[y][x].type = Tile::on;
            tiles[y][x].color = color;
         }
      }
   }
}

void GameState::draw_next_tetromino() {
   for (int y = 1; y < next_grid.y - 1; ++y) {
      for (int x = 1; x < next_grid.x - 1; ++x) {
         next_tiles[y][x].type = Tile::off;
      }
   }
   int ox = 1 + (next_tetromino.size() != 4);
   int oy = 1 + (next_tetromino.size() == 2);

   for (int y = oy; y < next_tetromino.size() + oy; ++y) {
      for (int x = ox; x < next_tetromino.size() + ox; ++x) {
         if (next_tetromino[y - oy][x - ox]) {
            next_tiles[y][x].type = Tile::on;
            next_tiles[y][x].color = next_color;
         }
      }
   }
}

// Collision functions

bool GameState::can_move(Tetromino& tetromino, Path type) {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.size(); ++x) {
         if (not tetromino[y - pos.y][x - pos.x]) {
            continue;
         }

         if (type == Path::left and (x - 1 < 0 or tiles[y][x - 1].type)) {
            return false;
         } else if (type == Path::right and (x + 1 >= grid.x or tiles[y][x + 1].type)) {
            return false;
         } else if (type == Path::down and (y + 1 >= grid.y or tiles[y + 1][x].type)) {
            return false;
         } else if (type == Path::current and tiles[y][x].type) {
            return false;
         }
      }
   }
   return true;
}

bool GameState::rotate() {
   Tetromino new_tetromino = tetromino;

   for (int y = 0; y < tetromino.size(); ++y) {
      for (int x = 0; x < tetromino.size(); ++x) {
         new_tetromino[y][x] = tetromino[tetromino.size() - x - 1][y];
      }
   }

   bool can_rotate = can_move(new_tetromino, Path::current);
   if (can_rotate) {
      tetromino = new_tetromino;
   }
   return can_rotate;
}

// Clear function

void GameState::clear_cleared_rows() {
   std::vector<int> cleared;

   for (int y = 1; y < grid.y - 1; ++y) {
      for (int x = 1; x < grid.x - 1; ++x) {
         if (not tiles[y][x].type) {
            break;
         }

         if (x == grid.x - 2) {
            cleared.push_back(y);
         }
      }
   }

   for (const auto& cy : cleared) {
      for (int y = cy; y >= 1; --y) {
         for (int x = 1; x < grid.x - 1; ++x) {
            if (y == 1) {
               tiles[y][x].type = Tile::off;
            } else {
               tiles[y][x] = tiles[y - 1][x];
            }
         }
      }
   }
   total_clears += cleared.size();
   level = std::min(total_clears / 10, 15);
   down_after = level_speeds[level];

   bool perfect = true;
   for (int y = 1; y < grid.y - 1; ++y) {
      for (int x = 1; x < grid.x - 1; ++x) {
         if (tiles[y][x].type) {
            perfect = false;
            break;
         }
      }
   }

   if (cleared.empty()) {
      combo_count = -1;
   } else {
      combo_count++;
      add_score(50 * combo_count);
   }

   if (cleared.size() == 1) {
      add_score((perfect ? 800 : 100));
   } else if (cleared.size() == 2) {
      add_score((perfect ? 1200 : 300));
   } else if (cleared.size() == 3) {
      add_score((perfect ? 1800 : 500));
   } else if (cleared.size() == 4) {
      add_score((perfect ? 2600 : 800));
   }
}

// Utility functions

void GameState::add_drop_score(bool hard) {
   for (int y = 0; y < tetromino.size(); ++y) {
      for (int x = 0; x < tetromino.size(); ++x) {
         score += tetromino[y][x] * (hard + 1);
      }
   }
}

void GameState::add_score(int plus) {
   int level_multiplier = (level == 0 ? 1 : level);
   score += plus * level_multiplier;
}

bool GameState::key_pressed(int key) {
   return IsKeyPressed(key) or IsKeyPressedRepeat(key);
}

Tetromino GameState::get_random_tetromino() {
   if (bag.empty()) {
      for (const auto& tetromino : tetrominoes) {
         bag.push_back(tetromino);
      }
      std::random_device rd;
      std::mt19937 device(rd());
      std::shuffle(bag.begin(), bag.end(), device);
   }
   auto tetromino = bag.back();
   bag.pop_back();
   return tetromino;
}

Color GameState::get_random_color() {
   return colors[rand() % colors.size()];
}

// Save/load functions

void GameState::save_hi_score(int hi_score) {
   std::ofstream file {"save.data"};
   file << hi_score;
   file.close();
}

int GameState::load_hi_score() {
   std::fstream file {"save.data"};
   std::string line;
   file >> line;
   file.close();

   try {
      return std::stoi(line);
   } catch (...) {
      return 0;
   }
}
