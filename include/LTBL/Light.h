#ifndef LIGHT_H
#define LIGHT_H

#include "SFML_OpenGL.h"
#include "Constructs.h"
#include "QuadTree.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace ltbl
{
const float LightSubdivisionSize = static_cast<float>(M_PI) / 24.0f;

class LightSystem;

class Light : public qdt::QuadTreeOccupant {
 private:
  int numSubdivisions_;

  sf::RenderTexture* pStaticTexture;

  bool alwaysUpdate_;

  sf::RenderWindow* pWin;

 public:
  bool updateRequired;

  float intensity;
  float radius;
  float size;
  Vec2f center;

  float directionAngle;
  float spreadAngle;
  float softSpreadAngle;

  Color3f color;

  Light();
  ~Light();

  virtual void renderLightSolidPortion(float depth);
  virtual void renderLightSoftPortion(float depth);
  virtual void calculateAABB();
  qdt::AABB* getAABB();

  bool alwaysUpdate();
  void setAlwaysUpdate(bool always);

  friend class LightSystem;
};
}

#endif
