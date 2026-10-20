#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>

#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>

class Main;
class Window {
private:
    #define APPLICATION_NAME "Perlins"
    #define ENGINE_NAME "Vulkan"

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    Main& main;

    /*******
     * Surface
     *****/
    void createSurface(VkInstance instance) {
        if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

public:
    Window(Main& main);
    ~Window();

    GLFWwindow *window = nullptr;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    
    void set();
    void update();
    void init();
    void cleanup();
};