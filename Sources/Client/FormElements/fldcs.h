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
/* FieldCoordSystem
   by Wim Koolhoven, october 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   20 Apr 98   11:57 am
*/
   
#ifndef FIELDCS_H
#define FIELDCS_H

#ifndef OBJLIST_H
#include "Client\FormElements\objlist.h"
#endif

class FieldCoordSystemC: public FieldDataTypeC
{
public:
  _export FieldCoordSystemC(FormEntry* fe, const String& sQuestion, 
                    String* sCoordSystem)
  : FieldDataTypeC(fe, sQuestion, sCoordSystem, ".CSY", true, 
     (NotifyProc)&FieldCoordSystemC::CreateCoordSystem)
	, csTypes(-1)
  { sNewName = *sCoordSystem; }
  _export FieldCoordSystemC(FormEntry* fe, const String& sQuestion, 
              String* sCoordSystem, long types)
  : FieldDataTypeC(fe, sQuestion, sCoordSystem, new CoordSystemLister(types), true,
     (NotifyProc)&FieldCoordSystemC::CreateCoordSystem)
	, csTypes(types)
  { sNewName = *sCoordSystem; }
  void SetBounds(const CoordBounds& cb) { m_cb = cb;}
private:  
  String sNewName;
  CoordBounds m_cb;
  long csTypes;
  int _export CreateCoordSystem(void *);
};

class FormCreateCoordSystem: public FormWithDest
{
public:
  _export FormCreateCoordSystem(CWnd* wPar, String* sCS, long csTypes=-1);
  _export ~FormCreateCoordSystem();
private:    
  int exec();
  int CallBackName(Event *);
//  int CallBackRadio(Event *);
//  void HideAllGroups();
  FieldDataTypeCreate* fcs;  
  String* sCoordSystem;
  String sNewName;
  StaticText* stRemark;
  RadioGroup* rg;
//  FieldGroup* fgFormu;
//  FieldGroup* fgCTP;
//  FieldGroup* fgDirLin;
//  FieldGroup* fgOrthoPh;
//  FieldCoordSystem *fcsRelCsy0, *fcsRelCsy1;			// 0 = formula; 
//  FieldDataType *fdtRefMap1, *fdtRefMap2, *fdtRefMap3;	// 1 = tiepoints (planar)
//  FieldDataType *fdtDtmMap2, *fdtDtmMap3;				// 2 = dirlin; 3 = orthopho
  CWnd* wParent;
  String sDescr, sRefMap, sRelCsy0, sRelCsy1, sDTM;
  int iOption;
	long csTypes;
};

class FormCreateCoordSystemMW: public FormWithDest
{
public:
  _export FormCreateCoordSystemMW(CWnd* wPar, String* sCS);
  _export ~FormCreateCoordSystemMW();
private:    
  int exec();
  int CallBackName(Event *);
//  int CallBackRadio(Event *);
  FieldDataTypeCreate* fcs;  
  String* sCoordSystem;
  String sNewName;
	BaseMap map;
  StaticText* stRemark;
  RadioGroup* rg;
//	FieldGroup* fgFormu;
//  FieldGroup* fgCTP;
//  FieldGroup* fgDirLin;
//  FieldGroup* fgOrthoPh;
//	FieldCoordSystem *fcsRelCsy0, *fcsRelCsy1;			// 0 = formula; 
//  FieldDataType *fdtRefMap1, *fdtRefMap2, *fdtRefMap3;	// 1 = tiepoints (planar)
//  FieldDataType *fdtDtmMap2, *fdtDtmMap3;				// 2 = dirlin; 3 = orthopho
  CWnd* wParent;
  String sDescr, sRefMap, sRelCsy0, sRelCsy1, sDTM;
  int iOption;
};

#endif // FIELDCS_H




