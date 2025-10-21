#include "game_state.hpp"

// Includes

#include "menu_state.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Constants

namespace {
   // Tetromino width and height must be the same!
   static const std::vector<Tetromino> tetrominoes {
      {{{1, 1}, {1, 1}}},
      {{{0, 0, 1}, {1, 1, 1}, {0, 0, 0}}},
      {{{1, 0, 0}, {1, 1, 1}, {0, 0, 0}}},
      {{{0, 1, 1}, {1, 1, 0}, {0, 0, 0}}},
      {{{1, 1, 0}, {0, 1, 1}, {0, 0, 0}}},
      {{{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}, true},
      {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
   };

   static const std::unordered_map<int, std::vector<Vector2>> wall_kick_data_jlstz {
      {0, {{0, 0}, {-1, 0}, {-1, -1}, {-1, +2}, {0, -2}}},
      {4, {{0, 0}, {+1, 0}, {+1, +1}, {+1, +2}, {0, -2}}},
      {1, {{0, 0}, {+1, 0}, {+1, +1}, {+1, +2}, {0, -2}}},
      {5, {{0, 0}, {-1, 0}, {-1, -1}, {-1, +2}, {0, -2}}},
      {2, {{0, 0}, {+1, 0}, {+1, -1}, {+1, +2}, {0, -2}}},
      {6, {{0, 0}, {-1, 0}, {-1, +1}, {-1, +2}, {0, -2}}},
      {3, {{0, 0}, {-1, 0}, {-1, +1}, {-1, +2}, {0, -2}}},
      {7, {{0, 0}, {+1, 0}, {+1, -1}, {+1, +2}, {0, -2}}},
   };

   static const std::unordered_map<int, std::vector<Vector2>> wall_kick_data_i {
      {0, {{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}}},
      {4, {{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}}},
      {1, {{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}}},
      {5, {{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}}},
      {2, {{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}}},
      {6, {{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}}},
      {3, {{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}}},
      {7, {{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}}},
   };

   static const std::vector<Color> colors {
      RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK,
   };

   static const std::vector<float> level_speeds {
      1.1f, 1.05f, 1.f, .95f, .9f, .85f, .8f, .7f, .6f, .5f, .4f, .35f, .3f, .25f, .2f, .175f
   };

   constexpr Vector2 next_grid {6, 6};
   constexpr float tile_scale = .5f;
   constexpr float fade_in_time = .5f;
   constexpr float fade_out_time = .5f;
   constexpr int rows_for_level_up = 9;
}

// Constructor

GameState::GameState(const Vector2& grid)
   : grid(grid) {
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
   pos = {(grid.x - 2) / 2, 1};
   screen_tint = BLACK;
   lost_color = {0, 0, 0, 0};

   SetWindowSize(tile.x * (grid.x + 8), GetScreenHeight());
   game_width = tile.x * (grid.x + 1);

   restart_button.rectangle = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 175.f, 50.f};
   continue_button.rectangle = {restart_button.rectangle.x - 185.f, restart_button.rectangle.y, 175.f, 50.f};
   menu_button.rectangle = {restart_button.rectangle.x + 185.f, restart_button.rectangle.y, 175.f, 50.f};

   restart_button.text = "RESTART";
   continue_button.text = "CONTINUE";
   menu_button.text = "MENU";

   btb_sound = LoadSound("assets/back_to_back.wav");
   combo_sound = LoadSound("assets/combo.wav");
   lost_sound = LoadSound("assets/lost.wav");
   place_sound = LoadSound("assets/place.wav");

}

GameState::~GameState() {
   if (score >= hi_score) {
      save_hi_score(score);
   }
   UnloadTexture(tile_tx);
   UnloadSound(btb_sound);
   UnloadSound(combo_sound);
   UnloadSound(lost_sound);
   UnloadSound(place_sound);
}

// Update functions

void GameState::update() {
   switch (phase) {
   case Phase::fading_in:  update_fading_in();    break;
   case Phase::fading_out: update_fading_out();   break;
   case Phase::playing:    update_game();         break;
   case Phase::paused:     update_pause_screen(); break;
   case Phase::lost:       update_lost_screen();  break;
   }
}

void GameState::update_fading_in() {
   fade_in_timer += GetFrameTime();
   screen_tint.a = 255 - 255 * (fade_in_timer / fade_in_time);
   
   if (fade_in_timer >= fade_in_time) {
      phase = Phase::playing;
      screen_tint.a = 0;
   }
   update_game();
}

void GameState::update_fading_out() {
   fade_out_timer += GetFrameTime();
   screen_tint.a = 255 * (fade_out_timer / fade_out_time);

   if (fade_out_timer >= fade_out_time) {
      quit = true;
      screen_tint.a = 255;
   }
}

void GameState::update_game() {
   if (make_next_tetromino) {
      clear_cleared_rows();
      tetromino = next_tetromino;
      color = next_color;
      next_tetromino = get_random_tetromino();
      next_color = get_random_color();
      draw_next_tetromino();

      pos = {(grid.x - 2) / 2, 1};
      make_next_tetromino = false;
      down_timer = 0.f;

      if (soft_drop or hard_drop) {
         add_drop_score(hard_drop);
      }
      soft_drop = hard_drop = false;

      if (not can_move(tetromino, Path::current)) {
         lost = true;
         PlaySound(lost_sound);
         preview_y = pos.y;
         phase = Phase::lost;

         restart_button.rectangle.x = GetScreenWidth() / 2.f - 92.5f;
         menu_button.rectangle.x = GetScreenWidth() / 2.f + 92.5f;
         return;
      }
   }

   clear_tetromino();
   if (IsKeyPressed(KEY_Q) or IsKeyPressed(KEY_E) or IsKeyPressed(KEY_W) or IsKeyPressed(KEY_UP)) {
      rotate_wall_kicks(not IsKeyPressed(KEY_Q));
   }

   if ((IsKeyPressed(KEY_S) or IsKeyPressedRepeat(KEY_S) or IsKeyPressed(KEY_DOWN) or IsKeyPressedRepeat(KEY_DOWN)) and can_move(tetromino, Path::down)) {
      pos.y++;
      down_timer = 0.f;
      soft_drop = true;
   }

   pos.x += (IsKeyPressed(KEY_D) or IsKeyPressedRepeat(KEY_D) or IsKeyPressed(KEY_RIGHT) or IsKeyPressedRepeat(KEY_RIGHT)) and can_move(tetromino, Path::right);
   pos.x -= (IsKeyPressed(KEY_A) or IsKeyPressedRepeat(KEY_A) or IsKeyPressed(KEY_LEFT) or IsKeyPressedRepeat(KEY_LEFT)) and can_move(tetromino, Path::left);

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
         PlaySound(place_sound);
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

   if (IsKeyPressed(KEY_ESCAPE) and phase == Phase::playing) {
      phase = Phase::paused;
   }
}

void GameState::update_pause_screen() {
   continue_button.update();
   restart_button.update();
   menu_button.update();

   if (IsKeyPressed(KEY_ESCAPE) or continue_button.clicked) {
      phase = Phase::playing;
   }

   if (restart_button.clicked) {
      phase = Phase::fading_out;
      restart = true;
   }

   if (menu_button.clicked) {
      phase = Phase::fading_out;
   }
}

void GameState::update_lost_screen() {
   lost_timer += GetFrameTime();
   lost_color.a = 255 * lost_timer;

   if (lost_timer >= 1.f) {
      lost_color.a = 255;   
   }
   
   restart_button.update();
   menu_button.update();

   if (restart_button.clicked) {
      phase = Phase::fading_out;
      restart = true;
   }

   if (menu_button.clicked) {
      phase = Phase::fading_out;
   }   
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
               DrawTextureEx(tile_tx, {(x + grid.x + 1) * tile.x, (y + 2) * tile.y}, 0.f, tile_scale, next_tiles[y][x].color);
            }
         }
      }

      if (preview_y != pos.y) {
         for (int y = preview_y; y < preview_y + (int)tetromino.tiles.size() and y < grid.y; ++y) {
            for (int x = pos.x; x < pos.x + (int)tetromino.tiles.size() and x < grid.x; ++x) {
               if (tetromino.tiles[y - preview_y][x - pos.x]) {
                  DrawRectangleLines(x * tile.x, y * tile.y, tile.x, tile.y, color);
               }
            }
         }
      }

      DrawText("NEXT:", game_width, tile.y, 20, WHITE);
      DrawText(("SCORE: "s + std::to_string(score)).c_str(), game_width, 9 * tile.y, 20, WHITE);
      DrawText(("HI-SCORE: "s + std::to_string(hi_score)).c_str(), game_width, 11 * tile.y, 20, WHITE);
      DrawText(("LEVEL: "s + std::to_string(level)).c_str(), game_width, 13 * tile.y, 20, WHITE);
      DrawText(("COMBO: "s + std::to_string((combo_count == -1 ? 0 : combo_count))).c_str(), game_width, 15 * tile.y, 20, WHITE);

      if (phase == Phase::paused) {
         DrawText("PAUSED", GetScreenWidth() / 2.f - MeasureText("PAUSED", 60) / 2.f, GetScreenHeight() / 3.f, 60, WHITE);
         continue_button.draw();
         restart_button.draw();
         menu_button.draw();
      } else if (lost) {
         DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), lost_color);

         std::string final_score = "SCORE: "s + std::to_string(score);
         std::string best_score = "HI-SCORE: "s + std::to_string(hi_score);
         DrawText(final_score.c_str(), GetScreenWidth() / 2.f - MeasureText(final_score.c_str(), 30) / 2.f, GetScreenHeight() / 4.f + 50.f, 30, WHITE);
         DrawText(best_score.c_str(), GetScreenWidth() / 2.f - MeasureText(best_score.c_str(), 30) / 2.f, GetScreenHeight() / 4.f + 100.f, 30, WHITE);
         DrawText("GAME OVER", GetScreenWidth() / 2.f - MeasureText("GAME OVER", 60) / 2.f, GetScreenHeight() / 4.f - 10.f, 60, WHITE);

         restart_button.draw();
         menu_button.draw();
      }
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), screen_tint);
   EndDrawing();
}

