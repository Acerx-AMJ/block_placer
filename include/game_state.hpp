#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

// Includes

#include "button.hpp"
#include "state.hpp"
#include <unordered_map>
#include <vector>

// Structs

// Tetromino

struct Tetromino {
   std::vector<std::vector<bool>> tiles;
   int rotation = 0;
};

// Tile

struct Tile {
   enum Type { off, on, border };

   Type type = Type::off;
   Color color;
};

// Keybinds

struct Keys {
   int rotate, left, right, down, send;
};

// Player

struct Player {
   std::vector<Tetromino> bag;
   Tetromino tetromino, next_tetromino;
   Keys key;
   Color color, next_color;
   Vector2 pos, starting_pos;
   int preview_y = 0, id = 0;
   bool soft_drop = false, hard_drop = false;
   float down_timer = 0;
};

// Game state

class GameState : public State {
   // Enums

   enum class Path { left, right, down, current };
   enum class Phase { fading_in, fading_out, playing, paused, lost };

   // Variables

   std::unordered_map<int, float> keys_down;
   std::vector<std::vector<std::vector<Tile>>> tiles;
   std::vector<std::vector<std::vector<Tile>>> next_tiles;
   std::vector<Player> players;
   
   Texture tile_tx;
   Vector2 grid, tile;
   Color screen_tint, lost_screen_tint;
   Button continue_button, restart_button, menu_button;

   int game_width = 0, game_height = 0, score = 0, hi_score = 0, total_clears = 0, combo_count = -1, difficult_count = 0, level = 0, player_count = 0;
   float down_after = 1, fade_in_timer = 0, fade_out_timer = 0, lost_timer = 0;
   bool restart = false, lost = false, versus = false, left_win = false;
   Phase phase = Phase::fading_in;
   
public:
   // Constructors

   GameState(const Vector2& grid_size, int player_count, bool versus);
   ~GameState();

   // Update

   void update() override;
   void update_fading_in();
   void update_fading_out();
   void update_game();
   void update_pause_screen();
   void update_lost_screen();

   // Render

   void render() override;

   // Change states

   void change_state(States& states) override;

   // Utility

   void draw_tetromino(const Player& player);
   void draw_next_tetromino(const Player& player);

   bool can_move(const Tetromino& tetromino, const Vector2& pos, Path type, int id);
   void rotate(Player& player);

   void clear_cleared_rows(const Player& player);
   void add_drop_score(const Player& player, bool hard);
   void add_score(int plus);
   Tetromino get_random_tetromino(Player& player);
   Color get_random_color();
   bool is_versus_block(const Color& color);

   void save_hi_score(int hi_score);
   int load_hi_score();

   void update_key(int key);
   bool key_down(int key);
};

#endif
