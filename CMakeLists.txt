cmake_minimum_required(VERSION 1.8)

project(light)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/cmake_modules" ${CMAKE_MODULE_PATH})

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})

# Find SFML
find_package(SFML 2.0 REQUIRED system window graphics network audio)

# Project
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(light_SRC
    src/Constructs.cpp
    src/ConvexHull.cpp
    src/Light.cpp
    src/LightSystem.cpp
    src/Light_Beam.cpp
    src/QuadTree.cpp
    src/QuadTreeNode.cpp
    src/QuadTreeOccupant.cpp
    src/SFML_OpenGL.cpp
    src/ShadowFin.cpp)
include_directories("include")

add_library(ltbl SHARED ${light_SRC})
target_link_libraries(ltbl ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
install(
    TARGETS ltbl
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin
    ARCHIVE DESTINATION lib COMPONENT dev
)
install(DIRECTORY include DESTINATION .)

# Sample
add_subdirectory(sample bin)
