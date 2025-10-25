#ifndef AUDIO_HPP
#define AUDIO_HPP

// Includes

#include <string>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Load/unload functions

void load_audio();
void unload_audio();

// Audio functions

void set_sound_volume(float volume);
void play_audio(const std::string& name);

// Music functions

void set_music_volume(float volume);
void update_music();

#endif
