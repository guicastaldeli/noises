#include <world.hpp>

World::World(Main& main) : main(main) {
    mesh = new Mesh(main);
}
World::~World() {
    delete mesh; mesh = nullptr;
}

/**
 * 
 * Render
 * 
 */
void World::render() {
    mesh->load(1, "triangle");
}