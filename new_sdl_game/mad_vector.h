#ifndef MAD_VECTOR
#define MAD_VECTOR

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>

#include "fpm/fixed.hpp"
#include "fpm/ios.hpp"
#include "fpm/math.hpp"

namespace mad {
    template <typename T>
    class FixedVector3 {
        public:
            T x;
            T y;
            T z;

            FixedVector3(T X, T Y, T Z) {
                x = X;
                y = Y;
                z = Z;
            }

            // addition
            inline FixedVector3 operator + (const FixedVector3& obj) {
                return { this->x + obj.x, this->y + obj.y, this->z + obj.z};
            }
            inline FixedVector3& operator += (const FixedVector3& obj) {
                this->x += obj.x;
                this->y += obj.y;
                this->z += obj.z;
                return *this;
            }

            // subtraction
            inline FixedVector3 operator - (const FixedVector3& obj) {
                return { this->x - obj.x, this->y - obj.y, this->z - obj.z };
            }
            inline FixedVector3& operator -= (const FixedVector3& obj) {
                this->x -= obj.x;
                this->y -= obj.y;
                this->z -= obj.z;
                return *this;
            }

            // scaling
            inline FixedVector3 operator * (T scalar) {
                return { this->x * scalar, this->y * scalar, this->z * scalar };
            }
            inline FixedVector3& operator *= (T scalar) {
                this->x *= scalar;
                this->y *= scalar;
                this->z *= scalar;
                return *this;
            }

            // check if zero
            inline bool is_zero() {
                if ((this->x == (T)0) && (this->y == (T)0) && (this->z == (T)0)) return true;
                return false;
            }

            // magnitude
            inline T mag() {
                // is important that i put the abs in the right spots here
                // this was breaking earlier since I didn't
                T thetaA;
                if (this->x == T(0)) {
                    thetaA = T::half_pi();
                }
                else {
                    thetaA = fpm::atan(this->y / this->x);
                }
                T lengthA{ fpm::abs((this->x * fpm::cos(thetaA))) + fpm::abs((this->y * fpm::sin(thetaA))) };

                T thetaB;
                if (lengthA == T(0)) {
                    thetaB = T::half_pi();
                }
                else {
                    thetaB = fpm::atan(this->z / lengthA);
                };
                T lengthB{ fpm::abs((lengthA * fpm::cos(thetaB))) + fpm::abs((this->z * fpm::sin(thetaB))) };

                return lengthB;
            }

            // normalize
            inline FixedVector3 norm() {
                T magnitude{ this->mag() };
                return { (this->x) / magnitude, (this->y) / magnitude, (this->z) / magnitude };
            }

            // dot product
            inline T dot(const FixedVector3& obj) {
                return (this->x * obj.x) + (this->y * obj.y) + (this->z * obj.z);
            }

            // a nicely formatted string
            std::string str() {
                std::stringstream sstream;
                sstream << '(' << this->x << ',' << this->y << ',' << this->z << ')';
                return sstream.str();
            }
    };
}

#endif