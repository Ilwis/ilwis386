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
// NorthArrowLayoutItem.cpp: implementation of the NorthArrowLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\NorthArrowLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Client\Editors\Utils\line.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(NorthArrowLayoutItem, TextLayoutItem)
	//{{AFX_MSG_MAP(NorthArrowLayoutItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#undef Polygon


NorthArrowLayoutItem::NorthArrowLayoutItem(LayoutDoc* ld, MapLayoutItem* mapli)
	: TextLayoutItem(ld)
	, mli(mapli)
{
	MinMax mm = mli->mmPosition();
	mm.MaxRow() = mm.MinRow() + 400;
	mm.MinCol() -= 250; // 2.5 cm left of map.
	mm.MaxCol() = mm.MinCol() + 400;
	LayoutItem::SetPosition(mm,-1);
	eType = eONEARROW;
	fText = true;
	fAllSides = false;
	fHalf = false;
	fCircle = false;
	eRose = e8;
	rLineWidth = 0;
}

NorthArrowLayoutItem::NorthArrowLayoutItem(LayoutDoc* ld)
	: TextLayoutItem(ld), mli(0)
{
}

NorthArrowLayoutItem::~NorthArrowLayoutItem()
{
}

void NorthArrowLayoutItem::OnDraw(CDC* cdc)
{
	InitFont init(this,cdc);
	cdc->SetTextAlign(TA_LEFT|TA_TOP);

	CPen pen(PS_SOLID,3,clr); // 0.3 mm pen 
	CPen* penOld = cdc->SelectObject(&pen);

	double rAzim = mli->rAzimuth();
	CRect rect = rectPos();
	CSize sz = cdc->GetTextExtent("W");
	int iLength = (int)(rect.Height() - 2.2 * sz.cy);

	switch (eType) 
	{
		case eONEARROW:
			DrawOneArrow(cdc, rect, rAzim, iLength);
			break;
		case eUPARROW:
			DrawUpArrow(cdc, rect, rAzim, iLength);
			break;
		case eSIMPLECROSS:
			DrawSimpleCross(cdc, rect, rAzim, iLength);
			break;
		case eWINDROSE:
			DrawWindRose(cdc, rect, rAzim, iLength);
			break;
	}
	
	cdc->SelectObject(penOld);
}

void NorthArrowLayoutItem::DrawText(CDC* cdc, CPoint pt, double rAngle, const char* sText)
{
	CString str = sText;
	CSize sz = cdc->GetTextExtent(str);
	if (rAngle < 0)
		rAngle += 2 * M_PI;
  int iAngle = rounding(rAngle * 180 / M_PI) % 360;
	double rDX, rDY;
	if (iAngle < 45) {
		rDX = 0.5; 
		rDY = 1.1;
	}
	else if (iAngle < 135) {
		rDX = 1.1; 
		rDY = 0.5;
	}
	else if (iAngle < 225) {
		rDX = 0.5; 
		rDY = -0.1;
	}
	else if (iAngle < 315) {
		rDX = -0.1; 
		rDY = 0.5;
	}
	else {
		rDX = 0.5; 
		rDY = 1.1;
	}
	pt.x -= rDX * sz.cx;
	pt.y -= rDY * sz.cy;
	cdc->TextOut(pt.x, pt.y, str);
}

void NorthArrowLayoutItem::DrawOneArrow(CDC* cdc, CRect rct, double rAngle, long iLength)
{
	CPoint pt = rct.CenterPoint();

	int iDX =  rounding(iLength * sin(rAngle));
	int iDY = -rounding(iLength * cos(rAngle));
	Line ln(clr, rLineWidth);
	pt.x -= iDX / 2;
	pt.y -= iDY / 2;
	CPoint pBottom = pt;
	pt.x += iDX;
	pt.y += iDY;
	ln.drawLine(cdc, pBottom, pt);	
	CPoint pTop = pt;

	double rHeadAngle = M_PI / 6;  // 30 degrees
	double rTop = 0.2 * iLength;
	if (!fHalf) {
		pt.x -= rounding(rTop * sin(rAngle+rHeadAngle));
		pt.y += rounding(rTop * cos(rAngle+rHeadAngle));
		ln.drawLine(cdc, pTop, pt);
		pt = pTop;
	}
	pt.x -= rounding(rTop * sin(rAngle-rHeadAngle));
	pt.y += rounding(rTop * cos(rAngle-rHeadAngle));
	ln.drawLine(cdc, pTop, pt);

	if (fCircle) {
		cdc->SelectStockObject(HOLLOW_BRUSH);
		CPoint pt = rct.CenterPoint();
		int iLen = (int)(iLength * 0.3);
		CPoint p0(pt.x - iLen/2, pt.y - iLen/2);
		CPoint p1(p0.x + iLen, p0.y + iLen);
		CRect rect(p0, p1);
		cdc->Ellipse(&rect);
	}
	if (fText) 
	{
		DrawText(cdc, pTop, rAngle, "N");
		if (fAllSides)
			DrawText(cdc, pBottom, rAngle + M_PI, "S");
  }
}

