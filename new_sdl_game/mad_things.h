#ifndef MAD_THINGS
#define MAD_THINGS

#include <iostream>
#include <cstdio>
#include <cstdint>
#include "mad_vector.h"

namespace mad {
	const std::uint8_t TICKS_BETWEEN_SNAPSHOTS = 15;
	const std::uint8_t THING_LIMIT = 24;

	const mad::fp DELTA{ (mad::fp)TICKS_BETWEEN_SNAPSHOTS / (mad::fp)1000 };
	mad::fp gravity {200};

	bool thing_debug = true;

	enum THING_TYPES {
		PLAYER,
		ENEMY
	};

	class Thing {
		public:
			mad::FixedVec3D pos{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			mad::FixedVec3D prev_pos{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			mad::FixedVec3D vel{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			mad::fp dir{ 0 };

			mad::FixedVec3D dim{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
			int16_t type = -1;

			Thing (int16_t t, mad::FixedVec3D spawnPos) {
				this->type = t;
				switch (t) {
					case PLAYER:
						std::printf("\tSetting values for player thing\n");
						this->dim = { (mad::fp)1,(mad::fp)1,(mad::fp)1 };
						this->pos = spawnPos;
						this->prev_pos = spawnPos;
						break;
					default:
						std::printf("\tCannot init, thing has invalid type: %i\n", type);
						break;
				}
			}
			
			void move() {
				this->prev_pos = this->pos;
				this->pos += this->vel.scale(DELTA);
			}

			void render2D(SDL_Renderer* renderer, mad::fp lerp, std::uint8_t scale) {
				SDL_Rect objRect;
				const static int THING_SCALE = scale / 4;
				const static int HALF_THING_WIDTH = THING_SCALE / 2;

				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				const int32_t prev_posX = (int32_t)(this->prev_pos.x * fp(scale)) - HALF_THING_WIDTH;
				const int32_t prev_posY = (int32_t)(this->prev_pos.y * fp(scale)) - HALF_THING_WIDTH;
				objRect = { prev_posX,prev_posY, THING_SCALE , THING_SCALE };
				SDL_RenderFillRect(renderer, &objRect);

				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
				const int32_t posX = (int32_t)(this->pos.x * fp(scale)) - HALF_THING_WIDTH;
				const int32_t posY = (int32_t)(this->pos.y * fp(scale)) - HALF_THING_WIDTH;
				objRect = { posX,posY, THING_SCALE , THING_SCALE };
				SDL_RenderFillRect(renderer, &objRect);

				FixedVec3D subVec = this->pos - this->prev_pos;
				subVec = subVec.scale(lerp);
				FixedVec3D interpPos = this->prev_pos + subVec;

				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				const int32_t interpX = (int32_t)(interpPos.x * fp(scale)) - HALF_THING_WIDTH;
				const int32_t interpY = (int32_t)(interpPos.y * fp(scale)) - HALF_THING_WIDTH;
				objRect = { interpX,interpY, THING_SCALE , THING_SCALE };
				SDL_RenderFillRect(renderer, &objRect);
			}
	};
}

#endif