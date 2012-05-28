#include "LTBL/ShadowFin.h"

using namespace ltbl;

ShadowFin::ShadowFin()
{
}

ShadowFin::~ShadowFin()
{
}

void ShadowFin::Render(float depth)
{
	glBegin(GL_TRIANGLES);
		glTexCoord2i(0, 1); glVertex3f(rootPos.x, rootPos.y, depth);
		glTexCoord2i(0, 0); glVertex3f(rootPos.x + penumbra.x, rootPos.y + penumbra.y, depth);
		glTexCoord2i(1, 0); glVertex3f(rootPos.x + umbra.x, rootPos.y + umbra.y, depth);
	glEnd();
}
