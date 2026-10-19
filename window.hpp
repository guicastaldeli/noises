#pragma once

class Window {
private:
    #define APPLICATION_NAME "Perlins"
    #define ENGINE_NAME "Vulkan"

    GLFWwindow *window = nullptr;
    
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

public:
    Window();
    ~Window();

    void set();
    void update();
    void init();
    void cleanup();
};