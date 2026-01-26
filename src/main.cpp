#include "menu_state.hpp"
#include "util/audio.hpp"
#include <ctime>
#include <raylib.h>
#include <cstdlib>

// Constants

namespace {
   constexpr const char* title = "Block Placer";
   constexpr Vector2 screen {636, 700};
   constexpr int target_fps = 60;
}

// Main function

int main() {
   srand(time(nullptr));
   InitWindow(screen.x, screen.y, title);
   InitAudioDevice();
   SetTargetFPS(target_fps);
   SetExitKey(0);

   auto icon = LoadImage("assets/icon.png");
   SetWindowIcon(icon);
   load_audio();

   State *state = new MenuState();
   while (not WindowShouldClose()) {
      if (state->quit) {
        State *newState = state->change_state();
        delete state;
        state = newState;
      }

      if (!state) {
        break;
      }

      state->update();
      state->render();
   }

   unload_audio();
   CloseWindow();
   CloseAudioDevice();
}
