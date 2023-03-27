#ifndef MAD_INPUT
#define MAD_INPUT

#include <cstdint>
#include <SDL.h>

namespace mad {
	class Action {
		SDL_Keycode bind;
		SDL_Keycode altBind;
		std::uint8_t flags;
		// first bit checks if it's binded
		// second bit checks if the alt is binded
		// third flag check if the key is being held
	};
}

#endif