#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include "QuadTreeNode.h"

class QuadTreeNode;

class QuadTree
{
   public:
      QuadTree(const CoordBounds& cb, int nmax=25, double bw = 0.00001);
      ~QuadTree();

      void insert(Geometry*);
	  void remove(Geometry*);
      void query(const Coord& crd, std::vector<Geometry*>&);
      void query(const CoordBounds& cb, std::vector<Geometry*>&);
      void getAllItems(std::vector<Geometry*>&);
      void getAllItems(std::vector< std::vector<Geometry*> >&);

      CoordBounds getArea() { return bounds; }
	  void setBounds(const CoordBounds& bnd) ;
	  void setMinBucketWidth(double b) { minBucketWidth = b; }

      void update();

   private:
      CoordBounds   bounds;
	  double		minBucketWidth;
      int           nMaxItems;
      QuadTreeNode* root;
};

#endif // QUADTREE_H_INCLUDED
