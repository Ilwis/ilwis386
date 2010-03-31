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
// FieldFiducialMark.h: interface for the FieldFiducialMark class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELDFIDUCIALMARK_H__0DCDE0C7_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_)
#define AFX_FIELDFIDUCIALMARK_H__0DCDE0C7_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FieldFiducialMark: public FieldGroup  
{
public:
  FieldFiducialMark(FormEntry* par, int iNr, const bool fGrf);
  ~FieldFiducialMark();
  void create();
  FormEntry* CheckData();
  void SetCallBack(NotifyProc np);
  void SetCallBack(NotifyProc np, CallBackHandler* evh);
  int FocusChange(Event*);
  void ShowVals();
	double& rRow() { return crdRowCol.x; } 
	double& rCol() { return crdRowCol.y; } 
	double& rDigGridX() { return crdDigGrid.x; } 
	double& rDigGridY() { return crdDigGrid.y; } 
	RowCol rc() const { return RowCol(crdRowCol.x, crdRowCol.y); }
  Coord c() const { return crdRowCol; }
	Coord cDG() const { return crdDigGrid; }
	Coord crdPhoto, crdRowCol, crdDigGrid;
  double rDRow, rDCol;
  bool fAct, fPhoto, fRowCol, fDigGrid, fOk;
  CheckBox* cb;
  FieldCoord* fcPhoto;
  FieldCoord* frc;
	FieldCoord* fcDigGrid;
  StaticTextSimple *stDRow, *stDCol;
};

#endif // !defined(AFX_FIELDFIDUCIALMARK_H__0DCDE0C7_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_)
