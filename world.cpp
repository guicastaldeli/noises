#include <world.hpp>

World::World(Main& main) : main(main) {
    mesh = new Mesh(main);
}
World::~World() {
    delete mesh; mesh = nullptr;
}

/**
 * Get Mesh
 */
Mesh* World::getMesh() const {
    return mesh;
}

/**
 * 
 * Render
 * 
 */
void World::render() {
    mesh->load(1, "triangle");
}

/**
 * Cleanup
 */
void World::cleanup() {
    mesh->cleanup();
}