include_directories(.)

set(light_SRC
    main.cpp Constructs.cpp ConvexHull.cpp Light.cpp LightSystem.cpp
    Light_Beam.cpp QuadTree.cpp QuadTreeNode.cpp QuadTreeOccupant.cpp
    SFML_OpenGL.cpp ShadowFin.cpp)

add_executable(light ${light_SRC})
target_link_libraries(light ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

file(COPY "${PROJECT_SOURCE_DIR}/data" DESTINATION ${PROJECT_BINARY_DIR})
