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
// GraphAxis.cpp: implementation of the GraphAxis class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Engine\Base\Round.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\Graph.hs"
#include "Client\FormElements\fldfontn.h"
#include "Headers\Htp\Ilwismen.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CFont* fntRotated(CFont* fnt, int iDegrees)
{
	LOGFONT lf;
	fnt->GetLogFont(&lf);
	lf.lfEscapement = 10 * iDegrees;
	lf.lfOrientation = lf.lfEscapement;
	CFont* fntRot = new CFont();
	fntRot->CreateFontIndirect(&lf);
	return fntRot;
}


GraphAxis::GraphAxis(GraphAxisPos gapos, CartesianGraphDrawer* cgd)
: gap(gapos), fShowGrid(true), fShowCodes(false), fFloating(true), gat(gatNORMAL), grdrw(cgd), ds(0), iTextRot(0), fDomainNone(false), _iSize(0), iDefTicks(8)
{
	fVisible = gap != gapYRight;
	if (0 != cgd)
	  gvw = cgd->gvw;
  CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
  fnt->GetLogFont(&lfTitle);
}

GraphAxis::~GraphAxis()
{
}

void GraphAxis::Set(const DomainValueRangeStruct& dv)
{
	dvrs = dv;
  if (dvrs.dm()->pdp()) // replace picture by image
    dvrs = Domain("Image");
	ds = dvrs.dm()->pdsrt();
  if (0 != ds) {
		iTextRot = 30;
    if (ds->iNettoSize() > 20)
      fShowGrid = false;
  }
	fDomainNone = dvrs.dm().fValid() && 0 != dvrs.dm()->pdnone();
  m_rGridStep = 1;
	if (dvrs.fValues()) 
		SetMinMax(dvrs.rrMinMax());
}

void GraphAxis::SetMinMax(const RangeReal& rr)
{
  double rStep = rRound(rr.rWidth()/iDefTicks);
  if (dvrsData.rStep() == 1 && rStep < 1)
    rStep = 1;
	vr = ValueRange(rr.rLo(), rr.rHi(), rStep);
  if (gat == gatNORMALPROBABILITY)
  {
    if (rr.rHi() < 10) 
      vr = ValueRange(0,1,0.001);
    else 
      vr = ValueRange(0,100,0.1);
  }
  dvrs.SetValueRange(vr);
  if (rr.rLo() < 1e-10 && gat == gatLOGARITHMIC)
    gat = gatNORMAL;
  SetGridStep(vr->rStep());
}

void GraphAxis::SetGridStep(double r)
{
  m_rGridStep = r;
  if (r > 1)
    r = 1;
	vr.SetStep(r);
 	dvrs.SetValueRange(vr);
}

double GraphAxis::rConv(double r) const
{
  switch (gat)
  {
    case gatNORMAL:
  		r = (r - rMin()) / (rMax() - rMin()) * 1000;
      break;
    case gatLOGARITHMIC:
    {
      double rLogMin = log(rMin());
      double rLogMax = log(rMax());
      double rLog = log(r);
      r = (rLog - rLogMin) / (rLogMax - rLogMin) * 1000;
      break;
    }  
    case gatNORMALPROBABILITY:
    {
      double rMin = vr->rrMinMax().rLo(); 
      double rMax = vr->rrMinMax().rHi(); 
  		r = (r - rMin) / (rMax - rMin);
      bool fBelow = r < 0.5;
      if (!fBelow)      
        r = 1 - r;
      if (r <= 0)
        return rUNDEF;
      // coied from rankplot function "Sub Nfunc(p!, Z!)" in static.bas
      double r2 = log(1 / r / r);
      double r1 = sqrt(r2);
      double r3 = r1 * r2;
      r = r1 - (2.515517 + 0.802853 * r1 + 0.010328 * r2) / (1 + 1.432788 * r1 + 0.189269 * r2 + 0.001308 * r3);
      if (fBelow)
        r = -r;
      r /= 6.2; // arbitrary number to scale
      r += 0.5;
      r *= 1000;
      break;
    }  
  }
  if (gap == gapYLeft || gap == gapYRight)
  	r = 1000 - r;
  return r;
}	 

