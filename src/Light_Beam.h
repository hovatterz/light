#ifndef LIGHT_BEAM_H
#define LIGHT_BEAM_H

#include "Light.h"

namespace ltbl
{
	class Light_Beam :
		public Light
	{
	private:
		Vec2f innerPoint1, innerPoint2, outerPoint1, outerPoint2;
	public:
		float width;

		Light_Beam();
		~Light_Beam();

		void SetCenter(const Vec2f &newCenter);
		void UpdateDirectionAngle();

		// Overloaded from Light
		void RenderLightSolidPortion(float depth);
		void RenderLightSoftPortion(float depth);
		void CalculateAABB();
	};
}

#endif