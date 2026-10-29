#pragma once

#include <utils/vec3.hpp>
#include <utils/mat4.hpp>
#include <tick.hpp>

/**************** 
 * 
 * RigidBody
 * 
 * *****/
class RigidBody {
private:
    Vec3 position{ 0, 0, 0 };
    Vec3 velocity{ 0, 0, 0 };
    Vec3 size{ 1, 1, 1 };

    const Tick* tick = nullptr;
    
public:
    RigidBody() = default;
    RigidBody(const Vec3& position, const Vec3& size) :
                position(position),
                size(size) {};

    void setTick(const Tick& t);

    const Vec3& getPosition() const;
    void setPosition(const Vec3& p);

    const Vec3& getVelocity() const;
    void setVelocity(const Vec3& v);

    const Vec3& getSize() const;
    void setSize(const Vec3& s);

    void move(Vec3 m, float moveSpeed);
    void update();
};
/**
 * 
 */

/*************************
 * 
 * Camera
 * 
 *************************/
class Main;
class Input;
class CollisionManager;
class Camera {
private:
    Main& main;
    Tick& tick;
    Input* input = nullptr;

    RigidBody rigidBody;

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

    float MOVE_SPEED = 10.0f;
    float MOUSE_SENSV = 0.0025f;

    Vec3 rigidBodySize{ 1.0f, 1.0f, 1.0f };

    void setInput(Input* input); 

    void setPosition();
    void setYawPitch();

    Mat4 getProjectionMatrix();
    Mat4 getViewMatrix() const;
    Vec3 getPosition() const;
    Vec3 getForward() const;
    Vec3 getRight() const;
    Vec3 getUp() const;
    RigidBody& getRigidBody();

    void set();
    void init(Input* input, CollisionManager* collision);
    void move();
    void update();
};