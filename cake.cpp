#include <cake.hpp>
#include <iostream>

Main::Main() {};
Main::~Main() {};

/**
 * Update
 */
void Main::Update() {

}

/**
 * Init
 */
void Main::Init() {

}

/**
 * 
 * Run
 * 
 */
void Main::Run() {
    Init();
    Update();
    Cleanup();
}

/**
 * Cleanup
 */
void Main::Cleanup() {
    
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