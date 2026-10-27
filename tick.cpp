/****************************** 
 * 
 * 
 * main Tick implementation...
 * 
 * 
 * ***********************/
#include <tick.hpp>

Tick::Tick() {};
Tick::~Tick() {};

/**
 * Should Tick
 */
bool Tick::ShouldTick() const {
    return ticked;
}

/**
 * Get Delta Time
 */
float Tick::GetDeltaTime() const {
    return deltaTime;
}

/**
 * Get Elapsed Time
 */
float Tick::GetElapsedTime() const {
    return elapsedTime;
}

/**
 * Get Tick Rate
 */
float Tick::GetTickRate() const {
    return TICKS_PER_SECOND;
}

/**
 * Get Tick Count
 */
uint64_t Tick::GetTickCount() const {
    return tickCount;
}

/**
 * 
 * Update
 * 
 */
void Tick::Update() {
    TimePoint now = Clock::now();
    deltaTime = std::chrono::duration<float, std::milli>(now - lastTime).count();
    lastTime = now;

    elapsedTime += deltaTime;
    accumulator += deltaTime;

    ticked = false;
    if(accumulator >= MS_PER_TICK) {
        tickCount++;
        ticked = true;
        accumulator -= MS_PER_TICK;
    }
}