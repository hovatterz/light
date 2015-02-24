#ifndef LTBL_QUAD_TREE_NODE_H
#define LTBL_QUAD_TREE_NODE_H

#include "QuadTreeOccupant.h"
#include <unordered_set>
#include <vector>

namespace qdt
{
const unsigned int MaximumOccupants = 3;
const unsigned int MinimumOccupants = 1;
const float OversizedMultiplier = 1.2f;

const unsigned int MaxLevels = 20;

class QuadTree;

class QuadTreeNode {
 private:
  AABB region;
  Vec2f center;

  QuadTreeNode* pParentNode;
  QuadTree* pQuadTree;

  std::unordered_set<QuadTreeOccupant*> occupants;

  QuadTreeNode* children[2][2];
  bool hasChildren;

  unsigned int numOccupants;

  unsigned int level;

  void partition();

  void destroyChildren();

  void merge();
  void getOccupants(std::unordered_set<QuadTreeOccupant*> &upperOccupants, QuadTreeNode* newNode);
  void getOccupants(std::vector<QuadTreeOccupant*> &queryResult);
  Point2i getPossibleOccupantPos(QuadTreeOccupant* pOc);

 public:
  QuadTreeNode(const AABB &newRegion, unsigned int numLevels, QuadTreeNode* pParent = NULL, QuadTree* pContainer = NULL);
  ~QuadTreeNode();

  void addOccupant(QuadTreeOccupant* pOc);
  void query(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult);
  void queryToDepth(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult, int depth);

  void debugRender();
  
  friend class QuadTreeOccupant;
  friend class QuadTree;
};
}

#endif
