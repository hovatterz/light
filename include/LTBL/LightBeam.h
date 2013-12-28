#ifndef LTBL_LIGHT_BEAM_H
#define LTBL_LIGHT_BEAM_H

#include "Light.h"

namespace ltbl
{
class LightBeam : public Light {
 private:
  Vec2f innerPoint1, innerPoint2, outerPoint1, outerPoint2;
 public:
  float width;

  LightBeam();
  ~LightBeam();

  void setCenter(const Vec2f &newCenter);
  void updateDirectionAngle();

  // Overloaded from Light
  void renderLightSolidPortion(float depth);
  void renderLightSoftPortion(float depth);
  void calculateAABB();
};
}

#endif
