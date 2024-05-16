#pragma once

#include <array>
#include <cstddef>

#include "ofMain.h"

enum Sound { thruster, explosion, soundtrack };
const size_t soundCount{3};

class AudioSystem {
	static std::array<ofSoundPlayer, soundCount> sounds;

   public:
	AudioSystem() = delete;
	static void setup();
	static bool isLoaded();
	static void play(Sound sound);
	static bool isPlaying(Sound sound);
	static void stop(Sound sound);
};