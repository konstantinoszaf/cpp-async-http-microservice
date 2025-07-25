cmake_minimum_required(VERSION 3.15)
project(URLShortenerTests)

add_compile_options(-fsanitize=undefined)
add_link_options(-fsanitize=undefined)

find_package(Boost 1.83 REQUIRED
  COMPONENTS
    system
    thread
    date_time
    chrono
    atomic
    json
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

file(GLOB UNIT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/unit/*.cpp")
file(GLOB INT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/*.cpp")
set(TEST_SOURCES
    ${UNIT_TEST_SOURCES}
    ${INT_TEST_SOURCES}
)
foreach(test_src IN LISTS TEST_SOURCES)
  get_filename_component(test_name ${test_src} NAME_WE)

  add_executable(${test_name} ${test_src})
  target_include_directories(${test_name}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/test/include
      ${CMAKE_SOURCE_DIR}/test/mock
  )

  target_link_libraries(${test_name}
    PRIVATE
      gmock_main
      infra
      Boost::json
      Boost::system
  )

  gtest_discover_tests(${test_name})
endforeach()
