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

#define PI 3.14159265358979323846f

#include <utils/vec3.hpp>
#include <vector>
#include <stdexcept>
#include <map>
#include <cmath>

struct Mat4 {
    float m[4][4] = {0};

    static Mat4 Identity() {
        Mat4 mat;
        mat.m[0][0] = 1.0f; 
        mat.m[1][1] = 1.0f;
        
        mat.m[2][2] = 1.0f; 
        mat.m[3][3] = 1.0f;
        return mat;
    }
    static Mat4 Translate(const Vec3& v) {
        Mat4 mat = Identity();
        mat.m[0][3] = v.x; 
        mat.m[1][3] = v.y; 
        mat.m[2][3] = v.z;
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
        mat.m[1][1] = c; 
        mat.m[1][2] = -s;
        
        mat.m[2][1] = s; 
        mat.m[2][2] = c;
        return mat;
    }
    static Mat4 RotateY(float angle) {
        Mat4 mat = Identity();
        float c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = c; 
        mat.m[0][2] = s;
        
        mat.m[2][0] = -s; 
        mat.m[2][2] = c;
        return mat;
    }
    static Mat4 RotateZ(float angle) {
        Mat4 mat = Identity();
        float c = std::cos(angle), s = std::sin(angle);
        mat.m[0][0] = c; 
        mat.m[0][1] = -s;
        
        mat.m[1][0] = s; 
        mat.m[1][1] = c;
        return mat;
    }
    static Mat4 Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Mat4 m; // zero-initialized
    m.m[0][0] = 2.0f / (right - left);
    m.m[1][1] = 2.0f / (top - bottom);
    
    // Vulkan ZO (0 to 1) range
    m.m[2][2] = 1.0f / (nearZ - farZ);
    m.m[3][2] = nearZ / (nearZ - farZ);
    
    m.m[3][0] = -(right + left) / (right - left);
    m.m[3][1] = -(top + bottom) / (top - bottom);
    m.m[3][3] = 1.0f;
    return m;
}
    static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = Vec3(center.x - eye.x, center.y - eye.y, center.z - eye.z);
        float fl = std::sqrt(f.x*f.x + f.y*f.y + f.z*f.z);
        f = Vec3(f.x/fl, f.y/fl, f.z/fl);

        Vec3 s = Vec3(f.y*up.z - f.z*up.y,
                        f.z*up.x - f.x*up.z,
                        f.x*up.y - f.y*up.x);
        float sl = std::sqrt(s.x*s.x + s.y*s.y + s.z*s.z);
        s = Vec3(s.x/sl, s.y/sl, s.z/sl);

        Vec3 u = Vec3(s.y*f.z - s.z*f.y,
                        s.z*f.x - s.x*f.z,
                        s.x*f.y - s.y*f.x);

        Mat4 m;
        m.m[0][0] = s.x;  m.m[0][1] = s.y;  m.m[0][2] = s.z;  m.m[0][3] = -(s.x*eye.x + s.y*eye.y + s.z*eye.z);
        m.m[1][0] = u.x;  m.m[1][1] = u.y;  m.m[1][2] = u.z;  m.m[1][3] = -(u.x*eye.x + u.y*eye.y + u.z*eye.z);
        m.m[2][0] = -f.x; m.m[2][1] = -f.y; m.m[2][2] = -f.z; m.m[2][3] = f.x*eye.x + f.y*eye.y + f.z*eye.z;
        m.m[3][3] = 1.0f;
        return m;
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