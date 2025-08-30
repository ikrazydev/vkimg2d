#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "app.hpp"

int main() {
    VkImg2DApp app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
