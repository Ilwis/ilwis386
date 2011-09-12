/**********************************************************************
 * $Id: NodeBase.h 2556 2009-06-06 22:22:28Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: index/quadtree/NodeBase.java rev 1.9 (JTS-1.10)
 *
 **********************************************************************/

#ifndef GEOS_IDX_QUADTREE_NODEBASE_H
#define GEOS_IDX_QUADTREE_NODEBASE_H

#include <geos/platform.h>
#include <geos/export.h>
#include <vector>
#include <string>

// Forward declarations
namespace geos {
	namespace geom {
		class Coordinate;
		class Envelope;
	}
	namespace index {
		class ItemVisitor;
		namespace quadtree {
			class Node;
		}
	}
}

namespace geos {
namespace index { // geos::index
namespace quadtree { // geos::index::quadtree

/**
 * \brief
 * The base class for nodes in a Quadtree.
 *
 */
class GEOS_DLL NodeBase {

private:

	void visitItems(const geom::Envelope* searchEnv,
			ItemVisitor& visitor);
	
public:

	_export static int getSubnodeIndex(const geom::Envelope *env,
			const geom::Coordinate& centre);

	_export  NodeBase();

	_export  virtual ~NodeBase();

	_export  std::vector<void*>& getItems();

	/// Add an item to this node.
	/// Ownership of the item is left to caller.
	_export  void add(void* item);

	/// Push all node items to the given vector, return the argument
	_export  std::vector<void*>& addAllItems(std::vector<void*>& resultItems) const;

	_export  virtual void addAllItemsFromOverlapping(const geom::Envelope& searchEnv,
			std::vector<void*>& resultItems) const;

	_export unsigned int depth() const;

	_export unsigned int size() const;

	_export unsigned int getNodeCount() const;

	_export virtual std::string toString() const;

	_export virtual void visit(const geom::Envelope* searchEnv, ItemVisitor& visitor);

	/**
	 * Removes a single item from this subtree.
	 *
	 * @param searchEnv the envelope containing the item
	 * @param item the item to remove
	 * @return <code>true</code> if the item was found and removed
	 */
	_export bool remove(const geom::Envelope* itemEnv, void* item);
 
	_export bool hasItems() const;

	_export bool hasChildren() const;

	_export bool isPrunable() const;

protected:

	/// Actual items are NOT owned by this class
	std::vector<void*> items;

	/**
	 * subquads are numbered as follows:
	 * <pre>
	 *  2 | 3
	 *  --+--
	 *  0 | 1
	 * </pre>
	 *
	 * Nodes are owned by this class 
	 */
	Node* subnode[4];

	virtual bool isSearchMatch(const geom::Envelope& searchEnv) const=0;
};


// INLINES, To be moved in NodeBase.inl

inline bool
NodeBase::hasChildren() const
{
	for (int i = 0; i < 4; i++) 
		if (subnode[i]) return true;
	return false;
}

inline bool
NodeBase::isPrunable() const
{
	return ! (hasChildren() || hasItems());
}

inline bool
NodeBase::hasItems() const
{
	return ! items.empty();
}

} // namespace geos::index::quadtree
} // namespace geos::index
} // namespace geos

#endif // GEOS_IDX_QUADTREE_NODEBASE_H

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/22 12:22:50  strk
 * indexQuadtree.h split
 *
 **********************************************************************/

