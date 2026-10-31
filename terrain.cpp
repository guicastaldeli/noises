#include <terrain.hpp>
#include <iostream>

Terrain::Terrain(
    Tick& tick, 
    Mesh& mesh, 
    CollisionManager& collision
) : 
    tick(tick),
    mesh(mesh),
    collision(collision) {}
Terrain::~Terrain() {};

/**
 * 
 * Upload
 * 
 */
void Terrain::upload(const ChunkData& chunkData) {
    IdGen::reset(ids);
    mesh.clearInstances(spawnedIds);
    spawnedIds.clear();
    if(trackCollision) collision.clearColliders();

    TextureResult tex = mesh.getTextureLoader()->load("test.jpg");
    uint32_t texId = tex.texId;

    for(int z = 0; z < chunkData.sz; ++z)
    for(int y = 0; y < chunkData.sy; ++y)
    for(int x = 0; x < chunkData.sx; ++x) {
        if(chunkData.at(x, y, z) == 0) continue;

        const uint32_t id = static_cast<uint32_t>(IdGen::next(ids));
        spawnedIds.push_back(id);

        mesh.setPosition(id,
                    origin.x + float(x),
                    origin.y + float(y),
                    origin.z + float(z));
        mesh.load(id, MESH_TYPE);
        mesh.setTexture(id, texId);
        if(trackCollision) {
            collision.set(id, CollisionType::STATIC_COLLIDER);
        }
    }

    if(trackCollision) collision.build();
}

/**
 * Set
 */
void Terrain::set() {
    
}

/**
 * Update
 */
void Terrain::update() {
    //if(tick.ShouldTick()) std::cout << "Draw calls: " << mesh.getDrawCalls() << std::endl;
    /*float deltaTime = tick.GetDeltaTime() * 0.001f;
    mesh.setRotation(0, 0.0f, deltaTime, 0.0f);
    mesh.setRotation(1, deltaTime, 0.0f, 0.0f);
    mesh.setRotation(2, 0.0f, 0.0f, deltaTime);
*/}

/**
 * 
 * Render
 * 
 */
void Terrain::render() {
    /*uint32_t texId = mesh.getTextureLoader()->load(1, "test.jpg");
    for(int i = 0; i < 5; i++) {
        mesh.setPosition(i, i * -1.5f, 0.0f, 0.0f);
        mesh.load(i, "cube");
        mesh.setTexture(i, texId);
    }*/
}

/**
 * 
 * Init
 * 
 */
void Terrain::init() {
    
}

/**
 * Cleanup
 */
void Terrain::cleanup() {
    
}