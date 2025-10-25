#ifndef UTIL_SLIDER_HPP
#define UTIL_SLIDER_HPP

// Includes

#include <raylib.h>

// Slider class

class Slider {
public:
   Rectangle bg, fg;
   Vector2 knob_pos;
   float knob_radius;
   bool dragging = false, changed = false;
   float progress = 1.f;
   float step = 0.05f;

   void update();
   void draw();
};

#endif
