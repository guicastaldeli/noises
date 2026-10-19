#pragma once

class Window {
private:
    #define APPLICATION_NAME "Perlins"
    #define ENGINE_NAME "Vulkan"
    
public:
    Window();
    ~Window();

    void set();
    void init();
};