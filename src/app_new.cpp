#include <app_new.hpp>

void App::run()
{
    _initWindow();
    _initVulkan();

    _loop();
}

void App::_initWindow()
{
    mWindow = Window();
    mWindow.init();
}

void App::_initVulkan()
{
}

void App::_loop()
{
    while (!mWindow.shouldClose()) {
        mWindow.update();
    }
}
