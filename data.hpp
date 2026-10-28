#pragma once

#include <noise.hpp>
#include <chunk.hpp>
#include <heightmap.hpp>
#include <cstdint>

class Terrain;
class UI;
class Data {
private:
    Terrain& terrain;
    UI& ui;

public:
    Data(Terrain& terrain, UI& ui);
    ~Data();

    NoiseParams noise;
    ChunkParams chunk;
    bool collision = false;

    int chunksX = 1;
    int chunksZ = 1;

    Heightmap heightmap;
    ChunkData chunkData;
    bool hasTerrain = false;

    void init();

    const Heightmap& getHeightmap() const { return heightmap; }
    const ChunkData& getChunkData() const { return chunkData; }
};