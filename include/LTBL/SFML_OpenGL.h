#ifndef LTBL_SFML_OPENGL_H
#define LTBL_SFML_OPENGL_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <string>

namespace ltbl
{
extern bool GlewInitialized;
void InitGlew();
void DrawQuad(sf::Texture &Texture);
}

#endif
