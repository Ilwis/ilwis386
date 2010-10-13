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
#if !defined(AFX_PixelInfoDoc_H__D84FBDE6_24D2_11D3_B77D_00A0C9D5342F__INCLUDED_)
#define AFX_PixelInfoDoc_H__D84FBDE6_24D2_11D3_B77D_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PixelInfoDoc.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

#include "Client\Mapwindow\RECITEM.H"
typedef Array<RecItem*> RecItemArray;

/////////////////////////////////////////////////////////////////////////////
// PixelInfoDoc document

class IMPEXP PixelInfoDoc : public IlwisDocument
{
  friend class PixelInfoConfigureForm;
protected:

public:
	PixelInfoDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(PixelInfoDoc)
  int iSize() const;
  String sName(int) const;
  String sValue(int) const;
	zIcon icon() const;
  FileName fn(int) const;
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
protected:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
public:
	virtual ~PixelInfoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// Generated message map functions
  LRESULT OnUpdate(WPARAM, LPARAM);
	//{{AFX_MSG(PixelInfoDoc)
	afx_msg void OnPixConfigure();
	afx_msg void OnAddMaps();
	afx_msg void OnAddCsys();
	afx_msg void OnAddGrf();
	//}}AFX_MSG
	void AddMap(const BaseMap&);
	void AddMapList(const MapList&);
	void AddCoordSystem(const CoordSystem&);
	void AddGeoRef(const GeoRef&);
	void Update();
  bool fMouse, fMouseCont, fDigitizer, fDigitizerCont;
	DECLARE_MESSAGE_MAP()
  RecItemCoord riCoord;
  RecItemArray riArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PixelInfoDoc_H__D84FBDE6_24D2_11D3_B77D_00A0C9D5342F__INCLUDED_)
