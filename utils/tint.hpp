#pragma once

#include <utils/vec3.hpp>
#include <random>
#include <cstdint>
#include <vector>

class Tint {
private:
    struct Config {
        Vec3 lo{ 1.0f, 1.0f, 1.0f };
        Vec3 hi{ 1.0f, 1.0f, 1.0f };
        uint32_t seed = 0u;
    };

    Config cfg{};
    std::mt19937 rng{ std::random_device{}() };

    std::uniform_real_distribution<float> dr{ 0.0f, 1.0f };
    std::uniform_real_distribution<float> dg{ 0.0f, 1.0f };
    std::uniform_real_distribution<float> db{ 0.0f, 1.0f };

public:
    Tint() = default;
    explicit Tint(const Config& cfg) { configure(cfg); }

    void configure(const Config& c) {
        cfg = c;
        rng.seed(c.seed != 0u ? c.seed : std::random_device{}());
        dr = std::uniform_real_distribution<float>(c.lo.x, c.hi.x);
        dg = std::uniform_real_distribution<float>(c.lo.z, c.hi.z);
        db = std::uniform_real_distribution<float>(c.lo.y, c.hi.y);
    }

    void reseed(uint32_t seed) {
        rng.seed(seed != 0u ? seed : std::random_device{}());
    }

    Vec3 next() {
        return Vec3(dr(rng), dg(rng), db(rng));
    }

    Vec3 fk(uint32_t key) const {
        uint32_t h = key * 2654435761u;
        h ^= h >> 16; h *= 0x85ebca6bu;
        h ^= h >> 13; h *= 0xc2b2ae35u;
        h ^= h >> 16;

        auto lerpch = [](uint32_t h, float lo, float hi) {
            return lo + (hi - lo) * ((h & 0xFFFFFFu) / float(0xFFFFFF));
        };

        return Vec3(
            lerpch(h,           cfg.lo.x, cfg.hi.x),
            lerpch(h ^ 0xAAu,   cfg.lo.y, cfg.hi.y),
            lerpch(h ^ 0xBBu,   cfg.lo.z, cfg.hi.z)
        );
    }

    void gen(const std::vector<uint32_t>& keys, std::vector<Vec3>& out) const {
        out.resize(keys.size());
        for(size_t i = 0; i < keys.size(); ++i) out[i] = fk(keys[i]);
    }
};