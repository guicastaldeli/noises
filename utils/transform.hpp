/******************************* 
 * 
 * 
 * custom general Transform operations...
 * 
 * 
*/
#pragma once

#define MATH_TRANSFORM

#include <utils/vec3.hpp>
#include <utils/mat4.hpp>
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
        Mat4 T = Mat4::Translate(position);
        Mat4 Rx = Mat4::RotateX(rotation.x);
        Mat4 Ry = Mat4::RotateY(rotation.y);
        Mat4 Rz = Mat4::RotateZ(rotation.z);
        Mat4 S = Mat4::Scale(scale);
        model = T * Rx * Ry * Rz * S * model;
        return model;
    }
};