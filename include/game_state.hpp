#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

// Includes

#include "button.hpp"
#include "state.hpp"
#include <vector>

// Structs

struct Tetromino {
   std::vector<std::vector<bool>> tiles;
   int rotation = 0;
};

struct Tile {
   enum Type { off, on, border };

   Type type = Type::off;
   Color color;
};

struct Keys {
   int rotate, left, right, down, send;
};

struct Player {
   std::vector<Tetromino> bag;
   Tetromino tetromino, next_tetromino;
   Keys key;
   Color color, next_color;
   Vector2 pos, starting_pos;
   int preview_y = 0, id = 0;
   bool make_next_tetromino = false, soft_drop = false, hard_drop = false;
   float down_timer = 0;
};

// Game state

class GameState : public State {
   enum class Path { left, right, down, current };
   enum class Phase { fading_in, fading_out, playing, paused, lost };

   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<std::vector<Tile>>> next_tiles;
   std::vector<Player> players;
   
   Vector2 grid;
   Color screen_tint, lost_screen_tint;

   Texture tile_tx;
   Vector2 tile;
   Button continue_button, restart_button, menu_button;
   Sound btb_sound, combo_sound, lost_sound, place_sound;

   int game_width = 0, game_height = 0;
   int score = 0;
   int hi_score = 0;
   int total_clears = 0;
   int combo_count = -1;
   int difficult_count = 0;
   int level = 0;
   int player_count = 0;
   bool restart = false;
   bool lost = false;
   float down_after = 1;
   float fade_in_timer = 0;
   float fade_out_timer = 0;
   float lost_timer = 0;
   Phase phase = Phase::fading_in;
   
public:
   GameState(const Vector2& grid_size, int player_count);
   ~GameState();

   // Update functions

   void update() override;
   void update_fading_in();
   void update_fading_out();
   void update_game();
   void update_pause_screen();
   void update_lost_screen();

   // Render function

   void render() override;

   // Change states function

   void change_state(States& states) override;

   // Draw functions

   void draw_tetromino(const Player& player);
   void draw_next_tetromino(const Player& player);

   // Collision functions

   bool can_move(const Tetromino& tetromino, const Vector2& pos, Path type);
   void rotate_wall_kicks(Player& player);
   bool rotate(Player& player);

   // Clear function

   void clear_cleared_rows();

   // Utility functions

   void add_drop_score(const Player& player, bool hard);
   void add_score(int plus);
   Tetromino get_random_tetromino(Player& player);
   Color get_random_color();

   // Save/load functions

   void save_hi_score(int hi_score);
   int load_hi_score();
};

#endif
