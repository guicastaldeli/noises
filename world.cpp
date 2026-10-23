#include <world.hpp>
#include <cake.hpp>

World::World(Main& main, Mesh& mesh) : 
    main(main),
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
    static float t = 0.0f; //Temporary Tick...
    t += 0.016f;
    mesh.setRotation(1, 0.0f, t, 0.0f);
    mesh.setRotation(2, t, 0.0f, 0.0f);
    mesh.setRotation(3, 0.0f, 0.0f, t);
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