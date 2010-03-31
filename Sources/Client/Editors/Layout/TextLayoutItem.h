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
// TextLayoutItem.h: interface for the TextLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTLAYOUTITEM_H__42F0FC94_F907_11D3_B875_00A0C9D5342F__INCLUDED_)
#define AFX_TEXTLAYOUTITEM_H__42F0FC94_F907_11D3_B875_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef FIELDFONTNAME_H
#include "Client\FormElements\fldfontn.h"
#endif


class LayoutDoc;
class TextLayoutItemForm;

class TextLayoutItem: public LayoutItem   
{
public:
	TextLayoutItem(LayoutDoc* ld);
	TextLayoutItem(LayoutDoc* ld, const String& sText);
	virtual ~TextLayoutItem();
	virtual void ReadElements(ElementContainer&, const char* sSection);
	virtual void WriteElements(ElementContainer&, const char* sSection);
	virtual void OnDraw(CDC* cdc);
	void DrawSingleLine(CDC* cdc);
	virtual bool fIsotropic() const;
	virtual void SetPosition(MinMax mm, int iHit);
	virtual bool fConfigure();
	virtual String sType() const;
	virtual String sName() const;
	bool fTransparency() const;
	Color clrTextColor() const;
	void SetTextColor(Color c) ;
	void SetTransparency(bool fT) ;
	virtual bool fAddExtraClipboardItems();
	int iFontSize() const;
	FieldLogFont* flfAskLogFont(FormEntry* fe)
		{ return new FieldLogFont(fe, &lf); }

	class InitFont
	{
	public:
		InitFont(const TextLayoutItem* tli, CDC* cdc);
		InitFont(LOGFONT lf, Color clr, CDC* cdc, int iRot=0);
		~InitFont();
	private:
		void init(LOGFONT lf, Color clr, bool fTransparent);
		CDC* dc;
		CFont fnt;
		CFont* fntOld;
	};
protected:
	virtual void Setup();
	String sText;
	LOGFONT lf;
	Color clr;
	bool fTransparent;
	enum { eLEFT, eCENTER, eRIGHT } eAlignment;
	DECLARE_MESSAGE_MAP()
	friend class TextLayoutItemForm;
	friend class InitFont;
};

#endif // !defined(AFX_TEXTLAYOUTITEM_H__42F0FC94_F907_11D3_B875_00A0C9D5342F__INCLUDED_)
