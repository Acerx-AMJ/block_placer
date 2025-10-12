#ifndef STATE_HPP
#define STATE_HPP

// Includes

#include <deque>
#include <memory>

class State;
using States = std::deque<std::unique_ptr<State>>;

// State

class State {
public:
   bool quit = false;

   State() = default;
   virtual ~State() = default;

   virtual void update() = 0;
   virtual void render() = 0;
   virtual void change_state(States& states) = 0;
};

#endif
