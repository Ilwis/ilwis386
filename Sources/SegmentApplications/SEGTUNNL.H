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
/* SegmentMapTunneling
   Copyright Ilwis System Development ITC
   jan 1996, by Jan Hendrikse
	Last change:  JEL   6 May 97    6:01 pm
*/

#ifndef ILWSEGMAPTUNNEL_H
#define ILWSEGMAPTUNNEL_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Table\Col.h"

IlwisObjectPtr * createSegmentMapTunneling(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT SegmentMapTunneling: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapTunneling(const FileName&, SegmentMapPtr& p);
  SegmentMapTunneling(const FileName& fn, SegmentMapPtr& p, const SegmentMap& pm, double rTunnelWidth, bool fRemoveNodes=false);
  ~SegmentMapTunneling();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  static SegmentMapTunneling* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  SegmentMap smp;
  bool fYesF, fYesL; // flags indicating that current segment has still possible pseudonode
  ArrayLarge<bool> fPossible;
  ArrayLarge<Coord> acrdFirst, acrdLast;
  double rTunnelWidth;
  bool fRemoveNodes;
  void NodeCheck(long s, long t, bool& fNodeF,
                           bool& fNodeL, long& iNodSegF, long& iNodSegL);

  bool fRemovePseudoNodes(double rTol2);
  void Tunnel(long& iNrCoords, CoordBuf& crdBuf, double rTunnelTolerance);
};

#endif




