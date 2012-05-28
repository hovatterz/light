/*     _____   _    _      ___      _____    _______   ______    ______   ______
 *    /  _  \ | |  | |    / _ \    |  __ \  |__   __| |  __  \  |  ____| |  ____|
 *    | | | | | |  | |   / /_\ \   | |  | |    | |    | |__| |  |  ____| |  ____|
 *    | |_| | | |__| |  / _____ \  | |__| |    | |    |  __  /  | |____  | |____
 *    \__  _/  \____/  /_/     \_\ |_____/     |_|    |_|  \_\  |______| |______|
 *       \_\
 *
 *		Eric Laukien
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include "QuadTreeNode.h"
#include "QuadTreeOccupant.h"

#include <boost/unordered_set.hpp>

namespace qdt
{
	class QuadTree
	{
	private:
		boost::unordered_set<QuadTreeOccupant*> outsideRoot;

		QuadTreeNode* rootNode;

	public:
		QuadTree(const AABB &startRegion);
		~QuadTree();

		void AddOccupant(QuadTreeOccupant* pOc);
		void ClearTree(const AABB &newStartRegion);

		void Query(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult);
		void QueryToDepth(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult, int depth);

		unsigned int GetNumOccupants();

		AABB GetRootAABB();

		void DebugRender();

		friend class QuadTreeNode;
		friend class QuadTreeOccupant;
	};
}

#endif
