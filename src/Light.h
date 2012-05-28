#ifndef LIGHT_H
#define LIGHT_H

#include "SFML_OpenGL.h"
#include "Constructs.h"
#include "QuadTree.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace ltbl
{
	const float lightSubdivisionSize = static_cast<float>(M_PI) / 24.0f;

	class LightSystem;

	class Light :
		public qdt::QuadTreeOccupant
	{
	private:
		int numSubdivisions;

		sf::RenderTexture* pStaticTexture;

		bool alwaysUpdate;

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

		virtual void RenderLightSolidPortion(float depth);
		virtual void RenderLightSoftPortion(float depth);
		virtual void CalculateAABB();
		qdt::AABB* GetAABB();

		bool AlwaysUpdate();
		void SetAlwaysUpdate(bool always);

		friend class LightSystem;
	};
}

#endif
