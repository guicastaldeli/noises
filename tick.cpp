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
    float ms = std::chrono::duration<float, std::milli>(now - lastTime).count();
    lastTime = now;

    deltaMs = std::min(ms, 100.0f);
    deltaTime = deltaMs * 0.001;

    elapsedTime += deltaTime;
    accumulator += deltaMs;

    ticked = false;
    if(accumulator >= MS_PER_TICK) {
        tickCount++;
        ticked = true;
        accumulator -= MS_PER_TICK;
    }
}