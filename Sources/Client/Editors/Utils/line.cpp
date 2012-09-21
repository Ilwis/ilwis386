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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/line.cpp $
* 
* 18    4-03-03 16:27 Retsios
* Added function to resize the symbol of a line.
* 
* 17    19-02-03 13:51 Retsios
* Added a Resize function - for the time being this is only needed for
* the legend and representation of classes, where the original effect is
* too "thick" compared to the layout.
* 
* 16    20-03-01 16:16 Koolhoven
*  FieldLineTypeSimple::DrawItem() now uses a distance of 2 instead of 5,
* improving the drawing of blocked and symbol
* 
* 15    20-03-01 15:54 Koolhoven
* Line::drawSymbolLine() now sets the color of the symbol
* 
* 14    20-03-01 15:40 Koolhoven
* FieldLine::TypeChanged() improved. show frWidth with ldtSingle
* 
* 13    5-03-01 18:34 Koolhoven
* removed calls to GetDeviceCaps() always calculate with 0.1 mm units
* 
* 12    23-02-01 18:30 Koolhoven
* FieldLineType::sGetText() added
* 
* 11    26-01-01 15:55 Koolhoven
* creation of CPen does take for width no longer iSize() but uses always
* asumption of 0.1 mm units, so no correction for resolution of
* printer/display
* 
* 10    23-01-01 16:03 Koolhoven
* improved callback function FieldLine::TypeChanged() for symbol case
* 
* 9     4-05-00 10:00 Koolhoven
* Added drawRectangle()
* replaced zDisplay* with CDC*
* 
* 8     3-05-00 19:30 Koolhoven
* Added option to ask color in FieldLine also
* 
* 7     4-04-00 17:46 Koolhoven
* Added Read() and Write() functions to load/store in ODF
* 
* 6     4-04-00 10:06 Koolhoven
* Adapted FieldLine to show correctly when used under a CheckBox
* 
* 5     30-03-00 14:37 Koolhoven
* Corrected for use with metafiles (needed for Layout, Copying to
* ClipBoard etc.)
* 
* 4     19-01-00 13:16 Wind
* improved resource management (pen, brush, font)
* 
* 3     6/02/99 3:59p Martin
* 
* 2     5/26/99 12:56p Martin
* 
* 1     5/26/99 11:41a Martin
// Revision 1.2  1998/09/18 13:49:26  Wim
// Moved hdcLast and rPixPerMM from class Line to
// static variables in Line::rSize()
// Now copying to clipboard as metafile works
//
// Revision 1.1  1998-09-16 18:33:58+01  Wim
// 22beta2
//
/* line.c
Line
by Wim Koolhoven, march 1998
(c) ILWIS System Development, ITC
Last change:  WK   18 Sep 98    2:35 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\Smbext.h"
#include "Engine\Drawers\Drawer_n.h"

using namespace ILWIS;

#include "Client\Editors\Utils\line.h"
#include "Engine\Representation\Rprclass.h"
#include "Headers\Hs\Represen.hs"

#undef Polygon

Line::Line(const Representation& rpr, long iRaw)
: ldt(NewDrawer::ldtNone), rWidth(0), rDist(5), 
clr(0,0,0), clrFill(255,255,255),
fSupportLine(false),
smb(0)
{
	init(rpr.ptr(), iRaw);
}

Line::Line(const RepresentationPtr* rpr, long iRaw)
: ldt(NewDrawer::ldtNone), rWidth(0), rDist(5), 
clr(0,0,0), clrFill(255,255,255),
fSupportLine(false),
smb(0)
{
	init(rpr, iRaw);
}

Line::Line(Color color, double rWid, NewDrawer::LineDspType typ)
: ldt(typ), rWidth(rWid), rDist(5), 
clr(color), clrFill(255,255,255),
fSupportLine(false),
smb(0)
{
}

Line::Line(NewDrawer::LineDspType typ, Color color, Color colorFill)
: ldt(typ), rWidth(0), rDist(5), 
clr(color), clrFill(colorFill),
fSupportLine(false),
smb(0)
{
	if (NewDrawer::ldtSymbol == ldt) {
		smb = new ExtendedSymbol();
		smb->smType = ExtendedSymbol::smFONT;
		smb->sFaceName = "WingDings";
		smb->cSmb = 74;
		smb->col = clr;
		smb->iSize = 12;
	}
}

Line::Line(const ExtendedSymbol& es, double rDis)
: ldt(NewDrawer::ldtSymbol), rWidth(0), rDist(5),
clr(0,0,0), clrFill(255,255,255),
fSupportLine(false),
smb(0)
{
	smb = new ExtendedSymbol(es);
}

Line::Line(const Line& lin)
{
	ldt = lin.ldt;
	rWidth = lin.rWidth;
	rDist = lin.rDist;
	clr = lin.clr;
	clrFill = lin.clrFill;
	fSupportLine = lin.fSupportLine;
	smb = 0;
	if (lin.smb)
		smb = new ExtendedSymbol(*lin.smb);
}

void Line::operator=(const Line& lin)
{
	ldt = lin.ldt;
	rWidth = lin.rWidth;
	rDist = lin.rDist;
	clr = lin.clr;
	clrFill = lin.clrFill;
	fSupportLine = lin.fSupportLine;
	if (smb)
		delete smb;
	smb = 0;
	if (lin.smb)
		smb = new ExtendedSymbol(*lin.smb);
}

Line::~Line()
{
	if (smb)
		delete smb;
}

void Line::init(const RepresentationPtr* rpr, long iRaw)
{
	ldt = (NewDrawer::LineDspType)rpr->iLine(iRaw);
	rWidth = rpr->rLineWidth(iRaw);
	rDist = rpr->rLineDist(iRaw);
	smb = new ExtendedSymbol(rpr, iRaw);
	clr = rpr->clrRaw(iRaw);
	clrFill = (Color)rpr->clrLineFill(iRaw);
	if (-1 == clrFill.iVal()) {
		fSupportLine = false;
		clrFill = Color(255,255,255);
	}
	else
		fSupportLine = true;
}

void Line::Store(RepresentationClass* rc, long iRaw)
{
	rc->PutLine(iRaw, ldt);
	rc->PutLineWidth(iRaw, rWidth);
	rc->PutLineDist(iRaw, rDist);
	rc->PutColor(iRaw, clr);
	if (fSupportLine ||
		NewDrawer::ldtDouble == ldt || NewDrawer::ldtTriple == ldt ||
		NewDrawer::ldtBlocked == ldt)
		rc->PutLineFillColor(iRaw, clrFill);
	else
		rc->PutLineFillColor(iRaw, Color(-1));
	if (NewDrawer::ldtSymbol == ldt) {
		if (smb) {
			smb->col = clr;
			smb->Store(rc, iRaw);
		}
	}
}

void Line::Read(const char* sSection, const char* sPrefix, const FileName& filename)
{
	String sType("%s Type", sPrefix);
	String sLineType;
	ObjectInfo::ReadElement(sSection, sType.c_str(), filename, sLineType);
	ldt = ldtConv(sLineType);
	String sWidth("%s Width", sPrefix);
	ObjectInfo::ReadElement(sSection, sWidth.c_str(), filename, rWidth);
	String sDist("%s Dist", sPrefix);
	ObjectInfo::ReadElement(sSection, sDist.c_str(), filename, rDist);
	String sColor("%s Color", sPrefix);
	ObjectInfo::ReadElement(sSection, sColor.c_str(), filename, clr);
	String sFillColor("%s Fill Color", sPrefix);
	ObjectInfo::ReadElement(sSection, sFillColor.c_str(), filename, clrFill);
	String sSupportLine("%s Support Line", sPrefix);
	ObjectInfo::ReadElement(sSection, sSupportLine.c_str(), filename, fSupportLine);
	if (NewDrawer::ldtSymbol == ldt) {
		smb = new ExtendedSymbol;
		String sSymbol("%s Symbol", sPrefix);
		smb->Read(sSection, sSymbol.c_str(), filename);
	}
}

void Line::Write(const char* sSection, const char* sPrefix, const FileName& filename)
{
	String sType("%s Type", sPrefix);
	ObjectInfo::WriteElement(sSection, sType.c_str(), filename, sConv(ldt));
	String sWidth("%s Width", sPrefix);
	ObjectInfo::WriteElement(sSection, sWidth.c_str(), filename, rWidth);
	String sDist("%s Dist", sPrefix);
	ObjectInfo::WriteElement(sSection, sDist.c_str(), filename, rDist);
	String sColor("%s Color", sPrefix);
	ObjectInfo::WriteElement(sSection, sColor.c_str(), filename, clr);
	String sFillColor("%s Fill Color", sPrefix);
	ObjectInfo::WriteElement(sSection, sFillColor.c_str(), filename, clrFill);
	String sSupportLine("%s Support Line", sPrefix);
	ObjectInfo::WriteElement(sSection, sSupportLine.c_str(), filename, fSupportLine);
	if (NewDrawer::ldtSymbol == ldt) {
		String sSymbol("%s Symbol", sPrefix);
		smb->Write(sSection, sSymbol.c_str(), filename);
	}
}

double Line::rSize(HDC hDC, double rMM) // convert from mm to pixels
{
	double rPixPerMM = 10; // 0.1 mm units
	return rPixPerMM * rMM;
}

/*
inline int Line::iSize(HDC hDC, double rMM)     // convert from mm to pixels
{
return round(rSize(hDC, rMM));
}
*/
void Line::drawRectangle(CDC* cdc, const CRect& rect)
{
	zPoint p[6];
	p[0] = CPoint(rect.left+1, rect.top);
	p[1] = CPoint(rect.right, rect.top);
	p[2] = CPoint(rect.right, rect.bottom);
	p[3] = CPoint(rect.left, rect.bottom);
	p[4] = CPoint(rect.left, rect.top);
	p[5] = p[0];
	drawLine(cdc, p, 6);
}

