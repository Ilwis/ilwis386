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
// RoseDiagramAxis.cpp: implementation of the RoseDiagramAxis class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Htp\Ilwismen.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CFont* fntRotated(CFont* fnt, int iDegrees)
{
	LOGFONT lf;
	fnt->GetLogFont(&lf);
	lf.lfEscapement = 10 * iDegrees;
	lf.lfOrientation = lf.lfEscapement;
	CFont* fntRot = new CFont();
	fntRot->CreateFontIndirect(&lf);
	return fntRot;
}


RoseDiagramAxis::RoseDiagramAxis(GraphAxisPos gap, RoseDiagramDrawer* rd)
: GraphAxis(gap, 0), rdd(rd), deg(deg180), ds(ds30)
{
	gvw = rd->gvw;
	fVisible = true;
	iDefTicks = 3;
}

RoseDiagramAxis::~RoseDiagramAxis()
{
}

	
void RoseDiagramAxis::draw(CDC* cdc)
{
	if (!fVisible)
		return;
	String s;
	CPen penAxis, penFloatingAxis, *penOld;
	CFont* fntOld =	cdc->SelectObject(gvw->fnt);
	CFont* fntRot = 0;
  UINT textalign;

	Color colGrey(127,127,127);
	penFloatingAxis.CreatePen(PS_SOLID, 1, colGrey);
	penAxis.CreatePen(PS_SOLID, 1, SysColor(COLOR_WINDOWTEXT));
/*	int iFloatOffset = iFloatPos();
	if (iFloatOffset > 0) {
		penOld = cdc->SelectObject(&penFloatingAxis);
    cdc->SetTextColor(colGrey);
	}
	else */
		penOld = cdc->SelectObject(&penAxis);

	int bkmode = cdc->SetBkMode(TRANSPARENT);
	bool f360Deg = rdd->rdaxX->deg == RoseDiagramAxis::deg360;
	CPoint cp = CPoint(f360Deg ? 500 : 1000, 500); // center point
	CPoint p;
  switch (gap) {
  	case gapXRose: 
			 {
				 // draw (semi) circle
				 int iL1 = longConv(rdd->rdaxY->rLen(rdd->rdaxY->rMax()));
				 int iL2 = iL1;
   			 if (!f360Deg)
 					 iL2 *= 2;
				 CRect rect(rdd->ptPos(cp.x-iL2, cp.y-iL1),rdd->ptPos(cp.x+iL2, cp.y+iL1));
				 if (f360Deg) {
  				 CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
  				 cdc->Ellipse(&rect);
					 cdc->SelectObject(brOld);
				 }
				 else {
					 CPoint p1 = rdd->ptPos(cp.x, cp.y+iL1);
					 CPoint p2 = rdd->ptPos(cp.x, cp.y-iL1);
					 cdc->Arc(&rect, p1, p2);
				 }
         // draw text at and of radial grid
				 int iDegStep;
				 switch (ds) {
				   case ds30: iDegStep = 30; break;
				   case ds45: iDegStep = 45; break;
				   case ds90: iDegStep = 90; break;
				 }
				 int iDegMax = f360Deg ? 360 : 180;
				 double rMx1 = rdd->rdaxY->rLen(rdd->rdaxY->rMax());
				 double rMx2 = rMx1;
  			 if (!f360Deg)
					 rMx2 *= 2; 
				 for (int iDeg = 0; iDeg <= iDegMax; iDeg += iDegStep) {
					 if (iDeg == 360)
						 break;
					 double rRad = (M_PI * iDeg) / 180;
					 CPoint p2 = rdd->ptPos(cp.x-sin(rRad)*rMx2, cp.y+cos(rRad)*rMx1);
					 String s = sText(iDeg, true);
					 s = s.sTrimSpaces();
					 s &= '°';
					 int iXOff, iYOff, textalign;
					 if (iDeg <= 90) {
						 iXOff = 5;
						 iYOff = -5;
						 textalign = TA_LEFT | TA_BASELINE;
					 }
					 else if (iDeg <= 180) {
						 iXOff = -5;
						 iYOff = -5;
						 textalign = TA_RIGHT | TA_BASELINE;
					 }
					 else if (iDeg < 270) {
						 iXOff = -5;
						 iYOff = 5;
						 textalign = TA_RIGHT | TA_TOP;
					 }
					 else { // iDeg < 360
						 iXOff = 5;
						 iYOff = 5;
						 textalign = TA_LEFT | TA_TOP;
					 }
					 int taOld = cdc->SetTextAlign(textalign);
					 cdc->TextOut(p2.x+iXOff, p2.y+iYOff, s.c_str());
					 cdc->SetTextAlign(taOld);
				 }
			 }
			 break;
		case gapYRose: 
			 {
				 double r;
				 int iL;
				 CPoint p;
				 // draw axis
				 iL = longConv(rLen(rMax()));
				 p = rdd->ptPos(cp.x, cp.y-iL);
         cdc->MoveTo(p);
				 p = rdd->ptPos(cp.x, cp.y+iL);
         cdc->LineTo(p);
				 // draw ticks and text
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
				 // 'center' tick
				 iL = longConv(rLen(rMin()));
				 p = rdd->ptPos(cp.x, cp.y-iL);
				 cdc->MoveTo(p);
         p.y += gvw->iCharHeight/2;
				 cdc->LineTo(p);
				 s = sText(rMin(), false);
  			 s = s.sTrimSpaces();
				 cdc->TextOut(p.x, p.y, s.c_str());

				 r = rFirstGridLine();
				 while (r <= rMax()) {
					 iL = longConv(rLen(r));
					 p = rdd->ptPos(cp.x, cp.y-iL);
					 cdc->MoveTo(p);
           p.y += gvw->iCharHeight/2;
					 cdc->LineTo(p);
					 s = sText(r, false);
  				 s = s.sTrimSpaces();
					 cdc->TextOut(p.x, p.y, s.c_str());
					 p = rdd->ptPos(cp.x, cp.y+iL);
					 cdc->MoveTo(p);
           p.y += gvw->iCharHeight/2;
					 cdc->LineTo(p);
					 cdc->TextOut(p.x, p.y, s.c_str());
					 r += rGridStep();
				 }
				 if (iTextRot > 0) 
   				 cdc->SelectObject(fntOld);
         cdc->SetTextAlign(textalign);
/*
				 // draw title
				 textalign = cdc->SetTextAlign(TA_CENTER);
				 // center text above 90
				 if (rdd->rdaxX->deg == deg180) 
					 p = rdd->ptPos(cp.x-2*rLen(rMax()), cp.y);
				 else  
					 p = rdd->ptPos(cp.x-rLen(rMax()), cp.y);
				 p.y -= 3*gvw->iCharHeight;
				 cdc->TextOut(p.x, p.y, rdd->sTitle.c_str());
				 cdc->SetTextAlign(textalign);
*/         
			 }
			 break;
	}
	cdc->SelectObject(fntOld);
	cdc->SelectObject(penOld);
//	if (iFloatOffset > 0) 
//    cdc->SetTextColor(SysColor(COLOR_WINDOWTEXT));
	cdc->SetBkMode(bkmode);
}

