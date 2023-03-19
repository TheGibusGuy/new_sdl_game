#ifndef MAD_WORLD
#define MAD_WORLD

#include <cstdint>
#include <cstddef>
#include <string>
#include <fstream>
using namespace std;

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>
using namespace fpm;

#include "mad_things.h"
using namespace mad;

namespace mad {
	struct World {
		uint16_t w = 0, h = 0;
		size_t tileNum = 0;
		int8_t* tiles = NULL;
	};

	void destroy_world(World& obj) {
		if (obj.tiles == NULL) {
			printf("Destroy aborted, no world loaded\n");
			return;
		}
		delete[] obj.tiles;
		obj.tiles = NULL;
		obj.w = 0, obj.h = 0;
		obj.tileNum = 0;
		printf("Destroyed world\n");
	}

	bool load_world(World& obj, const char path[]) {
		printf(":? Loading world\n");

		destroy_world(obj);

		ifstream file(path);

		if (!file.fail()) {
			printf("Opened file: %s\nData may or may not be invalid\n", path);

			string data;

			getline(file, data); obj.w = stoi(data);
			getline(file, data); obj.h = stoi(data);
			printf("World is %u by %u tiles\n", obj.w, obj.h);

			obj.tileNum = obj.w * obj.h;
			printf("%llu bytes of tiles total\n", obj.tileNum);

			obj.tiles = new int8_t[obj.tileNum];
			for (size_t i = 0; i != obj.tileNum; i++) {
				getline(file, data, ','); obj.tiles[i] = stoi(data);
			}
		}
		else {
			printf("Failed to open file: %s\n", path);
		}
		
		if (obj.tiles == NULL) {
			printf(":( Failed to load world\n");
			return false;
		}
		else {
			printf(":) World loaded\n");
			return true;
		}
	}
}

#endif