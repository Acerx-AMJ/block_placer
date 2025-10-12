#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

// Includes

#include "state.hpp"
#include <raylib.h>
#include <vector>

// Structs

using Tetromino = std::vector<std::vector<bool>>;

struct Tile {
   enum Type { off, on, border };

   Type type = Type::off;
   Color color;
};

// Game state

class GameState : public State {
   enum class Path { left, right, down, current };

   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<Tile>> next_tiles;
   std::vector<Tetromino> bag;

   Tetromino tetromino;
   Tetromino next_tetromino;
   Color color;
   Color next_color;
   Vector2 pos;

   Texture tile_tx;
   Vector2 tile;
   
   int preview_y = 0;
   int score = 0;
   int hi_score = 0;
   bool make_next_tetromino = false;
   float down_timer = 0.f;
   float down_after = 1.f;
   
public:
   GameState();
   ~GameState();

   // Update functions

   void update() override;

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
   bool rotate();

   // Clear function

   void clear_cleared_rows();

   // Utility functions

   bool key_pressed(int key);
   Tetromino get_random_tetromino();
   Color get_random_color();

   // Save/load functions

   void save_hi_score(int hi_score);
   int load_hi_score();
};

#endif
