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
// CoordSysEditor.h: interface for the CoordSysEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COORDSYSEDITOR_H__242AF8DA_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_)
#define AFX_COORDSYSEDITOR_H__242AF8DA_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CoordSystemCTP;;
class CoordSystemTableDoc;
class AddTiePointCsyForm;
class EditFiducialMarksForm;


class CoordSystemEditor: public TiePointEditor    
{
	friend class CseConfigForm;
	friend class AddTiePointCsyForm;
public:
	CoordSystemEditor(MapPaneView*, CoordSystem);
	virtual ~CoordSystemEditor();
  virtual IlwisObject obj() const;
  virtual int draw(volatile bool* fDrawStop);
	virtual zIcon icon() const;
	virtual String sTitle() const;
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
	virtual bool OnLButtonDown(UINT nFlags, CPoint point);
	virtual LRESULT OnUpdate(WPARAM, LPARAM);
  void EditFieldCoordOK(const Coord& crd, const Coord& crdRef);
	void EditFieldCoordOK(const Coord& crd, const Coord& crdRef, double rZ);
  void Calc();
  void Redraw();
	void DelPoints(int iDel, int iRecs); 
protected:
  zRect rectPoint(long iNr);
  int drawPoint(long iNr);
	CoordSystem cs;
  CoordSystemCTP* csctp;
	TableDoc* cstd; 
  Column colDX, colDY, colZdem, colZdiff;
	Map mapDTM;
	double rSigma;
	bool fLatLon;
  AddTiePointCsyForm *atpf;
  EditFiducialMarksForm* efmf;
public:
	void OnTransfChanged();
//{{AFX_MSG(CoordSysEditor)
	afx_msg void OnAddPoint();
	afx_msg void OnDelPoint();
	afx_msg void OnTransformation();
	afx_msg void OnConfigure();
	afx_msg void OnEditFiducialMarks();
	afx_msg void OnStopFiducialMarks();
	afx_msg void OnBoundaries();
//}}AFX_MSG
private:
	bool fInCalc;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_COORDSYSEDITOR_H__242AF8DA_464E_11D3_B7B4_00A0C9D5342F__INCLUDED_)
