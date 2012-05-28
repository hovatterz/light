#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

#include "Light.h"
#include "ConvexHull.h"
#include "ShadowFin.h"
#include "SFML_OpenGL.h"
#include <boost/unordered_set.hpp>
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

	class EmissiveLight :
		public qdt::QuadTreeOccupant
	{
	private:
		sf::Texture* text;

		Vec2f center;
		Vec2f scale;

	public:
		EmissiveLight();

		void SetTexture(sf::Texture* texture);
		void Render();

		void SetCenter(const Vec2f &newCenter);
		void SetScale(const Vec2f &newScale);
		void IncCenter(const Vec2f &increment);

		Vec2f GetCenter();
		Vec2f GetScale();

		void Update();
	};

	class LightSystem
	{
	private:
		sf::RenderWindow* pWin;

		boost::unordered_set<Light*> lights;
	
		boost::unordered_set<EmissiveLight*> emissiveLights;

		boost::unordered_set<ConvexHull*> convexHulls;

		std::vector<Light*> lightsToPreBuild;

		std::auto_ptr<qdt::QuadTree> lightTree;
		std::auto_ptr<qdt::QuadTree> hullTree;
		std::auto_ptr<qdt::QuadTree> emissiveTree;

		sf::RenderTexture renderTexture;
		sf::RenderTexture lightTemp;

		std::vector<ShadowFin> finsToRender;

		sf::Texture softShadowTexture;

		int prebuildTimer;

		void MaskShadow(Light* light, ConvexHull* convexHull, float depth);
		void AddExtraFins(const ConvexHull &hull, ShadowFin* fin, const Light &light, Vec2f &mainUmbra, Vec2f &mainUmbraRoot, int boundryIndex, bool wrapCW);
		void CameraSetup();
		void SetUp(const qdt::AABB &region);

	public:
		sf::View view;
		sf::Color ambientColor;
		bool checkForHullIntersect;

		LightSystem(const qdt::AABB &region, sf::RenderWindow* pRenderWindow);
		~LightSystem();

		// All objects are controller through pointer, but these functions return indices that allow easy removal
		void AddLight(Light* newLight);
		void AddConvexHull(ConvexHull* newConvexHull);
		void AddEmissiveLight(EmissiveLight* newEmissiveLight);

		void RemoveLight(Light* pLight);
		void RemoveConvexHull(ConvexHull* pHull);
		void RemoveEmissiveLight(EmissiveLight* pEmissiveLight);

		void BuildLight(Light* pLight);

		// Clears all lights
		void ClearLights();

		// Clears all hulls
		void ClearConvexHulls();

		void ClearEmissiveLights();

		// Renders lights to the light texture
		void RenderLights();

		void RenderLightTexture(float renderDepth = 1.0f);
	};
}

#endif
