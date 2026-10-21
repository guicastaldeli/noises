/******************************* 
 * 
 * 
 * custom general Transform operations...
 * 
 * 
*/
#pragma once

#define MATH_TRANSFORM

#include <vec3.hpp>
#include <mat4.hpp>
#include <vector>
#include <stdexcept>
#include <map>
#include <cmath>

struct Transform {
    Vec3 position;
    Vec3 rotation;
    Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);

    Mat4 GetModelMatrix() const {
        Mat4 model = Mat4::Identity();
        model = Mat4::Translate(position) * model;
        model = Mat4::RotateX(rotation.x) * model;
        model = Mat4::RotateY(rotation.y) * model;
        model = Mat4::RotateZ(rotation.z) * model;
        model = Mat4::Scale(scale) * model;
        return model;
    }
};