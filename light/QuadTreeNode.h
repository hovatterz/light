#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#include "QuadTreeOccupant.h"
#include <vector>
#include <boost/unordered_set.hpp>

namespace qdt
{
	const unsigned int maximumOccupants = 3;
	const unsigned int minimumOccupants = 1;
	const float oversizedMultiplier = 1.2f;

	const unsigned int maxLevels = 20;

	class QuadTree;

	class QuadTreeNode
	{
	private:
		AABB region;
		Vec2f center;

		QuadTreeNode* pParentNode;
		QuadTree* pQuadTree;

		boost::unordered_set<QuadTreeOccupant*> occupants;

		QuadTreeNode* children[2][2];
		bool hasChildren;

		unsigned int numOccupants;

		unsigned int level;

		void Partition();

		void DestroyChildren();

		void Merge();
		void GetOccupants(boost::unordered_set<QuadTreeOccupant*> &upperOccupants, QuadTreeNode* newNode);
		void GetOccupants(std::vector<QuadTreeOccupant*> &queryResult);
		Point2i GetPossibleOccupantPos(QuadTreeOccupant* pOc);

	public:
		QuadTreeNode(const AABB &newRegion, unsigned int numLevels, QuadTreeNode* pParent = NULL, QuadTree* pContainer = NULL);
		~QuadTreeNode();

		void AddOccupant(QuadTreeOccupant* pOc);
		void Query(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult);
		void QueryToDepth(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult, int depth);

		void DebugRender();

		friend class QuadTreeNode;
		friend class QuadTreeOccupant;
		friend class QuadTree;
	};
}

#endif
