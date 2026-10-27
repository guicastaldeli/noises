#include <noise.hpp>
#include <cmath>
#include <cstdint>

namespace {

// hash: 32-bit integer hash -> float in [-1, 1]
inline uint32_t hash3(int32_t x, int32_t y, int32_t z, int32_t seed) {
    uint32_t h = uint32_t(seed);
    h ^= uint32_t(x) * 0x9E377B1u;
    h ^= uint32_t(y) * 0x85EBCA77u;
    h ^= uint32_t(z) * 0xC2B2AE3Du;
    h ^= h >> 16;
    h *= 0x7FEB352Du;
    h ^= h >> 15;
    h *= 0x846CA68Bu;
    h ^= h >> 16;
    return h;
}
inline float hash3f(int32_t x, int32_t y, int32_t z, int32_t seed) {
    return float(hash3(x, y, z, seed)) / 2147483648.0f - 1.0f;
}

inline uint32_t hash2(int32_t x, int32_t z, int32_t seed) {
    return hash3(x, 0, z, seed);
}
inline uint32_t hash2f(int32_t x, int32_t z, int32_t seed) {
    return float(hash2(x, z, seed)) / 2147483648.0f - 1.0f;
}

// Interpolants
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float hermite(float a, float b, float t) {
    float u = t*t*(3.0f - 2.0f*t);
    return a + (b - a) * u;
}

inline float quintic(float a, float b, float t) {
    float u = t*t*t*(t*(t*6.0f - 15.0f) + 10.0f);
    return a + (b - a) * u;
}

inline float interp(float a, float b, float t, InterpType it) {
    switch(it) {
        case InterpType::Linear:        return lerp(a, b, t);
        case InterpType::Hermite:       return hermite(a, b, t);
        case InterpType::Quintic:       return quintic(a, b, t);
    }
    return lerp(a, b, t);
}

// smooth t
inline float smoothT(float t, InterpType it) {
    switch(it) {
        case InterpType::Linear:        return t;
        case InterpType::Hermite:       return t*t*(3.0f - 2.0f*t);
        case InterpType::Quintic:       return t*t*t*(t*(t*6.0f - 15.0f) + 10.0f);
    }
    return t;
}

// Value noise
// hash-based value noise in 2d
float valueNoise2(float x, float z, int seed, InterpType it) {
    int xi = int(std::floor(x)),    zi = int(std::floor(z));
    float xf = x - xi,              zf = z - zi;
    
    float v00 = hash2f(xi, zi, seed);
    float v10 = hash2f(xi + 1, zi, seed);
    float v01 = hash2f(xi, zi + 1, seed);
    float v11 = hash2f(xi + 1, zi + 1, seed);

    float sx = smoothT(xf, it), sz = smoothT(zf, it);
    float a = v00 + (v10 - v00) * sx;
    float b = v01 + (v11 - v01) * sx;
    return a + (b - a) * sz;
}

// 3d value noise
float valueNoise3(float x, float y, float z, int seed, InterpType it) {
    int xi = int(std::floor(x));
    int yi = int(std::floor(y));
    int zi = int(std::floor(z));
    
    float xf = x - xi;
    float yf = y - yi;
    float zf = z - zi;

    float c000 = hash3f(xi, yi, zi, seed);
    float c100 = hash3f(xi+1, yi, zi, seed);
    float c010 = hash3f(xi, yi+1, zi, seed);
    float c110 = hash3f(xi+1, yi+1, zi, seed);
    float c001 = hash3f(xi, yi, zi+1, seed);
    float c101 = hash3f(xi+1, yi, zi+1, seed);
    float c011 = hash3f(xi, yi+1, zi+1, seed);
    float c111 = hash3f(xi+1, yi+1, zi+1, seed);

    float sx = smoothT(xf, it);
    float sy = smoothT(yf, it);
    float sz = smoothT(zf, it);

    float a0 = c000 + (c100 - c000) * sx;
    float a1 = c010 + (c110 - c010) * sx;
    float a2 = c001 + (c101 - c001) * sx;
    float a3 = c011 + (c111 - c011) * sx;
    float b0 = a0 + (a1 - a0) * sx;
    float b1 = a2 + (a3 - a2) * sy;
    return b0 + (b1 - b0) * sz;
}

// Gradient noise
// gradient at grid corner from hash
inline void grad2(int ix, int iz, int seed, float& gx, float& gz) {

}

}