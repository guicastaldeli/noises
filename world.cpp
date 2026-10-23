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
    float deltaTime = tick.GetDeltaTime() * 0.001f;
    mesh.setRotation(1, 0.0f, deltaTime, 0.0f);
    mesh.setRotation(2, deltaTime, 0.0f, 0.0f);
    mesh.setRotation(3, 0.0f, 0.0f, deltaTime);
}

/**
 * 
 * Render
 * 
 */
void World::render() {
    mesh.setPosition(1, -1.5f, 0.0f, 0.0f);
    mesh.load(1, "cube");

    mesh.setPosition(2,  0.0f, 0.0f, 0.0f);
    mesh.load(2, "cube");

    mesh.setPosition(3, 1.5f, 0.0f, 0.0f);
    mesh.load(3, "cube");

    if(tick.ShouldTick()) {
        std::cout << "Draw calls: " << mesh.getDrawCalls() << std::endl;
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