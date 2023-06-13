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
    typedef std::uint8_t tchunk_size;
    static constexpr tchunk_size THING_CHUNK_SIZE = 255;
    class ThingChunk {
        public:
            tchunk_size thing_total = 0;
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
            struct WorldState {
                // not constexpr, since I could change this later
                const std::uint8_t ticks_per_snapshot = DEFAULT_SNAPSHOT_LENGTH;
                const std::uint8_t& ticks_per_snap = ticks_per_snapshot;

                // timing
                std::uint64_t current_tick = 0;
                std::uint64_t latest_snapshot_tick = 0;
                std::uint64_t next_snapshot_tick = 0;

                mad::fp delta_time = (mad::fp)ticks_per_snapshot / (mad::fp)1000;
                mad::fp& dt = delta_time;

                std::uint64_t snapshot_index = 0;

                // world
                mad::World world;

                // things
                std::vector<ThingChunk*> thing_chunks;
                std::size_t thing_chunk_total = STARTING_THING_CHUNKS;
                std::size_t thing_total = STARTING_THING_CHUNKS * THING_CHUNK_SIZE;
            };
        private:
            WorldState state;

            void new_ThingChunk() {
                this->state.thing_chunks.push_back(new ThingChunk);
            }

            void snapshot() {
                // printf("%llu\n",this->state.snapshot_index);

                // go through every chunk
                // then through every thing in each chunk
                for (auto chunk_pointer : this->state.thing_chunks) {
                    if (chunk_pointer->thing_total > 0) {
                        static tchunk_size things_indexed = 0;
                        for (auto thing : chunk_pointer->things) {
                            if (!mikey::read_bit(thing.flags, NO_THING)) {
                                things_indexed++;
                            }
                            if (things_indexed >= THING_CHUNK_SIZE) {
                                break;
                            }
                        }
                    }
                }

                this->state.snapshot_index++;
            }
        public:
            Simulation() {
                for (auto i = 0; i != this->state.thing_chunk_total; i++) {
                    new_ThingChunk();
                }
            }

            const WorldState* get_snapshot() {
                return &(this->state);
            } 

            // returns index of new thing if successful
            Thing* new_thing() {
                for (auto chunk_pointer : this->state.thing_chunks) {
                    // make sure the chunk isn't full
                    if (chunk_pointer->thing_total >= THING_CHUNK_SIZE) {
                        continue;
                    }
                    // find the first free space
                    for (auto thing : chunk_pointer->things) {
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

            // only run this once!
            void set_start_tick(std::uint64_t newTick) {
                this->state.current_tick = newTick;
                this->state.latest_snapshot_tick = newTick;
                this->state.next_snapshot_tick = newTick;
            }

            void advance_time(std::uint64_t newTick) {
                if (newTick <= this->state.current_tick) {
                    // this should never happen, but just in case
                    // get some error logging up in this bitch
                    return;
                }

                this->state.current_tick = newTick;

                if (state.current_tick >= state.next_snapshot_tick) {
                    state.latest_snapshot_tick = state.next_snapshot_tick;
                    snapshot();

                    // schedule the next snapshot
                    state.next_snapshot_tick += state.ticks_per_snapshot;
                    while (state.current_tick >= state.next_snapshot_tick) state.next_snapshot_tick += state.ticks_per_snapshot;
                }
            }
    };
}

#endif