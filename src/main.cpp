#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <app.hpp>

int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "[[EXCEPTION OCCURRED]]\n";
        std::cerr << e.what() << "\n";

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
