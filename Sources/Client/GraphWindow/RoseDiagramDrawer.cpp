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
// RoseDiagramDrawer.cpp: implementation of the RoseDiagramDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphLegend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


RoseDiagramDrawer::RoseDiagramDrawer(GraphView* gv)
: GraphDrawer(gv)
{
	rdaxX = new RoseDiagramAxis(GraphAxis::gapXRose, this);
	rdaxY = new RoseDiagramAxis(GraphAxis::gapYRose, this);
}

RoseDiagramDrawer::~RoseDiagramDrawer()
{
	delete rdaxX;
	delete rdaxY;
}

void RoseDiagramDrawer::SetView(GraphView* gv)
{
  GraphDrawer::SetView(gv);
  rdaxX->gvw = gv;
  rdaxY->gvw = gv;
}

void RoseDiagramDrawer::draw(CDC* cdc)
{
	rdaxX->CalcSize();
	rdaxY->CalcSize();
	rdaxX->drawGrid(cdc);
	rdaxY->drawGrid(cdc);
  for (int i=0; i < agl.iSize(); i++)
	  agl[i]->draw(cdc);
  drawTitle(cdc);
	rdaxX->draw(cdc);
	rdaxY->draw(cdc);
  grleg->draw(cdc);
}

void RoseDiagramDrawer::drawTitle(CDC* cdc)
{
  if ("" == sTitle)
    return;
  CPoint pt = ptPos(0,500); // find x position
  pt.y = 5; // y position: title at the top
  CFont fntTitle;
  fntTitle.CreateFontIndirect(&lfTitle);
	CFont* fntOld =	cdc->SelectObject(&fntTitle);
  UINT ta = cdc->SetTextAlign(TA_CENTER);
  cdc->TextOut(pt.x, pt.y, sTitle.scVal());
  cdc->SetTextAlign(ta);
	cdc->SelectObject(fntOld);
}

bool RoseDiagramDrawer::fConfigure()
{
	return fConfigure(0, true);
}

bool RoseDiagramDrawer::fConfigure(RoseDiagramLayer* rdl, bool fLastGraph)
{
  RoseDiagramOptionsForm frm(this);
	if (0 == rdl && fLastGraph)
		frm.SetActivePage(frm.iGraphPropPages+agl.iSize()-1);
	else if (0 != rdl)
		for (int i=0; i < agl.iSize(); i++)
			if (this->rdl(i) == rdl) {
				frm.SetActivePage(frm.iGraphPropPages+i);
				break;
			}
  return IDOK == frm.DoModal();
}


String RoseDiagramDrawer::sText(CPoint point) const
{
	double rRow, rCol;
	Pt2RowCol(point, rRow, rCol);
	double rRad, rLen;
	bool f360Deg = rdaxX->deg == RoseDiagramAxis::deg360;
	CPoint cp = CPoint(500, f360Deg ? 500 : 1000); // center point
  double x = rCol - cp.x;
  double y = rRow - cp.y;
  if (!f360Deg)
		y /= 2;
	rRad = atan2(y,x);
	if (rRad < 0)
		rRad = - rRad;
	else
    rRad = 2 * M_PI - rRad;
	rLen = sqrt(x*x + y*y);
	return String("%S : %S", rdaxX->sText(rdaxX->rConvRad(rRad), true), rdaxY->sText(rdaxY->rConvLen(rLen), true));
}


void RoseDiagramDrawer::SaveSettings(const FileName& fn, const String& sSection)
{
  GraphDrawer::SaveSettings(fn, sSection);
	String sSec("%S XRose", sSection);
	rdaxX->SaveSettings(fn, sSec);
	sSec = String("%S YRose", sSection);
	rdaxY->SaveSettings(fn, sSec);
	ObjectInfo::WriteElement(sSection.scVal(), "NrGraphs", fn, (long)agl.iSize());
	for (int i=0; i  < agl.iSize(); i++) {
  	sSec = String("%S %i", sSection, i);
		agl[i]->SaveSettings(fn, sSec);
	}
}

void RoseDiagramDrawer::LoadSettings(const FileName& fn, const String& sSection)
{
  GraphDrawer::LoadSettings(fn, sSection);
	String sSec("%S XRose", sSection);
	rdaxX->LoadSettings(fn, sSec);
	sSec = String("%S YRose", sSection);
	rdaxY->LoadSettings(fn, sSec);
	int iNrGraphs;
	ObjectInfo::ReadElement(sSection.scVal(), "NrGraphs", fn, iNrGraphs);
	for (int i=0; i  < iNrGraphs; i++) {
  	sSec = String("%S %i", sSection, i);
		GraphLayer* gl = new RoseDiagramLayer(this, fn, sSec);
	  agl.push_back(gl);
	}
}

CPoint RoseDiagramDrawer::ptPos(double rRow, double rCol) const
{
	zPoint p = gvw->pntPos(rRow, rCol);
	p.x += gvw->iBorder;
	p.y += gvw->iBorder;
	p.x += iSpaceNeededX() / 2;
	p.y += iSpaceNeededY() / 2;
  p.y += m_iTitleSize /  2; // half is already in iSpaceNeededY();
	return p;
}
  
void RoseDiagramDrawer::Pt2RowCol(const CPoint& p, double& rRow, double &rCol) const
{
	zPoint pt = p;
	pt.x -= gvw->iBorder;
	pt.y -= gvw->iBorder;
	pt.x -= iSpaceNeededX() / 2;
	pt.y -= iSpaceNeededY() / 2;
	pt.y -= m_iTitleSize / 2;
	gvw->Pnt2RowCol(pt, rRow, rCol);
}

int RoseDiagramDrawer::iSpaceNeededX() const
{ 
	int iSpace = 0;
	if (rdaxX->fVisible)
		iSpace += 4 * gvw->iCharHeight;
	return iSpace; 
}

int RoseDiagramDrawer::iSpaceNeededY() const
{ 
	int iSpace = 0;
  iSpace += m_iTitleSize;
  if (rdaxX->fVisible) {
		if (rdaxX->deg == RoseDiagramAxis::deg360)
		  iSpace += 4 * gvw->iCharHeight;
		else {
    	if (rdaxY->fVisible)
        iSpace += rdaxY->iSize();
		}
	}
	return iSpace;
}

void RoseDiagramDrawer::CalcAxisSizes()
{
	rdaxX->CalcSize();
	rdaxY->CalcSize();
  CalcTitleSize();
}  

GraphDrawer::Area RoseDiagramDrawer::aPoint(CPoint pt) const
{
  Area area = GraphDrawer::aPoint(pt);
  if (area != aGRAPH)
    return area;
	pt.x -= gvw->iBorder;
	pt.y -= gvw->iBorder;
	pt.x -= iSpaceNeededX() / 2;
	pt.y -= iSpaceNeededY() / 2;
	pt.y -= m_iTitleSize;
  if (pt.x < 0)
    return aTITLE;
  if (pt.x > gvw->dimGraph().cx)
    return aROSEY;
  if (pt.y > gvw->dimGraph().cy)
    return aROSEX;

  return aGRAPH;
}

