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
// PointMapDrawerForm.h: interface for the PointMapDrawerForm class.
//
//////////////////////////////////////////////////////////////////////

#pragma once;

class PointMapDrawerForm : public BaseMapDrawerForm  
{
  friend class PointMapSymbolForm;
  friend class PointMapSymbolStretchForm;
  friend class PointMapByAttributeForm;
  friend class PointMapArrowForm;
  friend class PointMapGraphForm;
public:
  PointMapDrawerForm(PointMapDrawer* mdr, bool fShowForm=true);
  void DomUseChanged();
  int exec();
private:
  int AttCallBack(Event*);
  int SelectFont(Event*);
  int Symbol(Event*);
  int ValueSymbol(Event*);
  int ByAttribute(Event*);
  int Arrow(Event*);
  int Graph(Event*);
  int iValueOption;
  int iClassOption;
  int iIdentOption;
  String sMask;
  ExtendedSymbol smb;
  bool fStretch;
  int iSize;
  RangeInt riStretch, riSize;
  RangeReal rrStretch;
  int iLinLog, iRadiusArea;
  PointMapDrawer* mdw;
  int iSymbolType; // Single, by Attribute, Special
};


