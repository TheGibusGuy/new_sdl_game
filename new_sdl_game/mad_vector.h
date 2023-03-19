#ifndef MAD_VECTOR
#define MAD_VECTOR

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>

using namespace std;
using namespace fpm;

namespace mad {
    template <typename T>
    class Vector2D {
        public:
            T x;
            T y;

            Vector2D(T X, T Y) {
                x = X;
                y = Y;
            }

            // addition
            Vector2D operator + (const Vector2D& obj) {
                return { this->x + obj.x, this->y + obj.y };
            }
            void operator += (const Vector2D& obj) {
                this->x += obj.x;
                this->y += obj.y;
            }

            // subtraction
            Vector2D operator - (const Vector2D& obj) {
                return { this->x - obj.x, this->y - obj.y };
            }
            void operator -= (const Vector2D& obj) {
                this->x -= obj.x;
                this->y -= obj.y;
            }

            // multiplication
            Vector2D operator * (const Vector2D& obj) {
                return { this->x * obj.x, this->y * obj.y };
            }
            void operator *= (const Vector2D& obj) {
                this->x *= obj.x;
                this->y *= obj.y;
            }

            // division
            Vector2D operator / (const Vector2D& obj) {
                return { this->x / obj.x, this->y / obj.y };
            }
            void operator /= (const Vector2D& obj) {
                this->x /= obj.x;
                this->y /= obj.y;
            }

            // magnitude
            T mag() {
                T thetaA{ fpm::atan(this->y / this->x) };
                T lengthA{ (this->x * fpm::cos(thetaA)) + (this->y * fpm::sin(thetaA)) };
;               return fpm::abs(lengthA);
            }

            // normalize
            Vector2D norm() {
                T magnitude{ this->mag() };
                return { this->x / magnitude, this->y / magnitude };
            }

            // dot product
            T dot(const Vector2D& obj) {
                return (this->x * obj.x) + (this->y * obj.y);
            }

            // a nicely formatted string
            string str() {
                stringstream sstream;
                sstream << '(' << this->x << ',' << this->y << ')';
                return sstream.str();
            }
    };

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
            Vector3D operator + (const Vector3D& obj) {
                return { this->x + obj.x, this->y + obj.y, this->z + obj.z};
            }
            void operator += (const Vector3D& obj) {
                this->x += obj.x;
                this->y += obj.y;
                this->z += obj.z;
            }

            // subtraction
            Vector3D operator - (const Vector3D& obj) {
                return { this->x - obj.x, this->y - obj.y, this->z - obj.z };
            }
            void operator -= (const Vector3D& obj) {
                this->x -= obj.x;
                this->y -= obj.y;
                this->z -= obj.z;
            }

            // multiplication
            Vector3D operator * (const Vector3D & obj) {
                return { this->x * obj.x, this->y * obj.y, this->z * obj.z };
            }
            void operator *= (const Vector3D& obj) {
                this->x *= obj.x;
                this->y *= obj.y;
                this->z *= obj.z;
            }

            // division
            Vector3D operator / (const Vector3D& obj) {
                return { this->x / obj.x, this->y / obj.y, this->z / obj.z };
            }
            void operator /= (const Vector3D& obj) {
                this->x /= obj.x;
                this->y /= obj.y;
                this->z /= obj.z;
            }

            // magnitude
            T mag() {
                T thetaA{ fpm::atan(this->y / this->x) };
                T lengthA{ (this->x * fpm::cos(thetaA)) + (this->y * fpm::sin(thetaA)) };
                T thetaB{ fpm::atan( this->z / lengthA) };
                T lengthB{ (lengthA * fpm::cos(thetaB)) + (this->z * fpm::sin(thetaB)) };
                return fpm::abs(lengthB);
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
            string str() {
                stringstream sstream;
                sstream << '(' << this->x << ',' << this->y << ',' << this->z << ')';
                return sstream.str();
            }
    };
}

#endif