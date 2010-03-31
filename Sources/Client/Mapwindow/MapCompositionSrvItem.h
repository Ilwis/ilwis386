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
#if !defined(AFX_MAPCOMPOSITIONSRVITEM_H__0A762784_B6C0_11D3_B835_00A0C9D5342F__INCLUDED_)
#define AFX_MAPCOMPOSITIONSRVITEM_H__0A762784_B6C0_11D3_B835_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapCompositionSrvItem.h : header file
//

class MapCompositionDoc;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// MapCompositionSrvItem document

class IMPEXP MapCompositionSrvItem : public COleServerItem
{
public:
	MapCompositionSrvItem(COleServerDoc* pServerDoc = NULL, BOOL bAutoDelete = FALSE);	// protected constructor used by dynamic creation
	virtual ~MapCompositionSrvItem();
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	MapCompositionDoc* GetDocument()
		{ return (MapCompositionDoc*)(COleServerItem::GetDocument()); }
	//{{AFX_VIRTUAL(MapCompositionSrvItem)
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnGetExtent(DVASPECT nDrawAspect, CSize& rSize);
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
	virtual BOOL OnDrawEx(CDC* pDC, DVASPECT nDrawAspect, CSize& rSize);
	virtual BOOL OnSetExtent(DVASPECT nDrawAspect, const CSize& size); 
	//}}AFX_VIRTUAL
	RowCol rcOffset() const;
	RowCol rcSize() const;
	void ResetSize();
	void ResetOffsetAndSize();
	void SetOffsetSize(RowCol rcOffset, RowCol rcSize);
protected:
	bool fViewPort;
	RowCol m_rcOffset, m_rcSize;
private:  
  HMODULE hmodMsimg32;
  typedef BOOL (WINAPI *AlphaBlendFunc)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
  AlphaBlendFunc alphablendfunc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPCOMPOSITIONSRVITEM_H__0A762784_B6C0_11D3_B835_00A0C9D5342F__INCLUDED_)
