#pragma once

#include <tick.hpp>
#include <mesh.hpp>
#include <chunk.hpp>

class Terrain {
private:
    Tick& tick;
    Mesh& mesh;

public:
    Terrain(Tick& tick, Mesh& mesh);
    ~Terrain();

    void set();
    void upload(const ChunkData& chunkData);
    void update();
    void render();
    void init();
    
    void cleanup();
};