void Line::drawLine(CDC* cdc, zPoint* p, int iCount)
{
	drawLine(cdc->m_hDC, p, iCount);
}

void Line::drawLine(CDC* cdc, zPoint pFrom, zPoint pTo)
{
	drawLine(cdc->m_hDC, pFrom, pTo);
}

void Line::drawLine(HDC hDC, zPoint pFrom, zPoint pTo)
{
	zPoint p[2];
	p[0] = pFrom;
	p[1] = pTo;
	drawLine(hDC, p, 2);
}

void Line::drawLine(HDC hDC, zPoint* p, int iCount)
{
	switch (ldt) {
	case NewDrawer::ldtNone:
		if (fSupportLine)
			drawSupportLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtSingle:
		if (fSupportLine)
			drawSupportLine(hDC, p, iCount);
		drawSimpleLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtDouble:
		drawDoubleLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtTriple:
		drawDoubleLine(hDC, p, iCount);
		drawSimpleLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtDot:
	case NewDrawer::ldtDash:
	case NewDrawer::ldtDashDot:
	case NewDrawer::ldtDashDotDot:
		if (fSupportLine)
			drawSupportLine(hDC, p, iCount);
		drawSimpleLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtBlocked:
		drawBlockedLine(hDC, p, iCount);
		break;
	case NewDrawer::ldtSymbol:
		if (fSupportLine)
			drawSupportLine(hDC, p, iCount);
		drawSymbolLine(hDC, p, iCount);
		break;
	}
}

