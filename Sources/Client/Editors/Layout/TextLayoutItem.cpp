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
// TextLayoutItem.cpp: implementation of the TextLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Headers\Hs\Layout.hs"
#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(TextLayoutItem, LayoutItem)
	//{{AFX_MSG_MAP(TextLayoutItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


TextLayoutItem::TextLayoutItem(LayoutDoc* ld)
	: LayoutItem(ld)
{
	MinMax mm = mmPosition();
	mm.MaxCol() = mm.MinCol() + 800;
	LayoutItem::SetPosition(mm,-1);
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = 12;
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lstrcpy(lf.lfFaceName, "Arial");
	eAlignment = eLEFT;
}

TextLayoutItem::TextLayoutItem(LayoutDoc* ld, const String& str)
	: LayoutItem(ld)
{
	sText = str;
	// remove tabs because they often show wrong
//	for (char* s = sText.sVal(); *s; ++s) 
//		if ('\t' == *s)
//			*s = ' ';
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = 16;
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lstrcpy(lf.lfFaceName, "Arial");
	eAlignment = eLEFT;
}

TextLayoutItem::~TextLayoutItem()
{
}

int TextLayoutItem::iFontSize() const 
{ 
	return (int)(abs(lf.lfHeight * 254.0 / 72)); // 25.4 mm per inch, 72 points per inch
}

TextLayoutItem::InitFont::InitFont(const TextLayoutItem* tli, CDC* cdc)
: dc(cdc)
{
	init(tli->lf, tli->clr, tli->fTransparent);
}

TextLayoutItem::InitFont::InitFont(LOGFONT lf, Color clr, CDC* cdc, int iHorzRot)
: dc(cdc)
{
	lf.lfOrientation = iHorzRot * 10;
	lf.lfEscapement = iHorzRot * 10;	
	init(lf, clr, true);
}


void TextLayoutItem::InitFont::init(LOGFONT lf, Color clr, bool fTransparent)
{
	lf.lfHeight *= 254.0 / 72; // 25.4 mm per inch, 72 points per inch
	fnt.CreateFontIndirect(&lf);
	fntOld = dc->SelectObject(&fnt);

  if (fTransparent)
    dc->SetBkMode(TRANSPARENT);
  else
		dc->SetBkMode(OPAQUE);
	dc->SetBkColor(RGB(255,255,255));
	dc->SetTextColor(clr);
}

TextLayoutItem::InitFont::~InitFont()
{
	dc->SelectObject(fntOld);
}

void TextLayoutItem::Setup()
{
	if (!fInitialized) {
		CWindowDC cdc(CWnd::GetDesktopWindow());
		InitFont init(this,&cdc);
		CString str = sText.c_str();
		CRect rect = rectPos();
		cdc.DrawText(str, &rect, DT_EXPANDTABS|DT_WORDBREAK|DT_CALCRECT);
		CSize sz = rect.Size();
		sz.cx += 1;
		sz.cy += 1;

		MinMax mm = mmPosition();
		mm.MaxCol() = mm.MinCol() + sz.cx;
		mm.MaxRow() = mm.MinRow() + sz.cy;
		LayoutItem::SetPosition(mm,-1);
	}
}

void TextLayoutItem::OnDraw(CDC* cdc)
{
	InitFont init(this,cdc);
	cdc->SetTextAlign(TA_LEFT|TA_TOP);
	CString str = sText.c_str();
	CRect rect = rectPos();
	//unsigned int iStyle = DT_NOPREFIX | DT_TABSTOP;
	unsigned int iStyle = DT_NOPREFIX | DT_EXPANDTABS;
	switch (eAlignment) 
	{
		case eLEFT:   iStyle |= DT_LEFT; break;
		case eCENTER: iStyle |= DT_CENTER; break;
		case eRIGHT:  iStyle |= DT_RIGHT; break;
	}
	if (lf.lfEscapement != 0)
		iStyle |= DT_NOCLIP;
	else
		iStyle |= DT_WORDBREAK;

	cdc->DrawText(str, &rect, iStyle);
	fInitialized = true;
}

void TextLayoutItem::DrawSingleLine(CDC* cdc)
{
	InitFont init(this,cdc);
	cdc->SetTextAlign(TA_LEFT|TA_TOP);
	CString str = sText.c_str();
	CRect rect = rectPos();
	CPoint pt = rect.TopLeft();
	CSize sz = cdc->GetTextExtent(str);

	MinMax mm = mmPosition();
	mm.MaxCol() = mm.MinCol() + sz.cx;
	mm.MaxRow() = mm.MinRow() + sz.cy;
	LayoutItem::SetPosition(mm,-1);

	cdc->TextOut(pt.x, pt.y, str);
	fInitialized = true;
}

bool TextLayoutItem::fIsotropic() const
{
	return false;
}

void TextLayoutItem::SetPosition(MinMax mm, int iHit)
{
	switch (iHit) {
		case CRectTracker::hitTopLeft:
		case CRectTracker::hitTopRight:
		case CRectTracker::hitBottomLeft:
		case CRectTracker::hitBottomRight:
		{
			MinMax mmOldPos = mmPosition();
			double rFact = double(mm.width()) / mmOldPos.width();
			lf.lfHeight *= rFact;
		} break;
/*
		case CRectTracker::hitLeft:
		case CRectTracker::hitRight:
			break;
		case CRectTracker::hitTop:
		case CRectTracker::hitBottom:
			LayoutItem::SetPosition(mm, iHit);
			Setup();
			return;
*/
	}
	LayoutItem::SetPosition(mm, iHit);
}

