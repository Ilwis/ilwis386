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
#if !defined(AFX_PIXELINFOVIEW_H__6848AD61_24D1_11D3_B77D_00A0C9D5342F__INCLUDED_)
#define AFX_PIXELINFOVIEW_H__6848AD61_24D1_11D3_B77D_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PixelInfoView.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class COleDropTarget;

/////////////////////////////////////////////////////////////////////////////
// PixelInfoView view

class IMPEXP PixelInfoView : public BaseTablePaneView
{
protected:
	// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(PixelInfoView)
public:
	PixelInfoView();
	PixelInfoDoc* GetDocument();
	const PixelInfoDoc* GetDocument() const;
public:
	virtual void InitColPix(CDC* cdc);
	virtual int iCols() const;
	virtual long iRows() const;
	virtual String sColButton(int iCol) const;
	virtual String sRowButton(long iRow) const;
	virtual void OnInitialUpdate();
protected:
	virtual String sField(int iCol, long iRow) const;
	virtual String sDescrULButton() const; // upper left button
	virtual void OnULButtonPressed();  // upper left button
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
	virtual ~PixelInfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	COleDropTarget* odt;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXELINFOVIEW_H__6848AD61_24D1_11D3_B77D_00A0C9D5342F__INCLUDED_)
