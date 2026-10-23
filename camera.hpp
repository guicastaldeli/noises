#pragma once

#include <utils/vec3.hpp>
#include <utils/mat4.hpp>

class Main;
class Camera {
private:
    Main& main;

    Vec3 position;
    float yaw = 0.0f;
    float pitch = 0.0f;

    Vec3 forward;
    Vec3 right;
    Vec3 up;

public:
    Camera(Main& main);
    ~Camera();

    float zNear = 1.0f;
    float zFar = 100.0f;
    float fov = 120.0f;

    float POS_X = 0.0f;
    float POS_Y = 0.0f;
    float POS_Z = 0.0f;

    void setPosition();
    void setYawPitch();

    static Mat4 getProjectionMatrix();
    Mat4 getViewMatrix() const;
    Vec3 getPosition() const;
    Vec3 getForward() const;
    Vec3 getRight() const;
    Vec3 getUp() const;

    void set();
    void init();
};