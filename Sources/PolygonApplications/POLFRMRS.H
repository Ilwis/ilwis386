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
/* PolygonMapFromRas
   Copyright Ilwis System Development ITC
   nov. 1996, by Jelle Wind
	Last change:  JEL   6 May 97    5:57 pm
*/

#ifndef ILWPOLFRMRAS_H
#define ILWPOLFRMRAS_H
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Map\Raster\Map.h"

enum DirBound { dbNONE, dbUP, dbDOWN, dbLEFT, dbRIGHT };

class ChainRec {
public:
  ChainRec() { iLength = 1; dbCode = dbNONE; }
  ChainRec(DirBound db, long iLen){ iLength = iLen; dbCode = db; }  
  DirBound dbCode;  
  long iLength;
};  

//#ifdef ILWPOLFRMRAS_C
static ChainRec undef(const ChainRec& ) { return ChainRec(); }

//#endif
class SegBound {
public: 
  long iSegNr; 
  bool fBeginSeg, fEndSeg;
  Coord crdFrom, crdTo;
  DList<ChainRec> dlChain;
  long iLeftRaw, iRightRaw;
//long iForw, iBackw;
  void Insert(DirBound db, long iLength) {
    dlChain.insert(ChainRec(db, iLength));
  }  
  void Append(DirBound db, long iLength) {
    dlChain.append(ChainRec(db, iLength));
  }  
  ChainRec& crFirst() {
    return dlChain.first();
  }  
  ChainRec& crLast() {
    return dlChain.last();
  }
};

IlwisObjectPtr * createPolygonMapFromRas(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PolygonMapFromRas: public PolygonMapVirtual
{
  friend class PolygonMapVirtual;
public:
  PolygonMapFromRas(const FileName&, PolygonMapPtr& p);
  PolygonMapFromRas(const FileName& fn, PolygonMapPtr& p, const Map& mp, bool f8Con, bool fSmooth);
  ~PolygonMapFromRas();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static PolygonMapFromRas* create(const FileName&, PolygonMapPtr& p, const String& sExpression);
  void Init();

private:
  void NewNode(long iLine, long iCol, byte bt);
  void AppendLeftUp(long iLine, long iCol);
  void AppendUp(long iLine, long iCol, byte bt);
  void AppendLeft(long iLine, long iCol, byte bt);
  SegBound* sbNewInBetween(long iCol);
  SegBound* sbNewWithOneEnd(long iLine, long iCol, bool fRight, BOOL &fBegin);
  void EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, BOOL& fBegin);
  void StoreSegm(SegBound& sb);
  void StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd);
  long iNewSegNr();
  void DetLink(DirBound db1, DirBound db2, DirBound db3, 
                const Array<BOOL>& fSegExist, const Array<BOOL>& fBeginSeg,
                const Array<SegBound*>& sbSeg);

  bool fFindBoundaries(const Map& mpAreas);
  bool fInitPolygonMap();
  bool appendCoords(geos::geom::CoordinateSequence* & coordsA, geos::geom::CoordinateSequence & coordsB, bool fForward) const;
  void autocorrectCoords(std::vector<geos::geom::CoordinateSequence*> & coords) const;
  std::vector<std::pair<geos::geom::LinearRing *, std::vector<geos::geom::Geometry *> *>> makePolys(std::vector<geos::geom::CoordinateSequence*> & coords, GeometryFactory * fact) const;

  Map map;
  LongBufExt iBufInp;     // rasvec 1.4 var: line
  LongBufExt iBufInpPrev; // rasvec 1.4 var: last_line
  ByteBufExt dbBufPrev;   // rasvec 1.4 var: prev
  ByteBufExt dbBufCurr;   // rasvec 1.4 var: cur
  ByteBufExt dbBufNext;   // rasvec 1.4 var: next
  bool fEightCon, fSmooth;
  long iSegNr;
  BufExt<SegBound*> sbHoriz, sbVert;  // rasvec 1.4 vars: segline[RIGHT] and segline[DOWN]

  LongArrayLarge aiSegNr;
  long iNrPol, iNrSeg;
  LongArrayLarge aiFwl, aiBwl;
  std::map<long, CoordBuf> topologySegments;
  std::map<long, std::pair<long,long>> topology;
  std::map<long, long> topStarts;
};

#endif




