#ifndef GAME_HPP
#define GAME_HPP

// Includes

#include "state.hpp"

// Game

class Game {
   States states;

public:
   Game();
   ~Game();

   void run();
};

#endif