void RoseDiagramAxis::drawGrid(CDC* cdc)
{
	if (!fVisible || !fShowGrid)
		return;
	CPoint p;
	CPen penGrid;
	penGrid.CreatePen(PS_DOT, 1, Color(175,175,175));
	bool f360Deg = rdd->rdaxX->deg == RoseDiagramAxis::deg360;
	CPoint cp = CPoint(f360Deg ? 500 : 1000, 500); // center point
  switch (gap) {
	  case gapXRose:
			 {
  			 CPen* penOld = cdc->SelectObject(&penGrid);
				 int iDegStep;
				 switch (ds) {
				   case ds30: iDegStep = 30; break;
				   case ds45: iDegStep = 45; break;
				   case ds90: iDegStep = 90; break;
				 }
				 int iDegMax = f360Deg ? 360 : 180;
				 double rMx1 = rdd->rdaxY->rLen(rdd->rdaxY->rMax());
				 double rMx2 = rMx1;
  			 if (!f360Deg)
					 rMx2 *= 2; 
         CPoint p1 = rdd->ptPos(cp.x, cp.y); // center point
				 for (int iDeg = 0; iDeg <= iDegMax; iDeg += iDegStep) {
					 cdc->MoveTo(p1);
					 double rRad = (M_PI * iDeg) / 180;
					 CPoint p2 = rdd->ptPos(cp.x-sin(rRad)*rMx2, cp.y+cos(rRad)*rMx1);
					 cdc->LineTo(p2);
				 }
				 cdc->SelectObject(penOld);
			 }
			 break;
		case gapYRose: 
			 {
				 CPen* penOld = cdc->SelectObject(&penGrid);
				 CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
				 double r = rFirstGridLine();
				 while (r <= rMax()) {
					 int iL1 = longConv(rLen(r));
					 int iL2 = iL1;
    			 if (!f360Deg)
  					 iL2 *= 2;
					 CRect rect(rdd->ptPos(cp.x-iL2, cp.y-iL1),rdd->ptPos(cp.x+iL2, cp.y+iL1));
  				 if (f360Deg) 
	  				 cdc->Ellipse(&rect);
					 else {
						 CPoint p1 = rdd->ptPos(cp.x, cp.y+iL1);
						 CPoint p2 = rdd->ptPos(cp.x, cp.y-iL1);
						 cdc->Arc(&rect, p1, p2);
					 }
					 r += rGridStep();
				 }
				 cdc->SelectObject(penOld);
				 cdc->SelectObject(brOld);
			 }
			 break;
	}
}

