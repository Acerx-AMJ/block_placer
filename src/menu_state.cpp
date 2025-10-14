#include "menu_state.hpp"

// Include

#include "game_state.hpp"

// Constants

namespace {
   constexpr float fade_in_time = 1.f;
   constexpr float fade_out_time = .5f;
}

// Constructors

MenuState::MenuState() {
   button_bounds = {GetScreenWidth() / 2.f - 75.f, 300.f, 150.f, 150.f};
}

// Update functions

void MenuState::update() {
   switch (phase) {
      case Phase::fading_in:  update_fading_in();  break;
      case Phase::idle:       update_idle_state(); break;
      case Phase::fading_out: update_fading_out(); break;
   }
}

void MenuState::update_fading_in() {
   fade_in_timer += GetFrameTime();
   screen_tint.a = 255 - 255 * (fade_in_timer / fade_in_time);
   
   if (fade_in_timer >= fade_in_time) {
      phase = Phase::idle;
      screen_tint.a = 0;
   }
}

void MenuState::update_idle_state() {
   auto mouse = GetMousePosition();
   bool hovering = CheckCollisionPointRec(mouse, button_bounds);

   if (hovering and IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      phase = Phase::fading_out;
   }
}

void MenuState::update_fading_out() {
   fade_out_timer += GetFrameTime();
   screen_tint.a = 255 * (fade_out_timer / fade_out_time);

   if (fade_out_timer >= fade_out_time) {
      quit = true;
      screen_tint.a = 255;
   }
}

// Render functions

void MenuState::render() {
   BeginDrawing();
      DrawRectangleRec(button_bounds, GRAY);
      DrawText("BLOCK PLACER", GetScreenWidth() / 2.f - MeasureText("BLOCK PLACER", 60) / 2.f, 150.f, 60, WHITE);
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), screen_tint);
   EndDrawing();
}

// Change states function

void MenuState::change_state(States& states) {
   states.push_back(std::make_unique<GameState>());
}
