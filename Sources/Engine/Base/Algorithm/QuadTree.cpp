#include "Headers\toolspch.h"
#include <Geos.h>
#include <stdexcept>

#include "QuadTree.h"

QuadTree::QuadTree(const CoordBounds& cb,
				   int    maxItem, double fac ) : bounds(cb), nMaxItems(maxItem)
{
	minBucketWidth = fac * cb.width();
	root = new QuadTreeNode(0, bounds, nMaxItems, minBucketWidth);
}

QuadTree::~QuadTree()
{
	root->destroy();
	delete root;
}

void
QuadTree::insert(Geometry* item)
{
	const Envelope *env = item->getEnvelopeInternal();
	CoordBounds cb(Coord(env->getMinX(), env->getMinY()), Coord(env->getMaxX(), env->getMaxY()));
	if (!bounds.fContains(cb))
		throw std::runtime_error("Trying to insert an item into QuadTree, but it is not contained inside it.");

	root->insert(item);
}

void
QuadTree::query(const Coord& crd, std::vector<Geometry*>&  container)
{
	root->getItems(crd, container);
}

void
QuadTree::query(const CoordBounds& cb, std::vector<Geometry*>&  container)
{
	root->getItems(bounds, container);
}

void
QuadTree::getAllItems(std::vector<Geometry*>&  container)
{
	root->getAllItems(container);
}

void QuadTree::remove(Geometry *geom) {
	root->remove(geom);
}

void
QuadTree::getAllItems(std::vector< std::vector<Geometry*> >&  container)
{
	root->getAllItems(container);
}

void
QuadTree::update()
{
	root->update();
}

void QuadTree::setBounds(const CoordBounds& bnd) {
	bounds += bnd;
	root->setBounds(bnd);
	root->setMinBucketWidth(minBucketWidth);
}