void Line::drawSimpleLine(HDC hDC, zPoint* p, int iCount)
{
	CPen* pen;
	switch (ldt) {
	case NewDrawer::ldtSingle:
		if (fSupportLine)
			pen = new CPen(PS_SOLID, 1, clr);
		else
			pen = new CPen(PS_SOLID, 10*rWidth, clr );
		break;
	case NewDrawer::ldtTriple:
		pen = new CPen(PS_SOLID, 1, clr);
		break;
	case NewDrawer::ldtDot:
		pen = new CPen(PS_DOT, 1, clr);
		break;
	case NewDrawer::ldtDash:
		pen = new CPen(PS_DASH, 1, clr);
		break;
	case NewDrawer::ldtDashDot:
		pen = new CPen(PS_DASHDOT,1, clr);
		break;
	case NewDrawer::ldtDashDotDot:
		pen = new CPen(PS_DASHDOTDOT, 1, clr);
		break;
	}
	::SetBkMode(hDC, TRANSPARENT);
	HANDLE hPen = ::SelectObject(hDC, (HANDLE)*pen);
	::Polyline(hDC, (POINT*)p, iCount);
	::SelectObject(hDC, hPen);
	delete pen;
}

void Line::drawDoubleLine(HDC hDC, zPoint* p, int iCount)
{
	zPoint* pRight = new zPoint[2 * iCount];
	zPoint* pLeft = new zPoint[2 * iCount];
	int iRight, iLeft;
	getBoundary(hDC, p, iCount, pRight, iRight, pLeft, iLeft);
	int iPol = iRight + iLeft;
	zPoint* pPol = new zPoint[iPol];
	int i, j;
	for (i = 0, j =0; i < iRight; i++)
		pPol[j++] = pRight[i];
	for (i = iLeft - 1; i >= 0; i--)
		pPol[j++] = pLeft[i];
	CPen penNull(PS_NULL,1,1);
	CBrush br(clrFill);
	HANDLE hPen = ::SelectObject(hDC, penNull);
	HANDLE hBrush = ::SelectObject(hDC, br);
	::Polygon(hDC, (LPPOINT)pPol, iPol);
	::SelectObject(hDC, hBrush);
	CPen pen(PS_SOLID, 1, clr);
	::SelectObject(hDC, pen);
	::Polyline(hDC, (LPPOINT)pRight, iRight);
	::Polyline(hDC, (LPPOINT)pLeft, iLeft);
	::SelectObject(hDC, hPen);
	delete [] pRight;
	delete [] pLeft;
	delete [] pPol;
}