double GraphAxis::rConvInv(double r) const
{
	if (gap != gapX)
		r = 1000 - r;
	if ((r < 0) || (r > 1000))
		return rUNDEF;
  switch (gat)
  {
    case gatNORMAL:
    	r = r * (rMax() - rMin()) / 1000 + rMin();
      break;
    case gatLOGARITHMIC:
    {
      double rLogMin = log(rMin());
      double rLogMax = log(rMax());
    	r = exp(r * (rLogMax - rLogMin) / 1000 + rLogMin);
      break;
    }  
    case gatNORMALPROBABILITY:
      r = rUNDEF; // tja what to do ?
  }
	return r;
}
	 
double GraphAxis::rFirstGridLine() const
{
	double r = rMin();
  switch (gat)
  {
    case gatNORMAL:
    	r = floor(r / rGridStep() + 0.000005) * rGridStep();
      break;
    case gatLOGARITHMIC:
      r = floor(log10(r));
      r = pow(10, r);
      break;
    case gatNORMALPROBABILITY:
      return 0.001 * vr->rrMinMax().rHi(); 
  }
  while (r < rMin())
    r = rNextGridLine(r);
	return r;
}

double GraphAxis::rNextGridLine(double rCurr) const
{
  double rFloor, rFact;
  switch (gat)
  {
    case gatNORMAL:
   		rCurr += rGridStep();
      break;
    case gatLOGARITHMIC:
      rFloor = floor(log10(rCurr));
      rFloor = pow(10, rFloor);
      rFact = rCurr / rFloor;
      rFact = round(rFact);
      if (rFact < 1.5) // == 1
        rFact = 2;
      else if (rFact < 3) // == 2
        rFact = 5;
      else if (rFact < 7) // == 5
        rFact = 10;
      else
        rFact = 20;
      rCurr = rFact * rFloor;        
      break;
    case gatNORMALPROBABILITY:
      double rMax = vr->rrMinMax().rHi();
      if (rCurr < rMax / 20)
        rCurr *= 10;
      else if (rCurr < 0.4 * rMax)
        rCurr = rMax / 2;
      else if (rCurr < 0.8 * rMax)
        rCurr = 0.9 * rMax;
      else 
      {
        rCurr = rMax - rCurr;
        rCurr /= 10;
        rCurr = rMax - rCurr;
      }
  }
  return rCurr;
}

double GraphAxis::rMin() const 
{ 
  switch (gat)
  {
    case gatNORMALPROBABILITY:
      return 0.001 * vr->rrMinMax().rHi();
    default:      
    	double r = 1;
    	if (vr.fValid())
        r = vr->rrMinMax().rLo(); 
    	return r;
  }      
}

double GraphAxis::rMax() const 
{ 
  switch (gat)
  {
    case gatNORMALPROBABILITY:
      return 0.999 * vr->rrMinMax().rHi(); 
    default:      
      if (vr.fValid())
        return vr->rrMinMax().rHi(); 
    	if (0 != ds)
    		return ds->iNettoSize();
    	return 0;
  }      
}

double GraphAxis::rGridStep() const 
{ 
  return m_rGridStep;
/*  
	if (dvrs.fValues()) {
		double r = vr->rStep();
		if (r == 0)
			return 1;
  	return r;
	}
	return 1;
*/  
}
	
