#ifndef MAD_WORLD
#define MAD_WORLD

#include <cstdint>
#include <cstddef>
#include <string>
#include <fstream>

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>

namespace mad {
	struct World {
		std::uint16_t w = 0, h = 0;
		std::size_t tileNum = 0;
		std::int8_t* tiles = nullptr;
	};

	void destroy_world(World& obj) {
		if (obj.tiles == nullptr) {
			std::printf("Destroy aborted, no world loaded\n");
			return;
		}
		delete[] obj.tiles;
		obj.tiles = nullptr;
		obj.w = 0, obj.h = 0;
		obj.tileNum = 0;
		std::printf("Destroyed world\n");
	}

	bool load_world(World& obj, const char path[]) {
		std::printf(":? Loading world\n");

		destroy_world(obj);

		std::ifstream file(path);

		if (!file.fail()) {
			std::printf("Opened file: %s\nData may or may not be invalid\n", path);

			std::string data;

			std::getline(file, data); obj.w = stoi(data);
			std::getline(file, data); obj.h = stoi(data);
			std::printf("World is %u by %u tiles\n", obj.w, obj.h);

			obj.tileNum = obj.w * obj.h;
			std::printf("%llu bytes of tiles total\n", obj.tileNum);

			obj.tiles = new int8_t[obj.tileNum];
			for (size_t i = 0; i != obj.tileNum; i++) {
				std::getline(file, data, ','); obj.tiles[i] = std::stoi(data);
			}
		}
		else {
			std::printf("Failed to open file: %s\n", path);
		}
		
		if (obj.tiles == nullptr) {
			std::printf(":( Failed to load world\n");
			return false;
		}
		else {
			std::printf(":) World loaded\n");
			return true;
		}
	}
}

#endif