class TextLayoutItemForm : public FormWithDest
{
public:
  TextLayoutItemForm(CWnd* wnd, TextLayoutItem* tli, String& sText) 
		: FormWithDest(wnd, TR("Edit Text"))
  {
      fsm = new FieldStringMulti(root, &sText);
      fsm->SetWidth(200);
      fsm->SetHeight(50);
      fsm->SetIndependentPos();

			RadioGroup* rg = new RadioGroup(root, TR("&Alignment"), (int*)&tli->eAlignment, true);
			rg->SetIndependentPos();
			new RadioButton(rg, TR("&Left"));
			new RadioButton(rg, TR("&Center"));
			new RadioButton(rg, TR("&Right"));

      FieldColor *fc = new FieldColor(root, TR("&Color"), &tli->clr);
			new FieldLogFont(root, &tli->lf, FieldLogFont::faROTATION);
	    new CheckBox(root, TR("&Transparent"), &tli->fTransparent);

      SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_text.htm");
      create();
  }
  FormEntry* feDefaultFocus() {
    return fsm;
  }
  FieldStringMulti* fsm;
};


bool TextLayoutItem::fConfigure()
{
	int iFontSize = lf.lfHeight;
	TextLayoutItemForm frm(ld->wndGetActiveView(), this, sText);
	if (sText == " ")
		return false;
	if (frm.fOkClicked()) {
		double rFact = abs((double)lf.lfHeight / iFontSize);
		if (1 != rFact) {
			MinMax mm = mmPosition();
			mm.MaxCol() = mm.MinCol() + rounding(mm.width() * rFact);
			mm.MaxRow() = mm.MinRow() + rounding(mm.height() * rFact);
			LayoutItem::SetPosition(mm,-1);
		}
		Setup();
		return true;
	}
	return false;
}

void TextLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::ReadElements(en, sSection);
	ObjectInfo::ReadElement(sSection, "TextColor", en, clr);
	ObjectInfo::ReadElement(sSection, "Transparency", en, fTransparent);
	String sAlignment;
	ObjectInfo::ReadElement(sSection, "Alignment", en, sAlignment);
	if ("Center" == sAlignment)
		eAlignment = eCENTER;
	else if ("Right" == sAlignment)
		eAlignment = eRIGHT;
	else
		eAlignment = eLEFT;
	String sFont;
	ObjectInfo::ReadElement(sSection, "Font", en, sFont);
	lstrcpy(lf.lfFaceName, sFont.c_str());
	int iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::ReadElement(sSection, "LogFont", en, (char*)&lf, iLen);
	iLen = 0;
	ObjectInfo::ReadElement(sSection, "Text Length", en, iLen);
	if (iLen > 0) {
		char* str = new char[iLen];
		ObjectInfo::ReadElement(sSection, "Text", en, str, iLen);
		sText = str;
		delete [] str;
	}
	else
		sText = "";
}

void TextLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::WriteElements(en, sSection);
	ObjectInfo::WriteElement(sSection, "TextColor", en, clr);
	ObjectInfo::WriteElement(sSection, "Transparency", en, fTransparent);
	String sAlignment;
	switch (eAlignment) 
	{
		case eLEFT: sAlignment = "Left"; break;
		case eCENTER: sAlignment = "Center"; break;
		case eRIGHT: sAlignment = "Right"; break;
	}
	ObjectInfo::WriteElement(sSection, "Alignment", en, sAlignment);
	ObjectInfo::WriteElement(sSection, "Font", en, lf.lfFaceName);
	long iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::WriteElement(sSection, "LogFont", en, (char*)&lf, iLen);
	iLen = 1+sText.length();
	ObjectInfo::WriteElement(sSection, "Text", en, sText.c_str(), iLen);
	ObjectInfo::WriteElement(sSection, "Text Length", en, iLen);
}

String TextLayoutItem::sType() const
{
	return "Text";
}

bool TextLayoutItem::fAddExtraClipboardItems()
{
	if ("" != sText) {
		int iLen = sText.length();
		HGLOBAL hnd = GlobalAlloc(GMEM_FIXED, iLen+2);
		char* pc = (char*)GlobalLock(hnd);
		strcpy(pc,sText.c_str());
		GlobalUnlock(hnd);
		SetClipboardData(CF_TEXT,hnd);
		return false;
	}
	return true;
}

String TextLayoutItem::sName() const
{
	String str = sText;
	for (char* c = str.sVal(); *c; ++c)
		if (*c < 0x20)
			*c = ' ';
	return str;	
}

Color TextLayoutItem::clrTextColor() const
{
	return clr;
}

bool TextLayoutItem::fTransparency() const
{
	return fTransparent;
}

void TextLayoutItem::SetTextColor(Color c) 
{
	clr =c;
}

void TextLayoutItem::SetTransparency(bool fT) 
{
	fTransparent = fT;
}

