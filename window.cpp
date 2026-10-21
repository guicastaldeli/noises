#include <window.hpp>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cake.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_raii.hpp>

Window::Window(Main& main) : main(main) {};
Window::~Window() {};

/**
 * Set
 */
void Window::set() {
    if(enableValitationLayers && !main.checkValidationLayerSupport()) {
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

    auto extensions = main.getRequiredExtensions();
    createInfo.enabledExtensionCount        = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames      = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValitationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(main.validationLayers.size());
        createInfo.ppEnabledLayerNames = main.validationLayers.data();
        main.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &main.instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }

    createSurface(main.instance);
    main.setupDebugMessenger();
}

/**
 * Update
 */
void Window::update() {
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        main.drawFrame();
    }

    vkDeviceWaitIdle(main.device);
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