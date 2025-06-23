#include <iostream>
#include "app/factory.h"
#include "adapters/incoming/server.h"

int main() {
    try {
        Factory factory;

        ServerSettings settings{8080};
        Server server{settings, factory, factory.getContext()};
        server.start();
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << '\n';
    }
    return 0;
}