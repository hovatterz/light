#ifndef SHADOWFIN_H
#define SHADOWFIN_H

#include "SFML_OpenGL.h"
#include "Constructs.h"

namespace ltbl
{
class ShadowFin {
 public:
  Vec2f rootPos;
  Vec2f umbra;
  Vec2f penumbra;

  ShadowFin();
  ~ShadowFin();

  void render(float depth);
};
}

#endif
