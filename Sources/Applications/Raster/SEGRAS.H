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
/* MapRasterizeSegment
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  JEL   7 May 97    9:37 am
*/

#ifndef ILWMAPSEGRAS_H
#define ILWMAPSEGRAS_H
#include "Engine\Applications\MAPFSEG.H"
#include "Engine\Base\DataObjects\Patch.h"

IlwisObjectPtr * createMapRasterizeSegment(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapRasterizeSegment: public MapFromSegmentMap
{
  friend class DATEXPORT MapVirtual;  
  friend class DATEXPORT MapFromSegmentMap;  
  friend class DATEXPORT MapInterpolContour;
public:
  static const char* sSyntax();
  virtual void Store();
  static MapRasterizeSegment* create(const FileName&, MapPtr& p, const String& sExpression);
  MapRasterizeSegment(const FileName&, MapPtr& p);
protected:
  MapRasterizeSegment(const FileName&, MapPtr& p, const SegmentMap& sm, 
                const GeoRef& gr, const bool&);
  ~MapRasterizeSegment();
  virtual String sExpression() const;
  virtual long iComputePixelRaw(RowCol) const;
  virtual double rComputePixelVal(RowCol) const;
  virtual bool fFreezing();
private:
  void Rasterize(RowCol begin, RowCol end, vector<RowCol>& result,RowCol rcSize);
  void *CreateMapBuffers();
  void PutInMapBuffer(void *buf, const vector<RowCol>& result, const ILWIS::Segment* seg);
  void StoreMapBuffer(void *buffer);
  bool inBounds(RowCol cur, RowCol rcSize);
 // bool fInitFill(Map&);
 // void CalcPatchBounds(const long, const long);
 // bool fInPatch(const ILWIS::Segment*);
 // bool fLineInPatch(RowCol, RowCol);
 // bool fClipOk(long&, long&, long);
 ////Coord cTopLeft, cBotRight, cTopRight, cBotLeft;
 // MinMax mm;//, mmIntern;
 // bool f8Conn;
};

#endif




