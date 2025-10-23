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
      {{{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}},
      {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
   };

   static const std::unordered_map<int, std::vector<Vector2>> wall_kick_data_jlstz {
      {0, {{0, 0}, {-1, 0}, {-1, -1}, {-1, +2}, {0, -2}}},
      {1, {{0, 0}, {+1, 0}, {+1, +1}, {+1, +2}, {0, -2}}},
      {2, {{0, 0}, {+1, 0}, {+1, -1}, {+1, +2}, {0, -2}}},
      {3, {{0, 0}, {-1, 0}, {-1, +1}, {-1, +2}, {0, -2}}},
   };

   static const std::unordered_map<int, std::vector<Vector2>> wall_kick_data_i {
      {0, {{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}}},
      {1, {{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}}},
      {2, {{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}}},
      {3, {{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}}},
   };

   static const std::vector<Color> colors {
      RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK,
   };

   static const std::vector<float> level_speeds {
      1.1f, 1.05f, 1.f, .95f, .9f, .85f, .8f, .7f, .6f, .5f, .4f, .35f, .3f, .25f, .2f, .175f
   };

   static const std::vector<Keys> keybinds {
      {KEY_W, KEY_A, KEY_D, KEY_S, KEY_SPACE},
      {KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_ENTER}
   };

   constexpr Vector2 next_grid {6, 6};
   constexpr float tile_scale = .5f;
   constexpr float fade_in_time = .5f;
   constexpr float fade_out_time = .5f;
   constexpr int rows_for_level_up = 9;
}

// Constructor