int RoseDiagramAxis::iCalcSize() const
{
 	return GraphAxis::iCalcSize();
}

int RoseDiagramAxis::iFloatPos() const
{
	return 0;
}


double RoseDiagramAxis::rShift() const
{
	return 0;
}

void RoseDiagramAxis::SaveSettings(const FileName& fn, const String& sSection)
{
	GraphAxis::SaveSettings(fn, sSection);
	if (gap == gapXRose) {
		String s;
		switch (deg) {
      case deg180: s = "Deg180"; break;
      case deg360: s = "Deg360"; break;
		}
		
	  ObjectInfo::WriteElement(sSection.c_str(), "Degrees", fn, s);
		switch (ds) {
      case ds30: s = "Grid30"; break;
      case ds45: s = "Grid45"; break;
      case ds90: s = "Grid90"; break;
		}
		ObjectInfo::WriteElement(sSection.c_str(), "Grid", fn, s);
	}
}

void RoseDiagramAxis::LoadSettings(const FileName& fn, const String& sSection)
{
	GraphAxis::LoadSettings(fn, sSection);
	if (gap == gapXRose) {
		String s;
		ObjectInfo::ReadElement(sSection.c_str(), "Degrees", fn, s);
		if (fCIStrEqual(s, "Deg180"))
			deg = deg180;
		else if (fCIStrEqual(s, "Deg360"))
			deg = deg360;
		ObjectInfo::ReadElement(sSection.c_str(), "Grid", fn, s);
		if (fCIStrEqual(s, "Grid30"))
			ds = ds30;
		else if (fCIStrEqual(s, "Grid45"))
			ds = ds45;
		else if (fCIStrEqual(s, "Grid90"))
			ds = ds90;
	}
}

double RoseDiagramAxis::rRad(double r) const
{
	if (gap != gapXRose)
		return rUNDEF;
	double rMax = deg == deg360 ? 360 : 180;
	if (r < 0 || r > rMax)
		return rUNDEF;
	r = M_PI * r / 180;
	return r;
}	 

double RoseDiagramAxis::rLen(double r) const
{
	if (gap != gapYRose)
		return rUNDEF;
	return rConv(r) / 2;
}

double RoseDiagramAxis::rConvRad(double r) const
{
	if (gap != gapXRose)
		return rUNDEF;
	double rMax = deg == deg360 ? 360 : 180;
	r = 180 * r / M_PI;
	if (r < 0 || r > rMax)
		return rUNDEF;
	return r;
}	 

double RoseDiagramAxis::rConvLen(double r) const
{
	if (gap != gapYRose)
		return rUNDEF;
	double r1 = rConvInv(r*2);
	if (r1 == rUNDEF)
		return rUNDEF;
	return rMin()+rMax()-r1; 
}

bool RoseDiagramAxis::fConfig()
{
  class ConfigFormXAxis : public FormWithDest
  {
  public:
    ConfigFormXAxis(RoseDiagramAxis* rda, const String& sTitle)
    : FormWithDest(0, sTitle)
    {
      iImg = IlwWinApp()->iImage("Axis");
      new CheckBox(root, TR("Show &Grid"), &rda->fShowGrid);
      RadioGroup* rg = new RadioGroup(root, TR("&Rose Size"), (int*)&rda->deg, true);
		  new RadioButton(rg, "1&80°");
		  new RadioButton(rg, "3&60°");
 			rg = new RadioGroup(root, TR("&Interval"), (int*)&rda->ds, true);
	    new RadioButton(rg, "&30°");
		  new RadioButton(rg, "&45°");
		  new RadioButton(rg, "&90°");
      SetMenHelpTopic("ilwismen\\graph_window_options_x_axis_rose.htm");
      create();      
    }
  };
  switch (gap)
  {
    case gapXRose: 
      sTitle = TR("X-Axis Rose Diagram"); break;
    default:
      return GraphAxis::fConfig();
  }
  ConfigFormXAxis frm(this, sTitle);
  if (!frm.fOkClicked())
    return false;
  return true;
}
