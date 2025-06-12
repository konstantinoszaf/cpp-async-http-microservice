cmake_minimum_required(VERSION 3.15)
project(URLShortenerTests)

# bring in Boost so we can compile rule_interface.h
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

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
foreach(test_src IN LISTS TEST_SOURCES)
  get_filename_component(test_name ${test_src} NAME_WE)

  add_executable(${test_name} ${test_src})
  target_include_directories(${test_name}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/includes
      ${CMAKE_SOURCE_DIR}/test/include
      ${CMAKE_SOURCE_DIR}/test/mock
      ${Boost_INCLUDE_DIRS}
  )

  target_link_libraries(${test_name}
    PRIVATE
      gmock_main
      Boost::system
      Boost::thread
      Boost::date_time
      Boost::chrono
      Boost::atomic
      Boost::json
  )

  gtest_discover_tests(${test_name})
endforeach()
