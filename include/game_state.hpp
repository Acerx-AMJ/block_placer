#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

// Includes

#include "state.hpp"
#include <raylib.h>
#include <vector>

// Structs

struct Tetromino {
   std::vector<std::vector<bool>> tiles;
   bool is_t = false;
   int rotation = 0;
};

struct Tile {
   enum Type { off, on, border };

   Type type = Type::off;
   Color color;
};

// Game state

class GameState : public State {
   enum class Path { left, right, down, current };
   enum class Phase { fading_in, fading_out, playing, paused };

   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<Tile>> next_tiles;
   std::vector<Tetromino> bag;

   Tetromino tetromino;
   Tetromino next_tetromino;
   Color color;
   Color next_color;
   Vector2 pos;
   Color screen_tint;

   Texture tile_tx;
   Vector2 tile;
   
   Vector3 tspin_info = {0, 0, 0};
   int preview_y = 0;
   int score = 0;
   int hi_score = 0;
   int total_clears = 0;
   int combo_count = -1;
   int difficult_count = 0;
   int level = 0;
   bool make_next_tetromino = false;
   bool soft_drop = false;
   bool hard_drop = false;
   float down_timer = 0;
   float down_after = 1;
   float fade_in_timer = 0;
   float fade_out_timer = 0;
   Phase phase = Phase::fading_in;
   
public:
   GameState();
   ~GameState();

   // Update functions

   void update() override;
   void update_fading_in();
   void update_fading_out();
   void update_game();
   void update_pause_screen();

   // Render function

   void render() override;

   // Change states function

   void change_state(States& states) override;

   // Draw functions

   void clear_tetromino();
   void draw_tetromino();
   void draw_next_tetromino();

   // Collision functions

   bool can_move(Tetromino& tetromino, Path type);
   void rotate_wall_kicks(bool right);
   bool rotate(bool right);

   // Clear function

   void clear_cleared_rows();

   // Utility functions

   void add_drop_score(bool hard);
   void add_score(int plus);
   Tetromino get_random_tetromino();
   Color get_random_color();

   // Save/load functions

   void save_hi_score(int hi_score);
   int load_hi_score();
};

#endif
