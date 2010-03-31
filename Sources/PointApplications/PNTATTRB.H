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
/* PointMapAttribute
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   6 May 97    5:55 pm
*/

#ifndef ILWPOINTMAPATTR_H
#define ILWPOINTMAPATTR_H
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Table\Col.h"

IlwisObjectPtr * createPointMapAttribute(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PointMapAttribute: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapAttribute(const FileName&, PointMapPtr&);
  PointMapAttribute(const FileName& fn, PointMapPtr&, const PointMap& pm, const String& sColAtt);
  ~PointMapAttribute();
  static PointMapAttribute* create(const FileName&, PointMapPtr&, const String& sExpression);
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual void InitFreeze();
  virtual long iRaw(long iRec) const;
  virtual long iValue(long iRec) const;
  virtual double rValue(long iRec) const;
  virtual String sValue(long iRec, short iWidth=-1, short iDec=-1) const;
  virtual Coord cValue(long iRec) const;

  virtual long iPnt() const;

  virtual bool fDomainChangeable() const;
protected:
//  static PointMapAttribute* create(const FileName&);
  void Init();
  PointMap pmp;
  Column colAtt;
  Table tblAttrb;
  LongArrayLarge aiAttRaw;
  LongArrayLarge aiAttVal;
  RealArrayLarge arAttVal;
  bool fFastAccess;
};

#endif




