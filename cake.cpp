#include <cake.hpp>
#include <iostream>
#include <window.hpp>

Main::Main() {
    window = new Window(*this);
    shader = new Shader(*this);
}
Main::~Main() {};

/**
 * Set
 */
void Main::Set() {
    window->set();
}

/**
 * Update
 */
void Main::Update() {
    window->update();
}

/**
 * 
 * Render
 * 
 */
void Main::Render() {
    auto stageInfos = shader->getStageInfos();

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

/**
 * 
 * Init
 * 
 */
void Main::Init() {
    window->init();
}

/**
 * 
 * Run
 * 
 */
void Main::Run() {
    Init();
    Set();

    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();

    shader->run();
    
    Update();
    Cleanup();
}

/**
 * Cleanup
 */
void Main::Cleanup() {
    if(device != VK_NULL_HANDLE) vkDeviceWaitIdle(device);
    shader->cleanup();

    for(auto iv : swapChainImageViews) vkDestroyImageView(device, iv, nullptr);
    swapChainImageViews.clear();
    
    if(swapChain != VK_NULL_HANDLE) vkDestroySwapchainKHR(device, swapChain, nullptr);
    if(device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);
    if(window->surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance, window->surface, nullptr);
    if(enableValitationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    if(instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);

    window->cleanup();
}

/******************
 * 
 * 
 * Main entry point and
 * initialization...
 * 
 * 
 ***********/
int main() {
    try {
        Main app;
        app.Run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}