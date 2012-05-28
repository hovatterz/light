set(light_SRC
    Constructs.cpp ConvexHull.cpp Light.cpp LightSystem.cpp
    Light_Beam.cpp QuadTree.cpp QuadTreeNode.cpp QuadTreeOccupant.cpp
    SFML_OpenGL.cpp ShadowFin.cpp)

add_library(light SHARED ${light_SRC})
target_link_libraries(light ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
