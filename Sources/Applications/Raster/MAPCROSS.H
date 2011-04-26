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
/* MapCross
   Copyright Ilwis System Development ITC
   jan 1996, by Jelle Wind
   Last change:  JEL   7 May 97   11:27 am
*/

#ifndef ILWMAPCROSS_H
#define ILWMAPCROSS_H
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapCross(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapCross(ApplicationQueryData *query);

class DATEXPORT MapCross: public MapFromMap
{
  friend MapFromMap;
public:  
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapCross* create(const FileName& fn, MapPtr& p, const String& sExpr);
  MapCross(const FileName& fn, MapPtr& p);
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
protected:
  virtual void Store();
  MapCross(const FileName& fn, MapPtr& p, const Map& map1, const Map& map2, const String& sCrossTbl);
  ~MapCross();
private:
  Map map2;
  String sCrossTbl;
};

#endif




