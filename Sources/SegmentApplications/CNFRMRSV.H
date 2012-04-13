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


#ifndef ILWCONFRMRASVAL_H
#define ILWCONFRMRASVAL_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Map\Raster\Map.h"

IlwisObjectPtr * createSegmentMapFromRasValueBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
IlwisObjectPtr * createSegmentMapFromRasAreaBnd(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class SegBound;

class DATEXPORT SegmentMapFromRasValueBnd: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapFromRasValueBnd(const FileName&, SegmentMapPtr& p);
  SegmentMapFromRasValueBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, double rStartVal, double rEndVal, double rStepVal, bool f8Con, bool fSmooth, double frags);
  SegmentMapFromRasValueBnd(const FileName& fn, SegmentMapPtr& p, const Map& mp, const set<double>& seq, bool f8Con, bool fSmth, double frags);
  ~SegmentMapFromRasValueBnd();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static SegmentMapFromRasValueBnd* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  void NewNode(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  void AppendLeftUp(long iLine, long iCol, int iCurrentIsoNr);
  void AppendUp(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  void AppendLeft(long iLine, long iCol, byte bt, int iCurrentIsoNr);
  SegBound* sbNewInBetween(long iCol);
  SegBound* sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool &fBegin);
  void EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin, int iCurrentIsoNr);
  void StoreSegm(const SegBound& sb, int& iCurrentIsoNr);
  void StoreSegm(CoordBuf& cBuf, long& iCrd, double& rVal);
	int iComparePixels(const double& rFirst, const double& rSecond, const double& rCurrentIsoVal);
	static void parseSequence(const String& sequence, set<double>& intervals);
//  long iNewSegNr();
  Map map;
  RealBufExt rBufInp;     // rasvec 1.4 var: line
  RealBufExt rBufInpPrev; // rasvec 1.4 var: last_line
  ByteBufExt * dbBufPrev;   // rasvec 1.4 var: prev
  ByteBufExt dbBufCurr;   // rasvec 1.4 var: cur
  ByteBufExt dbBufNext;   // rasvec 1.4 var: next
  bool fEightCon, fSmooth;
  long iSegNr;
  BufExt<SegBound*> *sbHoriz, *sbVert;  // rasvec 1.4 vars: segline[RIGHT] and segline[DOWN]
	Table tblAtt;
	Column col1, col2, colLength;
	double fragmentSize;
	double m_rIsoStartVal;
	double m_rIsoEndVal;
	double m_rIsoStepVal;
	list<CoordBuf> *tmpSegmentMap; // remember coordinates here so that they can be stored in the order we want
	list <SegBound*> cleanupList; // to gather the SegBounds that are new-ed and should be deleted
	set<double> intervals;

};

#endif




