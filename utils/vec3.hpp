/******************************* 
 * 
 * 
 * custom Vec3 calculations util, for
 * overall implementations.
 * 
 * 
*/
#pragma once

#define MATH_VEC3

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& o)   const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3 operator-(const Vec3& o)   const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3 operator*(float s)         const { return { x * s, y * s, z * s }; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s)       { x *= s; y *= s; z *= s; return *this; }
};