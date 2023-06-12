#ifndef MAD_SIMULATION
#define MAD_SIMULATION

#include "mad_things.h"
#include "mad_vector.h"
#include "mad_world.h"

#include <vector>

#include "mikey_bit_flags.h"

namespace mad {

    // to improve cache alignment
    
    // apparently you can't dynamically allocate structs, only classes
    static constexpr std::uint8_t THING_CHUNK_SIZE = 255;
    class ThingChunk {
        public:
            std::uint8_t thing_total = 0;
            Thing things[THING_CHUNK_SIZE];

            ThingChunk() {
                return;
            }
    };

    class Simulation {
        private:
            static constexpr std::uint8_t DEFAULT_SNAPSHOT_LENGTH = 15;

            static constexpr std::size_t STARTING_THING_CHUNKS = 1;
        public:
            // what the struct is is public
            // but the classes' instance of it is not
            struct State {
                // time
                std::uint64_t tick_current = 0;
                std::uint64_t& tick_curr = tick_current;

                std::uint64_t tick_latest_snapshot = 0;
                std::uint64_t& tick_ltst_snap = tick_latest_snapshot;

                std::uint64_t tick_next_snapshot = 0;
                std::uint64_t& tick_next_snap = tick_next_snapshot;

                std::uint8_t ticks_per_snapshot = DEFAULT_SNAPSHOT_LENGTH;
                std::uint8_t& ticks_per_snaps = ticks_per_snapshot;

                mad::fp delta_time = (mad::fp)ticks_per_snapshot / (mad::fp)1000;
                mad::fp& dt = delta_time;

                // world
                mad::World world;

                // things
                std::vector<ThingChunk*> thing_chunks;
                std::size_t thing_chunk_total = STARTING_THING_CHUNKS;
                std::size_t thing_total = STARTING_THING_CHUNKS * THING_CHUNK_SIZE;
            };
        private:
            State state;

            void add_ThingChunk() {
                this->state.thing_chunks.push_back(new ThingChunk);
            }
        public:
            Simulation() {
                for (auto i = 0; i != this->state.thing_chunk_total; i++) {
                    add_ThingChunk();
                }
            }

            const State* get_snapshot() {
                return &(this->state);
            } 

            // returns index of new thing if successful
            Thing* new_thing() {
                for (auto chunk : this->state.thing_chunks) {
                    // make sure the chunk isn't full
                    // god damn that's two arrows
                    if (chunk->thing_total >= THING_CHUNK_SIZE) {
                        continue;
                    }

                    for (auto thing : chunk->things) {
                        if (mikey::read_bit(thing.flags,NO_THING)){
                            // found a free space
                            mikey::set_bit_to_0(thing.flags, NO_THING);
                            return &thing;
                        }
                    }

                }
                // no space!
                return nullptr;
            }

            void advance_time(std::uint64_t newTime) {
                if (this->state.tick_current = newTime) {

                } else {
                    // this should never happen, but just in case
                    // get some error logging up in this bitch
                }
            }
    };
}

#endif