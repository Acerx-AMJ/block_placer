#include "util/slider.hpp"

// Includes

#include "util/audio.hpp"
#include <algorithm>
#include <cmath>

// Update slider

void Slider::update() {
   bool hover = CheckCollisionPointCircle(GetMousePosition(), knob_pos, knob_radius);
   bool was_dragging = dragging;

   changed = false;
   float old = progress;
   
   if (hover and not dragging and IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      dragging = true;
   }

   if (not IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      dragging = false;
   }

   if (dragging) {
      float new_progress = (GetMouseX() - (bg.x - bg.width / 2.f)) / bg.width;
      float steps = 1.f / step;
      progress = std::round(std::clamp(new_progress, 0.f, 1.f) * steps) / steps;
      changed = old != progress;
   }

   knob_pos.x = (fg.x - fg.width / 2.f) + fg.width * progress;
   knob_pos.y = fg.y;
   if (was_dragging and not dragging) {
      play_audio("click"s);
   }
}

// Render slider

void Slider::draw() {
   DrawRectanglePro(bg, {bg.width / 2.f, bg.height / 2.f}, 0.f, GRAY);
   DrawRectanglePro({fg.x, fg.y, fg.width * progress, fg.height}, {bg.width / 2.f, bg.height / 2.f}, 0.f, WHITE);
   DrawCircle(knob_pos.x, knob_pos.y, knob_radius, {64, 64, 64, 255});
}
