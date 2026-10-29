#pragma once

#include <noise.hpp>
#include <chunk.hpp>
#include <heightmap.hpp>
#include <cstdint>

class Terrain;
class Data {
private:
    Terrain& terrain;

public:
    Data(Terrain& terrain);
    ~Data();

    NoiseParams noise;
    Heightmap heightmap;
    ChunkParams chunk;
    ChunkData chunkData;

    bool hasTerrain = false;
    bool collision = false;

    int chunksX = 1;
    int chunksZ = 1;

    void loadTerrain();
    void init();

    const Heightmap& getHeightmap() const { return heightmap; }
    const ChunkData& getChunkData() const { return chunkData; }
};