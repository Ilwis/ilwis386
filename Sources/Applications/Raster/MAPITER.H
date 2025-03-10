/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* MapIterator
   Copyright Ilwis System Development ITC
   july 1997, by Jelle Wind
   Last change:  WK   23 Sep 97    4:54 pm
*/

#ifndef ILWMAPITER_H
#define ILWMAPITER_H
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapIterator(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapIter(ApplicationQueryData *query);

class CALCEXPORT Instructions;

class DATEXPORT MapIterator: public MapFromMap
{
  friend class DATEXPORT MapFromMap;
public:
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapIterator* create(const FileName& fn, MapPtr&, const String& sExpr);
  MapIterator(const FileName& fn, MapPtr&);
protected:
  virtual void Store();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
 MapIterator(const FileName& fn, MapPtr&, const Map& mpStart, const String& sExpr, long iTimes, bool fProp);
  ~MapIterator();
private:
  void Init();
  bool fCalcIter(long& iChanges, bool fDown, long iLines, long iCols, Map& mpTmp);
  Map mpInit, mpCalc;
  long iTimes;
  String sIterExpr;
  Instructions* inst;
  bool fPropagation;
};

#endif




