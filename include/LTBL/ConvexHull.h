#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include "SFML_OpenGL.h"
#include "Constructs.h"
#include "QuadTree.h"
#include <vector>

namespace ltbl
{
	template<class T> T Wrap(T val, T size)
	{
		if((signed)val < 0)
			return val + size;

		if(val >= size)
			return val - size;

		return val;
	}

	struct ConvexHullVertex
	{
		Vec2f position;

		// Other information can be added later
	};

	class ConvexHull :
		public qdt::QuadTreeOccupant
	{	
	private:
		bool aabbGenerated;

		Vec2f worldCenter;

	public:
		bool updateRequired;

		bool render;

		std::vector<ConvexHullVertex> vertices;
		std::vector<Vec2f> normals;

		float shadowDepthOffset;

		ConvexHull();

		void CenterHull();
		bool LoadShape(const char* fileName);
		Vec2f GetWorldVertex(unsigned int index) const;

		void CalculateNormals();

		void RenderHull(float depth);

		void GenerateAABB();

		bool HasGeneratedAABB();

		void SetWorldCenter(const Vec2f &newCenter);
		void IncWorldCenter(const Vec2f &increment);

		Vec2f GetWorldCenter() const;

		bool PointInsideHull(const Vec2f &point);
	};

	float GetFloatVal(std::string strConvert);
}

#endif