void NorthArrowLayoutItem::DrawUpArrow(CDC* cdc, CRect rct, double rAngle, long iLen)
{
	CBrush brWhite(RGB(255,255,255));
	CBrush brClr(clr);
	CBrush* brOld = cdc->SelectObject(&brClr);

	int iDX =  rounding(iLen * sin(rAngle));
	int iDY = -rounding(iLen * cos(rAngle));
	double rAng = rAngle + M_PI_4 * 3.5;
  int iDXr =  rounding(iLen * sin(rAng));
  int iDYr = -rounding(iLen * cos(rAng));
	rAng -= M_PI_4 * 7;
  int iDXl =  rounding(iLen * sin(rAng));
  int iDYl = -rounding(iLen * cos(rAng));

	CPoint pt = rct.CenterPoint();

	CPoint ptTop = pt;
	ptTop.x += iDX / 2;
	ptTop.y += iDY / 2;
	CPoint ptLow = pt;
	ptLow.x -= iDX / 6;
	ptLow.y -= iDY / 6;
	CPoint ptBottom = ptTop;
	ptBottom.x -= iDX;
	ptBottom.y -= iDY;
	CPoint ptRight = pt;
	ptRight.x += iDXr / 2;
	ptRight.y += iDYr / 2;
	CPoint ptLeft = pt;
	ptLeft.x += iDXl / 2;
	ptLeft.y += iDYl / 2;

  CPoint Arrow[4];
	Arrow[0] = Arrow[3] = ptTop;	
	Arrow[1] = ptRight;
	Arrow[2] = ptLow;
	cdc->Polygon(Arrow, 4);

	if (fHalf)
		cdc->SelectObject(&brWhite);
	Arrow[1] = ptLeft;
	cdc->Polygon(Arrow, 4);

	if (fCircle) {
		cdc->SelectStockObject(HOLLOW_BRUSH);
		CPoint pt = rct.CenterPoint();
		CPoint p0(pt.x - iLen/2, pt.y - iLen/2);
		CPoint p1(p0.x + iLen, p0.y + iLen);
		CRect rect(p0, p1);
		cdc->Ellipse(&rect);
	}

	cdc->SelectObject(brOld);

	if (fText) 
	{
		DrawText(cdc, ptTop, rAngle, "N");
		if (fAllSides)
			DrawText(cdc, ptBottom, rAngle + M_PI, "S");
  }
}

void NorthArrowLayoutItem::DrawSimpleCross(CDC* cdc, CRect rct, double rAngle, long iLength)
{
	fHalf = false;
	CPoint pt = rct.CenterPoint();
	int iDX = rounding(iLength * cos(rAngle) / 4);
	int iDY = rounding(iLength * sin(rAngle) / 4);

	CPoint ptE = pt;
	ptE.x += iDX;
	ptE.y += iDY;
	CPoint ptW = pt;
	ptW.x -= iDX;
	ptW.y -= iDY;

	Line ln(clr, rLineWidth);
	ln.drawLine(cdc, ptE, ptW);
/*	cdc->MoveTo(pt);
	cdc->LineTo(ptE);
	cdc->MoveTo(pt);
	cdc->LineTo(ptW);*/

	DrawOneArrow(cdc, rct, rAngle, iLength);

	if (fText && fAllSides) {
		DrawText(cdc, ptW, rAngle + M_PI_2, "W");
		DrawText(cdc, ptE, rAngle - M_PI_2, "E");
	}
}

