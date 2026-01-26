#ifndef STATE_HPP
#define STATE_HPP

// State

class State {
public:
   bool quit = false;

   State() = default;
   virtual ~State() = default;

   virtual void update() = 0;
   virtual void render() = 0;
   virtual State* change_state() = 0;
};

#endif
