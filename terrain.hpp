#pragma once

#include <vector>
#include <string>
#include <tick.hpp>
#include <mesh.hpp>
#include <chunk.hpp>
#include <collision.hpp>
#include <utils/vec3.hpp>
#include <utils/idgen.hpp>

/*************************** 
 * 
 * Skybox
 * 
 * *****************/
class Terrain;
struct Skybox {
    inline static uint32_t MESH_ID = 0;
    inline static constexpr const char* MESH_TYPE = "cube";
    inline static const char* ids = "skybox"; 

    inline static constexpr const char* TEX_PATH = "skybox.jpg";

    inline static Mesh* mesh = nullptr;
    inline static Terrain* terrain = nullptr;

    inline static float posX = 0.0f;
    inline static float posY = 0.0f;
    inline static float posZ = 0.0f;

    inline static float scaleX = MAX_CHUNK_X_SIZE * 10.0f;
    inline static float scaleY = MAX_CHUNK_Y_SIZE * 10.0f;
    inline static float scaleZ = MAX_CHUNK_Z_SIZE * 10.0f;

    inline static void render();
    inline static void set();
    inline static void init(Mesh* m, Terrain* t);
};

/*****************************************
 * 
 * Terrain
 * 
 ******************************/
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

    Mesh& getMesh() const;

    void upload(const ChunkData& chunkData);

    void update();
    void updateBoundary(const ChunkData& chunkData);
    
    void render();
    void init();
    
    void cleanup();
};