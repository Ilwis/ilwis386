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
// GeoRefEditor.h: interface for the GeoRefEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOREFEDITOR_H__02578176_4326_11D3_B7AF_00A0C9D5342F__INCLUDED_)
#define AFX_GEOREFEDITOR_H__02578176_4326_11D3_B7AF_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GeoRefCTP;
class GeoRefTableDoc;
class AddTiePointForm;
class EditFiducialMarksForm;

class GeoRefEditor: public TiePointEditor  
{
	friend class AddTiePointForm;
	friend class GreConfigForm;
public:
	GeoRefEditor(MapPaneView*, GeoRef);
	virtual ~GeoRefEditor();
  virtual IlwisObject obj() const;
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
	virtual zIcon icon() const;
	virtual String sTitle() const;
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual LRESULT OnUpdate(WPARAM, LPARAM);
	 void EditFieldCoordOK(RowCol, Coord);
  void EditFieldCoordOK(RowCol, Coord, double rZ);
  void EditFieldLatLonOK(RowCol rc, LatLon ll);
  void EditFieldLatLonOK(RowCol rc, LatLon ll, double rZ);
  void EditFieldCoordOK(Coord, Coord);
  void EditFieldCoordOK(Coord, Coord, double rZ);
  void EditFieldLatLonOK(Coord crdRC, LatLon ll);
  void EditFieldLatLonOK(Coord crdRC, LatLon ll, double rZ);
  void Calc();
  void Redraw();
	void DelPoints(int iDel, int iRecs); 
protected:
  int drawPoint(long iNr);
  zRect rectPoint(long iNr);
	GeoRef grf;
  GeoRefCTP* grc;
	TableDoc* grtd; 
  Column colDRow, colDCol, colZdem, colZdiff;
  Map mapDTM;
	double rSigma;
  bool fLatLon;
  AddTiePointForm *atpf;  
  EditFiducialMarksForm* efmf;
//{{AFX_VIRTUAL(GeoRefEditor)
//}}AFX_VIRTUAL
public:
	void OnTransfChanged();
//{{AFX_MSG(GeoRefEditor)
	afx_msg void OnAddPoint();
	afx_msg void OnDelPoint();
	afx_msg void OnTransformation();
	afx_msg void OnConfigure();
	afx_msg void OnEditFiducialMarks();
	afx_msg void OnStopFiducialMarks();
//}}AFX_MSG
private:
	bool fInCalc;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GEOREFEDITOR_H__02578176_4326_11D3_B7AF_00A0C9D5342F__INCLUDED_)
