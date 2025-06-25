cmake_minimum_required(VERSION 3.14)
project(URLShortener LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD      20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External deps
find_package(OpenSSL REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(Boost 1.83 REQUIRED
  COMPONENTS
    system
    thread
    date_time
    chrono
    atomic
    json
)

# -----------------------------------------------------------------------------
# Core (pure domain & use-cases, no external deps)
# -----------------------------------------------------------------------------
add_library(core STATIC
    src/core/usecases/shortly_handler.cpp
)
target_link_libraries(core PUBLIC
    Boost::json
)
target_include_directories(core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

# -----------------------------------------------------------------------------
# Adapters (incoming & outgoing translation layers)
# -----------------------------------------------------------------------------
add_library(adapters STATIC
    src/adapters/incoming/session.cpp
    src/adapters/incoming/router.cpp
    src/adapters/incoming/server.cpp
    src/adapters/outgoing/boost_http_adapter.cpp
)
target_link_libraries(adapters
  PUBLIC
    core
  PRIVATE
    Boost::thread
    Boost::json
)
target_include_directories(adapters
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

# -----------------------------------------------------------------------------
# Infrastructure (concrete implementations of ports)
# -----------------------------------------------------------------------------
add_library(infra STATIC
    src/infra/parser/json_parser.cpp
    src/infra/validator/json_validator.cpp
    src/infra/validator/key_rule.cpp
    src/infra/validator/key_count_rule.cpp
    src/infra/provider/provider.cpp
    src/infra/provider/bitly.cpp
    src/infra/provider/tinyurl.cpp
    src/infra/client/http_client.cpp
    src/infra/factory/provider_factory.cpp
    src/infra/network/dns_cache.cpp
    src/infra/network/dns_resolver.cpp
    src/infra/cache/redis_client.cpp
)
target_link_libraries(infra
  PUBLIC
    core
  PRIVATE
    Boost::json
    OpenSSL::SSL
    OpenSSL::Crypto
    redis++::redis++_static
)
target_include_directories(infra
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

# -----------------------------------------------------------------------------
# Application (composition root)
# -----------------------------------------------------------------------------
add_executable(URLShortener
    src/app/main.cpp
    src/app/factory.cpp
)
target_link_libraries(URLShortener
  PRIVATE
    adapters
    infra
)

# -----------------------------------------------------------------------------
# Optional tests
# -----------------------------------------------------------------------------
option(BUILD_TESTS "Enable building of unit tests" OFF)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
