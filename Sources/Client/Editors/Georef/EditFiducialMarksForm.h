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
// EditFiducialMarksForm.h: interface for the EditFiducialMarksForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITFIDUCIALMARKSFORM_H__0DCDE0C6_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_)
#define AFX_EDITFIDUCIALMARKSFORM_H__0DCDE0C6_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DIGREF_H
#include "Client\Editors\Digitizer\DIGREF.H"
#endif

class FieldFiducialMark;
class Positioner;
class MapPaneView;

class EditFiducialMarksForm: public FormWithDest
{
public:
	EditFiducialMarksForm(MapPaneView*, GeoRefOrthoPhoto*);
	EditFiducialMarksForm(MapPaneView*, CoordSystemOrthoPhoto*);
	virtual ~EditFiducialMarksForm();
  FieldFiducialMark* ffmCurr();
  FieldFiducialMark* ffmNext();
  int exec();
	void OnLButtonDownRC(Coord);
	void OnLButtonDown(Coord);
  int draw(CDC* cdc, zRect rect, Positioner* psn);
	int drawPrincPoint(CDC* cdc, zRect rect, Positioner* psn);
  void SetActDigPoint(FieldFiducialMark*);
	afx_msg HBRUSH OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	int ChangePDistCallBack(Event*);
	int ChangePPointCallBack(Event*);
	int TransformationChange(Event*);

  double x0, y0, x10, x01, y10, y01;
  Color colFidMarks;
  bool fGeoRef; // true if GeoRefOrtho, false if CoordSysOrtho
	FieldReal* frPrincDist;
	FieldCoord* fcPrincPoint;
	Coord cCameraPrincPmm;

private:
  int calc(Event*);
  int initFMF(Event*);
  void SetDirty(RowCol);
  void SetDirty(Coord);
	void SetDirty(zPoint p);

  StaticText *stRem1, *stRem2, *stRem3, *stRMSrow, *stRMScol;
  FieldFiducialMark *ffm[8];
  int iActDigPnt;
	MapPaneView* mpv;
	GeoRefOrthoPhoto* grf;
	CoordSystemOrthoPhoto* cs;
  Color clrNormal, clrActive, clrBackNormal, clrBackActive;
  CBrush *brNormal, *brActive;
  double rPrincDist;
	int iNrFiducialInputFields;
	int iTransformOption;
	RadioGroup* rgTransf;
	//enum TransfFidu {CONFORM, AFFINE, BILINEAR, PROJECTIVE} transfFidu;
};

#endif // !defined(AFX_EDITFIDUCIALMARKSFORM_H__0DCDE0C6_3538_11D4_B8BC_00A0C9D5342F__INCLUDED_)
