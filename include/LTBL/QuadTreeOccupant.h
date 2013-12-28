#ifndef QUADTREEOCCUPANT_H
#define QUADTREEOCCUPANT_H

#include "Constructs.h"

namespace qdt
{
class QuadTreeNode;
class QuadTree;

struct AABB {
  // Members
  Vec2f lowerBound;
  Vec2f upperBound;

  // Constructor
  AABB();
  AABB(const Vec2f &bottomLeft, const Vec2f &topRight);

  // Accessors
  Vec2f getCenter() const;
  Vec2f getDims() const;
  Vec2f getLowerBound() const;
  Vec2f getUpperBound() const;

  // Modifiers
  void setCenter(const Vec2f &newCenter);
  void incCenter(const Vec2f &increment);
  void setDims(const Vec2f &newDims);

  // Utility
  bool intersects(const AABB &other) const;
  bool contains(const AABB &other) const;

  // Render the AABB for debugging purposes
  void debugRender();

  friend struct AABB;
};

class QuadTreeOccupant {
 private:
  QuadTreeNode* pQuadTreeNode;
  QuadTree* pQuadTree;

 public:
  AABB aabb;

 public:
  QuadTreeOccupant();
  virtual ~QuadTreeOccupant();

  // Call this whenever the AABB is modified or else stuff will break!
  void updateTreeStatus();
  void removeFromTree();

  friend class QuadTreeNode;
  friend class QuadTree;
};
}

#endif
