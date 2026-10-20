#include <cake.hpp>
#include <iostream>

Main::Main() :
    window(*this)
{};
Main::~Main() {};

/**
 * Set
 */
void Main::Set() {
    window.set();
}

/**
 * Update
 */
void Main::Update() {
    window.update();
}

/**
 * Init
 */
void Main::Init() {
    window.init();
}

/**
 * 
 * Run
 * 
 */
void Main::Run() {
    Init();
    Set();
    Update();
    Cleanup();
}

/**
 * Cleanup
 */
void Main::Cleanup() {
    if(enableValitationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    if(instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    window.cleanup();
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