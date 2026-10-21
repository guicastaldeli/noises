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
};