/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* Data Interface for ILWIS 2.0
// part for the two dimensional classes and routines
// 14 june 1993, by Wim Koolhoven
// (c) Computer Department ITC
// 11 october 1993, WK: enlarged functionality of MinMax

// never rely on private parts, they will change in near future
	Last change:  WK    9 Sep 98    4:18 pm
*/
#ifndef ILWDAT2_H
#define ILWDAT2_H
#include "Headers\base.h"
//#include "Engine\Base\DataObjects\Buf.h"
#include <Geos.h>

#undef IMPEXP
//#ifdef DAT2_C
#define IMPEXP __export
//#else
//#define IMPEXP __import
//#endif

class CoordBuf;

struct _export RowCol
//
// The RowCol is the internal representation of a geographical position.
// In the case of a raster image even the user can access this number.
//
{
  long& r() { return Row; }
  long Row, Col;
  RowCol()                         : Row(iUNDEF), Col(iUNDEF) {}
  explicit RowCol(long iRow, long iCol)     : Row(iRow), Col(iCol) {}
  explicit RowCol(int iRow, int iCol)       : Row(iRow), Col(iCol) {}
  explicit RowCol(short iR, short iC)         : Row(iR), Col(iC) {}
  explicit RowCol(double rRow, double rCol) 
    { if (rRow < LONG_MIN || rRow > LONG_MAX)
        Row = iUNDEF;
      else
        Row = (long)floor(rRow);
      if (rCol < LONG_MIN || rRow > LONG_MAX)
        Col = iUNDEF;
      else
        Col = (long)floor(rCol);
    }    
  bool fUndef() const { return (Row == iUNDEF) || (Col == iUNDEF); }
  bool operator == (const RowCol& rc) const
      { return (Row == rc.Row) && (Col == rc.Col); }
  bool operator != (const RowCol& rc) const
      { return (Row != rc.Row) || (Col != rc.Col); }
  bool fNear(RowCol,long iTolerance) const;
  friend double rDist2(const RowCol&, const RowCol&);
  friend double rDist(const RowCol&, const RowCol&);
//  friend RowCol operator-(const RowCol& rcA, const RowCol& rcB);
};

RowCol _export operator-(const RowCol& rcA, const RowCol& rcB);

inline RowCol undef(const RowCol&) { return RowCol(); }
typedef RowCol* pRowCol;

struct MinMax
// The MinMax is the boundary of a certain object represented in
// internal (RowCol) coordinates. Used for Segments and Polygons.
{
  MinMax(): rcMin(LONG_MAX,LONG_MAX), rcMax(-LONG_MAX,-LONG_MAX) {}
  MinMax(RowCol rc1, RowCol rc2) : rcMin(rc1), rcMax(rc2)
                                                 { Check(); }
  bool fUndef() const { return rcMin.fUndef() || rcMax.fUndef(); }
  void IMPEXP Check();    // make sure that rcMin < rcMax both with Row and Col
  MinMax _export &operator += (RowCol);         // change this so that point is inside
  MinMax _export &operator += (const MinMax&);	// change this so that mm is inside
  long MinRow() const { return rcMin.Row; }
  long MaxRow() const { return rcMax.Row; }
  long MinCol() const { return rcMin.Col; }
  long MaxCol() const { return rcMax.Col; }
  long& MinRow() { return rcMin.Row; }
  long& MaxRow() { return rcMax.Row; }
  long& MinCol() { return rcMin.Col; }
  long& MaxCol() { return rcMax.Col; }
  long width() const
    { if (fUndef()) return iUNDEF;
      return MaxCol() - MinCol(); }
  long height() const
    { if (fUndef()) return iUNDEF;
      return MaxRow() - MinRow(); }
  bool _export fContains(RowCol) const;
  bool _export fNear(RowCol,long iTolerance) const;
  bool _export fContains(MinMax) const;  // true if partly contains
  bool operator == (const MinMax& mm) const
      { return (rcMin == mm.rcMin) && (rcMax == mm.rcMax); }
  bool operator != (const MinMax& mm) const
      { return (rcMin != mm.rcMin) || (rcMax != mm.rcMax); }
  RowCol rcSize() const
    { return RowCol(height(), width()); }
  void _export SetSize(const RowCol& rcSz);
  RowCol rcMin;
  RowCol rcMax;
};
inline MinMax undef(const MinMax&) { return MinMax(); }

struct Coord : public Coordinate
//
// The Coord is the metric (or what ever system will be used) coordinates
// of a geographical position. These are intended for user communication.
//
{
public:
  Coord() 
    : Coordinate(rUNDEF, rUNDEF, rUNDEF)
      {}
  Coord(double x, double y, double z=rUNDEF) : Coordinate(x,y,z) {}
  Coord(const Coordinate& crd) : Coordinate(crd) {}
  //double X, Y;
  bool _export operator == (const Coord& crd) const;
  bool _export operator != (const Coord& crd) const
    { return !operator==(crd); }
  Coord& operator *= (double rFact)
    { x *= rFact; x *= rFact; z *= rFact; return *this; }
  Coord& operator /= (double rFact)
    { x /= rFact; y /= rFact; z /= rFact; return *this; }
  Coord& operator += (const Coord& crd)
    { x += crd.x; y += crd.y; z += crd.z;return *this; }
  Coord& operator -= (const Coord& crd)
    { x -= crd.x; y -= crd.y;  z -= crd.z; return *this; }
  bool fUndef() const { return (x == rUNDEF) || (y == rUNDEF); }
  bool _export fNear(const Coord&,double rTolerance) const;
  bool fInside(const Coord&, const Coord&) const;
  friend double rDist2(const Coord&, const Coord&);
  friend double rDist(const Coord&, const Coord&);
};

