#pragma once

#include <cstdint>
#include <vector>
#include <heightmap.hpp>

/************* 
 * 
 * Chunk Type
 * 
 * ******/
enum class ChunkType {
    Full,
    Flat,
    Wave,
    Heightmap,
    Cheese,
    Cave
};

/*******************
 * 
 * Chunk Params
 * 
 */
struct ChunkParams {
    ChunkType type  = ChunkType::Flat;
    int sizeX       = 16;
    int sizeY       = 64;
    int sizeZ       = 16;

    // chunk type-specific
    float waveAmplitide = 4.0f;
    float waveFrequency = 0.5f;
    float cheeseDensity = 0.3f;
    float caveThreshold = 2.0f;

    // material ids (0 = air)
    uint8_t matSolid = 1;
    uint8_t matSurface = 3;
};

/*********** 
 *
 * Chunk Buffer 
 * 
 ******/
struct ChunkBuffer {
    int sx = 0, sy = 0, sz = 0;
    std::vector<uint8_t> meshes;

    uint8_t& at(int x, int y, int z) {
        return meshes[std::size_t(x) + std::size_t(z) * 
                    std::size_t(sx) + std::size_t(y) *
                    std::size_t(y) * 
                    std::size_t(sx) *
                    std::size_t(sx) *
                    std::size_t(sz)];
    }
    uint8_t at(int x, int y, int z) const {
        return meshes[std::size_t(x) + std::size_t(z) *
                    std::size_t(sx) + std::size_t(y) *
                    std::size_t(y) *
                    std::size_t(sx) *
                    std::size_t(sz)];
    }
};

/******************
 * 
 * Chunk methods.
 * 
 ******/
class Chunk {
public:
    // heightmap + params -> chunk buffer.
    ChunkBuffer generateChunk(const ChunkParams& p, const Heightmap& heightmap);
};