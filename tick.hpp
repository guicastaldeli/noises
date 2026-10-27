/****************************** 
 * 
 * 
 * ***header
 * main Tick implementation...
 * 
 * 
 * ***********************/

#pragma once

#include <chrono>

class Tick {
private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    int TICKS_PER_SECOND = 20;
    float MS_PER_TICK = 1000.0f / TICKS_PER_SECOND;
    
    float accumulator = 0.0f;
    float deltaTime = 0.0f;
    float elapsedTime = 0.0f;
    uint64_t tickCount = 0;
    bool ticked = false;

    TimePoint lastTime = Clock::now();

public:
    Tick();
    ~Tick();

    bool ShouldTick() const;
    float GetDeltaTime() const;
    float GetTickRate() const;
    float GetElapsedTime() const;
    uint64_t GetTickCount() const;
    
    void Update();
};