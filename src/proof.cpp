#include <iostream>
#include <server.h>

int main() {
    try {
        ServerSettings settings{8080};
        Server server{settings};
        server.start();
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << '\n';
    }
    return 0;
}