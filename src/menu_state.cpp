#include "menu_state.hpp"

// Include

#include "game_state.hpp"

// Constants

namespace {
   constexpr float fade_in_time = 1.f;
   constexpr float fade_out_time = .5f;
   constexpr Vector2 single_mode_grid {12, 22};
   constexpr Vector2 co_op_mode_grid {18, 22};
   constexpr Vector2 screen {636, 700};
}

// Constructors

MenuState::MenuState() {
   SetWindowSize(screen.x, screen.y);
   play_button.rectangle = {GetScreenWidth() / 2.f, 250.f, 175.f, 50.f};
   co_op_button.rectangle = {play_button.rectangle.x, play_button.rectangle.y + 75.f, 175.f, 50.f};
   versus_button.rectangle = {co_op_button.rectangle.x, co_op_button.rectangle.y + 75.f, 175.f, 50.f};
   quit_button.rectangle = {versus_button.rectangle.x, versus_button.rectangle.y + 75.f, 175.f, 50.f};
   play_button.text = "PLAY";
   co_op_button.text = "CO-OP";
   versus_button.text = "VERSUS";
   quit_button.text = "QUIT";
}

// Update functions

// Update

void MenuState::update() {
   switch (phase) {
      case Phase::fading_in:  update_fading_in();  break;
      case Phase::idle:       update_idle_state(); break;
      case Phase::fading_out: update_fading_out(); break;
   }
}

// Update fading in

void MenuState::update_fading_in() {
   fade_in_timer += GetFrameTime();
   screen_tint.a = 255 - 255 * (fade_in_timer / fade_in_time);
   
   if (fade_in_timer >= fade_in_time) {
      phase = Phase::idle;
      screen_tint.a = 0;
   }
}

// Update fading out

void MenuState::update_fading_out() {
   fade_out_timer += GetFrameTime();
   screen_tint.a = 255 * (fade_out_timer / fade_out_time);

   if (fade_out_timer >= fade_out_time) {
      quit = true;
      screen_tint.a = 255;
   }
}

// Update idle state

void MenuState::update_idle_state() {
   play_button.update();
   co_op_button.update();
   versus_button.update();
   quit_button.update();

   if (play_button.clicked) {
      phase = Phase::fading_out;
   }

   if (co_op_button.clicked) {
      phase = Phase::fading_out;
      play_co_op = true;
   }

   if (versus_button.clicked) {
      phase = Phase::fading_out;
      play_versus = true;
   }

   if (quit_button.clicked) {
      phase = Phase::fading_out;
      quit_for_good = true;
   }
}

// Other functions

// Render

void MenuState::render() {
   BeginDrawing();
      ClearBackground(BLACK);
      play_button.draw();
      co_op_button.draw();
      versus_button.draw();
      quit_button.draw();
      DrawText("BLOCK PLACER", GetScreenWidth() / 2.f - MeasureText("BLOCK PLACER", 60) / 2.f, 150.f, 60, WHITE);
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), screen_tint);
   EndDrawing();
}

// Change states

void MenuState::change_state(States& states) {
   if (quit_for_good) {
      return;
   }
   
   if (play_co_op) {
      states.push_back(std::make_unique<GameState>(co_op_mode_grid, 2, false));
   } else if (play_versus) {
      states.push_back(std::make_unique<GameState>(single_mode_grid, 1, true));
   } else {
      states.push_back(std::make_unique<GameState>(single_mode_grid, 1, false));
   }
}