// Change states function

void GameState::change_state(States& states) {
   if (restart) {
      states.push_back(std::make_unique<GameState>(grid));
   } else {
      states.push_back(std::make_unique<MenuState>());
   }
}

// Draw functions

void GameState::clear_tetromino() {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.tiles.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.tiles.size(); ++x) {
         if (tetromino.tiles[y - pos.y][x - pos.x] and tiles[y][x].type != Tile::border) {
            tiles[y][x].type = Tile::off;
         }
      }
   }
}

void GameState::draw_tetromino() {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.tiles.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.tiles.size(); ++x) {
         if (tetromino.tiles[y - pos.y][x - pos.x] and tiles[y][x].type != Tile::border) {
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
   int ox = 1 + (next_tetromino.tiles.size() != 4);
   int oy = 1 + (next_tetromino.tiles.size() == 2);

   for (int y = oy; y < next_tetromino.tiles.size() + oy; ++y) {
      for (int x = ox; x < next_tetromino.tiles.size() + ox; ++x) {
         if (next_tetromino.tiles[y - oy][x - ox]) {
            next_tiles[y][x].type = Tile::on;
            next_tiles[y][x].color = next_color;
         }
      }
   }
}

// Collision functions

bool GameState::can_move(Tetromino& tetromino, Path type) {
   for (int y = pos.y; y < grid.y and y < pos.y + (int)tetromino.tiles.size(); ++y) {
      for (int x = pos.x; x < grid.x and x < pos.x + (int)tetromino.tiles.size(); ++x) {
         if (not tetromino.tiles[y - pos.y][x - pos.x]) {
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

void GameState::rotate_wall_kicks(bool right) {
   if (tetromino.tiles.size() == 2) {
      rotate(right);
      return;
   }
   int rotation = tetromino.rotation + (not right) * 4;
   Vector2 original_pos = pos;
   const auto& data = (tetromino.tiles.size() == 3 ? wall_kick_data_jlstz.at(rotation) : wall_kick_data_i.at(rotation)); 

   for (const auto& offset : data) {
      pos = {original_pos.x + offset.x, original_pos.y + offset.y};
      if (rotate(right)) {
         tspin_info = {pos.y, offset.x, offset.y};
         return;
      }
   }
   pos = original_pos;
}

bool GameState::rotate(bool right) {
   Tetromino new_tetromino = tetromino;

   for (int y = 0; y < tetromino.tiles.size(); ++y) {
      for (int x = 0; x < tetromino.tiles.size(); ++x) {
         if (right) {
            new_tetromino.tiles[y][x] = tetromino.tiles[tetromino.tiles.size() - x - 1][y];
         } else {
            new_tetromino.tiles[tetromino.tiles.size() - x - 1][y] = tetromino.tiles[y][x];
         }
      }
   }

   bool can_rotate = can_move(new_tetromino, Path::current);
   if (can_rotate) {
      tetromino = new_tetromino;
      tetromino.rotation = (tetromino.rotation + 1) % 4;
   }
   return can_rotate;
}

// Clear function

void GameState::clear_cleared_rows() {
   int last_difficult = difficult_count;
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
   level = std::min(total_clears / rows_for_level_up, 15);
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

   bool tspinned = false;
   if (tetromino.is_t and tspin_info.x == pos.y) {
      int front_count = 0;
      int back_count = 0;
      
      if (not tetromino.tiles[0][1]) {
         front_count += (bool)tiles[pos.y + 2][pos.x].type + (bool)tiles[pos.y + 2][pos.x + 2].type;
         back_count += (bool)tiles[pos.y][pos.x].type + (bool)tiles[pos.y][pos.x + 2].type;
      } else if (not tetromino.tiles[1][2]) {
         front_count += (bool)tiles[pos.y][pos.x].type + (bool)tiles[pos.y + 2][pos.x].type;
         back_count += (bool)tiles[pos.y][pos.x + 2].type + (bool)tiles[pos.y + 2][pos.x + 2].type;
      } else if (not tetromino.tiles[1][0]) {
         front_count += (bool)tiles[pos.y][pos.x + 2].type + (bool)tiles[pos.y + 2][pos.x + 2].type;
         back_count += (bool)tiles[pos.y][pos.x].type + (bool)tiles[pos.y + 2][pos.x].type;
      } else if (not tetromino.tiles[2][1]) {
         front_count += (bool)tiles[pos.y][pos.x].type + (bool)tiles[pos.y][pos.x + 2].type;
         back_count += (bool)tiles[pos.y + 2][pos.x].type + (bool)tiles[pos.y + 2][pos.x + 2].type;
      }
      int multiplier = 1;

      if ((front_count == 2 and back_count >= 1) or (tspin_info.y == 1 and tspin_info.z == 2)) {
         tspinned = true;
         multiplier = 4;
      } else if (front_count >= 1 and back_count == 2) {
         tspinned = true;
      }

      if (tspinned and cleared.empty()) {
         add_score(100 * multiplier);
      } else if (tspinned and cleared.size() == 1) {
         add_score(200 * multiplier);
         difficult_count++;
      } else if (tspinned and cleared.size() == 2) {
         add_score(400 * multiplier);
         difficult_count++;
      }
   }
   tspin_info = {0, 0, 0};

   if (cleared.empty()) {
      combo_count = -1;
   } else {
      combo_count++;
      add_score(50 * combo_count);

      if (combo_count > 0) {
         PlaySound(combo_sound);
      }
   }

   if (cleared.size() == 1) {
      add_score((perfect ? 800 : 100));
   } else if (cleared.size() == 2) {
      add_score((perfect ? 1200 : 300));
   } else if (cleared.size() == 3) {
      add_score((perfect ? 1800 : 500));
   } else if (cleared.size() == 4) {
      add_score((perfect ? 2600 : 800));
      difficult_count++;
   }

   if (cleared.size() != 4 and not tspinned and not cleared.empty()) {
      difficult_count = 0;
   }

   if (difficult_count >= 2 and last_difficult != difficult_count) {
      PlaySound(btb_sound);
   }
}

// Utility functions

void GameState::add_drop_score(bool hard) {
   for (int y = 0; y < tetromino.tiles.size(); ++y) {
      for (int x = 0; x < tetromino.tiles.size(); ++x) {
         score += tetromino.tiles[y][x] * (hard + 1);
      }
   }
}

void GameState::add_score(int plus) {
   int level_multiplier = (level == 0 ? 1 : level);
   score += plus * level_multiplier * (difficult_count >= 2 ? 1.5f : 1.f);
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