void Line::drawSupportLine(HDC hDC, zPoint* p, int iCount)
{
	CPen pen(PS_SOLID, 10*rWidth, clrFill );
	HANDLE hPen = ::SelectObject(hDC, (HANDLE)pen);
	::Polyline(hDC, (POINT*)p, iCount);
	::SelectObject(hDC, hPen);
}

void Line::drawSymbol(HDC hDC, zPoint p)
{
	if (0 == smb)
		return;
	CDC dc;
	dc.Attach(hDC);
	smb->drawSmb(&dc, 0, p);
	dc.Detach();
}

void Line::drawSymbolLine(HDC hDC, zPoint* p, int iCount)
{
	if (0 == smb)
		return;
	smb->col = clr;
	double rPixDist = rSize(hDC, rDist);
	if (rPixDist < 2)
		rPixDist = 2;
	zPoint pCurr = p[0];
	double rCurDist = rPixDist / 2; // start half way an interval
	for (int i = 1; i < iCount; ++i) {
		double rStillThere = rPixDist - rCurDist;
		double rDX = p[i].x - pCurr.x;
		double rDY = p[i].y - pCurr.y;
		double rD = sqrt(rDX * rDX + rDY * rDY);
		rCurDist += rD;
		if (rD <= rStillThere) {
			pCurr = p[i];
			continue;
		}
		double rCoorX = pCurr.x;
		double rCoorY = pCurr.y;
		rCoorX += rDX * rStillThere / rD;
		rCoorY += rDY * rStillThere / rD;
		rCurDist -= rPixDist;
		pCurr = zPoint(round(rCoorX), round(rCoorY));
		drawSymbol(hDC, pCurr);

		rDX *= rPixDist / rD;
		rDY *= rPixDist / rD;
		while (rCurDist > rPixDist) {
			rCurDist -= rPixDist;
			rCoorX += rDX;
			rCoorY += rDY;
			pCurr = zPoint(round(rCoorX), round(rCoorY));
			drawSymbol(hDC, pCurr);
		}
		pCurr = p[i];
	}
}

void Line::drawBlock(HDC hDC, zPoint* pTmp, int iCount)
{
	int iRight, iLeft;
	zPoint* pLeft = new zPoint[2 * iCount];
	zPoint* pRight = new zPoint[2 * iCount];
	getBoundary(hDC, pTmp, iCount, pRight, iRight, pLeft, iLeft);
	int iPol = iRight + iLeft;
	zPoint* pPol = new zPoint[iPol];
	int j, k;
	for (j = 0, k =0; j < iRight; j++)
		pPol[k++] = pRight[j];
	for (j = iLeft - 1; j >= 0; j--)
		pPol[k++] = pLeft[j];
	::Polygon(hDC, (LPPOINT)pPol, iPol);
	delete [] pPol;
	delete [] pLeft;
	delete [] pRight;
}

