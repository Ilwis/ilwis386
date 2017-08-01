#ifndef QUADTREENODE_H_INCLUDED
#define QUADTREENODE_H_INCLUDED

#include <vector>
#include <Geos.h>

class QuadTreeNode
{
   public:
      QuadTreeNode(QuadTreeNode*, const CoordBounds& cb, int, double);
      ~QuadTreeNode();

      CoordBounds getArea() { return bounds; }

      void insert(Geometry*);

      bool pushItemDown(int);
      bool pushItemDown(Geometry*);

      void pushItemUp(int);
      void pushItemUp(Geometry*);

	  bool remove(Geometry*);

      void getItems(const Coord& crd, std::vector<Geometry*>&);
      void getItems(const CoordBounds& bnds, std::vector<Geometry*>&);
      void getAllItems(std::vector<Geometry*>&);
      void getAllItems(std::vector< std::vector<Geometry*> >&);

      QuadTreeNode* findItemNode(Geometry*);

      void destroy();

      void update();
	  void setBounds(const CoordBounds& bnd) { bounds += bnd; }
	  void setMinBucketWidth(double b) { minBucketWidth = b; }

   private:
      bool removeItem(Geometry*);
      void removeItem(int);
      bool canBeInsertedInChild(Geometry*);
      bool insertInChild(Geometry*);
	  bool hasItems() const;
	  bool hasChildren() const;
      bool isPrunable() const;
	  bool contains(const CoordBounds& cbOuter,const CoordBounds& cbInner) const;

      void partition();

      enum NodeID
      {
         NODE_TOPLEFT,
         NODE_TOPRIGHT,
         NODE_BOTTOMRIGHT,
         NODE_BOTTOMLEFT
      };

      vector<QuadTreeNode *>  nodes;
      QuadTreeNode*  parent;
      CoordBounds    bounds;
	  double		 minBucketWidth;
      int            nMaxItems;
      bool           isPartitioned;

      std::vector<Geometry*> items;
};

#endif // QUADTREENODE_H_INCLUDED
