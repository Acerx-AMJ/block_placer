#include "audio.hpp"

// Includes

#include <algorithm>
#include <random>
#include <raylib.h>
#include <filesystem>
#include <unordered_map>
#include <vector>

// Global variables

static std::unordered_map<std::string, Sound> sounds;
static std::vector<std::string> music_pool, music_bag;
static Music current_song;
static float music_volume = 1.f, sound_volume = 1.f;

// Load/unload functions

void load_audio() {   
   for (const auto& file : std::filesystem::directory_iterator("assets/audio/")) {
      Sound sound = LoadSound(file.path().c_str());
      sounds[file.path().stem().string()] = sound;
   }

   for (const auto& file : std::filesystem::directory_iterator("assets/music/")) {
      music_pool.push_back(file.path().string());
   }
}

void unload_audio() {
   for (auto& [_, sound] : sounds) {
      UnloadSound(sound);
   }
}

// Sounds functions

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

// Music functions

void set_music_volume(float volume) {
   if (IsMusicValid(current_song)) {
      SetMusicVolume(current_song, volume);
   }
   music_volume = volume;
}

void update_music() {
   if (not IsMusicValid(current_song) or GetMusicTimePlayed(current_song) + 1.f >= GetMusicTimeLength(current_song)) {
      if (IsMusicValid(current_song)) {
         UnloadMusicStream(current_song);
      }

      if (music_bag.empty()) {
         music_bag = music_pool;
         std::random_device rd;
         std::mt19937 device(rd());
         std::shuffle(music_bag.begin(), music_bag.end(), device);
      }
      auto next_song = music_bag.back();
      music_bag.pop_back();
      
      current_song = LoadMusicStream(next_song.c_str());
      SetMusicVolume(current_song, music_volume);
      PlayMusicStream(current_song);
   }
   UpdateMusicStream(current_song);
}
