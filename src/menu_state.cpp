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
   play_button.rectangle = {GetScreenWidth() / 2.f, 250.f, 175.f, 50.f};
   quit_button.rectangle = {play_button.rectangle.x, play_button.rectangle.y + 75.f, 175.f, 50.f};
   play_button.text = "PLAY";
   quit_button.text = "QUIT";
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
   play_button.update();
   quit_button.update();

   if (play_button.clicked) {
      phase = Phase::fading_out;
   }

   if (quit_button.clicked) {
      phase = Phase::fading_out;
      quit_for_good = true;
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
      ClearBackground(BLACK);
      play_button.draw();
      quit_button.draw();
      DrawText("BLOCK PLACER", GetScreenWidth() / 2.f - MeasureText("BLOCK PLACER", 60) / 2.f, 150.f, 60, WHITE);
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), screen_tint);
   EndDrawing();
}

// Change states function

void MenuState::change_state(States& states) {
   if (not quit_for_good) {
      states.push_back(std::make_unique<GameState>());
   }
}
