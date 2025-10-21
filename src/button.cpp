#include "button.hpp"

// Includes

#include <algorithm>

// Sounds

Sound hover_sound;
Sound click_sound;

// Sound functions

void load_button_sounds() {
   hover_sound = LoadSound("assets/hover.wav");
   click_sound = LoadSound("assets/click.wav");
}

void unload_button_sounds() {
   UnloadSound(hover_sound);
   UnloadSound(click_sound);
}

// Button functions

void Button::update() {
   bool was_hovering = hovering;
   hovering = CheckCollisionPointRec({GetMouseX() + rectangle.width / 2.f, GetMouseY() + rectangle.height / 2.f}, rectangle);
   down = hovering and IsMouseButtonDown(MOUSE_LEFT_BUTTON);
   clicked = hovering and IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

   if (down) {
      scale = std::max(scale * .975f, .9f);
   } else if (hovering) {
      scale = std::min(scale * 1.025f, 1.1f);
   } else if (scale != 1.f) {
      scale = (scale < 1.f ? std::min(1.f, scale * 1.025f) : std::max(1.f, scale * .975f));
   }

   if (not was_hovering and hovering) {
      PlaySound(hover_sound);
   }

   if (clicked) {
      PlaySound(click_sound);
   }
}

void Button::draw() {
   float nw = rectangle.width * scale, nh = rectangle.height * scale;
   DrawRectanglePro(Rectangle{rectangle.x, rectangle.y, nw, nh}, {nw / 2.f, nh / 2.f}, 0.f, GRAY);

   Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), 20 * scale, 1.f);
   DrawTextPro(GetFontDefault(), text.c_str(), {rectangle.x, rectangle.y}, {text_size.x / 2.f, text_size.y / 2.f}, 0.f, 20 * scale, 1.f, BLACK);
}