void Line::drawBlockedLine(HDC hDC, zPoint* p, int iCount)
{
	CBrush br1(clr);
	CBrush br2(clrFill);
	CPen pen(PS_SOLID, 1, clr);
	HANDLE hPen = ::SelectObject(hDC, pen);
	HANDLE hBrush = ::SelectObject(hDC, br1);
	double rPixDist = rSize(hDC, rDist);
	if (rPixDist < 1)
		rPixDist = rSize(hDC, rWidth * 2);
	if (rPixDist < 1)
		rPixDist = 10;
	if (rPixDist < 5)
		rPixDist = 5;

	zPoint* pTmp = new zPoint[iCount + 2];
	bool fFill = false;
	zPoint pCurr = p[0];
	double rCurDist = 0;
	int iCur = 0;
	for (int i = 1; i < iCount; ++i) {
		pTmp[iCur++] = pCurr;
		double rStillThere = rPixDist - rCurDist;
		double rDX = p[i].x - pCurr.x;
		double rDY = p[i].y - pCurr.y;
		double rD = sqrt(rDX * rDX + rDY * rDY);
		rCurDist += rD;
		if (rD <= rStillThere) {
			pCurr = p[i];
			continue;
		}
		double rCoorX = pCurr.x;
		double rCoorY = pCurr.y;
		rCoorX += rDX * rStillThere / rD;
		rCoorY += rDY * rStillThere / rD;
		rCurDist -= rPixDist;
		pCurr = zPoint(round(rCoorX), round(rCoorY));
		pTmp[iCur++] = pCurr;
		::SelectObject(hDC, hBrush);
		hBrush = ::SelectObject(hDC, fFill ? br2 : br1);
		fFill = !fFill;
		drawBlock(hDC, pTmp, iCur);
		iCur = 0;

		rDX *= rPixDist / rD;
		rDY *= rPixDist / rD;
		while (rCurDist > rPixDist) {
			pTmp[iCur++] = pCurr;
			rCurDist -= rPixDist;
			rCoorX += rDX;
			rCoorY += rDY;
			pCurr = zPoint(round(rCoorX), round(rCoorY));
			pTmp[iCur++] = pCurr;
			::SelectObject(hDC, hBrush);
			hBrush = ::SelectObject(hDC, fFill ? br2 : br1);
			fFill = !fFill;
			drawBlock(hDC, pTmp, iCur);
			iCur = 0;
		}
		pTmp[iCur++] = pCurr;
		pCurr = p[i];
	}
	pTmp[iCur++] = pCurr;
	if (iCur > 1) {
		::SelectObject(hDC, hBrush);
		hBrush = SelectObject(hDC, fFill ? br2 : br1);
		drawBlock(hDC, pTmp, iCur);
	}

	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	delete [] pTmp;
}

void Line::getBoundary(HDC hDC, const zPoint* p, int iCount,
					   zPoint* pRight, int& iRight,
					   zPoint* pLeft, int& iLeft)
{
	double rX = rSize(hDC, rWidth/2);
	double rY = rX;
	if (rWidth == 0) {
		rX = 2;
		rY = 2;
	}
	iLeft = iRight = 0;
	zPoint pC0r, pC1r, pC0l, pC1l, pC;  // current points
	zPoint p0r, p1r, p0l, p1l;      // last points
	getBoundPoints(rX,rY,p[0],p[1],pC0r,pC1r,pC0l,pC1l);
	pRight[iRight++] = pC0r;
	pLeft[iLeft++] = pC0l;
	for (int i = 2; i < iCount; ++i) {
		p0r = pC0r;
		p1r = pC1r;
		p0l = pC0l;
		p1l = pC1l;
		getBoundPoints(rX,rY,p[i-1],p[i],pC0r,pC1r,pC0l,pC1l);
		if (fCross(p0r, p1r, pC0r, pC1r, pC))
			pRight[iRight++] = pC;
		else {
			pRight[iRight++] = p1r;
			pRight[iRight++] = pC0r;
		}
		if (fCross(p0l, p1l, pC0l, pC1l, pC))
			pLeft[iLeft++] = pC;
		else {
			pLeft[iLeft++] = p1l;
			pLeft[iLeft++] = pC0l;
		}

	}
	pRight[iRight++] = pC1r;
	pLeft[iLeft++] = pC1l;
}

void Line::getBoundPoints(double rX, double rY,
						  zPoint p1, zPoint p2,
						  zPoint& p1r, zPoint& p2r,
						  zPoint& p1l, zPoint& p2l)
{
	p1l = p1r = p1;
	p2l = p2r = p2;
	double rDx = p2.x - p1.x;
	double rDy = p2.y - p1.y;
	double rD = sqrt(rDx * rDx + rDy * rDy);
	rDx /= rD;
	rDy /= rD;
	rX *= rDy;
	rY *= -rDx;
	int iX = round(rX);
	int iY = round(rY);
	p1r.x += iX;
	p1r.y += iY;
	p2r.x += iX;
	p2r.y += iY;
	p1l.x -= iX;
	p1l.y -= iY;
	p2l.x -= iX;
	p2l.y -= iY;
}

