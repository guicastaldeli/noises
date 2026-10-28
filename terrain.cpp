#include <terrain.hpp>
#include <iostream>

Terrain::Terrain(Tick& tick, Mesh& mesh) : 
    tick(tick),
    mesh(mesh) {}
Terrain::~Terrain() {};

/**
 * Upload
 */
void Terrain::upload(const ChunkData& chunkData) {
    
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
    float deltaTime = tick.GetDeltaTime() * 0.001f;
    mesh.setRotation(0, 0.0f, deltaTime, 0.0f);
    mesh.setRotation(1, deltaTime, 0.0f, 0.0f);
    mesh.setRotation(2, 0.0f, 0.0f, deltaTime);
}

/**
 * 
 * Render
 * 
 */
void Terrain::render() {
    for(int i = 0; i < 10; i++) {
        mesh.setPosition(i, i * -1.5f, 0.0f, 0.0f);
        mesh.load(i, "cube");
    }
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