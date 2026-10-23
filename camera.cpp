#include <camera.hpp>
#include <cake.hpp>
#include <cmath>

Camera::Camera(Main& main) : main(main) {
    position = Vec3(POS_X, -POS_Y, POS_Z);
    yaw = 0.0f;
    pitch = 0.0f;
    set();
}
Camera::~Camera() {};

/**
 * Set Position
 */
void Camera::setPosition() {
    position = Vec3(POS_X, -POS_Y, POS_Z);
    set();
    setYawPitch();
}

/**
 * Get Position
 */
Vec3 Camera::getPosition() const {
    return position;
}

/**
 * Get Forward
 */
Vec3 Camera::getForward() const {
    return forward;
}

/**
 * Get Right
 */
Vec3 Camera::getRight() const {
    return right;
}

/**
 * Get Up
 */
Vec3 Camera::getUp() const {
    return up;
}

/**
 * Set Yaw Pitch
 */
void Camera::setYawPitch() {
    const float limit = 1.55334f;
    if(pitch > limit) pitch = limit;
    if(pitch < -limit) pitch = -limit;

    set();
}

/**
 * Get View Matrix
 */
Mat4 Camera::getViewMatrix() const {
    Mat4 v = Mat4::Identity();

    v.m[0][0] = right.x;
    v.m[0][1] = right.y;
    v.m[0][2] = right.z;
    v.m[0][3] = -right.x * position.x - right.y * position.y - right.z * position.z;

    v.m[1][0] = up.x;
    v.m[1][1] = up.y;
    v.m[1][2] = up.z;
    v.m[1][3] = -up.x * position.x - up.y * position.y - up.z * position.z;

    v.m[2][0] = -forward.x;
    v.m[2][1] = -forward.y;
    v.m[2][2] = -forward.z;
    v.m[2][3] = forward.x * position.x + forward.y * position.y + forward.z * position.z;

    return v;
}

/**
 * Get Projection Matrix
 */
Mat4 Camera::getProjectionMatrix() {
    Mat4 p;
    float c = fov * 3.14159265f / 180.0f;
    float f = 1.0f / std::tan(c * 0.5f);

    p.m[0][0] = f / main.getAspectRatio();
    p.m[1][1] = f;

    p.m[2][2] = zFar / (zNear - zFar);
    p.m[2][3] = (zNear * zFar) / (zNear - zFar);

    p.m[3][2] = -1.0f;
    p.m[3][3] = 0.0f;

    return p;
}

/**
 * 
 * Set
 * 
 */
void Camera::set() {
    float cy = std::cos(yaw);
    float sy = std::sin(yaw);
    float cp = std::cos(pitch);
    float sp = std::sin(pitch);

    forward.x = -sy * cp;
    forward.y = sp;
    forward.z = -cy * cp;

    Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f);
    right.x = forward.x * worldUp.z - forward.z * worldUp.y;
    right.y = forward.z * worldUp.x - forward.x * worldUp.z;
    right.z = forward.x * worldUp.y - forward.y * worldUp.x;

    int v = 1e-6f;
    float rl = std::sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
    if(rl > v) { right.x /= rl; right.y /= rl; right.z /= rl; }

    up.x = right.y * forward.z - right.z * forward.y;
    up.y = right.z * forward.x - right.x * forward.z;
    up.z = right.x * forward.y - right.y * forward.x;
}

/**
 * 
 * Init
 * 
 */
void Camera::init() {
    set();
    setPosition();
}