bool Line::fCross(zPoint pA0, zPoint pA1, zPoint pB0, zPoint pB1, zPoint& pC)
{
	MinMax mmA(RowCol((long)pA0.x,(long)pA0.y),
		RowCol((long)pA1.x,(long)pA1.y));
	MinMax mmB(RowCol((long)pB0.x,(long)pB0.y),
		RowCol((long)pB1.x,(long)pB1.y));
	if (!mmA.fContains(mmB))
		return false;
	int dxA = pA1.x - pA0.x;
	int dyA = pA1.y - pA0.y;
	int dxB = pB1.x - pB0.x;
	int dyB = pB1.y - pB0.y;
	if (0 == dxA) {
		if (0 == dxB) {
			// line returns horizontal on itself
			return false;
		}
		else {
			double rB = double(dyB) / dxB;
			int x = pA0.x;
			pC.x = x;
			pC.y = round(pB0.y + rB * (x - pB0.x));
			return true;
		}
	}
	else if (0 == dxB) {
		double rA = double(dyA) / dxA;
		int x = pB0.x;
		pC.x = x;
		pC.y = round(pA0.y + rA * (x - pA0.x));
		return true;
	}
	else {
		double rA = double(dyA) / dxA;
		double rB = double(dyB) / dxB;
		double r = rA - rB;
		if (abs(r) < 1e-6)
			return false;
		double t = pB0.y - pA0.y + rA * pA0.x - rB * pB0.x;
		int x = round(t/r);
		pC.x = x;
		pC.y = round(pA0.y + (x - pA0.x) * rA);
		return true;
	}
}

void Line::Resize(double rFactor)
{
	rWidth *= rFactor;
	rDist *= rFactor;
	if (smb)
		smb->Resize(rFactor);
}

void Line::ResizeSymbol(double rFactor)
{
	if (smb)
		smb->Resize(rFactor);
}

static char* sLDT[] =
{ "None", "Single", "Double", "Triple",
"Dot", "Dash", "DashDot", "DashDotDot",
"Blocked", "Symbol" };
static int psLDT[] =
{ PS_NULL, PS_SOLID, PS_NULL, PS_NULL,
PS_DOT, PS_DASH, PS_DASHDOT, PS_DASHDOTDOT,
PS_NULL, PS_NULL };

String Line::sConv(NewDrawer::LineDspType ldt)
{
	return sLDT[ldt];
}

NewDrawer::LineDspType Line::ldtConv(const String& s)
{
	for (int i = 0; i <= NewDrawer::ldtSymbol; ++i)
		if (s == sLDT[i])
			return (NewDrawer::LineDspType)i;
	return NewDrawer::ldtNone;
}

int Line::psConv(NewDrawer::LineDspType ldt)
{
	return psLDT[ldt];
}

FieldLineTypeSimple::FieldLineTypeSimple(FormEntry* par, NewDrawer::LineDspType* type,
										 bool fonlysimple)
										 : FieldOneSelect(par,&iLdt),
										 ldt(type), iLdt(*type),
										 fOnlySimple(fonlysimple)
{
	psn->iMinWidth = 1.5 * FLDNAMEWIDTH;
}

