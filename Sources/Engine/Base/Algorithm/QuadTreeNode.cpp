#include <stdexcept>
#include <algorithm>
#include <set>
#include <list>
#include "Headers\toolspch.h"
#include "QuadTreeNode.h"


QuadTreeNode::QuadTreeNode(QuadTreeNode* par,
						   const CoordBounds& cb,
						   int maxIt, double bw) : parent(par),
						   bounds(cb),
						   nMaxItems(maxIt),
						   isPartitioned(false),
						   minBucketWidth(bw)
{
}

QuadTreeNode::~QuadTreeNode()
{
	destroy();
}

bool
QuadTreeNode::canBeInsertedInChild(Geometry* item)
{
	if (!isPartitioned)
		return false;

	for (int i = 0; i < nodes.size(); i++)
	{
		const Envelope *env = item->getEnvelopeInternal();
		CoordBounds cb(Coord(env->getMinX(), env->getMinY()), Coord(env->getMaxX(), env->getMaxY()));
		if (nodes[i]->getArea().fContains(cb))
		{
			return true;
		}
	}

	return false;
}

bool
QuadTreeNode::insertInChild(Geometry* item)
{
	if (!isPartitioned)
		return false;

	for (int i = 0; i < nodes.size(); i++)
	{
		const Envelope *env = item->getEnvelopeInternal();
		CoordBounds cb(Coord(env->getMinX(), env->getMinY()), Coord(env->getMaxX(), env->getMaxY()));
		if (nodes[i]->getArea().fContains(cb))
		{
			nodes[i]->insert(item);
			return true;
		}
	}

	return false;
}

void
QuadTreeNode::insert(Geometry* item)
{
	if (!insertInChild(item))
	{
		items.push_back(item);

		if (!isPartitioned && (int)items.size() >= nMaxItems && minBucketWidth < bounds.width())
			partition();
	}
}

bool
QuadTreeNode::pushItemDown(int iItem)
{
	if (insertInChild(items[iItem]))
	{
		removeItem(iItem);
		return true;
	}

	return false;
}

bool
QuadTreeNode::pushItemDown(Geometry* item)
{
	if (insertInChild(item))
	{
		removeItem(item);
		return true;
	}

	return false;
}

void
QuadTreeNode::pushItemUp(int iItem)
{
	if (parent == 0)
		throw ErrorObject(TR("Trying to push an item above the root node of QuadTree."));

	removeItem(iItem);
	parent->insert(items[iItem]);
}

void
QuadTreeNode::pushItemUp(Geometry* item)
{
	if (parent == 0)
		throw ErrorObject(TR("Trying to push an item above the root node of QuadTree."));

	removeItem(item);
	parent->insert(item);
}

void
QuadTreeNode::partition()
{
	if (isPartitioned)
		throw ErrorObject(TR("Trying to partition a branched node."));

	double width   = bounds.width() / 2.0f;
	double height  = bounds.height() / 2.0f;
	nodes.resize(4);
	nodes[NODE_TOPLEFT] = new QuadTreeNode(this,
		CoordBounds(Coord(bounds.MinX(), bounds.MinY()),Coord(bounds.MinX() + width, bounds.MinY() + height)),
		nMaxItems,minBucketWidth);

	nodes[NODE_TOPRIGHT] = new QuadTreeNode(this,
		CoordBounds(Coord(bounds.MinX() + width,
		bounds.MinY()),
		Coord(bounds.MaxX(),
		bounds.MinY() + height)),
		nMaxItems,minBucketWidth);

	nodes[NODE_BOTTOMRIGHT] = new QuadTreeNode(this,
		CoordBounds(Coord(bounds.MinX() + width,
		bounds.MinY() + height),
		Coord(bounds.MaxX(),
		bounds.MaxY())),
		nMaxItems,minBucketWidth);

	nodes[NODE_BOTTOMLEFT] = new QuadTreeNode(this,
		CoordBounds(Coord(bounds.MinX(),
		bounds.MinY() + height),
		Coord(bounds.MinX() + width,
		bounds.MaxY())),
		nMaxItems,minBucketWidth);

	isPartitioned = true;

	int i = 0;

	while (i < (int)items.size())
	{
		if (!pushItemDown(i))
		{
			i++;
		}
	}
}

