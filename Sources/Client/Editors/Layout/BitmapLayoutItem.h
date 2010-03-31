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
// BitmapLayoutItem.h: interface for the BitmapLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPLAYOUTITEM_H__B73121A6_09FD_11D4_B88C_00A0C9D5342F__INCLUDED_)
#define AFX_BITMAPLAYOUTITEM_H__B73121A6_09FD_11D4_B88C_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class BitmapLayoutItem: public LayoutItem  
{
public:
	BitmapLayoutItem(LayoutDoc* ld);
	BitmapLayoutItem(LayoutDoc* ld, HBITMAP hnd);
	virtual ~BitmapLayoutItem();
	virtual void Serialize(CArchive& ar, const char* sSection); // calls by default ReadElements or WriteElements
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual void OnDraw(CDC* cdc);
	virtual bool fConfigure();
	virtual String sType() const;
	virtual String sName() const;
	virtual bool fAddExtraClipboardItems();
	virtual bool fIsotropic() const;
	virtual double rHeightFact() const;
	HANDLE DibFromBitmap ( HBITMAP hb, HDC dc)	;
	WORD PaletteSize (VOID FAR * pv);
	BOOL StretchDibBlt (HDC hdc, zRect rct, LONG rop);
	WORD DibNumColors (VOID FAR * pv);
protected:
	CBitmap bm;
	bool m_fIsotropic; 
};

#endif // !defined(AFX_BITMAPLAYOUTITEM_H__B73121A6_09FD_11D4_B88C_00A0C9D5342F__INCLUDED_)
