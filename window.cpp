#include <window.hpp>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_raii.hpp>

Window::Window() {};
Window::~Window() {};

/**
 * Set
 */
void Window::set() {
    if(enableValitationLayers && !checkValidationLayerSupport) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.pApplicationName                = APPLICATION_NAME;
    appInfo.applicationVersion              = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName                     = ENGINE_NAME;
    appInfo.engineVersion                   = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion                      = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                        = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo             = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount        = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames      = extensions.data;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValitationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }
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
    if(enableValitationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    if(instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}