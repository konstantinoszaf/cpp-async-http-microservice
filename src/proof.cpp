#include <iostream>
#include <server.h>
#include "router.h"
#include "tiny_handler.h"

int main() {
    try {
        SessionFactory factory = [&](tcp::socket sock, std::shared_ptr<IRouter> rtr) {
            return std::make_shared<Session>(std::move(sock), rtr);
        };

        auto router = std::make_shared<Router>();
        router->add_route(http::verb::get, "/tiny", std::make_shared<TinyHandler>());
        ServerSettings settings{8080, factory};
        Server server{settings, router};
        server.start();
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << '\n';
    }
    return 0;
}