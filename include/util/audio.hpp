#ifndef UTIL_AUDIO_HPP
#define UTIL_AUDIO_HPP

// Includes

#include <string>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Load/unload functions

void load_audio();
void unload_audio();

// Audio functions

float get_sound_volume();
void set_sound_volume(float volume);
void play_audio(const std::string& name);

// Music functions

float get_music_volume();
void set_music_volume(float volume);
void update_music();

#endif