void NorthArrowLayoutItem::DrawWindRose(CDC* cdc, CRect rct, double rAngle, long iLength)
{
	CPen pen(PS_SOLID,0,clr); // hairline mm pen 
	CPen* penOld = cdc->SelectObject(&pen);
	int i;
	CPoint pt = rct.CenterPoint();
	double rot;

	CBrush brWhite(RGB(255,255,255));
	CBrush brClr(clr);
	CBrush* brOld = cdc->SelectObject(&brWhite);

	double rA = rAngle + M_PI_4 / 2;
	if (eRose > e8) {
	  for (i = 0, rot = rA; i < 8; ++i, rot += M_PI_4)
      DrawTriangle(cdc, (int)(0.3 * iLength), rot, pt, false);
		cdc->SelectObject(&brClr);
		for (i = 0, rot = rA; i < 8; ++i, rot += M_PI_4)
      DrawTriangle(cdc, (int)(0.3 * iLength), rot, pt, true);
	}
  rA = rAngle + M_PI_4;  
	cdc->SelectObject(&brWhite);
	if (eRose > e4) {
		for (i = 0, rot = rA; i < 4; ++i, rot += M_PI_2)
      DrawTriangle(cdc, (int)(0.4 * iLength), rot, pt, false);
		cdc->SelectObject(&brClr);
		for (i = 0, rot = rA; i < 4; ++i, rot += M_PI_2)
      DrawTriangle(cdc, (int)(0.4 * iLength), rot, pt, true);
	}
  rA = rAngle;  
	cdc->SelectObject(&brWhite);
  for (i = 0, rot = rA; i < 4; ++i, rot += M_PI_2)
      DrawTriangle(cdc, (int)(0.5 * iLength), rot, pt, false);
	cdc->SelectObject(&brClr);
  for (i = 0, rot = rA; i < 4; ++i, rot += M_PI_2)
      DrawTriangle(cdc, (int)(0.5 * iLength), rot, pt, true);

	if (fCircle) {
		cdc->SelectObject(&brWhite);
		CPoint pt = rct.CenterPoint();
		int iLen = (int)(0.3 * iLength);
		/*
		switch (eRose) {
			case e4: iLen = 0.5 * iLength; break;
			case e8: iLen = 0.4 * iLength; break;
			case e16: iLen = 0.3 * iLength; break;
		}	*/
		CPoint p0(pt.x - iLen/2, pt.y - iLen/2);
		CPoint p1(p0.x + iLen, p0.y + iLen);
		CRect rect(p0, p1);
		cdc->Ellipse(&rect);
	}

	cdc->SelectObject(brOld);
	cdc->SelectObject(penOld);

	if (fText) {
		int iDX =  rounding(iLength * sin(rAngle) / 2);
		int iDY = -rounding(iLength * cos(rAngle) / 2);
		CPoint ptN = pt;
		ptN.x += iDX;
		ptN.y += iDY;
		DrawText(cdc, ptN, rAngle, "N");
		if (fAllSides) {
			CPoint ptS = pt;
			ptS.x -= iDX;
			ptS.y -= iDY;
			DrawText(cdc, ptS, rAngle + M_PI, "S");
			CPoint ptE = pt;
			ptE.x -= iDY;
			ptE.y += iDX;
			DrawText(cdc, ptE, rAngle - M_PI_2, "E");
			CPoint ptW = pt;
			ptW.x += iDY;
			ptW.y -= iDX;
			DrawText(cdc, ptW, rAngle + M_PI_2, "W");
		}
	}
}

void NorthArrowLayoutItem::DrawTriangle(CDC* cdc, int iLen, double rAngle, CPoint pt, bool fBlack)
{
  int iX = (int)(iLen * cos(rAngle));
  int iY = (int)(iLen * sin(rAngle));
  CPoint top = pt;
  top.x += iX;
  top.y += iY;
  iLen /= 5;
  if (fBlack) {
    rAngle -= M_PI_4;
  }
  else {
    rAngle += M_PI_4;
  }  
  int iHX = (int)(iLen * cos(rAngle));
  int iHY = (int)(iLen * sin(rAngle));
  CPoint side = pt;
  side.x += iHX;
  side.y += iHY;
  CPoint Arrow[4];
  Arrow[0] = Arrow[3] = top;
  Arrow[1] = pt; 
  Arrow[2] = side;
	cdc->Polygon(Arrow, 4);
}

