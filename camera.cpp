#include <camera.hpp>
#include <cake.hpp>
#include <cmath>
#include <input.hpp>
#include <collision.hpp>

/**************** 
 * 
 * RigidBody
 * 
 * *****/
// Pause
void RigidBody::pause() {
    paused = true;
    velocity = Vec3(0, 0, 0);
}

// Resume
void RigidBody::resume() {
    paused = false;
}

// Toggle
void RigidBody::toggle() {
    paused ? resume() : pause();
}

// Is Paused
bool RigidBody::isPaused() const {
    return paused;
} 

// Set Camera
void RigidBody::setCamera(const Camera& c) {
    camera = &c;
}

// Set Tick
void RigidBody::setTick(const Tick& t) {
    tick = &t;
}

// Get Position
const Vec3& RigidBody::getPosition() const {
    return position;
}

// Set Position
void RigidBody::setPosition(const Vec3& p) {
    position = p;
}

// Get Velocity
const Vec3& RigidBody::getVelocity() const {
    return velocity;
}

// Set Velocity
void RigidBody::setVelocity(const Vec3& v) {
    velocity = v;
}

// Get Size
const Vec3& RigidBody::getSize() const {
    return size;
}

// Set Size
void RigidBody::setSize(const Vec3& s) {
    size = s;
}

// Is Grounded
bool RigidBody::isGrounded() const {
    return grounded;
}

// Set Grounded
void RigidBody::setGrounded(bool g) {
    grounded = g;
}

// Move
void RigidBody::move(Vec3 m, float moveSpeed, bool flyMode) {
    if(paused) return;

    Vec3 vel = velocity;

    float v = 1e-6f;
    float len = std::sqrt(m.x*m.x + m.z*m.z);
    if(len > v) {
        m.x /= len; 
        m.z /= len;
        vel.x = m.x * moveSpeed;
        vel.z = m.z * moveSpeed;
    } else {
        vel.x = 0.0f;
        vel.z = 0.0f;
    }
    if(flyMode) {
        vel.y = m.y * moveSpeed;
    }

    setVelocity(vel);
}

// Jump
void RigidBody::jump(bool g) {
    if(isJumping && g) {
        if(verticalVel <= 0.0f) {
            isJumping = false;
            grounded = true;
            wantsJump = false;
        }
        return;
    }
}

// Set Gravity
void RigidBody::setGravity(float deltaTime) {
    float MAX_FALL_SPEED = 50.0f;
    if(!grounded) {
        verticalVel -= GRAVITY * deltaTime;
        if(verticalVel < -MAX_FALL_SPEED) verticalVel = -MAX_FALL_SPEED;
    } else {
        if(verticalVel <= 0.0f) verticalVel = 0.0f;
    }
}

// Update
void RigidBody::update() {
    if(paused || !tick) return;
    float deltaTime = tick->GetDeltaTime() * 0.001f;
    position += velocity * deltaTime;
}

// Init
void RigidBody::init() {
    setTick(camera->getTick());
    setSize(camera->rigidBodySize);
    setPosition(Vec3(camera->POS_X, 
                    camera->POS_Y, 
                    camera->POS_Z));
}
/**
 * 
 */

/*************************
 * 
 * Camera
 * 
 *************************/
Camera::Camera(Main& main, Tick& tick) : 
    main(main),
    tick(tick) {
    position = Vec3(POS_X, POS_Y, POS_Z);

    rigidBody.setCamera(*this);
    rigidBody.init();

    set();
}
Camera::~Camera() {};

/**
 * Get Tick
 */
Tick& Camera::getTick() const {
    return tick;
}

/**
 * Get RigidBody
 */
RigidBody& Camera::getRigidBody() {
    return rigidBody;
}

/**
 * Set Input
 */
void Camera::setInput(Input* input) {
    this->input = input;
}

/**
 * Set Position
 */
void Camera::setPosition() {
    position = Vec3(POS_X, POS_Y, POS_Z);
    set();
    setYawPitch();
}

/**
 * Get Position
 */
Vec3 Camera::getPosition() const {
    return rigidBody.getPosition();
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
    const Vec3 p = getPosition();
    Mat4 v = Mat4::Identity();

    v.m[0][0] = right.x;
    v.m[0][1] = right.y;
    v.m[0][2] = right.z;
    v.m[0][3] = -right.x * p.x - right.y * p.y - right.z * p.z;

    v.m[1][0] = -up.x;
    v.m[1][1] = -up.y;
    v.m[1][2] = -up.z;
    v.m[1][3] = up.x * p.x + up.y * p.y + up.z * p.z;

    v.m[2][0] = -forward.x;
    v.m[2][1] = -forward.y;
    v.m[2][2] = -forward.z;
    v.m[2][3] = forward.x * p.x + forward.y * p.y + forward.z * p.z;

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
    p.m[1][1] = -f;

    p.m[2][2] = zFar / (zNear - zFar);
    p.m[2][3] = (zNear * zFar) / (zNear - zFar);

    p.m[3][2] = -1.0f;
    p.m[3][3] = 0.0f;

    return p;
}

