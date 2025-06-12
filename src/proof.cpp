#include <iostream>
#include "server/server.h"
#include "router/router.h"
#include "handler/shortly_handler.h"
#include "session/session.h"

int main() {
    try {
        SessionFactory factory = [&](tcp::socket sock, std::shared_ptr<IRouter> rtr) {
            return std::make_shared<Session>(std::move(sock), rtr);
        };

        auto router = std::make_shared<Router>();
        router->add_route(http::verb::post, "/shortly", std::make_shared<ShortlyHandler>());
        ServerSettings settings{8080, factory};
        Server server{settings, router};
        server.start();
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << '\n';
    }
    return 0;
}