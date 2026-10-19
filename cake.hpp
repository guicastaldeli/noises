#pragma once

#include <window.hpp>

class Main {
public:
    Main();
    ~Main();

    Window window;

    void Set();
    void Update();
    void Init();

    void Cleanup();
    void Run();
};