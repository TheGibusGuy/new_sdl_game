#ifndef MAD_INPUT
#define MAD_INPUT

#include <cstdint>
#include <SDL.h>

#include "mikey_bit_flags.h"

namespace mad {
	enum KEYBOARD_LAYOUTS : std::uint8_t {
		QWERTY_LAYOUT,
		DVORAK_LAYOUT,
	};

	class Action {
		private:
			static std::uint8_t keyboard_layout;
			// cannot be initialized in here
			// if it were it would be propagated to all source files
			// it can only be set once everything is linked
			// so that a concensus on the static variable

			// I could use bit flags for effieciecy
			// but I don't think it's too important in this case
			bool binded = false;
			SDL_Scancode bind;
			bool alt_binded = false;
			SDL_Scancode alt_bind;

			bool pressed = false;
			bool held = false;
			bool released = false;
		public:
			void setBind(SDL_Scancode new_bind) {
				this->bind = new_bind;
				this->binded = true;
			}
			void removeBind() {
				this->binded = false;
			}
			void setAltBind(SDL_Scancode new_alt_bind) {
				this->alt_bind = new_alt_bind;
				this->alt_binded = true;
			}
			void removeAltBind() {
				this->alt_binded = false;
			}
			void readKeyStates(const std::uint8_t*& current_key_states) {
				if ( (this->binded && current_key_states[this->bind]) || (this->alt_binded && current_key_states[this->alt_bind]) ) {

					if (!this->held) {
						this->pressed = true;
					}
					else {
						this->pressed = false;
					}

					this->held = true;
				} else {

					if (this->held) {
						this->released = true;
					}
					else {
						this->released = false;
					}

					this->held = false;
				}
			}

			bool wasPressed() {
				return this->pressed;
			}

			bool isHeld() {
				return this->held;
			}

			bool wasReleased() {
				return this->released;
			}
	};
}

#endif