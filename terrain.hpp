#pragma once

#include <vector>
#include <string>
#include <tick.hpp>
#include <mesh.hpp>
#include <chunk.hpp>
#include <collision.hpp>
#include <utils/vec3.hpp>
#include <utils/idgen.hpp>

class Terrain {
private:
    Tick& tick;
    Mesh& mesh;
    CollisionManager& collision;

    inline static constexpr const char* MESH_TYPE = "cube";
    const char* ids = "terrain";
    std::vector<uint32_t> spawnedIds;

    Vec3 origin = Vec3(0, 0, 0);

public:
    Terrain(Tick& tick, Mesh& mesh, CollisionManager& collision);
    ~Terrain();

    bool trackCollision = true;

    void set();
    void upload(const ChunkData& chunkData);
    void update();
    void render();
    void init();
    
    void cleanup();
};