void FieldLineTypeSimple::create()
{
	FieldOneSelect::create();
	int id, iSel = 0;

	id = ose->AddString(TR("Single").c_str());
	ose->SetItemData(id, NewDrawer::ldtSingle); //SendMessage(*ose,CB_SETITEMDATA,id,ldtSingle);
	if (NewDrawer::ldtSingle == *ldt)
		iSel = id;
	if (!fOnlySimple) {
		id = ose->AddString(TR("Double").c_str());
		ose->SetItemData(id, NewDrawer::ldtDouble); //(CB_SETITEMDATA,id,ldtDouble);
		if (NewDrawer::ldtDouble == *ldt)
			iSel = id;
		id = ose->AddString(TR("Triple").c_str());
		ose->SetItemData(id,NewDrawer::ldtTriple);
		if (NewDrawer::ldtTriple == *ldt)
			iSel = id;
	}
	id = ose->AddString(TR("Dot").c_str());
	ose->SetItemData(id,NewDrawer::ldtDot);
	if (NewDrawer::ldtDot == *ldt)
		iSel = id;
	id = ose->AddString(TR("Dash").c_str());
	ose->SetItemData(id,NewDrawer::ldtDash);
	if (NewDrawer::ldtDash == *ldt)
		iSel = id;
	id = ose->AddString(TR("Dash dot").c_str());
	ose->SetItemData(id,NewDrawer::ldtDashDot);
	if (NewDrawer::ldtDashDot == *ldt)
		iSel = id;
	id = ose->AddString(TR("Dash dot dot").c_str());
	ose->SetItemData(id,NewDrawer::ldtDashDotDot);
	if (NewDrawer::ldtDashDotDot == *ldt)
		iSel = id;
	if (!fOnlySimple) {
		id = ose->AddString(TR("Blocked").c_str());
		ose->SetItemData(id,NewDrawer::ldtBlocked);
		if (NewDrawer::ldtBlocked == *ldt)
			iSel = id;
		id = ose->AddString(TR("Symbol").c_str());
		ose->SetItemData(id,NewDrawer::ldtSymbol);
		if (NewDrawer::ldtSymbol == *ldt)
			iSel = id;
	}
	ose->SetCurSel(iSel);
}

void FieldLineTypeSimple::DrawItem(Event* ev)
{

	DrawItemEvent *dev= dynamic_cast<DrawItemEvent *>(ev);
	ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);
	DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

	int id = dis->itemID;
	if (id < 0) { // skip if nothing selected
		return ;
	}
	CString s;
	ose->GetLBText(id, s);
	bool fSel = ose->GetCurSel() == id;
	long value = ose->GetItemData(id);

	HFONT hFnt = (HFONT)::SelectObject(dis->hDC, const_cast<CFont *>(ILWISAPP->GetFont(IlwisWinApp::sfFORM)));
	idw.DrawItem(dis, 60, String(s), fSel, true, false);
	::SelectObject(dis->hDC, hFnt);

	NewDrawer::LineDspType type = (NewDrawer::LineDspType)value;
	Color clr, clrFill;
	if (dis->itemState & ODS_SELECTED) {
		clr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		clrFill = ::GetSysColor(COLOR_HIGHLIGHT);
	}
	else {
		clr = ::GetSysColor(COLOR_WINDOWTEXT);
		clrFill = ::GetSysColor(COLOR_WINDOW);
	}
	Line line(type, clr, clrFill);
	line.rDist = 2;

	zPoint p1, p2;
	p1.x = dis->rcItem.left + 2;
	p1.y = dis->rcItem.top / 2 + dis->rcItem.bottom / 2;
	p2.x = p1.x + 55;
	p2.y = p1.y;

	if (line.fSymbolType()) {
		p1.y += 2;
		p2.y += 2;
	}
	line.drawLine(dis->hDC, p1, p2);
	return ;
}

void FieldLineTypeSimple::StoreData()
{
	FieldOneSelect::StoreData();
	*ldt = (NewDrawer::LineDspType)iLdt;
}

FieldLineType::FieldLineType(FormEntry* parent,
							 const String& sQuestion,
							 NewDrawer::LineDspType* ldt, bool fOnlySimple)
							 : FormEntry(parent, 0, true)
{
	if (sQuestion.length() != 0)
		st = new StaticTextSimple(this, sQuestion);
	else
		st = 0;
	fld = new FieldLineTypeSimple(this, ldt, fOnlySimple);
	if (st)
		fld->Align(st, AL_AFTER);
}

void FieldLineType::create()             // overriden
{
	CreateChildren();
}

void FieldLineType::StoreData()          // overriden
{
	fld->StoreData();
}

void FieldLineType::SetFocus()
{
	fld->SetFocus();
}

String FieldLineType::sGetText()
{
	return sChildrensText();
}