void GraphAxis::draw(CDC* cdc)
{
  if (!fVisible)
    return;
  String s;
  CPen penAxis, penFloatingAxis, *penOld;
  CFont* fntOld =	cdc->SelectObject(gvw->fnt);
  CFont* fntRot = 0;
  UINT textalign;
  CFont fntTitle;
  fntTitle.CreateFontIndirect(&lfTitle);
  
  Color colGrey(127,127,127);
  penFloatingAxis.CreatePen(PS_SOLID, 1, colGrey);
  penAxis.CreatePen(PS_SOLID, 1, SysColor(COLOR_WINDOWTEXT));
  int iFloatOffset = iFloatPos();
  if (iFloatOffset > 0) {
    penOld = cdc->SelectObject(&penFloatingAxis);
    cdc->SetTextColor(colGrey);
  }
  else 
    penOld = cdc->SelectObject(&penAxis);
  
  int bkmode = cdc->SetBkMode(TRANSPARENT);
  double x, y;
  CPoint p;
  switch (gap) {
  case gapX: 
    {
      x = rConv(rMin());
      double rMinY = grdrw->gaxYLeft->rConv(grdrw->gaxYLeft->rMin());
      y = rMinY;
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.y = iFloatOffset;
      cdc->MoveTo(p);
      x = rConv(rMax()+2*rShift());
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.y = iFloatOffset;
      cdc->LineTo(p);
      if (iTextRot == 0)
        textalign = cdc->SetTextAlign(TA_CENTER);
      else {
        if (iTextRot <= 50)
          textalign = cdc->SetTextAlign(TA_RIGHT | TA_TOP);
        else
          textalign = cdc->SetTextAlign(TA_RIGHT | TA_BASELINE);
        fntRot = fntRotated(gvw->fnt, iTextRot);
        fntOld = cdc->SelectObject(fntRot);
      }
      x = rFirstGridLine();
      while (x <= rMax()) {
        p = grdrw->ptPos(y, rConv(x+rShift()));
        if (iFloatOffset > 0)
          p.y = iFloatOffset;
        cdc->MoveTo(p);
        p.y += gvw->iCharHeight/2;
        cdc->LineTo(p);
        s = sText(x, false);
        s = s.sTrimSpaces();
        cdc->TextOut(p.x, p.y, s.c_str());
        x = rNextGridLine(x);
      }
      if (iTextRot > 0) {
        cdc->SelectObject(fntOld);
        cdc->SetTextAlign(TA_CENTER);
      }
      p = grdrw->ptPos(rMinY, rConv((rMin()+rMax())/2));
      if (iFloatOffset > 0)
        p.y = iFloatOffset;
      p.y += iSize()-gvw->iCharHeight * 0.5;
      cdc->SelectObject(&fntTitle);      
      cdc->SetTextAlign(TA_BOTTOM|TA_CENTER);
      cdc->TextOut(p.x, p.y, sTitle.c_str());
      cdc->SetTextAlign(textalign);
    }
    break;
  case gapYLeft: 
    {
      double rMinX = grdrw->gaxX->rConv(grdrw->gaxX->rMin());
      x = rMinX;
      y = rConv(rMin());
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      cdc->MoveTo(p);
      y = rConv(rMax());
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      cdc->LineTo(p);
      if (iTextRot == 0)
        textalign = cdc->SetTextAlign(TA_RIGHT);
      else {
        if (iTextRot <= 50)
          textalign = cdc->SetTextAlign(TA_RIGHT | TA_TOP);
        else
          textalign = cdc->SetTextAlign(TA_RIGHT | TA_BASELINE);
        fntRot = fntRotated(gvw->fnt, iTextRot);
        fntOld = cdc->SelectObject(fntRot);
      }
      y = rFirstGridLine();
      while (y <= rMax()) {
        p = grdrw->ptPos(rConv(y), x);
        if (iFloatOffset > 0)
          p.x = iFloatOffset;
        cdc->MoveTo(p);
        p.x -= gvw->iCharHeight/2;
        cdc->LineTo(p);
        s = sText(y, false);
        s = s.sTrimSpaces();
        //			 p.x -= gvw->iBorder;
        p.y -= gvw->iCharHeight/2;
        cdc->TextOut(p.x, p.y, s.c_str());
        y = rNextGridLine(y);
      }
      cdc->SelectObject(fntOld);
      CFont* fntVert = fntRotated(&fntTitle, 90);
      fntOld = cdc->SelectObject(fntVert);
      p = grdrw->ptPos(rConv((rMin()+rMax())/2), rMinX);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      p.x -= iSize();
      cdc->SetTextAlign(TA_CENTER);
      cdc->TextOut(p.x, p.y, sTitle.c_str());
      cdc->SelectObject(fntOld);
      delete fntVert;
      cdc->SetTextAlign(textalign);
    }
    break;
  case gapYRight: 
    {
      double rMaxX = grdrw->gaxX->rConv(grdrw->gaxX->rMax()+2*grdrw->gaxX->rShift());
      x = rMaxX;
      y = rConv(rMin());
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      cdc->MoveTo(p);
      y = rConv(rMax());
      p = grdrw->ptPos(y, x);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      cdc->LineTo(p);
      if (iTextRot == 0)
        textalign = cdc->SetTextAlign(TA_LEFT);
      else {
      /*	 if (iTextRot <= 50)
      textalign = cdc->SetTextAlign(TA_LEFT | TA_BASELINE);
        else*/
        textalign = cdc->SetTextAlign(TA_LEFT | TA_TOP);
        fntRot = fntRotated(gvw->fnt, iTextRot);
        fntOld = cdc->SelectObject(fntRot);
      }
      y = rFirstGridLine();
      while (y <= rMax()) 
      {
        p = grdrw->ptPos(rConv(y), x);
        if (iFloatOffset > 0)
          p.x = iFloatOffset;
        cdc->MoveTo(p);
        p.x += gvw->iCharHeight/2;
        cdc->LineTo(p);
        s = sText(y, false);
        //					 if (iTextRot > 20)
        s = s.sTrimSpaces();
        p.y -= gvw->iCharHeight/2;
        cdc->TextOut(p.x, p.y, s.c_str());
        y = rNextGridLine(y);
      }
      cdc->SelectObject(fntOld);
      CFont* fntVert = fntRotated(&fntTitle, 90);
      fntOld = cdc->SelectObject(fntVert);
      p = grdrw->ptPos(rConv((rMin()+rMax())/2), rMaxX);
      if (iFloatOffset > 0)
        p.x = iFloatOffset;
      p.x += iSize();
      p.x -= gvw->iCharHeight * 0.5;
      cdc->SetTextAlign(TA_CENTER|TA_BASELINE);
      cdc->TextOut(p.x, p.y, sTitle.c_str());
      cdc->SelectObject(fntOld);
      delete fntVert;
      cdc->SetTextAlign(textalign);
    }
    break;
  }
  cdc->SelectObject(fntOld);
  delete fntRot;
  cdc->SelectObject(penOld);
  if (iFloatOffset > 0) 
    cdc->SetTextColor(SysColor(COLOR_WINDOWTEXT));
  cdc->SetBkMode(bkmode);
}

