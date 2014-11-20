#ifndef LTBL_LIGHT_H
#define LTBL_LIGHT_H

#include "SFML_OpenGL.h"
#include "Constructs.h"
#include "QuadTree.h"

const double PI = 3.14159265359;

namespace ltbl
{
const float LightSubdivisionSize = static_cast<float>(PI) / 24.0f;

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
