#include "util/audio.hpp"

// Includes

#include <algorithm>
#include <random>
#include <raylib.h>
#include <filesystem>
#include <unordered_map>

// Global variables

static std::unordered_map<std::string, Sound> sounds;
static float sound_volume = 1.f;

// Load/unload functions

void load_audio() {   
   for (const auto& file : std::filesystem::directory_iterator("assets/audio/")) {
      Sound sound = LoadSound(file.path().c_str());
      sounds[file.path().stem().string()] = sound;
   }
}

void unload_audio() {
   for (auto& [_, sound] : sounds) {
      UnloadSound(sound);
   }
}

// Sounds functions

float get_sound_volume() {
   return sound_volume;
}

void set_sound_volume(float volume) {
   for (auto& [_, sound] : sounds) {
      SetSoundVolume(sound, volume);
   }
   sound_volume = volume;
}

void play_audio(const std::string& name) {
   PlaySound(sounds[name]);
   SetSoundVolume(sounds[name], sound_volume);
}
