#include "AudioSystem.h"

std::array<ofSoundPlayer, soundCount> AudioSystem::sounds{};

void AudioSystem::setup() {
	sounds[Sound::soundtrack].load("audio/soundtrack.wav");
	sounds[Sound::soundtrack].setLoop(true);
	sounds[Sound::soundtrack].setVolume(0.5f);

	sounds[Sound::thruster].load("audio/thruster-loop.wav");
	sounds[Sound::thruster].setLoop(true);
	sounds[Sound::thruster].setVolume(0.5f);

	sounds[Sound::explosion].load("audio/explosion.wav");
	sounds[Sound::explosion].setMultiPlay(true);
	sounds[Sound::explosion].setVolume(0.5f);

}

bool AudioSystem::isLoaded() {
	for (auto& sound : sounds) {
		if (!sound.isLoaded()) return false;
	}
	return true;
}

void AudioSystem::play(Sound sound) { sounds[sound].play(); }

bool AudioSystem::isPlaying(Sound sound) { return sounds[sound].isPlaying(); }

void AudioSystem::stop(Sound sound) { sounds[sound].stop(); }
