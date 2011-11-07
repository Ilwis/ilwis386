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
/* PointMapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  14 May 97   10:49 am
*/

#ifndef ILWPOINTMAPSTORE_H
#define ILWPOINTMAPSTORE_H
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Table\Col.h"

namespace geos {
	namespace index {
		namespace quadtree{
		class Quadtree;
		}
	}
}

class _export PointMapStore : public MultiPoint
{
  friend class PointMapPtr;
  friend class PointMapVirtual;
  friend class PointMapImport;
public:
  PointMapStore(const FileName&, PointMapPtr& p, bool fDoNotLoad = false);
  PointMapStore(const FileName& fn, PointMapPtr& p, long iPoints);
  virtual ~PointMapStore();
  void Export(const FileName& fn) const;
  virtual void Store();
  static void UnStore(const FileName& fnObj);
  long iRaw(long iRec) const;
  long iValue(long iRec) const;
  double rValue(long iRec) const;
  String sValue(long iRec, short iWidth=-1, short iDec=-1) const;
  Coord cValue(long iRec) const;
  long iRec(const String&) const;
  long iRec(long iRaw) const;
  long iRec(const Coord&, double rPrx=rUNDEF) const;
  void PutRaw(long iRec, long iRaw);
  void PutVal(long iRec, double rValue);
  void PutVal(long iRec, const String& sValue);
  void PutVal(long iRec, const Coord&);
  void Delete(long iRec);
  long iAdd(long iRecs);
  long iAddVal(const Coord&, const String& sValue);
  long iAddVal(const Coord&, double rValue);
  long iAddRaw(const Coord&, long iRaw);
	void PutBufRaw(const CoordBuf&, const LongBuf&);
  void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  bool fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
  void KeepOpen(bool f =true);
  void Flush();
  long iPnt() const;
  void Updated();
  void SetErase(bool f=true);
  void SaveAsFormat20();
  virtual void DoNotUpdate();	
  virtual Geometry *getFeature(long i) const;
  Feature *pntNew(geos::geom::Geometry *pnt=NULL);
  virtual vector<Geometry *> getFeatures(Coord crd, double rPrx=rUNDEF);
  bool removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>());
  Geometry	*getFeatureById(const String& id) const;
  vector<Feature *> getFeatures(const CoordBounds& cb, bool complete=true) const;

protected:
  const FileName& fnObj; // fnObj from ptr (for convenience)
  PointMapPtr& ptr;  // the PointMapPtr of which this is a member
  ObjectTime timStore; // time of last change in data
  //Table tbl;
  //Column colVal, colCoord;
  void SetCoordSystem(const CoordSystem& cs);
  void SetPoint(const Coord& crd, double v, bool usesReal);
  bool fErase;
  geos::index::quadtree::Quadtree *spatialIndex;
};

#endif




