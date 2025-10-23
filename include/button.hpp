#ifndef BUTTON_HPP
#define BUTTON_HPP

// Includes

#include <raylib.h>
#include <string>

// Button class

class Button {
public:
   Rectangle rectangle;
   std::string text;
   bool hovering = false, down = false, clicked = false;
   float scale = 1;

   void update();
   void draw();
};

#endif
