#pragma once

#include <cake.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class Main;
class Window {
private:
    #define APPLICATION_NAME "Perlins"
    #define ENGINE_NAME "Vulkan"

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    GLFWwindow *window = nullptr;
    Main& main;

public:
    Window(Main& main);
    ~Window();
    
    void set();
    void update();
    void init();
    void cleanup();
};