void GraphAxis::drawGrid(CDC* cdc)
{
	if (!fVisible || !fShowGrid)
		return;
	double x, y;
	CPoint p;
	CPen penGrid;
	penGrid.CreatePen(PS_DOT, 1, Color(175,175,175));
  switch (gap) {
  	case gapX: 
    {
      double rMinY = grdrw->gaxYLeft->rConv(grdrw->gaxYLeft->rMin());
      double rMaxY = grdrw->gaxYLeft->rConv(grdrw->gaxYLeft->rMax());
      x = rFirstGridLine();
      while (x <= rMax()) 
      {
        CPen* penOld = cdc->SelectObject(&penGrid);
        p = grdrw->ptPos(rMaxY, rConv(x + rShift()));
        cdc->MoveTo(p);
        p = grdrw->ptPos(rMinY, rConv(x + rShift()));
        cdc->LineTo(p);
        cdc->SelectObject(penOld);
        x = rNextGridLine(x);
      }
    } break;
		case gapYLeft: 
    case gapYRight: 
    {
      double rMinX = grdrw->gaxX->rConv(grdrw->gaxX->rMin());
      double rMaxX = grdrw->gaxX->rConv(grdrw->gaxX->rMax()+2*grdrw->gaxX->rShift());
      y = rFirstGridLine();
      while (y <= rMax()) 
      {
        CPen* penOld = cdc->SelectObject(&penGrid);
        p = grdrw->ptPos(rConv(y), rMinX);
        cdc->MoveTo(p);
        p = grdrw->ptPos(rConv(y), rMaxX);
        cdc->LineTo(p);
        cdc->SelectObject(penOld);
        y = rNextGridLine(y);
      }
    }
    break;
	}
}

int GraphAxis::iSize() const
{
	return _iSize;
}

void GraphAxis::CalcSize()
{
	_iSize = iCalcSize();
}

