#ifndef QUADTREEOCCUPANT_H
#define QUADTREEOCCUPANT_H

#include "Constructs.h"

namespace qdt
{
	class QuadTreeNode;
	class QuadTree;

	struct AABB
	{
		// Members
		Vec2f lowerBound;
		Vec2f upperBound;

		// Constructor
		AABB();
		AABB(const Vec2f &bottomLeft, const Vec2f &topRight);

		// Accessors
		Vec2f GetCenter() const;
		Vec2f GetDims() const;
		Vec2f GetLowerBound() const;
		Vec2f GetUpperBound() const;

		// Modifiers
		void SetCenter(const Vec2f &newCenter);
		void IncCenter(const Vec2f &increment);
		void SetDims(const Vec2f &newDims);

		// Utility
		bool Intersects(const AABB &other) const;
		bool Contains(const AABB &other) const;

		// Render the AABB for debugging purposes
		void DebugRender();

		friend struct AABB;
	};

	class QuadTreeOccupant
	{
	private:
		QuadTreeNode* pQuadTreeNode;
		QuadTree* pQuadTree;

	public:
		AABB aabb;

	public:
		QuadTreeOccupant();
		virtual ~QuadTreeOccupant();

		// Call this whenever the AABB is modified or else stuff will break!
		void UpdateTreeStatus();
		void RemoveFromTree();

		friend class QuadTreeNode;
		friend class QuadTree;
	};
}

#endif