bool QuadTreeNode::remove(Geometry* item) {
	if(!bounds.fContains(CoordBounds(item->getEnvelopeInternal())))
		return false;

	bool found = false;
	for (int i = 0; i < nodes.size(); ++i)
	{
		if ( nodes[i] )
		{
			found = nodes[i]->remove(item);
			if (found)
			{
				// trim subtree if empty
				if (nodes[i]->isPrunable())
				{
					delete nodes[i];
					nodes[i] = NULL;
				}
				break;
			}
		}
	}
	if (found) return found;

	return removeItem(item);
}

bool QuadTreeNode::removeItem(Geometry* item)
{
	std::vector<Geometry*>::iterator iRemove =
		std::find(items.begin(), items.end(), item);

	if (iRemove != items.end()) {
		items.erase(iRemove);
		return true;
	}

	return false;
}

void QuadTreeNode::removeItem(int iItem)
{
	if (iItem >= (int)items.size())
		throw ErrorObject(TR("Trying to remove an item inside QuadTreeNode, but the given index is out of bound."));

	std::vector<Geometry*>::iterator iRemove = items.begin();
	for (int i = 0; i < iItem; i++, iRemove++);
	items.erase(iRemove);
}

void
QuadTreeNode::getItems(const Coord& crd, std::vector<Geometry*>&  container)
{
	if (bounds.fContains(crd))
	{
		for (std::vector<Geometry*>::iterator i = items.begin(); i != items.end(); i++)
			if ((*i)->getEnvelopeInternal()->contains(crd))
				container.push_back(*i);

		if (isPartitioned)
			for (int i = 0; i < nodes.size(); i++)
				nodes[i]->getItems(crd, container);
	}
}

void
QuadTreeNode::getItems(const CoordBounds& bnd, std::vector<Geometry*>&  container)
{
	if (bounds.fContains(bnd))
	{
		for (std::vector<Geometry*>::iterator i = items.begin(); i != items.end(); i++) {
			const Envelope *env = (*i)->getEnvelopeInternal();
			CoordBounds cb(Coord(env->getMinX(), env->getMinY()), Coord(env->getMaxX(), env->getMaxY()));
			if (cb.fContains(bnd))
				container.push_back(*i);
		}

		if (isPartitioned)
			for (int i = 0; i < nodes.size(); i++)
				nodes[i]->getItems(bounds, container);
	}
}

void
QuadTreeNode::getAllItems(std::vector<Geometry*>&  container)
{
	for (std::vector<Geometry*>::iterator i = items.begin(); i != items.end(); i++)
		container.push_back(*i);

	if (isPartitioned)
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->getAllItems(container);
}

void
QuadTreeNode::getAllItems(std::vector< std::vector<Geometry*> >&  container)
{
	if (items.size() > 0)
		container.push_back(items);

	if (isPartitioned)
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->getAllItems(container);
}

QuadTreeNode*
QuadTreeNode::findItemNode(Geometry* item)
{
	if (std::find(items.begin(), items.end(), item) != items.end())
		return this;

	if (isPartitioned)
	{
		QuadTreeNode* result = 0;

		for (int i = 0; i < nodes.size(); i++)
		{
			if (result == 0)
				result = nodes[i]->findItemNode(item);

			else
				break;
		}

		return result;
	}

	return 0;
}

void
QuadTreeNode::update()
{
	std::vector<Geometry*> toPushUp, toPushDown;

	for (std::vector<Geometry*>::iterator i = items.begin(); i != items.end(); i++)
	{
		if (canBeInsertedInChild(*i))
			toPushDown.push_back(*i);

		else if (parent != 0)
			toPushUp.push_back(*i);
	}

	for (std::vector<Geometry*>::iterator i = toPushDown.begin(); i != toPushDown.end(); i++)
		pushItemDown(*i);

	for (std::vector<Geometry*>::iterator i = toPushUp.begin(); i != toPushUp.end(); i++)
		pushItemUp(*i);

	if (isPartitioned)
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->update();
}



void QuadTreeNode::destroy()
{
	if (isPartitioned)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->destroy();
			delete nodes[i];
			nodes[i] = 0;
		}

		isPartitioned = false;
	}

	items.clear();
}

inline bool QuadTreeNode::hasChildren() const
{
	for (int i = 0; i < nodes.size(); i++) 
		if (nodes[i]) return true;
	return false;
}

inline bool QuadTreeNode::isPrunable() const
{
	return ! (hasChildren() || hasItems());
}

inline bool QuadTreeNode::hasItems() const
{
	return ! items.empty();
}