int GraphAxis::iCalcSize() const
{
	if (!fVisible)
		return 0;
  if (gvw->fnt == 0)
		return 0;
	CDC dc;
	dc.CreateCompatibleDC(0);
	CFont* fnt = fntRotated(gvw->fnt, iTextRot);
	CFont* fntOld = dc.SelectObject(fnt);
 	double y = rFirstGridLine();
	int iW = 0;
	while (y <= rMax()) {
		String s = sText(y, false);
		s = s.sTrimSpaces();
		CSize siz = dc.GetTextExtent(s.c_str());
    if (siz.cx > iW)
			iW = siz.cx;
     y = rNextGridLine(y);
	}
	dc.SelectObject(fntOld);
	delete fnt;
  int iSize = 0;
	if (gap == gapX || gap == gapYRose)
	  iSize = max(sin(iTextRot*M_PI/180.0) * iW, gvw->iCharHeight) + gvw->iCharHeight;
	else
	  iSize = max(cos(iTextRot*M_PI/180.0) * iW, gvw->iCharHeight) + gvw->iCharHeight;

  CFont fntTitle;
  fntTitle.CreateFontIndirect(&lfTitle);
  fntOld = dc.SelectObject(&fntTitle);  
	CSize siz = dc.GetTextExtent(sTitle.c_str());
  iSize += siz.cy;
	dc.SelectObject(fntOld);
  
  return iSize;
}

int GraphAxis::iFloatPos() const
{
	if (!fFloating)
	  return 0;
	CPoint pnt;
	switch (gap) {
	  case gapX: 
			{ 
        pnt = gvw->pntPos(1000,0);
				if (pnt.y > gvw->dimGraph().height() + gvw->iBorder) {
					CRect rect;
					gvw->GetClientRect(&rect);
					return rect.Height() - iSize() - gvw->iBorder;
				}
			}
			break;
		case gapYLeft:
			{
        pnt = gvw->pntPos(0,0);
				if (pnt.x < 0)
					return iSize();
			}
			break;
		case gapYRight:
			{
        pnt = gvw->pntPos(0,1000);
				if (pnt.x > gvw->dimGraph().width() + gvw->iBorder + grdrw->gaxYLeft->iSize()) {
					CRect rect;
					gvw->GetClientRect(&rect);
					return rect.Width() - iSize() - gvw->iBorder;
				}
			}
			break;
	}
	return 0;
}

String GraphAxis::sText(double rVal, bool fUseDvrsData) const
{
	if (rVal == rUNDEF)
		return sUNDEF;
	if (!dvrs.dm().fValid() || 0 != dvrs.dm()->pdnone()) {
		long i = longConv(rVal);
		if (i != iUNDEF)
		  return String("%i", i);
		else
			return sUNDEF;
	}
	if (dvrs.fValues()) 
    if (fUseDvrsData && dvrsData.dm().fValid())
			return dvrsData.sValue(rVal,0);
    else {
			String sRet = dvrs.sValue(rVal,0);
      // protect against the possibility that the user selects weird limits
      if ("?" != sRet)
        return sRet;
			return dvrsData.sValue(rVal,0);
    }
	if (0 != ds)
		if (!fShowCodes)
		  return ds->sValue(longConv(rVal));
		else
		  return ds->sCodeByRaw(ds->iKey(longConv(rVal)));
	return dvrs.sValueByRaw(longConv(rVal));
}

double GraphAxis::rShift() const
{
  if (gap == gapX && (fDomainNone || 0 != ds))
 		return rGridStep()/2;
	return 0;
}

void GraphAxis::SaveSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::WriteElement(sSection.c_str(), "Title", fn, sTitle);
	ObjectInfo::WriteElement(sSection.c_str(), "ShowGrid", fn, fShowGrid);
	ObjectInfo::WriteElement(sSection.c_str(), "Visible", fn, fVisible);
	if (0 != ds)
    ObjectInfo::WriteElement(sSection.c_str(), "ShowCodes", fn, fShowCodes);
	else
    ObjectInfo::WriteElement(sSection.c_str(), "ShowCodes", fn, (char*)0);
	ObjectInfo::WriteElement(sSection.c_str(), "TextRotation", fn, iTextRot);
	ObjectInfo::WriteElement(sSection.c_str(), "Domain", fn, dvrs.dm());
  ValueRange valrng = vr;
  valrng.SetStep(m_rGridStep);
	ObjectInfo::WriteElement(sSection.c_str(), "ValRange", fn, valrng);
	ObjectInfo::WriteElement(sSection.c_str(), "DomainData", fn, dvrsData.dm());
	ObjectInfo::WriteElement(sSection.c_str(), "ValRangeData", fn, dvrsData.vr());
  String sAxisType;
  switch (gat)
  {
    case gatNORMAL: sAxisType = "Linear"; break;
    case gatLOGARITHMIC: sAxisType = "Logarithmic"; break;
    case gatNORMALPROBABILITY: sAxisType = "Normal Probability"; break;
  }
	ObjectInfo::WriteElement(sSection.c_str(), "Axis Type", fn, sAxisType);
	ObjectInfo::WriteElement(sSection.c_str(), "Font", fn, lfTitle.lfFaceName);
	long iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::WriteElement(sSection.c_str(), "LogFont", fn, (char*)&lfTitle, iLen);
}

