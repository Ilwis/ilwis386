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
// LayoutItem.h: interface for the LayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYOUTITEM_H__0E66FD29_F668_11D3_B873_00A0C9D5342F__INCLUDED_)
#define AFX_LAYOUTITEM_H__0E66FD29_F668_11D3_B873_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LayoutDoc;

class LayoutItem: public CCmdTarget  
{
public:
	LayoutItem(LayoutDoc* ld);
	virtual ~LayoutItem();
	virtual void Serialize(CArchive& ar, const char* sSection); // calls by default ReadElements or WriteElements
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual String sType() const;
	virtual String sName() const;
	void SetID(int iID);
	int iID() const;
	virtual void OnDraw(CDC* cdc);
	virtual bool fIsotropic() const;
	virtual double rHeightFact() const;
	virtual void SetPosition(MinMax mm, int iHit);	// in 0.1 mm units
	MinMax mmPosition() const 
		{ return mmPos; }
	CRect rectPos() const; // return position in MetaFile units
	virtual bool fOnChangedItemSize(LayoutItem*);
	virtual bool fDependsOn(LayoutItem*);
	virtual bool fClip() const;
	virtual bool fConfigure();
	virtual bool fAddExtraClipboardItems(); // return whether to copy picture
	afx_msg void OnItemEdit();
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual double rScale() const;   // 1:....
	virtual void SetScale(double rScale);
	virtual void SaveModified();
	bool fInitialized;
protected:
	LayoutDoc* ld;
private:
	int ID;
	MinMax mmPos; // in 0.01 mm units
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_LAYOUTITEM_H__0E66FD29_F668_11D3_B873_00A0C9D5342F__INCLUDED_)
