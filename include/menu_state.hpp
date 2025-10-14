#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

// Includes

#include "state.hpp"
#include <raylib.h>

// Menu state

class MenuState : public State {
   enum class Phase { fading_in, idle, fading_out };
   
   Rectangle button_bounds;
   Color screen_tint {0, 0, 0, 255};

   float fade_in_timer = 0;
   float fade_out_timer = 0;
   Phase phase = Phase::fading_in;
   
public:
   MenuState();
   ~MenuState() = default;

   // Update functions

   void update() override;
   void update_fading_in();
   void update_idle_state();
   void update_fading_out();

   // Render functions
   
   void render() override;

   // Change states function

   void change_state(States& states) override;
};

#endif