FieldLine::FieldLine(FormEntry* parent, Line* lin, bool fAskColor, bool _noSupport)
: FieldGroup(parent), pLine(lin), line(*lin), noSupport(_noSupport)
{
	flt = new FieldLineType(this, TR("&Line Type"), &line.ldt, true);
	flt->SetCallBack((NotifyProc)&FieldLine::TypeChanged, this);

	if (fAskColor)
		new FieldColor(this, TR("&Color"), &line.clr);

	if ( !noSupport) {
		cbSupport = new CheckBox(this, TR("&Support Line"), &line.fSupportLine);
		cbSupport->SetCallBack((NotifyProc)&FieldLine::SupportChanged, this);
		fcBG = new FieldColor(this, TR("&Background Color"), &line.clrFill);
		fcBG->Align(cbSupport, AL_UNDER);
		frDist = new FieldReal(this, TR("&Block Distance"), &line.rDist, ValueRange(1,100,0.1));
		pbSymbol = new PushButton(this, TR("&Symbol..."),
			(NotifyProc)&FieldLine::SymbolButton, true);
		pbSymbol->Align(cbSupport, AL_AFTER);
		frSymbDist = new FieldReal(this, TR("&Symbol Distance"), &line.rDist, ValueRange(1,100,0.1));
		frSymbDist->Align(frWidth, AL_UNDER);
	}
	frWidth = new FieldReal(this, TR("&Line width"), &line.rWidth, ValueRange(0,100,0.1));
}

int FieldLine::TypeChanged(void *)
{
	flt->StoreData();
	switch (line.ldt) {
	case NewDrawer::ldtNone:
	case NewDrawer::ldtSingle:
		if ( !noSupport) {
			cbSupport->Show();
			frWidth->Show();
			frDist->Hide();
			frSymbDist->Hide();
			pbSymbol->Hide();
		}
		break;
	case NewDrawer::ldtDot:
	case NewDrawer::ldtDash:
	case NewDrawer::ldtDashDot:
	case NewDrawer::ldtDashDotDot:
		if ( !noSupport) {
			cbSupport->Show();
			frDist->Hide();
			frSymbDist->Hide();
			pbSymbol->Hide();
		}
		break;
	case NewDrawer::ldtDouble:
	case NewDrawer::ldtTriple:
		if ( !noSupport) {
			cbSupport->Hide();
			fcBG->Show();
			frWidth->Show();
			frDist->Hide();
			frSymbDist->Hide();
			pbSymbol->Hide();
		}
		break;
	case NewDrawer::ldtBlocked:
		if (!noSupport) {
			cbSupport->Hide();
			fcBG->Show();
			frWidth->Show();
			frDist->Show();
			frSymbDist->Hide();
			pbSymbol->Hide();
		}
		break;
	case NewDrawer::ldtSymbol:
		if (!noSupport) {
			cbSupport->Show();
			frDist->Hide();
			frSymbDist->Show();
			pbSymbol->Show();
			SupportChanged(0);
		}
	}
	return 0;
}

int FieldLine::SupportChanged(void *)
{
	if (noSupport)
		return 1;
	cbSupport->StoreData();
	switch (line.ldt) {
	case NewDrawer::ldtNone:
	case NewDrawer::ldtDot:
	case NewDrawer::ldtDash:
	case NewDrawer::ldtDashDot:
	case NewDrawer::ldtDashDotDot:
	case NewDrawer::ldtSymbol:
		if (line.fSupportLine) {
			fcBG->Show();
			frWidth->Show();
		}
		else {
			fcBG->Hide();
			frWidth->Hide();
		}
		break;
	case NewDrawer::ldtSingle:
		if (line.fSupportLine)
			fcBG->Show();
		else
			fcBG->Hide();
		frWidth->Show();
		break;
	}
	return 0;
}

class SymbolForm: public FormWithDest
{
public:
	SymbolForm(CWnd* wPar, ExtendedSymbol& smb)
		: FormWithDest(wPar, TR("Line Symbols"))
	{
		new FieldExtendedSymbol(root, smb, true, false);
		SetMenHelpTopic("ilwismen\\representation_class_editor_line_symbols_on_segments.htm");
		create();
	}
};

int FieldLine::SymbolButton(void*)
{
	if (0 == line.smb)
		line.smb = new ExtendedSymbol();
	SymbolForm(frm()->wnd(), *line.smb);
	return 0;
}

void FieldLine::StoreData()
{
	FieldGroup::StoreData();
	*pLine = line;
}

// set show or hide sw = SW_SHOW, SW_HIDE
void FieldLine::show(int sw)
{
	FieldGroup::show(sw);
	if (sw == SW_SHOW) {
		TypeChanged(0);
		SupportChanged(0);
	}
}


