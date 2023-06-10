#ifndef MAD_WORLD
#define MAD_WORLD

#include <SDL.h>

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
		std::size_t tile_num = 0;
		std::int8_t* tiles = nullptr;
	};

	void destroy_world(World& obj) {
		if (obj.tiles == nullptr) {
			std::printf("\tDestroy aborted, no world loaded\n");
			return;
		}
		delete[] obj.tiles;
		obj.tiles = nullptr;
		obj.w = 0, obj.h = 0;
		obj.tile_num = 0;
		std::printf("\tDestroyed world\n");
	}

	bool load_world(World& obj, const char path[]) {
		std::printf(":? Loading world\n");

		destroy_world(obj);

		std::ifstream file(path);

		if (!file.fail()) {
			std::printf("\tOpened file: %s\n\tData may or may not be invalid\n", path);

			std::string data;

			std::getline(file, data); obj.w = stoi(data);
			std::getline(file, data); obj.h = stoi(data);
			std::printf("\tWorld is %u by %u tiles\n", obj.w, obj.h);

			obj.tile_num = obj.w;
			obj.tile_num *= obj.h;
			std::printf("\t%llu bytes of tiles total\n", obj.tile_num);

			obj.tiles = new int8_t[obj.tile_num];
			for (size_t i = 0; i != obj.tile_num; i++) {
				std::getline(file, data, ','); obj.tiles[i] = std::stoi(data);
			}
		}
		else {
			std::printf("\tFailed to open file: %s\n", path);
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
	
	// lerp is unused here but here for consistency with thing rendering
	void world_render2D(SDL_Renderer* renderer, World& obj, fp lerp, uint8_t scale) {
		// i could use a callback function for this #1
		for (uint16_t x = 0; x != obj.w; x++) {
			for (uint16_t y = 0; y != obj.h; y++) {
				const SDL_Rect fillRect = { x * scale, y * scale, scale, scale };
				switch (obj.tiles[x + (y * obj.w)]) {
				case -1:
					SDL_SetRenderDrawColor(renderer, 127, 35, 76, 255);
					break;
				case 69:
					SDL_SetRenderDrawColor(renderer, 69, 69, 69, 69);
					break;
				default:
					SDL_SetRenderDrawColor(renderer, 42, 64, 89, 255);
					break;
				}
				SDL_RenderFillRect(renderer, &fillRect);
			}
		}
	}
}

#endif