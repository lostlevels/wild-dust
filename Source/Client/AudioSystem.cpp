#include "Precompiled.h"
#include "Audio.h"

AudioSystem::AudioSystem() {
	SDL_InitSubSystem(SDL_INIT_AUDIO);
}

AudioSystem::~AudioSystem() {
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioSystem::update() {
	
}