GameState::GameState(const Vector2& grid, int player_count)
   : grid(grid), player_count(player_count) {
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

   for (int i = 0; i < player_count; ++i) {
      std::vector<std::vector<Tile>> grid;
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
         grid.push_back(row);
      }
      next_tiles.push_back(grid);
   }

   for (int i = 0; i < player_count; ++i) {
      Player player;
      player.id = i;
      player.tetromino = get_random_tetromino(player);
      player.color = get_random_color();
      player.next_tetromino = get_random_tetromino(player);
      player.next_color = get_random_color();
      draw_next_tetromino(player);

      player.starting_pos = player.pos = {(float)int((grid.x - 2) / (player_count + 1) * (i + 1)), 1};
      player.key = keybinds[i];
      players.push_back(player);
   }

   hi_score = load_hi_score();
   screen_tint = BLACK;
   lost_screen_tint = {0, 0, 0, 0};

   game_height = next_tiles.size() * (next_grid.y + 2);
   game_width = tile.x * (grid.x + 1);
   SetWindowSize(tile.x * (grid.x + 8), std::max(tile.y * grid.y, (game_height + 8) * tile.y));

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
   for (auto& player : players) {
      if (player.make_next_tetromino) {
         draw_tetromino(player);
         clear_cleared_rows();
         player.tetromino = player.next_tetromino;
         player.color = player.next_color;
         player.next_tetromino = get_random_tetromino(player);
         player.next_color = get_random_color();
         draw_next_tetromino(player);

         player.pos = player.starting_pos;
         player.make_next_tetromino = false;
         player.down_timer = 0.f;

         if (player.soft_drop or player.hard_drop) {
            add_drop_score(player, player.hard_drop);
         }
         player.soft_drop = player.hard_drop = false;

         if (not can_move(player.tetromino, player.pos, Path::current)) {
            lost = true;
            PlaySound(lost_sound);
            phase = Phase::lost;

            for (auto& p : players) {
               p.preview_y = p.pos.y;
            }

            restart_button.rectangle.x = GetScreenWidth() / 2.f - 92.5f;
            menu_button.rectangle.x = GetScreenWidth() / 2.f + 92.5f;
            return;
         }
      }

      if (IsKeyPressed(player.key.rotate)) {
         rotate_wall_kicks(player);
      }

      if ((IsKeyPressed(player.key.down) or IsKeyPressedRepeat(player.key.down)) and can_move(player.tetromino, player.pos, Path::down)) {
         player.pos.y++;
         player.down_timer = 0.f;
         player.soft_drop = true;
      }

      player.pos.x += (IsKeyPressed(player.key.right) or IsKeyPressedRepeat(player.key.right)) and can_move(player.tetromino, player.pos, Path::right);
      player.pos.x -= (IsKeyPressed(player.key.left) or IsKeyPressedRepeat(player.key.left)) and can_move(player.tetromino, player.pos, Path::left);

      if (IsKeyPressed(player.key.send)) {
         while (can_move(player.tetromino, player.pos, Path::down)) {
            player.pos.y++;
         }
         player.down_timer = down_after;
         player.hard_drop = true;
      }

      player.down_timer += GetFrameTime();

      if (player.down_timer >= down_after) {
         player.down_timer -= down_after;

         if (can_move(player.tetromino, player.pos, Path::down)) {
            player.pos.y++;
         } else {
            PlaySound(place_sound);
            player.make_next_tetromino = true;
         }
      }
      int original = player.pos.y;
      player.preview_y = player.pos.y;

      while (can_move(player.tetromino, player.pos, Path::down)) {
         player.pos.y = player.preview_y = player.pos.y + 1;
      }
      player.pos.y = original;
   }

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
   lost_screen_tint.a = 255 * lost_timer;

   if (lost_timer >= 1.f) {
      lost_screen_tint.a = 255;   
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

      for (int i = 0; i < next_tiles.size(); ++i) {
         DrawText(("NEXT P"s + std::to_string(i + 1) + ": "s).c_str(), game_width, ((next_grid.y + 2) * i + 1) * tile.y, 20, WHITE);
         for (int y = 0; y < next_grid.y; ++y) {
            for (int x = 0; x < next_grid.x; ++x) {
               if (next_tiles[i][y][x].type) {
                  DrawTextureEx(tile_tx, {(x + grid.x + 1) * tile.x, (next_grid.y + 2) * i * tile.y + (y + 2) * tile.y}, 0.f, tile_scale, next_tiles[i][y][x].color);
               }
            }
         }
      }

      for (const auto& player : players) {
         if (player.preview_y == player.pos.y) {
            continue;
         }

         for (int y = player.preview_y; y < player.preview_y + (int)player.tetromino.tiles.size() and y < grid.y; ++y) {
            for (int x = player.pos.x; x < player.pos.x + (int)player.tetromino.tiles.size() and x < grid.x; ++x) {
               if (player.tetromino.tiles[y - player.preview_y][x - player.pos.x]) {
                  DrawRectangleLines(x * tile.x, y * tile.y, tile.x, tile.y, player.color);
               }
            }
         }

         for (int y = player.pos.y; y < player.pos.y + (int)player.tetromino.tiles.size() and y < grid.y; ++y) {
            for (int x = player.pos.x; x < player.pos.x + (int)player.tetromino.tiles.size() and x < grid.x; ++x) {
               if (player.tetromino.tiles[y - player.pos.y][x - player.pos.x]) {
                  DrawTextureEx(tile_tx, {x * tile.x, y * tile.y}, 0.f, tile_scale, player.color);
               }
            }
         }
      }

      DrawText(("SCORE: "s + std::to_string(score)).c_str(), game_width, (game_height + 1) * tile.y, 20, WHITE);
      DrawText(("HI-SCORE: "s + std::to_string(hi_score)).c_str(), game_width, (game_height + 3) * tile.y, 20, WHITE);
      DrawText(("LEVEL: "s + std::to_string(level)).c_str(), game_width, (game_height + 5) * tile.y, 20, WHITE);
      DrawText(("COMBO: "s + std::to_string((combo_count == -1 ? 0 : combo_count))).c_str(), game_width, (game_height + 7) * tile.y, 20, WHITE);

      if (phase == Phase::paused) {
         DrawText("PAUSED", GetScreenWidth() / 2.f - MeasureText("PAUSED", 60) / 2.f, GetScreenHeight() / 3.f, 60, WHITE);
         continue_button.draw();
         restart_button.draw();
         menu_button.draw();
      } else if (lost) {
         DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), lost_screen_tint);

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
      states.push_back(std::make_unique<GameState>(grid, player_count));
   } else {
      states.push_back(std::make_unique<MenuState>());
   }
}

