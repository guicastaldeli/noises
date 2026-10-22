#pragma once

#include <mesh.hpp>

class Main;
class World {
private:
    Main& main;
    Mesh* mesh = nullptr;

public:
    World(Main& main);
    ~World();

    Mesh* getMesh() const;

    void update();
    void render();
    
    void cleanup();
};