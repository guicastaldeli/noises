#include <world.hpp>
#include <cake.hpp>

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
 * Set
 */
void World::set() {
    mesh->set(main.device,
            main.commandPool,
            main.graphicsQueue,
            main.physicalDevice,
            main.MAX_FRAMES_IN_FLIGHT);
}

/**
 * Update
 */
void World::update() {
    static float t = 0.0f; //Temporary Tick...
    t += 0.016f;
    mesh->setRotation(1, 0.0f, t, 0.0f);
}

/**
 * 
 * Render
 * 
 */
void World::render() {
    mesh->load(1, "cube");
}

/**
 * 
 * Init
 * 
 */
void World::init() {
    mesh->init();
}

/**
 * Cleanup
 */
void World::cleanup() {
    mesh->cleanup();
}