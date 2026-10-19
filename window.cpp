#include <window.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_raii.hpp>

Window::Window() {};
Window::~Window() {};

/**
 * Set
 */
void Window::set() {
    VkInstance instance;
    VkApplicationInfo appInfo{};
    appInfo.pApplicationName                = APPLICATION_NAME;
    appInfo.applicationVersion              = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName                     = ENGINE_NAME;
    appInfo.engineVersion                   = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion                      = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                        = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo             = &appInfo;
    createInfo.enabledLayerCount            = 0;
    createInfo.enabledLayerCount            = 0;
    createInfo.ppEnabledExtensionNames      = nullptr;
    createInfo.enabledLayerCount            = 0;
    createInfo.ppEnabledLayerNames          = nullptr;

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    vkDestroyInstance(instance, nullptr);
}

/**
 * Update
 */
void Window::update() {
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

/**
 * 
 * Init
 * 
 */
void Window::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, APPLICATION_NAME, nullptr, nullptr);
}

/**
 * Cleanup
 */
void Window::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}