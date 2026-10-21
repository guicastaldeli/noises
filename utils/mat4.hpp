/******************************* 
 * 
 * 
 * custom Mat4 operations util, for
 * overall implementations.
 * 
 * 
*/
#pragma once

#define MATH_MAT4

#include <vec3.hpp>
#include <vector>
#include <stdexcept>
#include <map>
#include <cmath>

struct Mat4 {
    float m[4][4] = {0};

    static Mat4 Identity() {
        Mat4 mat;
        mat.m[0][0] = 1.0f; mat.m[1][1] = 1.0f;
        mat.m[2][2] = 1.0f; mat.m[3][3] = 1.0f;
        return mat;
    }

    static Mat4 Translate(const Vec3& v) {
        Mat4 mat = Identity();
        mat.m[3][0] = v.x; mat.m[3][1] = v.y; mat.m[3][2] = v.z;
        return mat;
    }

    static Mat4 Scale(const Vec3& v) {
        Mat4 mat = Identity();
        mat.m[0][0] = v.x; mat.m[1][1] = v.y; mat.m[2][2] = v.z;
        return mat;
    }

    static Mat4 RotateX(float angle) {
        Mat4 mat = Identity();
        float c = std::cos(angle), s = std::sin(angle);
        mat.m[1][1] = c; mat.m[1][2] = s;
        mat.m[2][1] = -s; mat.m[2][2] = c;
        return mat;
    }

    static Mat4 RotateY(float angle) {
        Mat4 mat = Identity();
        float c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = c; mat.m[0][2] = -s;
        mat.m[2][0] = s; mat.m[2][2] = c;
        return mat;
    }

    static Mat4 RotateZ(float angle) {
        Mat4 mat = Identity();
        float c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = c; mat.m[0][1] = s;
        mat.m[1][0] = -s; mat.m[1][1] = c;
        return mat;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 res;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                for(int k = 0; k < 4; k++) {
                    res.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }

        return res;
    }
};