#pragma once

#define TINT

#include <utils/vec3.hpp>
#include <random>
#include <cstdint>
#include <vector>

struct Tint {
    struct Config {
        Vec3 lo{ 1.0f, 1.0f, 1.0f };
        Vec3 hi{ 1.0f, 1.0f, 1.0f };
        uint32_t seed = 0u;
    };

    static uint32_t hash(uint32_t k) {
        k *= 2654435761u;
        k ^= k >> 16; k *= 0x85ebca6bu;
        k ^= k >> 13; k *= 0xc2b2ae35u;
        k ^= k >> 16;
        return k;
    }

    static Vec3 key(const Config& cfg, uint32_t k) {
        uint32_t h = hash(k);
        auto lerpch = [](uint32_t h, float lo, float hi) {
            return lo + (hi - lo) * 
                    ((h & 0xFFFFFFu) / float(0xFFFFFF));
        };

        return Vec3(
            lerpch(h,           cfg.lo.x, cfg.hi.x),
            lerpch(h ^ 0xAAu,   cfg.lo.y, cfg.hi.y),
            lerpch(h ^ 0xBBu,   cfg.lo.z, cfg.hi.z)
        );
    }

    static Vec3 next(const Config& cfg, std::mt19937& rng) {
        std::uniform_real_distribution<float> dr(cfg.lo.x, cfg.hi.x);
        std::uniform_real_distribution<float> dg(cfg.lo.y, cfg.hi.y);
        std::uniform_real_distribution<float> db(cfg.lo.z, cfg.hi.z);
        return Vec3(dr(rng), dg(rng), db(rng));
    }

    static void gen(const Config& cfg,
                    const std::vector<uint32_t>& keys,
                    std::vector<Vec3>& out)
    {
        out.resize(keys.size());
        for(size_t i = 0; i < keys.size(); ++i) {
            out[i] = key(cfg, keys[i]);
        }
    }

    static Config seeded(float lo, float hi, uint32_t seed) {
        Config c;
        c.lo = Vec3(lo, lo, lo);
        c.hi = Vec3(hi, hi, hi);
        c.seed = seed;
        return c;
    }
};