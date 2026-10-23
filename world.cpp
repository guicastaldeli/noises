#include <world.hpp>
#include <iostream>

World::World(Tick& tick, Mesh& mesh) : 
    tick(tick),
    mesh(mesh) {}
World::~World() {};

/**
 * Set
 */
void World::set() {
    
}

/**
 * Update
 */
void World::update() {
    if(tick.ShouldTick()) {
        std::cout << "Draw calls: " << mesh.getDrawCalls() << std::endl;
    }
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
void World::render() {
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
void World::init() {
    
}

/**
 * Cleanup
 */
void World::cleanup() {
    
}