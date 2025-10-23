#include "game.hpp"

// Includes

#include "audio.hpp"
#include "menu_state.hpp"
#include <raylib.h>
#include <cstdlib>

// Constants

namespace {
   constexpr const char* title = "Block Placer";
   constexpr Vector2 screen {636, 700};
   constexpr int target_fps = 60;
}

// Constructors

Game::Game() {
   srand(time(nullptr));
   InitWindow(screen.x, screen.y, title);
   InitAudioDevice();
   SetTargetFPS(target_fps);
   SetExitKey(0);

   auto icon = LoadImage("assets/icon.png");
   SetWindowIcon(icon);
   load_audio();

   states.push_back(std::make_unique<MenuState>());
}

Game::~Game() {
   unload_audio();
   CloseWindow();
   CloseAudioDevice();
}

// Run function

void Game::run() {
   while (not WindowShouldClose()) {
      if (states.front()->quit) {
         states.front()->change_state(states);
         states.pop_front();
      }

      if (states.empty()) {
         return;
      }

      states.front()->update();
      states.front()->render();
   }
}
