#include "LTBL/QuadTreeOccupant.h"

#include "LTBL/QuadTreeNode.h"
#include "LTBL/QuadTree.h"

#include "LTBL/SFML_OpenGL.h"

#include <assert.h>

#include "LTBL/Light.h"
#include "LTBL/ConvexHull.h"

using namespace qdt;

AABB::AABB() : lowerBound(0.0f, 0.0f), upperBound(1.0f, 1.0f)
{
}

AABB::AABB(const Vec2f &bottomLeft, const Vec2f &topRight)
: lowerBound(bottomLeft),
    upperBound(topRight)
{
}

Vec2f AABB::getCenter() const
{
  return (lowerBound + upperBound) / 2.0f;
}

Vec2f AABB::getDims() const
{
  return upperBound - lowerBound;
}

Vec2f AABB::getLowerBound() const
{
  return lowerBound;
}

Vec2f AABB::getUpperBound() const
{
  return upperBound;
}

void AABB::setCenter(const Vec2f &newCenter)
{
  Vec2f difference = newCenter - getCenter();

  upperBound += difference;
  lowerBound += difference;
}

void AABB::incCenter(const Vec2f &increment)
{
  upperBound += increment;
  lowerBound += increment;
}

void AABB::setDims(const Vec2f &newDims)
{
  Vec2f center = getCenter();

  Vec2f halfDims = newDims / 2.0f;

  lowerBound = center - halfDims;
  upperBound = center + halfDims;
}

bool AABB::intersects(const AABB &other) const
{
  if(upperBound.x < other.lowerBound.x)
    return false;

  if(upperBound.y < other.lowerBound.y)
    return false;

  if(lowerBound.x > other.upperBound.x)
    return false;

  if(lowerBound.y > other.upperBound.y)
    return false;

  return true;
}

bool AABB::contains(const AABB &other) const
{
  if(other.lowerBound.x >= lowerBound.x &&
     other.upperBound.x <= upperBound.x &&
     other.lowerBound.y >= lowerBound.y &&
     other.upperBound.y <= upperBound.y)
    return true;

  return false;
}

void AABB::debugRender()
{
  // Render the AABB with lines
  glBegin(GL_LINES);

  // Bottom
  glVertex3f(lowerBound.x, lowerBound.y, 0.0f);
  glVertex3f(upperBound.x, lowerBound.y, 0.0f);

  // Right
  glVertex3f(upperBound.x, lowerBound.y, 0.0f);
  glVertex3f(upperBound.x, upperBound.y, 0.0f);

  // Top
  glVertex3f(upperBound.x, upperBound.y, 0.0f);
  glVertex3f(lowerBound.x, upperBound.y, 0.0f);

  // Left
  glVertex3f(lowerBound.x, upperBound.y, 0.0f);
  glVertex3f(lowerBound.x, lowerBound.y, 0.0f);

  glEnd();
}

QuadTreeOccupant::QuadTreeOccupant()
: pQuadTreeNode(NULL), pQuadTree(NULL)
{
}

QuadTreeOccupant::~QuadTreeOccupant()
{
}

void QuadTreeOccupant::updateTreeStatus()
{
  if(pQuadTreeNode == NULL)
  {
    // Not in the tree, so see if it fits in the root partition now that the AABB has been changed.
    if(pQuadTree != NULL) // It must not have been added to a tree if this is NULL as well
      if(pQuadTree->rootNode->region.contains(aabb))
      {
        // Fits, remove it from the outside root set and add it to the root
        pQuadTree->outsideRoot.erase(this);

        pQuadTree->rootNode->addOccupant(this);
      }
  }
  else
  {
    assert(pQuadTreeNode->numOccupants >= 1);

    // First remove the occupant from the set (may be re-added later, this is not highly
    // optimized, but we use this method for simplicity's sake)
    pQuadTreeNode->occupants.erase(this);

    // See of the occupant still fits
    if(pQuadTreeNode->region.contains(aabb))
    {
      // Re-add to possibly settle into a new position lower in the tree

      // AddOccupant will raise this number to indicate more occupants than there actually are
      pQuadTreeNode->numOccupants--;

      pQuadTreeNode->addOccupant(this);
    }
    else
    {
      // Doesn't fit in this node anymore, so we will continue going
      // up levels in the tree until it fits. If it doesn't fit anywhere,
      // add ot to the quad tree outside of root set.

      // Check to see if this partition should be destroyed.
      // Then, go through the parents until enough occupants are present and
      // merge everything into that parent
      if(pQuadTreeNode->numOccupants - 1 < MinimumOccupants)
      {
        // Move up pNode until we have a partition above the minimum count
        while(pQuadTreeNode->pParentNode != NULL)
        {
          if(pQuadTreeNode->numOccupants - 1 >= MinimumOccupants)
            break;

          pQuadTreeNode = pQuadTreeNode->pParentNode;
        }

        pQuadTreeNode->merge();
      }

      // Now, go up and decrement the occupant counts and search for a new place for the modified occupant
      while(pQuadTreeNode != NULL)
      {
        pQuadTreeNode->numOccupants--;

        // See if this node contains the occupant. If so, add it to that occupant.
        if(pQuadTreeNode->region.contains(aabb))
        {
          // Add the occupant to this node and break
          pQuadTreeNode->addOccupant(this);

          return;
        }

        pQuadTreeNode = pQuadTreeNode->pParentNode;
      }

      // If we did not break out of the previous loop, this means that we
      // cannot fit the occupantinto the root node. We must therefore add
      // it to the outside root set using our pointer to the quad tree container.
      assert(pQuadTree != NULL);

      pQuadTree->outsideRoot.insert(this);

      // Occupant's parent is already NULL, or else it would not have made it here
      assert(pQuadTreeNode == NULL);
    }
  }
}

void QuadTreeOccupant::removeFromTree()
{
  if(pQuadTreeNode != NULL) // If part of a quad tree
  {
    pQuadTreeNode->occupants.erase(this);

    // Doesn't fit in this node anymore, so we will continue going
    // up levels in the tree until it fits. If it doesn't fit anywhere,
    // add ot to the quad tree outside of root set.

    // Check to see if this partition should be destroyed.
    // Then, go through the parents until enough occupants are present and
    // merge everything into that parent
    if(pQuadTreeNode->numOccupants - 1 < MinimumOccupants)
    {
      // Move up pNode until we have a partition above the minimum count
      while(pQuadTreeNode->pParentNode != NULL)
      {
        if(pQuadTreeNode->numOccupants - 1 >= MinimumOccupants)
          break;

        pQuadTreeNode = pQuadTreeNode->pParentNode;
      }

      pQuadTreeNode->merge();
    }

    // Decrement the remaining occupant counts
    while(pQuadTreeNode != NULL)
    {
      pQuadTreeNode->numOccupants--;
      pQuadTreeNode = pQuadTreeNode->pParentNode;
    }
  }
  else if(pQuadTree != NULL) // See if still part of a quad tree, indicating that it must be in the outside root set
    pQuadTree->outsideRoot.erase(this);

  pQuadTreeNode = NULL;
  pQuadTree = NULL;
}