/**
 * Toggle Fly Mode
 */
void Camera::toggleFlyMode() {
    flyMode = !flyMode;

    Vec3 v = rigidBody.getVelocity();
    v.y = 0.0;
    rigidBody.setVelocity(v);
    
    rigidBody.verticalVel = 0.0f;
    rigidBody.grounded = false;
}

/**
 * Is Flying
 */
bool Camera::isFlying() const {
    return flyMode;
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

    Vec3 terrainUp = Vec3(0.0f, 1.0f, 0.0f);
    right.x = forward.x * terrainUp.z - forward.z * terrainUp.y;
    right.y = forward.z * terrainUp.x - forward.x * terrainUp.z;
    right.z = forward.x * terrainUp.y - forward.y * terrainUp.x;

    int v = 1e-6f;
    float rl = std::sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
    if(rl > v) { right.x /= rl; right.y /= rl; right.z /= rl; }

    up.x = right.y * forward.z - right.z * forward.y;
    up.y = right.z * forward.x - right.x * forward.z;
    up.z = right.x * forward.y - right.y * forward.x;
}

/**
 * Move
 */
// Move
void Camera::move() {
    Vec3 move(0.0f, 0.0f, 0.0f);
    Vec3 fwd(forward.x, 0.0f, forward.z);

    float v = 1e-6f;
    float fl = std::sqrt(fwd.x*fwd.x + fwd.z*fwd.z);
    if(fl > v) { fwd.x /= fl; fwd.z /= fl; }

    float f = 1.0f;
    if(input->down(Input::Bindings::FORWARD))       move = move + fwd;
    if(input->down(Input::Bindings::BACKWARD))      move = move - fwd;
    if(input->down(Input::Bindings::RIGHT))         move = move + right;
    if(input->down(Input::Bindings::LEFT))          move = move - right;
    if(flyMode) {
        if(input->down(Input::Bindings::UP))        move.y += f;
        if(input->down(Input::Bindings::DOWN))      move.y -= f;
    } else {
        Vec3 vel = rigidBody.getVelocity();
        vel.y = rigidBody.verticalVel;
        rigidBody.setVelocity(vel);
    }

    rigidBody.move(move, MOVE_SPEED, flyMode);
}

// Set Move
void Camera::setMove(float deltaTime) {
    if(!flyMode) {
        jump();
        rigidBody.setGravity(deltaTime);
    } else {
        rigidBody.verticalVel = 0.0f;
        rigidBody.grounded = false;
    }
}

/**
 * Jump
 */
void Camera::jump() {
    if(input->pressed(Input::Bindings::UP)) {
        rigidBody.wantsJump = true;
    }
    if(rigidBody.grounded && !rigidBody.isJumping && rigidBody.wantsJump) {
        rigidBody.verticalVel = rigidBody.JUMP_SPEED;
        rigidBody.grounded = false;
        rigidBody.isJumping = true;
    }
}

/**
 * Pause
 */
// Pause
void Camera::pause() {
    paused = true;
}

// Is Paused
bool Camera::isPaused() const {
    return paused;
}

/**
 * Resume
 */
void Camera::resume() {
    paused = false;
}

/**
 * Toggle
 */
void Camera::toggle() {
    paused = !paused;
}

/**
 * 
 * Update
 * 
 */
void Camera::update() {
    float deltaTime = tick.GetSDeltaTime();

    if(paused) {
        rigidBody.pause();
        set();
        return;
    }
    rigidBody.resume();
    
    if(input->isCursorCaptured()) {
        yaw -= static_cast<float>(input->mouseDeltaX()) * MOUSE_SENSV;
        pitch -= static_cast<float>(input->mouseDeltaY()) * MOUSE_SENSV;

        const float limit = 1.55334f;
        if(pitch > limit) pitch = limit;
        if(pitch < -limit) pitch = -limit;
    }

    if(input->pressed(Input::Bindings::FLY)) toggleFlyMode();
    set();
    setMove(deltaTime);
    move();

    /*if(tick.ShouldTick()) {
        Vec3 p = getPosition();
        std::cout << "[camera] pos: ("
                  << p.x << ", " << p.y << ", " << p.z
                  << ")  yaw: " << yaw
                  << "  pitch: " << pitch
                  << std::endl;
    }*/
}

/**
 * 
 * Init
 * 
 */
void Camera::init(Input* input, CollisionManager* collision) {
    setInput(input);
    this->input->setCursorCaptured(true);
    set();
    setPosition();
    rigidBody.update();
    
    collision->onResolved = [this](RigidBody& rb, bool g) { 
        if(&rb == &rigidBody) {
            rigidBody.jump(g);
            rigidBody.grounded = g; 
        }
    };
    collision->set(getRigidBody(), CollisionType::STATIC_COLLIDER);
}