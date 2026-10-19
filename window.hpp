#pragma once

#include <vulkan/vulkan_raii.hpp>

class Window {
private:
    #define APPLICATION_NAME "Perlins"
    #define ENGINE_NAME "Vulkan"

    GLFWwindow *window = nullptr;
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;

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