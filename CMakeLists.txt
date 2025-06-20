cmake_minimum_required(VERSION 3.14)
project(URLShortener LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD      20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
# Find Boost (1.83.0 or newer), only the linkable components
find_package(Boost 1.83 REQUIRED
  COMPONENTS
    system
    thread
    date_time
    chrono
    atomic
    json
)

# Core library
add_library(url_service
    src/entrypoint.cpp
    src/session/session.cpp
    src/router/router.cpp
    src/handler/shortly_handler.cpp
    src/validator/json_validator.cpp
    src/validator/key_count_rule.cpp
    src/validator/key_rule.cpp
    src/parser/json_parser.cpp
    src/factory/factory.cpp
    src/factory/provider_factory.cpp
    src/http_adapter/boost_http_adapter.cpp
    src/client/http_client.cpp
    src/provider/provider.cpp
    src/provider/bitly.cpp
    src/provider/tinyurl.cpp
)

target_include_directories(url_service
  PUBLIC
    ${CMAKE_SOURCE_DIR}/includes
)

target_link_libraries(url_service
  PUBLIC
    Boost::system
    Boost::thread
    Boost::date_time
    Boost::chrono
    Boost::atomic
    Boost::json
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_executable(URLShortener
    src/entrypoint.cpp
)

target_link_libraries(URLShortener
  PRIVATE
    url_service
)

option(BUILD_TESTS "Enable building of unit tests" OFF)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()