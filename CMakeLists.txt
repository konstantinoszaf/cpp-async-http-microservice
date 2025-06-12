cmake_minimum_required(VERSION 3.14)
project(URLShortener LANGUAGES CXX)

# ───────────────────────────────────────────────────────────────────────
# C++ standard
set(CMAKE_CXX_STANDARD      20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ───────────────────────────────────────────────────────────────────────
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

# ───────────────────────────────────────────────────────────────────────
# Executable
add_executable(URLShortener
    src/proof.cpp
    src/session.cpp
    src/router.cpp
    src/handler/shortly_handler.cpp
    src/validator/json_validator.cpp
    src/validator/key_count_rule.cpp
    src/validator/key_rule.cpp
    src/parser/json_parser.cpp
)

target_include_directories(URLShortener
  PRIVATE
    ${CMAKE_SOURCE_DIR}/includes
)

target_link_libraries(URLShortener PRIVATE
  Boost::system
  Boost::thread
  Boost::date_time
  Boost::chrono
  Boost::atomic
  Boost::json
)

option(BUILD_TESTS "Enable building of unit tests" OFF)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()