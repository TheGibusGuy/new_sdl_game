#ifndef MAD_VECTOR
#define MAD_VECTOR

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>

namespace mad {
    // it seeems this using declaration has to go after all the include directives
    using fp = fpm::fixed_16_16;

    template <typename T>
    class Vector3D {
        public:
            T x;
            T y;
            T z;

            Vector3D(T X, T Y, T Z) {
                x = X;
                y = Y;
                z = Z;
            }

            // addition
            inline Vector3D operator + (const Vector3D& obj) {
                return { this->x + obj.x, this->y + obj.y, this->z + obj.z};
            }
            inline Vector3D& operator += (const Vector3D& obj) {
                this->x += obj.x;
                this->y += obj.y;
                this->z += obj.z;
                return *this;
            }

            // subtraction
            inline Vector3D operator - (const Vector3D& obj) {
                return { this->x - obj.x, this->y - obj.y, this->z - obj.z };
            }
            inline Vector3D& operator -= (const Vector3D& obj) {
                this->x -= obj.x;
                this->y -= obj.y;
                this->z -= obj.z;
                return *this;
            }

            // multiplication
            inline Vector3D operator * (const Vector3D & obj) {
                return { this->x * obj.x, this->y * obj.y, this->z * obj.z };
            }
            inline Vector3D& operator *= (const Vector3D& obj) {
                this->x *= obj.x;
                this->y *= obj.y;
                this->z *= obj.z;
                return *this;
            }

            // division
            inline Vector3D operator / (const Vector3D& obj) {
                return { this->x / obj.x, this->y / obj.y, this->z / obj.z };
            }
            inline Vector3D& operator /= (const Vector3D& obj) {
                this->x /= obj.x;
                this->y /= obj.y;
                this->z /= obj.z;
                return *this;
            }

            // scaling
            Vector3D scale(fp scalar) {
                return { this->x * scalar, this->y * scalar, this->z * scalar };
            }

            // check if zero
            bool is_zero() {
                if ((this->x == (fp)0) && (this->y == (fp)0) && (this->z == (fp)0)) return true;
                return false;
            }

            // magnitude
            T mag() {
                // is important that i put the abs in the right spots here
                // this was breaking earlier since I didn't
                T thetaA;
                if (this->x == fp(0)) {
                    thetaA = T::half_pi();
                }
                else {
                    thetaA = fpm::atan(this->y / this->x);
                }
                T lengthA{ fpm::abs((this->x * fpm::cos(thetaA))) + fpm::abs((this->y * fpm::sin(thetaA))) };

                T thetaB;
                if (lengthA == fp(0)) {
                    thetaB = T::half_pi();
                }
                else {
                    thetaB = fpm::atan(this->z / lengthA);
                };
                T lengthB{ fpm::abs((lengthA * fpm::cos(thetaB))) + fpm::abs((this->z * fpm::sin(thetaB))) };

                return lengthB;
            }

            // normalize
            Vector3D norm() {
                T magnitude{ this->mag() };
                return { (this->x) / magnitude, (this->y) / magnitude, (this->z) / magnitude };
            }

            // dot product
            T dot(const Vector3D& obj) {
                return (this->x * obj.x) + (this->y * obj.y) + (this->z * obj.z);
            }

            // a nicely formatted string
            std::string str() {
                std::stringstream sstream;
                sstream << '(' << this->x << ',' << this->y << ',' << this->z << ')';
                return sstream.str();
            }
    };

    // it seeems these using declarations have to go after all the include directives
    using FixedVec3D = Vector3D<fp>;
}

#endif