// Draw functions

void GameState::draw_tetromino(const Player& player) {
   for (int y = player.pos.y; y < grid.y and y < player.pos.y + (int)player.tetromino.tiles.size(); ++y) {
      for (int x = player.pos.x; x < grid.x and x < player.pos.x + (int)player.tetromino.tiles.size(); ++x) {
         if (player.tetromino.tiles[y - player.pos.y][x - player.pos.x] and tiles[y][x].type != Tile::border) {
            tiles[y][x].type = Tile::on;
            tiles[y][x].color = player.color;
         }
      }
   }
}

void GameState::draw_next_tetromino(const Player& player) {
   for (int y = 1; y < next_grid.y - 1; ++y) {
      for (int x = 1; x < next_grid.x - 1; ++x) {
         next_tiles[player.id][y][x].type = Tile::off;
      }
   }
   int ox = 1 + (player.next_tetromino.tiles.size() != 4);
   int oy = 1 + (player.next_tetromino.tiles.size() == 2);

   for (int y = oy; y < player.next_tetromino.tiles.size() + oy; ++y) {
      for (int x = ox; x < player.next_tetromino.tiles.size() + ox; ++x) {
         if (player.next_tetromino.tiles[y - oy][x - ox]) {
            next_tiles[player.id][y][x].type = Tile::on;
            next_tiles[player.id][y][x].color = player.next_color;
         }
      }
   }
}

// Collision functions

bool GameState::can_move(const Tetromino& tetromino, const Vector2& pos, Path type) {
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

void GameState::rotate_wall_kicks(Player& player) {
   if (player.tetromino.tiles.size() == 2) {
      return;
   }

   int rotation = player.tetromino.rotation;
   Vector2 original_pos = player.pos;
   const auto& data = (player.tetromino.tiles.size() == 3 ? wall_kick_data_jlstz.at(rotation) : wall_kick_data_i.at(rotation)); 

   for (const auto& offset : data) {
      player.pos = {original_pos.x + offset.x, original_pos.y + offset.y};
      if (rotate(player)) {
         return;
      }
   }
   player.pos = original_pos;
}

bool GameState::rotate(Player& player) {
   Tetromino new_tetromino = player.tetromino;

   for (int y = 0; y < player.tetromino.tiles.size(); ++y) {
      for (int x = 0; x < player.tetromino.tiles.size(); ++x) {
         new_tetromino.tiles[y][x] = player.tetromino.tiles[player.tetromino.tiles.size() - x - 1][y];
      }
   }

   bool can_rotate = can_move(new_tetromino, player.pos, Path::current);
   if (can_rotate) {
      player.tetromino = new_tetromino;
      player.tetromino.rotation = (player.tetromino.rotation + 1) % 4;
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

   if (cleared.size() != 4 and not cleared.empty()) {
      difficult_count = 0;
   }

   if (difficult_count >= 2 and last_difficult != difficult_count) {
      PlaySound(btb_sound);
   }
}

// Utility functions

void GameState::add_drop_score(const Player& player, bool hard) {
   for (int y = 0; y < player.tetromino.tiles.size(); ++y) {
      for (int x = 0; x < player.tetromino.tiles.size(); ++x) {
         score += player.tetromino.tiles[y][x] * (hard + 1);
      }
   }
}

void GameState::add_score(int plus) {
   int level_multiplier = (level == 0 ? 1 : level);
   score += plus * level_multiplier * (difficult_count >= 2 ? 1.5f : 1.f);
}

Tetromino GameState::get_random_tetromino(Player& player) {
   if (player.bag.empty()) {
      for (const auto& tetromino : tetrominoes) {
         player.bag.push_back(tetromino);
      }
      std::random_device rd;
      std::mt19937 device(rd());
      std::shuffle(player.bag.begin(), player.bag.end(), device);
   }
   auto tetromino = player.bag.back();
   player.bag.pop_back();
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
