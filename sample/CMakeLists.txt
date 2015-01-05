include_directories(.)
#include_directories(../light)

set(sample_SRC
    main.cpp)

add_executable(sample ${sample_SRC})
target_link_libraries(sample ltbl)
target_link_libraries(sample ${SFML_LIBRARIES} ${GLEW_LIBRARY} ${SFML_DEPENDENCIES} ${OPENGL_LIBRARIES})

file(COPY "${PROJECT_SOURCE_DIR}/data" DESTINATION ${PROJECT_BINARY_DIR})
