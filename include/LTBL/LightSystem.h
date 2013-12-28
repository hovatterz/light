#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

#include "Light.h"
#include "ConvexHull.h"
#include "ShadowFin.h"
#include "SFML_OpenGL.h"
#include <unordered_set>
#include <vector>
#include <memory>

namespace ltbl
{
// The radius value of a light is larger than what one actually 
// sees in the render of the light. Therefore, this multiplier 
// can be used to "cut down" the radius a bit more for more culling.
const float lightRadiusCullMultiplier = 1.0f;
const float renderDepth = 50.0f;

const int maxFins = 2;

class EmissiveLight : public qdt::QuadTreeOccupant {
 private:
  sf::Texture* text;

  Vec2f center;
  Vec2f scale;

 public:
  EmissiveLight();

  void setTexture(sf::Texture* texture);
  void render();

  void setCenter(const Vec2f &newCenter);
  void setScale(const Vec2f &newScale);
  void incCenter(const Vec2f &increment);

  Vec2f getCenter();
  Vec2f getScale();

  void update();
};

class LightSystem
{
 private:
  sf::RenderWindow* pWin;

  std::unordered_set<Light*> lights;
  std::unordered_set<EmissiveLight*> emissiveLights;
  std::unordered_set<ConvexHull*> convexHulls;

  std::vector<Light*> lightsToPreBuild;

  std::auto_ptr<qdt::QuadTree> lightTree;
  std::auto_ptr<qdt::QuadTree> hullTree;
  std::auto_ptr<qdt::QuadTree> emissiveTree;

  sf::RenderTexture renderTexture;
  sf::RenderTexture lightTemp;

  std::vector<ShadowFin> finsToRender;

  sf::Texture softShadowTexture;

  int prebuildTimer;

  void maskShadow(Light* light, ConvexHull* convexHull, float depth);
  void addExtraFins(const ConvexHull &hull, ShadowFin* fin, const Light &light, Vec2f &mainUmbra, Vec2f &mainUmbraRoot, int boundryIndex, bool wrapCW);
  void cameraSetup();
  void setUp(const qdt::AABB &region);

 public:
  sf::View view;
  sf::Color ambientColor;
  bool checkForHullIntersect;

  LightSystem(const qdt::AABB &region, sf::RenderWindow* pRenderWindow);
  ~LightSystem();

  // All objects are controller through pointer, but these functions return indices that allow easy removal
  void addLight(Light* newLight);
  void addConvexHull(ConvexHull* newConvexHull);
  void addEmissiveLight(EmissiveLight* newEmissiveLight);

  void removeLight(Light* pLight);
  void removeConvexHull(ConvexHull* pHull);
  void removeEmissiveLight(EmissiveLight* pEmissiveLight);

  void buildLight(Light* pLight);

  // Clears all lights
  void clearLights();

  // Clears all hulls
  void clearConvexHulls();

  void clearEmissiveLights();

  // Renders lights to the light texture
  void renderLights();

  void renderLightTexture(float renderDepth = 1.0f);
};
}

#endif
