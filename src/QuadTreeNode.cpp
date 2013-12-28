#include "LTBL/QuadTreeNode.h"
#include "LTBL/QuadTree.h"
#include <assert.h>

#include "LTBL/SFML_OpenGL.h"

using namespace qdt;

QuadTreeNode::QuadTreeNode(const AABB &newRegion, unsigned int numLevels, QuadTreeNode* pParent, QuadTree* pContainer)
: region(newRegion), hasChildren(false), numOccupants(0),
    pParentNode(pParent), pQuadTree(pContainer), level(numLevels)
{
  center = region.getCenter();
}

QuadTreeNode::~QuadTreeNode()
{
  if(hasChildren)
    destroyChildren();
}

void QuadTreeNode::merge()
{
  // Merge all children into this node
  if(hasChildren)
  {
    for(unsigned int x = 0; x < 2; x++)
      for(unsigned int y = 0; y < 2; y++)
        children[x][y]->getOccupants(occupants, this);

    destroyChildren();
    hasChildren = false;
  }
}

void QuadTreeNode::getOccupants(std::unordered_set<QuadTreeOccupant*> &upperOccupants, QuadTreeNode* newNode)
{
  // Assign the new node pointers while adding the occupants to the upper node
  for(std::unordered_set<QuadTreeOccupant*>::iterator it = occupants.begin(); it != occupants.end(); it++)
  {
    (*it)->pQuadTreeNode = newNode;
    upperOccupants.insert(*it);
  }

  // Recusively go through children if there are any
  if(hasChildren)
    for(unsigned int x = 0; x < 2; x++)
      for(unsigned int y = 0; y < 2; y++)
        children[x][y]->getOccupants(upperOccupants, newNode);
}

void QuadTreeNode::getOccupants(std::vector<QuadTreeOccupant*> &queryResult)
{
  // Assign the new node pointers while adding the occupants to the upper node
  for(std::unordered_set<QuadTreeOccupant*>::iterator it = occupants.begin(); it != occupants.end(); it++)
    queryResult.push_back(*it);

  // Recusively go through children if there are any
  if(hasChildren)
    for(unsigned int x = 0; x < 2; x++)
      for(unsigned int y = 0; y < 2; y++)
        children[x][y]->getOccupants(queryResult);
}

void QuadTreeNode::partition()
{
  // Create the children nodes with the appropriate bounds set
  Vec2f halfWidth = region.getDims() / 2.0f;

  const unsigned int nextLevel = level + 1;

  for(unsigned int x = 0; x < 2; x++)
    for(unsigned int y = 0; y < 2; y++)
    {
      children[x][y] = new QuadTreeNode(AABB(Vec2f(region.lowerBound.x + x * halfWidth.x, region.lowerBound.y + y * halfWidth.y),
                                             Vec2f(center.x + x * halfWidth.x, center.y + y * halfWidth.y)), nextLevel, this, pQuadTree);

      // Oversize
      children[x][y]->region.setDims(children[x][y]->region.getDims() * OversizedMultiplier);
    }

  hasChildren = true;
}

void QuadTreeNode::destroyChildren()
{
  for(unsigned int x = 0; x < 2; x++)
    for(unsigned int y = 0; y < 2; y++)
      delete children[x][y];
}

Point2i QuadTreeNode::getPossibleOccupantPos(QuadTreeOccupant* pOc)
{
  Point2i pos;

  // NOTE: The center position of the pOc->aabb is not tested, instead a corner is.
  // The center point is not required because if a corner is not in the
  // same partition as the center then it won't fit in any partition anyways.

  if(pOc->aabb.lowerBound.x > center.x)
    pos.x = 1;
  else
    pos.x = 0;

  if(pOc->aabb.lowerBound.y > center.y)
    pos.y = 1;
  else
    pos.y = 0;

  return pos;
}

void QuadTreeNode::addOccupant(QuadTreeOccupant* pOc)
{
  numOccupants++;

  // See if the new occupant fits into any of
  // the children if this node has children
  if(hasChildren)
  {
    // Add the occupant to a child which contains it
    Point2i pos = getPossibleOccupantPos(pOc);

    if(children[pos.x][pos.y]->region.contains(pOc->aabb))
    {
      // Fits into this child node, to continue
      // the adding process from there
      children[pos.x][pos.y]->addOccupant(pOc);

      return;
    }
  }
  else
  {
    // See if there is enough room still left in this node
    if(occupants.size() + 1 <= MaximumOccupants || level > MaxLevels)
    {
      // Add to this node's set
      occupants.insert(pOc);

      // Set the occupant's quad tree pointer to this node
      pOc->pQuadTreeNode = this;
      pOc->pQuadTree = pQuadTree;
    }
    else
    {
      // Doesn't fit here, so create a new partition
      partition();

      // Add the occupant to a child which contains it
      Point2i pos = getPossibleOccupantPos(pOc);

      if(children[pos.x][pos.y]->region.contains(pOc->aabb))
      {
        // Fits into this child node, to continue
        // the adding process from there
        children[pos.x][pos.y]->addOccupant(pOc);

        return;
      }
    }
  }

  // Previous tests failed, so add the occupant this node (even if it goes over the normal maximum occupant count)
  occupants.insert(pOc);

  // Set the occupant's quad tree pointer to this node
  pOc->pQuadTreeNode = this;
  pOc->pQuadTree = pQuadTree;
}

void QuadTreeNode::query(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult)
{
  // See if this region is visible
  if(region.intersects(queryRegion))
  {
    // Add the occupants of this node to the array and then parse the children
    for(std::unordered_set<QuadTreeOccupant*>::iterator it = occupants.begin(); it != occupants.end(); it++)
      if((*it)->aabb.intersects(queryRegion))
        queryResult.push_back(*it);

    if(hasChildren)
    {
      for(unsigned int x = 0; x < 2; x++)
        for(unsigned int y = 0; y < 2; y++)
          children[x][y]->query(queryRegion, queryResult);
    }
  }
}

void QuadTreeNode::queryToDepth(const AABB &queryRegion, std::vector<QuadTreeOccupant*> &queryResult, int depth)
{
  if(depth == 0)
  {
    getOccupants(queryResult);

    return;
  }

  // See if this region is visible
  if(region.intersects(queryRegion))
  {
    // Add the occupants of this node to the array and then parse the children
    for(std::unordered_set<QuadTreeOccupant*>::iterator it = occupants.begin(); it != occupants.end(); it++)
      if((*it)->aabb.intersects(queryRegion))
        queryResult.push_back(*it);

    if(hasChildren)
    {
      for(unsigned int x = 0; x < 2; x++)
        for(unsigned int y = 0; y < 2; y++)
          children[x][y]->queryToDepth(queryRegion, queryResult, depth - 1);
    }
  }
}

void QuadTreeNode::debugRender()
{
  // Render the region AABB
  glColor4f(0.7f, 0.1f, 0.5f, 1.0f);

  region.debugRender();

  glColor4f(0.3f, 0.5f, 0.5f, 1.0f);

  // Render the AABB's of the occupants in this node
  for(std::unordered_set<QuadTreeOccupant*>::iterator it = occupants.begin(); it != occupants.end(); it++)
    (*it)->aabb.debugRender();

  if(hasChildren)
  {
    // Recursively render all of the AABB's below this node
    for(unsigned int x = 0; x < 2; x++)
      for(unsigned int y = 0; y < 2; y++)
        children[x][y]->debugRender();
  }
}
