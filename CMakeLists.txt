cmake_minimum_required(VERSION 1.8)

project(light)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/cmake_modules" ${CMAKE_MODULE_PATH})

# Find Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})

# Find SFML
find_package(SFML 2.0 REQUIRED system window graphics network audio)

add_subdirectory(light lib)
add_subdirectory(sample bin)
