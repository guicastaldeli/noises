#pragma once

#include <utils/vec3.hpp>
#include <utils/mat4.hpp>
#include <tick.hpp>

class Main;
class Camera {
private:
    Main& main;
    Tick& tick;
    Input* input = nullptr;

    Vec3 position;
    float yaw = 0.0f;
    float pitch = 0.0f;

    Vec3 forward;
    Vec3 right;
    Vec3 up;

public:
    Camera(Main& main, Tick& tick);
    ~Camera();

    float zNear = 0.1f;
    float zFar = 100.0f;
    float fov = 60.0f;

    float POS_X = 0.0f;
    float POS_Y = 1.0f;
    float POS_Z = 6.0f;

    float MOVE_SPEED = 3.0f;
    float MOUSE_SENSV = 0.0025f;

    void setInput(Input* input); 

    void setPosition();
    void setYawPitch();

    Mat4 getProjectionMatrix();
    Mat4 getViewMatrix() const;
    Vec3 getPosition() const;
    Vec3 getForward() const;
    Vec3 getRight() const;
    Vec3 getUp() const;

    void set();
    void init();
    void move();
    void update();
};