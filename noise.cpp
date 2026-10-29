#include <noise.hpp>
#include <cmath>
#include <cstdint>

namespace {

// hash: 32-bit integer hash -> float in [-1, 1]
inline uint32_t hash3(int32_t x, int32_t y, int32_t z, int32_t seed) {
    uint32_t h = uint32_t(seed);
    h ^= uint32_t(x) * 0x9E3779B1;
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
    float a = float(hash2(ix, iz, seed)) / 2147483648.0f * 3.14159265f * 2.0f;
    gx = std::cos(a);
    gz = std::sin(a);
}

float perlin2(float x, float z, int seed, InterpType it) {
    int xi = int(std::floor(x)), zi = int(std::floor(z));
    float xf = x - xi,          zf = z - zi;

    auto dot = [&](int cx, int cz) {
        float gx, gz;
        grad2(cx, cz, seed, gx, gz);
        return gx * (x - cx) + gz * (z - cz);
    };

    float n00 = dot(xi, zi);
    float n10 = dot(xi+1, zi);
    float n01 = dot(xi, zi+1);
    float n11 = dot(xi+1, zi+1);

    float sx = smoothT(xf, it), sz = smoothT(zf, it);
    float a = n00 + (n10 - n00) * sx;
    float b = n01 + (n11 - n01) * sx;
    return (a + (b - a) * sz) * 1.4f;
}

float perlin3(float x, float y, float z, int seed, InterpType it) {
    static const float g[12][3] = {
        {1, 1, 0},{-1, 1, 0},{ 1,-1, 0},{-1,-1, 0},
        {1, 0, 1},{-1, 0, 1},{ 1, 0,-1},{-1, 0,-1},
        {0, 1, 1},{ 0,-1, 1},{ 0, 1,-1},{ 0,-1,-1},
    };

    int xi = int(std::floor(x)); 
    int yi = int(std::floor(y)); 
    int zi = int(std::floor(z));
    
    float xf = x - xi;
    float yf = y - yi;
    float zf = z - zi;

    auto dot = [&](int cx, int cy, int cz) {
        int idx = hash3(cx, cy, cz, seed) % 12;
        return g[idx][0] * (x - cx) + g[idx][1] * (y - cy) + g[idx][2] * (z - cz);
    };

    float n000 = dot(xi, yi, zi);
    float n100 = dot(xi+1, yi, zi);
    float n010 = dot(xi, yi+1, zi);
    float n110 = dot(xi+1, yi+1, zi);
    float n001 = dot(xi, yi, zi+1);
    float n101 = dot(xi+1, yi, zi+1);
    float n011 = dot(xi, yi+1, zi+1);
    float n111 = dot(xi+1, yi+1, zi+1);

    float sx = smoothT(xf, it);
    float sy = smoothT(yf, it);
    float sz = smoothT(zf, it);

    float a0 = n000 + (n100 - n000) * sx;
    float a1 = n010 + (n110 - n010) * sx;
    float a2 = n001 + (n101 - n001) * sx;
    float a3 = n011 + (n111 - n011) * sx;
    float b0 = a0 + (a1 - a0) * sy;
    float b1 = a2 + (a3 - a2) * sy;
    return (b0 + (b1 - b0) * sz) * 1.0f;
}

// Fractal wrapper
float fractal2(float x, float z, const NoiseParams& p,
                float (*base)(float, float, int, InterpType))
{
    float sum = 0.0f, amp = 1.0f, freq = p.frequency, norm = 0.0f;
    for(int o = 0; o < p.octaves; ++o) {
        float v = base(x * freq, z * freq, p.seed + o * 1013, p.interp);
        switch(p.fractal) {
            case FractalType::FBM:              sum += v * amp; break;
            case FractalType::Billow:           sum += (std::fabs(v) * 2.0f - 1.0f) * amp; break;
            case FractalType::RigitMulti:       sum += (1.0f - std::fabs(v)) * amp * 2.0f - 1.0f; break;
        }
        norm += amp;
        freq *= p.lacunary;
        amp *= p.gain;
    }
    return norm > 0.0f ? sum / norm : 0.0f;
}

}

/**********
 * 
 * Sample Noise
 * 
 */
float Noise::sampleNoise(const NoiseParams& p, float x, float y, float z) {
    const float f = p.frequency;
    switch(p.type) {
        case NoiseType::Value:                              return valueNoise2(x * f, z * f, p.seed, p.interp);
        case NoiseType::ValueFractual:                      return fractal2(x, z, p, valueNoise2);
        case NoiseType::Perlin:                             return perlin2(x * f, z * f, p.seed, p.interp);
        case NoiseType::PerlinFractual:                     return fractal2(x, z, p, perlin2);
        /* todo:*/case NoiseType::Cubic:                    return valueNoise2(x * f, z * f, p.seed, InterpType::Quintic);
        case NoiseType::CubicFractual:                      return fractal2(x, z, p, valueNoise2);
        case NoiseType::WhiteNoise: {
            int xi = int(std::floor(x * f)), zi = int(std::floor(z * f));
            return hash2f(xi, zi, p.seed);
        }
        case NoiseType::Cellular: {
            int xi = int(std::floor(x * f)), zi = int(std::floor(z * f));
            float best = 1e9f;
            for(int dz = -1; dz <= 1; ++dz) {
                for(int dx = -1; dx <= 1; ++dx) {
                    float px = float(xi + dx) + hash2f(xi+dx, zi+dz, p.seed) * 0.5f + 0.5f;
                    float pz = float(zi + dz) + hash2f(xi+dx, zi+dz, p.seed+1) * 0.5f + 0.5f;
                    float ddx = px - x * f, ddz = pz - z * f;
                    float d;
                    switch(p.cellDist) {
                        case CellularDistance::Manhattan:   d = std::fabs(ddx) + std::fabs(ddz); break;
                        case CellularDistance::Natural:     d = std::fabs(ddx) * std::fabs(ddx) + std::fabs(ddz) * std::fabs(ddz); break;
                        default:                            d = std::sqrt(ddx*ddx + ddz*ddz); break;
                    }
                    if(d < best) best = d;
                }

                float d = best;
                switch(p.cellRet) {
                    case CellularReturn::CellValue:         d = hash2f(xi, zi, p.seed); break;
                    case CellularReturn::Distance:          break;
                    case CellularReturn::Distance2:         d = d * d; break;
                    case CellularReturn::Distance2Add:      d = d + d; break;
                    case CellularReturn::Distance2Sub:      d = d - d; break;
                    case CellularReturn::Distance2Mul:      d = d * d; break;
                    case CellularReturn::Distance2Div:      d = d / (d + 1e6f); break;
                    case CellularReturn::NoiseLookup:       d = perlin2(x * f, z * f, p.seed, p.interp); break;
                }

                return d * 2.0f - 1.0f;
            }
        }
    }

    return 0.0f;
}