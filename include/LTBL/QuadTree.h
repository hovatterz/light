/*     _____   _    _      ___      _____    _______   ______    ______   ______
 *    /  _  \ | |  | |    / _ \    |  __ \  |__   __| |  __  \  |  ____| |  ____|
 *    | | | | | |  | |   / /_\ \   | |  | |    | |    | |__| |  |  ____| |  ____|
 *    | |_| | | |__| |  / _____ \  | |__| |    | |    |  __  /  | |____  | |____
 *    \__  _/  \____/  /_/     \_\ |_____/     |_|    |_|  \_\  |______| |______|
 *       \_\
 *
 *		Eric Laukien
 */

#ifndef LTBL_QUAD_TREE_H
#define LTBL_QUAD_TREE_H

#include "QuadTreeNode.h"
#include "QuadTreeOccupant.h"

#include <unordered_set>

namespace qdt
{
class QuadTree {
 private:
  std::unordered_set<QuadTreeOccupant*> outsideRoot;

  QuadTreeNode* rootNode;

 public:
  QuadTree(const AABB &startRegion);
  ~QuadTree();

  void addOccupant(QuadTreeOccupant* pOc);
  void clearTree(const AABB &newStartRegion);

  void query(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult);
  void queryToDepth(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult, int depth);

  unsigned int getNumOccupants();

  AABB getRootAABB();

  void debugRender();

  friend class QuadTreeNode;
  friend class QuadTreeOccupant;
};
}

#endif
