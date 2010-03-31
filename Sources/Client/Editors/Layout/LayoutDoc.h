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
#if !defined(AFX_LAYOUTDOC_H__26187CA0_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
#define AFX_LAYOUTDOC_H__26187CA0_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutDoc.h : header file
//

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

class MapCompositionDoc;
class MapCompositionSrvItem;
class LayoutItem;
class LayoutView;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// LayoutDoc document

class IMPEXP LayoutDoc : public IlwisDocument
{
public:
	enum Hints { hintITEM = 1, hintITEMSIZE, hintPAGEBORDER };
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LayoutDoc)
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL
  virtual zIcon icon() const;
	void AddItem(LayoutItem* li);
	LayoutItem* liAddMapView(const String& sMapView);
	LayoutItem* liAddPicture(const String& sPicture);
	LayoutItem* liFindID(int iID) const;				
	void RemoveItem(LayoutItem* li); // will also delete li
//	void SetPrinterDC(HGLOBAL hDevNames, HGLOBAL hDevMode);
	void CalcBitmapSize(int iDpi, int& iWidth, int& iHeight, int& iFileSize); 
	CSize szGetPrintableSize() const;
	bool fIsEmpty() const;
	CDC dcPrinter;
protected:
	LayoutDoc();           // protected constructor used by dynamic creation
	virtual ~LayoutDoc();
	virtual void DeleteContents();
	//{{AFX_MSG(LayoutDoc)
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnAddTextItem();
	afx_msg void OnAddMapView();
	afx_msg void OnAddPicture();
	afx_msg void OnAddBox();
	afx_msg void OnPageBorder();
	afx_msg void OnUpdatePageBorder(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	HENHMETAFILE ReadMetaFile(const char* str) const;

	CSize szPaper; // paper size in 0.01 mm units
	CRect rectMargins; // margins in 0.01 mm units
  list<LayoutItem*> lli;
	bool fPageBorder;
	CMultiDocTemplate dtViews;

	HGLOBAL m_hDevMode;             // printer Dev Mode
	HGLOBAL m_hDevNames;            // printer Device Names

	DECLARE_DYNCREATE(LayoutDoc)
	DECLARE_MESSAGE_MAP()
	friend class LayoutView;
	friend class LayoutListView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTDOC_H__26187CA0_F4C7_11D3_B871_00A0C9D5342F__INCLUDED_)
