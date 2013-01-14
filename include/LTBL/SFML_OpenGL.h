#ifndef SFML_OPENGL_H
#define SFML_OPENGL_H

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <string>

namespace ltbl
{
	extern bool glewInitialized;
	void InitGlew();
	void DrawQuad(sf::Texture &Texture);
}

#endif