class NorthArrowLayoutItemForm : public FormWithDest
{
public:
  NorthArrowLayoutItemForm(CWnd* wnd, NorthArrowLayoutItem* tli) 
		: FormWithDest(wnd, SLOTitleNorthArrow)
  {
      RadioGroup *rg = new RadioGroup(root, SLOUiArrowType, (int*)&tli->eType);
      RadioButton* rbSimple = new RadioButton(rg, SLOUiSimpleArrow);
      RadioButton* rbCross  = new RadioButton(rg, SLOUiSimpleCross);
      RadioButton* rbUp     = new RadioButton(rg, SLOUiUpArrow);
      RadioButton* rbRose   = new RadioButton(rg, SLOUiWindRose);

			CheckBox* cb = new CheckBox(rbSimple, SLOUiHalf, &tli->fHalf);
			cb->Align(rbSimple, AL_AFTER);
			cb = new CheckBox(rbUp, SLOUiHalf, &tli->fHalf);
			cb->Align(rbSimple, AL_AFTER);
			FieldReal *fi = new FieldReal(rbSimple, SLOUiLineWidth, &tli->rLineWidth, ValueRangeReal(0.0,100,0.1));			
			fi->Align(cb, AL_AFTER);
			fi->SetIndependentPos();
			RadioGroup* rgRose = new RadioGroup(rbRose, "", (int*)&tli->eRose, true);
			rgRose->SetIndependentPos();
			new RadioButton(rgRose, "&4");
			new RadioButton(rgRose, "&8");
			new RadioButton(rgRose, "1&6");

			CheckBox* cbCircle = new CheckBox(root, SLOUiCircle, &tli->fCircle);
			cbCircle->Align(cb, AL_UNDER);
			fi = new FieldReal(rbCross, SLOUiLineWidth, &tli->rLineWidth, ValueRangeReal(0.0,100,0.1));
			fi->Align(cbCircle, AL_AFTER);
			fi->SetIndependentPos();

      FieldColor* fc = new FieldColor(root, SLOUiColor, &tli->clr);
			fc->Align(rg, AL_UNDER);

      cb = new CheckBox(root, SLOUiText, &tli->fText);
      FieldGroup *fg = new FieldGroup(cb);
      CheckBox* cbAS = new CheckBox(fg, SLOUiAllSides, &tli->fAllSides);
			cbAS->Align(cb, AL_AFTER);

	    FieldLogFont* flf = new FieldLogFont(fg, &tli->lf);
      flf->Align(cb, AL_UNDER);

      SetMenHelpTopic(htpCnfAnnNorth);
      create();
  }
};



bool NorthArrowLayoutItem::fConfigure()
{
	NorthArrowLayoutItemForm frm(ld->wndGetActiveView(), this);
	fTransparent = true;
	return frm.fOkClicked();
}


bool NorthArrowLayoutItem::fIsotropic() const
{
	return true;
}

String NorthArrowLayoutItem::sType() const
{
	return "NorthArrow";
}

void NorthArrowLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::ReadElements(en, sSection);
	int iNr;
	ObjectInfo::ReadElement(sSection, "Map View", en, iNr);
	mli = dynamic_cast<MapLayoutItem*>(ld->liFindID(iNr));
	ObjectInfo::ReadElement(sSection, "Text", en, fText);
	ObjectInfo::ReadElement(sSection, "TextAllSides", en, fAllSides);
	ObjectInfo::ReadElement(sSection, "Half", en, fHalf);
	ObjectInfo::ReadElement(sSection, "Circle", en, fCircle);
	ObjectInfo::ReadElement(sSection, "LineWidth", en, rLineWidth);

	String sType;
	ObjectInfo::ReadElement(sSection, "Type", en, sType);
	if ("Arrow" == sType)
		eType = eONEARROW;
	else if ("Up" == sType)
		eType = eUPARROW;
	else if ("Cross" == sType)
		eType = eSIMPLECROSS;
	else if ("WindRose" == sType)
		eType = eWINDROSE;
	int iRose;
	ObjectInfo::ReadElement(sSection, "Rose", en, iRose);
	switch (iRose) {
		case 4: eRose = e4; break;
		case 8: eRose = e8; break;
		case 16: eRose = e16; break;
	}
}

void NorthArrowLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::WriteElements(en, sSection);
	ObjectInfo::WriteElement(sSection, "Map View", en, mli->iID());
	ObjectInfo::WriteElement(sSection, "Text", en, fText);
	ObjectInfo::WriteElement(sSection, "TextAllSides", en, fAllSides);
	ObjectInfo::WriteElement(sSection, "Half", en, fHalf);
	ObjectInfo::WriteElement(sSection, "Circle", en, fCircle);
	ObjectInfo::WriteElement(sSection, "LineWidth", en, rLineWidth);	
	String sType;
	switch (eType) {
		case eONEARROW:     sType = "Arrow"; break;
		case eUPARROW:     sType = "Up"; break;
		case eSIMPLECROSS:  sType = "Cross";  break;
		case eWINDROSE:     sType = "WindRose";  break;
	}
	ObjectInfo::WriteElement(sSection, "Type", en, sType);
	int iRose;
	switch (eRose) {
		case e4: iRose = 4; break;
		case e8: iRose = 8; break;
		case e16: iRose = 16; break;
	}
	ObjectInfo::WriteElement(sSection, "Rose", en, iRose);
}

String NorthArrowLayoutItem::sName() const
{
	if (0 == mli)
		return SLONameNorthArrow;
	else
		return String(SLONameNorthArrow_S.scVal(), mli->sName());
}

bool NorthArrowLayoutItem::fDependsOn(LayoutItem* li)
{
	return li == mli;
}

void NorthArrowLayoutItem::SetPosition(MinMax mm, int iHit)
{
	// do overjump behaviour of TextLayoutItem
	LayoutItem::SetPosition(mm, iHit);
}

