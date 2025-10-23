#include "audio.hpp"

// Includes

#include <raylib.h>
#include <filesystem>
#include <unordered_map>

// Global variables

static std::unordered_map<std::string, Sound> sounds;

// Functions

void load_audio() {
   for (auto& file : std::filesystem::directory_iterator("assets/")) {
      if (file.path().extension() == ".wav") {
         Sound sound = LoadSound(file.path().c_str());
         sounds[file.path().stem().string()] = sound;
      }
   }
}

void unload_audio() {
   for (auto& [_, sound] : sounds) {
      UnloadSound(sound);
   }
}

void play_audio(const std::string& name) {
   PlaySound(sounds[name]);
}
