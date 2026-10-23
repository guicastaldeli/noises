#pragma once

#include <mesh.hpp>

class Main;
class World {
private:
    Main& main;
    Mesh& mesh;

public:
    World(Main& main, Mesh& mesh);
    ~World();

    Mesh* getMesh() const;

    void set();
    void update();
    void render();
    void init();
    
    void cleanup();
};