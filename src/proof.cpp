#include <iostream>
#include "server/server.h"
#include "router/router.h"
#include "handler/shortly_handler.h"
#include "session/session.h"
#include "factory/factory.h"

int main() {
    try {
        Factory factory;

        ServerSettings settings{8080};
        Server server{settings, factory};
        server.start();
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << '\n';
    }
    return 0;
}