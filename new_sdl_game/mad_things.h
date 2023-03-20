#ifndef MAD_THINGS
#define MAD_THINGS
	
#include "mad_vector.h"
using namespace mad;

namespace mad {
	const uint8_t SNAPSHOT_TIME = 15;
	const fp DELTA{ (fp)SNAPSHOT_TIME / (fp)1000 };
	const fp GRAVITY {200};
	const uint8_t THING_LIMIT = 24;

	class Thing {
		FixedVec3D pos{ (fp)0,(fp)0,(fp)0 };
		FixedVec3D prevPos{ (fp)0,(fp)0,(fp)0 };
		FixedVec3D vel{ (fp)0,(fp)0,(fp)0 };
		FixedVec3D dim{ (fp)0,(fp)0,(fp)0 };
		fp dir{ 0 };
	};
}

#endif