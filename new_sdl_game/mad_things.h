#ifndef MAD_THINGS
#define MAD_THINGS

#include <iostream>
#include <cstdio>
#include <cstdint>
#include "mad_vector.h"
#include "mikey_bit_flags.h"

namespace mad {

	bool thing_debug = true;

	enum THING_TYPES {
		PLAYER,
		ENEMY
	};

	enum THING_FLAGS {
		NO_THING, // haha, no thing, nothing
		ACTIVE,
		VISIBLE,
	};

	class Thing {
		private:
			static constexpr std::uint8_t DEFAULT_FLAGS() {
				std::uint8_t default_flags = 0;
				mikey::set_bit_to_1(default_flags, NO_THING);
				return default_flags;
			}		
		public:
			// can add more if needed
			std::uint8_t flags;

			mad::FixedVec3D pos{ (mad::fp) 0,(mad::fp)0,(mad::fp)0 };
			mad::FixedVec3D prev_pos{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			mad::FixedVec3D vel{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			mad::fp dir{ 0 };

			mad::FixedVec3D dim{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			int16_t type = -1;

			Thing() {
				this->flags = 0 + DEFAULT_FLAGS();
				return;
			}

			void set_values (int16_t type, mad::FixedVec3D spawnPos) {
				this->type = type;
				switch (type) {
					case PLAYER:
						this->dim = { (mad::fp)1,(mad::fp)1,(mad::fp)1 };
						this->pos = spawnPos;
						this->prev_pos = spawnPos;
						break;
					default:
						std::printf("\tCannot init, thing has invalid type: %i\n", type);
						break;
				}
			}
			
			void move(fp &delta_time) {
				this->prev_pos = this->pos;
				this->pos += this->vel.scale(delta_time);
			}

			void render2D(SDL_Renderer* renderer, mad::fp lerp, std::uint8_t scale) {
				SDL_Rect objRect;
				int thing_scale = scale / 4;
				int half_thing_width = thing_scale / 2;

				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				const int32_t prev_posX = (int32_t)(this->prev_pos.x * fp(scale)) - half_thing_width;
				const int32_t prev_posY = (int32_t)(this->prev_pos.y * fp(scale)) - half_thing_width;
				objRect = { prev_posX,prev_posY, thing_scale , thing_scale };
				SDL_RenderFillRect(renderer, &objRect);

				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
				const int32_t posX = (int32_t)(this->pos.x * fp(scale)) - half_thing_width;
				const int32_t posY = (int32_t)(this->pos.y * fp(scale)) - half_thing_width;
				objRect = { posX,posY, thing_scale , thing_scale };
				SDL_RenderFillRect(renderer, &objRect);

				FixedVec3D subVec = this->pos - this->prev_pos;
				subVec = subVec.scale(lerp);
				FixedVec3D interpPos = this->prev_pos + subVec;

				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				const int32_t interpX = (int32_t)(interpPos.x * fp(scale)) - half_thing_width;
				const int32_t interpY = (int32_t)(interpPos.y * fp(scale)) - half_thing_width;
				objRect = { interpX,interpY, thing_scale , thing_scale };
				SDL_RenderFillRect(renderer, &objRect);
			}
	};
}

#endif