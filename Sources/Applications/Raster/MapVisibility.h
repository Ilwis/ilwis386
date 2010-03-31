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
/* MapVisibility
   Copyright Ilwis System Development ITC
   august 2001, by Jan Hendrikse
*/

#ifndef ILWMAPVISIBILITY_H
#define ILWMAPVISIBILITY_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapVisibility(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapVisibility: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:  
  static MapVisibility* create(const FileName& fn, MapPtr&, const String& sExpression);
  MapVisibility(const FileName& fn, MapPtr&);
  static const char* sSyntax();  
protected:
  virtual void Store();
  MapVisibility(const FileName& fn, MapPtr&, const Map& mp, 
            const Coord& crd, const double& rH);
  ~MapVisibility();
	virtual bool fFreezing();
  virtual String sExpression() const;
private:
  void Init();
  Coord crdObserver;
  double rHeightObserver;
};

#endif //ILWMAPVISIBILITY_H




