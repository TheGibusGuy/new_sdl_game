#ifndef MAD_THINGS
#define MAD_THINGS

#include <cstdint>
#include "mad_vector.h"

namespace mad {
	const std::uint8_t SNAPSHOT_TICKS = 15;
	const std::uint8_t THING_LIMIT = 24;
	const mad::fp DELTA{ (mad::fp)SNAPSHOT_TICKS / (mad::fp)1000 };
	const mad::fp GRAVITY {200};

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
						printf("Setting values for player thing\n");
						this->dim = { (mad::fp)1,(mad::fp)1,(mad::fp)1 };
						this->pos = spawnPos;
						this->prev_pos = spawnPos;
						break;
					default:
						printf("Cannot init, thing has invalid type: %i\n", type);
						break;
				}
			}
			
			void move() {
				this->prev_pos = this->pos;
				this->pos += this->vel.scale(DELTA);
			}
	};
}

#endif