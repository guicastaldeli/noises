#pragma once

#include <tick.hpp>
#include <mesh.hpp>

class World {
private:
    Tick& tick;
    Mesh& mesh;

public:
    World(Tick& tick, Mesh& mesh);
    ~World();

    void set();
    void update();
    void render();
    void init();
    
    void cleanup();
};