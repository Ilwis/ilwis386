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
/* $Log: /ILWIS 3.0/Import14/SEGIMP.H $
 * 
 * 3     17-06-99 2:27p Martin
 * added _exports
 * 
 * 2     17-06-99 1:08p Martin
 * added _exports for import/export dll
// Revision 1.3  1998/06/10 13:08:03  Willem
// The ILWIS 1.4 conversions will now also work from the script.
//
// Revision 1.2  1997/07/25 16:42:08  Wim
// Willem: import bugs
//
/* SegmentMapImport
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WN   25 Jul 97    4:52 pm
*/
#ifndef ILWSEGIMP_H
#define ILWSEGIMP_H
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Base\DataObjects\Tranq.h"

class SegmentMapImport: public SegmentMapPtr
{
public:
  static void _export import(const FileName&, const FileName& fnOut = FileName());
  static void _export import(const FileName& fn, const FileName& fnNew, DomainType dmt, const FileName& fnDom, 
                     const ValueRange& vr, const CoordSystem& cs,
                     const String& sDesc = ""); 
  virtual void Store();
  void WriteAlfaBeta(const FileName&);
  SegmentMapImport(const FileName& fn, const CoordSystem& cs,
                   const CoordBounds& cb, const Domain& dm)
  : SegmentMapPtr(fn,cs,cb,dm) { }
  static void StoreSegCodes(const SegmentMapPtr*, Tranquilizer& trq);
  static void _export GetImportInfo(const FileName& fn, DomainType& dmt, ValueRange& vr,
                                     ArrayLarge<String>& aSegCodes, Tranquilizer* trq=0);
private:  
  virtual long iRaw(const Coord&) const { return iUNDEF; }
  virtual long iSeg() const { return iUNDEF; }
  virtual ILWIS::Segment *segFirst() const { return NULL; }
  virtual ILWIS::Segment *segLast() const { return NULL; }
  virtual ILWIS::Segment *segFirstAcceptDeleted() const { return NULL; }
  virtual ILWIS::Segment *seg(long iNr) const { return NULL; }
  virtual long iNode(RowCol) const { return 0; }
  virtual RowCol rcNode(RowCol) const { return RowCol(); }
  virtual RowCol rcCoor(RowCol, ILWIS::Segment* seg, long& iNr) const { return RowCol(); }
  virtual RowCol rcPoint(RowCol, ILWIS::Segment* seg, long& iAft,
		 bool fAcceptDeleted = 0) const { return RowCol(); }
};  

#endif // ILWSEGIMP_H