Coord _export operator-(const Coord& crdA, const Coord& crdB);

inline Coord undef(const Coord&) { return Coord(); }

struct CoordBounds
// The CoordBounds is the boundary of a certain object represented in
// real world (Coord) coordinates. Used in BaseMap
{
  CoordBounds(): cMin(1e30,1e30,1e30), cMax(-1e30,-1e30,-1e30) {area=rUNDEF;}
  CoordBounds(Coord c1, Coord c2) : cMin(c1), cMax(c2) { Check(); area=rUNDEF; }
	_export CoordBounds(const CoordBuf& cBuf);
  bool fUndef() const
    { return cMin.x > cMax.x || cMin.y > cMax.y || cMin.fUndef() || cMax.fUndef(); }
  void _export Check();    // make sure that cMin < cMax both with x and y
  CoordBounds _export &operator += (Coord);   // change this so that point is inside
  CoordBounds _export &operator += (const CoordBounds&);  // change this so that cb is inside
  CoordBounds _export &operator *= (double);
  CoordBounds _export &operator += (double);
  double MinX() const { return cMin.x; }
  double MaxX() const { return cMax.x; }
  double MinY() const { return cMin.y; }
  double MaxY() const { return cMax.y; }
  double MinZ() const { return cMin.z; }
  double MaxZ() const { return cMax.z; }
  double& MinX() { return cMin.x; area=rUNDEF;}
  double& MaxX() { return cMax.x; area=rUNDEF;}
  double& MinY() { return cMin.y; area=rUNDEF;}
  double& MaxY() { return cMax.y; area=rUNDEF; }
  double& MinZ() { return cMin.z; area=rUNDEF;}
  double& MaxZ() { return cMax.z; area=rUNDEF;}
  double width() const
    { if (fUndef()) return rUNDEF;
      return MaxX() - MinX(); }
  double height() const
    { if (fUndef()) return rUNDEF;
      return MaxY() - MinY(); }
  double altitude() const
    { if (fUndef()) return rUNDEF;
      return MaxZ() - MinZ(); }
  bool _export fContains(Coord) const;
  bool _export fNear(Coord,double rTolerance) const;
  bool _export fContains(const CoordBounds&) const;  // true if partly contains
  bool operator == (const CoordBounds& cb) const {
    return cMin == cb.cMin && cMax == cb.cMax;
  }
  bool _export fValid() const;
  Coord _export middle() const;
  double getArea() {
	  if ( fUndef())
		  return rUNDEF;
	  if ( area == rUNDEF){
		  area = width() * height();
	  }
	  return area;
  }

  Coord cMin, cMax;
  double area;
};

struct IMPEXP LatLon
{
  LatLon(): Lat(rUNDEF), Lon(rUNDEF) {}
  LatLon(double rLat, double rLon): Lat(rLat), Lon(rLon) {}
  double Lat, Lon;  // in degrees
	LatLon& operator += (const LatLon& ll)
    { Lat += ll.Lat; Lon += ll.Lon; return *this; }
  LatLon& operator -= (const LatLon& ll)
    { Lat -= ll.Lat; Lon -= ll.Lon; return *this; }
  bool fUndef() const { return (Lat == rUNDEF) || (Lon == rUNDEF); }
  double Phi() const    { return M_PI / 180 * Lat; }
  double Lambda() const { return M_PI / 180 * Lon; }
  void Phi(double phi) { Lat = phi * 180 / M_PI; }
  void Lambda(double lambda) { Lon = lambda * 180 / M_PI; }
  String sLat(int iWidth=0) const;
  String sLon(int iWidth=0) const;
  String sValue(int iWidth=0) const;
  static String sLat(double rLat, int iWidth=0);
  static String sLon(double rLon, int iWidth=0);
  static String sDegree(double rDeg, int iWidth=0, bool fShowMin = false);
	static double rDegree(const String& sDegree);
};
inline LatLon undef(const LatLon&) { return LatLon(); }

// Inline functions
// These implementations can and will change without warning
// programmers should only rely on the .h include file

inline double rDist2(const RowCol& a, const RowCol& b)
  {
    if (a.fUndef() || b.fUndef()) return rUNDEF;
    double dr = (double) a.Row - b.Row;
    double dc = (double) a.Col - b.Col;
    return dr * dr + dc * dc; }
inline double rDist(const RowCol& a, const RowCol& b)
  { double r = rDist2(a,b);
    if (r == rUNDEF) return rUNDEF;
    return sqrt(r);
  }

inline double rDist2(const Coord& a, const Coord& b)
  {
    if (a.fUndef() || b.fUndef()) return rUNDEF;
    double dX = a.x - b.x;
    double dY = a.y - b.y;
    return dX * dX + dY * dY; }
inline double rDist(const Coord& a, const Coord& b)
  { double r = rDist2(a,b);
    if (r == rUNDEF) return rUNDEF;
    return sqrt(r);
  }
inline bool Coord::fInside(const Coord& a, const Coord& b) const
{
  if (((x < a.x) ^ (x < b.x)) && ((y < a.y) ^ (y < b.y)))
    return true;
  else
    return false;
}

#endif







