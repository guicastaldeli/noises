#pragma once

#include <cstdint>
#include <vector>
#include <heightmap.hpp>
#include <.lib/enum/enum_reflect.hpp>

#define CHUNK_X_SIZE 30
#define CHUNK_Y_SIZE 1
#define CHUNK_Z_SIZE 30
#define MAX_CHUNK_X_SIZE 128
#define MAX_CHUNK_Y_SIZE 128
#define MAX_CHUNK_Z_SIZE 128

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
template<> inline constexpr int reflect::enum_count<ChunkType> = 6;

/*******************
 * 
 * Chunk Params
 * 
 */
struct ChunkParams {
    ChunkType type  = ChunkType::Heightmap;
    int sizeX       = CHUNK_X_SIZE;
    int sizeY       = CHUNK_Y_SIZE;
    int sizeZ       = CHUNK_Z_SIZE;

    // chunk type-specific
    float waveAmplitude = 4.0f;
    float waveFrequency = 0.5f;
    float cheeseDensity = 0.3f;
    float caveThreshold = 2.0f;

    // material ids (0 = air)
    uint8_t matSolid = 1;
    uint8_t matSurface = 3;
};

/*********** 
 *
 * Chunk Data
 * 
 ******/
struct ChunkData {
    int sx = 0, sy = 0, sz = 0;
    std::vector<uint8_t> meshes;

    uint8_t& at(int x, int y, int z) {
        return meshes[std::size_t(x)
                + std::size_t(z) * std::size_t(sx)
                + std::size_t(y) * std::size_t(sx) * std::size_t(sz)];
    }
    uint8_t at(int x, int y, int z) const {
        return meshes[std::size_t(x)
                + std::size_t(z) * std::size_t(sx)
                + std::size_t(y) * std::size_t(sx) * std::size_t(sz)];
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
    static ChunkData generateChunk(const ChunkParams& p, const Heightmap& heightmap);
};