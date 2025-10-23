#ifndef AUDIO_HPP
#define AUDIO_HPP

// Includes

#include <string>
using namespace std::string_literals;
using namespace std::string_view_literals;

// Functions

void load_audio();
void unload_audio();
void play_audio(const std::string& name);

#endif
