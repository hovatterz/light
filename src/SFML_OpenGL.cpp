#include "LTBL/SFML_OpenGL.h"

#include <iostream>

namespace ltbl
{
	bool glewInitialized = false;

	void InitGlew()
	{
		if(glewInitialized)
			return;

		GLenum err = glewInit();

		if(GLEW_OK != err)
		{
			std::cout << "Could not initialize GLEW! Aborting..." << std::endl;
			abort();
		}

		glewInitialized = true;
	}

	void DrawQuad(sf::Texture &Texture)
	{
		float halfWidth = Texture.getSize().x / 2.0f;
		float halfHeight = Texture.getSize().y / 2.0f;

		// Bind the texture
    sf::Texture::bind(&Texture);

		// Have to render upside-down because SFML loads the Textures upside-down
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex3f(-halfWidth, -halfHeight, 0.0f);
			glTexCoord2i(1, 0); glVertex3f(halfWidth, -halfHeight, 0.0f);
			glTexCoord2i(1, 1); glVertex3f(halfWidth, halfHeight, 0.0f);
			glTexCoord2i(0, 1); glVertex3f(-halfWidth, halfHeight, 0.0f);
		glEnd();
	}
}
