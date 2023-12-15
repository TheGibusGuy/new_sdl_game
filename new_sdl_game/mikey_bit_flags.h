#ifndef MIKEY_BIT_FLAGS
#define MIKEY_BIT_FLAGS

#include <cstdint>
#include <string>
#include <sstream>

namespace mikey {

    constexpr bool read_bit(std::uint8_t& byte, std::uint8_t bit_index) {

        // ooh the 0's and 1's make a diagonal line
        switch (bit_index) {
        default:
            std::printf("Index out of range!\n");
        case 0:
            if (byte & 0b10000000) return true;
            break;
        case 1:
            if (byte & 0b01000000) return true;
            break;
        case 2:
            if (byte & 0b00100000) return true;
            break;
        case 3:
            if (byte & 0b00010000) return true;
            break;
        case 4:
            if (byte & 0b00001000) return true;
            break;
        case 5:
            if (byte & 0b00000100) return true;
            break;
        case 6:
            if (byte & 0b00000010) return true;
            break;
        case 7:
            if (byte & 0b00000001) return true;
            break;
        }

        return false;
    }

    constexpr void set_bit_to_1(std::uint8_t& byte, std::uint8_t bit_index) {

        switch (bit_index) {
        default:
            std::printf("Index out of range!\n");
            break;
        case 0:
            byte = byte | 0b10000000;
            break;
        case 1:
            byte = byte | 0b01000000;
            break;
        case 2:
            byte = byte | 0b00100000;
            break;
        case 3:
            byte = byte | 0b00010000;
            break;
        case 4:
            byte = byte | 0b00001000;
            break;
        case 5:
            byte = byte | 0b00000100;
            break;
        case 6:
            byte = byte | 0b00000010;
            break;
        case 7:
            byte = byte | 0b00000001;
            break;
        }

        return;
    }

    constexpr void set_bit_to_0(std::uint8_t& byte, std::uint8_t bit_index) {

        // set each bit to 1, then set it back to 0
        switch (bit_index) {
        default:
            std::printf("Index out of range!\n");
            break;
        case 0:
            byte = (byte | 0b10000000) - 0b10000000;
            break;
        case 1:
            byte = (byte | 0b01000000) - 0b01000000;
            break;
        case 2:
            byte = (byte | 0b00100000) - 0b00100000;
            break;
        case 3:
            byte = (byte | 0b00010000) - 0b00010000;
            break;
        case 4:
            byte = (byte | 0b00001000) - 0b00001000;
            break;
        case 5:
            byte = (byte | 0b00000100) - 0b00000100;
            break;
        case 6:
            byte = (byte | 0b00000010) - 0b00000010;
            break;
        case 7:
            byte = (byte | 0b00000001) - 0b00000001;
            break;
        }

        return;
    }

    std::string byte_to_string(std::uint8_t& byte) {
        std::stringstream sstream("");

        for (std::uint8_t index = 0; index < 8; index++) {
            if (read_bit(byte, index)) {
                sstream << '1';
            }
            else {
                sstream << '0';
            }
        }

        return sstream.str();
    }

    std::string bytes_to_string(std::uint8_t* bytes_array, std::size_t array_length) {
        std::stringstream sstream("");

        std::uint8_t* current_byte = nullptr;
        for (auto i = 0; i < array_length; i++) {
            current_byte = bytes_array + i;
            for (std::uint8_t index = 0; index < 8; index++) {
                if (read_bit(*current_byte, index)) {
                    sstream << '1';
                }
                else {
                    sstream << '0';
                }
            }
        }

        return sstream.str();
    }

}
#endif