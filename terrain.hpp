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

    Vec3 origin = Vec3(0, 0, 0);
    const ChunkData* lastChunk = nullptr;

    BoundaryCollider boundary{ 100.0f };
    inline static const float BOUNDARY_MULTIPLIER = 8.0f;

    inline static constexpr const char* MESH_TYPE = "cube";
    const char* ids = "terrain";
    std::vector<uint32_t> spawnedIds;

public:
    Terrain(Tick& tick, Mesh& mesh, CollisionManager& collision);
    ~Terrain();

    bool trackCollision = true;

    void upload(const ChunkData& chunkData);

    void update();
    void updateBoundary(const ChunkData& chunkData);
    
    void render();
    
    void cleanup();
};