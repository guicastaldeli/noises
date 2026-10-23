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
}

/**
 * 
 * Render
 * 
 */
void World::render() {
    mesh.load(1, "cube");
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