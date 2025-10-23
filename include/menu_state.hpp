#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

// Includes

#include "button.hpp"
#include "state.hpp"

// Menu state

class MenuState : public State {
   enum class Phase { fading_in, idle, fading_out };
   
   bool quit_for_good = false;
   bool play_co_op = false;
   float fade_in_timer = 0;
   float fade_out_timer = 0;
   Phase phase = Phase::fading_in;

   Button play_button, co_op_button, quit_button;
   Color screen_tint {0, 0, 0, 255};
   
public:
   MenuState();
   ~MenuState() = default;

   // Update

   void update() override;
   void update_fading_in();
   void update_fading_out();
   void update_idle_state();

   // Render
   
   void render() override;

   // Change states

   void change_state(States& states) override;
};

#endif