void GraphAxis::LoadSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::ReadElement(sSection.c_str(), "Title", fn, sTitle);
	ObjectInfo::ReadElement(sSection.c_str(), "ShowGrid", fn, fShowGrid);
	ObjectInfo::ReadElement(sSection.c_str(), "Visible", fn, fVisible);
	Domain dm;
	ObjectInfo::ReadElement(sSection.c_str(), "Domain", fn, dm);
	ValueRange vr;
	ObjectInfo::ReadElement(sSection.c_str(), "ValRange", fn, vr);
	Set(DomainValueRangeStruct(dm, vr));
	if (vr.fValid()) {
    SetMinMax(vr->rrMinMax());
		SetGridStep(vr->rStep());
	}
	else if (0 != ds)
    ObjectInfo::ReadElement(sSection.c_str(), "ShowCodes", fn, fShowCodes);
	ObjectInfo::ReadElement(sSection.c_str(), "TextRotation", fn, iTextRot);
	String s;
	ObjectInfo::ReadElement(sSection.c_str(), "DomainData", fn, s);
	if (s.length() > 0)
	  ObjectInfo::ReadElement(sSection.c_str(), "DomainData", fn, dm);
	ValueRange vrData;
	ObjectInfo::ReadElement(sSection.c_str(), "ValRangeData", fn, vrData);
	dvrsData = DomainValueRangeStruct(dm, vrData);
  String sAxisType;
	ObjectInfo::ReadElement(sSection.c_str(), "Axis Type", fn, sAxisType);
  if ("Linear" == sAxisType)
    gat = gatNORMAL;
  else if ("Logarithmic" == sAxisType)
    gat = gatLOGARITHMIC;
  else if ("Normal Probability" == sAxisType)
    gat = gatNORMALPROBABILITY;

	String sFont;
	ObjectInfo::ReadElement(sSection.c_str(), "Font", fn, sFont);
	lstrcpy(lfTitle.lfFaceName, sFont.c_str());
	int iLen = sizeof(LOGFONT) - LF_FACESIZE;
	int iRes = ObjectInfo::ReadElement(sSection.c_str(), "LogFont", fn, (char*)&lfTitle, iLen);
  if (0 == iRes || "" == sFont) 
  {
    CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
  	fnt->GetLogFont(&lfTitle);
  }
}

