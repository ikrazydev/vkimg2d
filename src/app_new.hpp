#pragma once

#include <window.hpp>

class App
{
public:
    void run();
private:
    void _initWindow();
    void _initVulkan();

    void _loop();

    Window mWindow;
};
