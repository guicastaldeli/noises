#pragma once
#include <string>
#include <cstdint>
#include <utils/vec3.hpp>

struct Color {
    /******************
     * Convert Hex to Vec3
     *****/
    static Vec3 HexToVec3(const std::string& hex) {
        std::string h = hex;
        if(!h.empty() && h[0] == '#') h = h.substr(1);

        if(h.length() == 3 || h.length() == 4) {
            std::string expanded;
            for(char c : h) {
                expanded += c;
                expanded += c;
            }
            h = expanded;
        }
        if(h.length() < 6) {
            return Vec3(1, 1, 1);
        }

        uint32_t r = std::stoul(h.substr(0, 2), nullptr, 16);
        uint32_t g = std::stoul(h.substr(2, 2), nullptr, 16);
        uint32_t b = std::stoul(h.substr(4, 2), nullptr, 16);

        return Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    } 
};