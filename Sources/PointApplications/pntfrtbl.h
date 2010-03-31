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
/* $Log: /ILWIS 3.0/PointMap/pntfrtbl.h $
 * 
 * 3     7-09-00 14:16 Leeuwen
 * //->/*
 * 
 * 2     1-09-00 12:11p Martin
 * Extended syntax to include coordinate columns
// Revision 1.1  1998/09/16 17:26:27  Wim
// 22beta2
//
/* PointMapFromTable
   Copyright Ilwis System Development ITC
   march 1998, by Wim Koolhoven
	Last change:  WK   20 Mar 98   12:54 pm
*/

#ifndef ILWPOINTMAPFROMTABLE_H
#define ILWPOINTMAPFROMTABLE_H
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Table\Col.h"

IlwisObjectPtr * createPointMapFromTable(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PointMapFromTable: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapFromTable(const FileName&, PointMapPtr&);
  PointMapFromTable(const FileName& fn, PointMapPtr&, const Table& tbl,
    const Column& colX, const Column& colY,
    const CoordSystem&, const DomainValueRangeStruct&,
    const Column& colAttr);
  PointMapFromTable(const FileName& fn, PointMapPtr&, const Table& tbl,
    const Column& colX, const CoordSystem&, const DomainValueRangeStruct&,
    const Column& colAttr);	
  ~PointMapFromTable();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual void InitFreeze();
  virtual long iRaw(long iRec) const;
  virtual long iValue(long iRec) const;
  virtual double rValue(long iRec) const;
  virtual String sValue(long iRec, short iWidth=-1, short iDec=-1) const;
  virtual Coord cValue(long iRec) const;
  virtual bool fDomainChangeable() const;
  static PointMapFromTable* create(const FileName&, PointMapPtr&, const String& sExpression);
protected:

  void Init();
  Table tbl;
  Column colX, colY, colCoord, colAttrib;
};

#endif




