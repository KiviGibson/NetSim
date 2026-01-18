cmake_minimum_required(VERSION 3.13)
project(implementation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include_directories(
        include
        test
)
set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/probability_generator.cpp
        src/receiver_preferences.cpp
        src/package_sender.cpp
        src/ramp.cpp
        src/nodes.cpp
        src/factory.cpp
        src/IO.cpp
        )
set(TEST_INCLUDE_FILES
        test/tests.cpp
        test/receiver_tests.cpp
        test/package_sender_tests.cpp
        test/ramp_tests.cpp
        test/factory_tests.cpp
        )
enable_testing()

add_executable(NetSim ${SOURCE_FILES} ${TEST_INCLUDE_FILES} main.cpp)

target_link_libraries(NetSim GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(NetSim)


