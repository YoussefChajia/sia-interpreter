cmake_minimum_required(VERSION 3.16)
project(sia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -ggdb")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# new lines
set(SIA_LIB "${PROJECT_NAME}_lib")
add_library(SIA_LIB STATIC ${SOURCES})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} SIA_LIB)

# add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
set(SIA_TEST "${PROJECT_NAME}_test")
add_executable(${SIA_TEST} ${TEST_SOURCES})

target_link_libraries(${SIA_TEST} PRIVATE 
    GTest::gtest_main
    SIA_LIB
)
target_include_directories(${SIA_TEST} PRIVATE 
    ${PROJECT_SOURCE_DIR}/src 
)

include(GoogleTest)
gtest_discover_tests(${SIA_TEST})
