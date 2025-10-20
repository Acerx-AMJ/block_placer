#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "raylib.h"
#include <algorithm>
#include <string>

class Button {
public:
   Rectangle rectangle;
   std::string text;
   bool hovering = false, down = false, clicked = false;
   float scale = 1;

   inline void update() {
      hovering = CheckCollisionPointRec({GetMouseX() + rectangle.width / 2.f, GetMouseY() + rectangle.height / 2.f}, rectangle);
      down = hovering and IsMouseButtonDown(MOUSE_LEFT_BUTTON);
      clicked = hovering and IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

      if (down) {
         scale = std::max(scale * .975f, .9f);
      } else if (hovering) {
         scale = std::min(scale * 1.025f, 1.1f);
      } else if (scale != 1.f) {
         scale = (scale < 1.f ? std::min(1.f, scale * 1.025f) : std::max(1.f, scale * .975f));
      }
   }

   inline void draw() {
      float nw = rectangle.width * scale, nh = rectangle.height * scale;
      DrawRectanglePro(Rectangle{rectangle.x, rectangle.y, nw, nh}, {nw / 2.f, nh / 2.f}, 0.f, GRAY);

      Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), 20 * scale, 1.f);
      DrawTextPro(GetFontDefault(), text.c_str(), {rectangle.x, rectangle.y}, {text_size.x / 2.f, text_size.y / 2.f}, 0.f, 20 * scale, 1.f, BLACK);
   }
};

#endif
