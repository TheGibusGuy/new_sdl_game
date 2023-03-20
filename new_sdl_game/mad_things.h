#ifndef MAD_THINGS
#define MAD_THINGS

#include <cstdint>
#include "mad_vector.h"

namespace mad {
	const std::uint8_t SNAPSHOT_TIME = 15;
	const std::uint8_t THING_LIMIT = 24;
	const mad::fp DELTA{ (mad::fp)SNAPSHOT_TIME / (mad::fp)1000 };
	const mad::fp GRAVITY {200};

	class Thing {
		mad::FixedVec3D pos{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
		mad::FixedVec3D prevPos{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
		mad::FixedVec3D vel{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
		mad::FixedVec3D dim{ (mad::fp)0,(mad::fp)0,(mad::fp)0 };
		mad::fp dir{ 0 };
	};
}

#endif