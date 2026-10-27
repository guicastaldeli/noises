#pragma once

#include <tick.hpp>
#include <mesh.hpp>

class Terrain {
private:
    Tick& tick;
    Mesh& mesh;

public:
    Terrain(Tick& tick, Mesh& mesh);
    ~Terrain();

    void set();
    void update();
    void render();
    void init();
    
    void cleanup();
};