bool GraphAxis::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(GraphAxis* gaxis, const String& sTitle)
    : FormWithDest(0, sTitle)
    , fosAxisType(0), frGridstep(0)
    {
      iImg = IlwWinApp()->iImage("Axis");
      if (gaxis->gap != GraphAxis::gapYRose) 
      {
        FieldString* fs = new FieldString(root, TR("Axis &Text"), &gaxis->sTitle);
        fs->SetWidth(90);
      }        
      CheckBox* cb = 0;
      if (gaxis->gap == GraphAxis::gapYRight) {
        cb = new CheckBox(root, TR("Show &Axis"), &gaxis->fVisible);
        new CheckBox(cb, TR("Show &Grid"), &gaxis->fShowGrid);
      }
      else
        new CheckBox(root, TR("Show &Grid"), &gaxis->fShowGrid);

      if (gaxis->dvrs.fValues()) 
      {
        iAxisType = (int)gaxis->gat;
        vsAxisType.resize(3);
        vsAxisType[0] = TR("Linear.axis");
        vsAxisType[1] = TR("Logarithmic.axis;");
        vsAxisType[2] = TR("Normal Probability.axis;");
        fosAxisType = new FieldOneSelectString(root, TR("&Axis Type"), &iAxisType, vsAxisType);
        if (cb)
  			  fosAxisType->Align(cb, AL_UNDER);
        fosAxisType->SetCallBack((NotifyProc)&ConfigForm::AxisTypeCallBack);
  			rrMinMax = RangeReal(gaxis->rMin(), gaxis->rMax());
  			rGridStep = gaxis->rGridStep();
  			frr = new FieldRangeReal(root, TR("Min - Max"), &rrMinMax, ValueRange(-1e300, 1e300, gaxis->dvrs.rStep()));
  			frGridstep = new FieldReal(root, TR("&Interval"), &rGridStep, ValueRange(0, 1e308, gaxis->dvrs.rStep()));
  		}
  		else if (0 != gaxis->ds) {
  			CheckBox* cbx = new CheckBox(root, TR("Show &Codes"), &gaxis->fShowCodes);
        if (cb)
    			cbx->Align(cb, AL_UNDER);
  		}
  		FieldInt* fi = new FieldInt(root, TR("Text &Rotation"), &gaxis->iTextRot, ValueRange(0,90), true);
  		if (!gaxis->dvrs.fValues() && 0 == gaxis->ds && cb) 
  			fi->Align(cb, AL_UNDER);

      if (GraphAxis::gapYRose != gaxis->gap)
			new FieldLogFont(root, &gaxis->lfTitle);

      switch (gaxis->gap)
      {
      case GraphAxis::gapX:
        SetMenHelpTopic("ilwismen\\graph_window_options_x_axis.htm");
        break;
      case GraphAxis::gapYLeft:
        SetMenHelpTopic("ilwismen\\graph_window_options_y_axis_left.htm");
        break;
      case GraphAxis::gapYRight:
        SetMenHelpTopic("ilwismen\\graph_window_options_y_axis_right.htm");
        break;
      case GraphAxis::gapYRose:
        SetMenHelpTopic("ilwismen\\graph_window_options_y_axis_rose.htm");
        break;
      }
      create();      
    }
    RangeReal rrMinMax;
    double rGridStep;
    long iAxisType;
  private:    
    int AxisTypeCallBack(Event*)
    {
      fosAxisType->StoreData();
      switch (GraphAxis::GraphAxisType(iAxisType))
      {
        case GraphAxis::gatNORMAL:
          frr->Show();
          frGridstep->Show();
          break;
        case GraphAxis::gatLOGARITHMIC:
          frr->Show();
          frGridstep->Hide();
          break;
        case GraphAxis::gatNORMALPROBABILITY:
          frr->Hide();
          frGridstep->Hide();
          break;
      }
      return 0;
    }
    FieldOneSelectString* fosAxisType;
    FieldRangeReal* frr;
    FieldReal* frGridstep;
    vector<String> vsAxisType;
  };
  String sTitle;
  switch (gap)
  {
    case gapX: 
      sTitle = TR("X-Axis"); break;
    case gapYLeft:
      sTitle = TR("Y-Axis (left)"); break;
    case gapYRight:
      sTitle = TR("Y-Axis (right)"); break;
    case gapYRose:
      sTitle = TR("Y-Axis Rose Diagram"); break;
  }
  ConfigForm frm(this, sTitle);
  if (!frm.fOkClicked())
    return false;
  if (dvrs.fValues()) 
  {
    gat = GraphAxisType(frm.iAxisType);
    switch (gat)
    {
      case gatLOGARITHMIC:
        if (frm.rrMinMax.rLo() < 1e-10)
          frm.rrMinMax.rLo() = 1;
        // fall through
      case gatNORMAL:
      	vr = ValueRange(frm.rrMinMax, frm.rGridStep);
        break;
      case gatNORMALPROBABILITY:
        {
          if (vr->rrMinMax().rHi() > 2)
            vr = ValueRange(0,100,0.1);
          else
            vr = ValueRange(0,1,0.001);
        }
    }
    SetMinMax(vr->rrMinMax());
    SetGridStep(frm.rGridStep);
	}
  return true;
}
