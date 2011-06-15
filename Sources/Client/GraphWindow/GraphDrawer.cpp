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
// GraphDrawer.cpp: implementation of the GraphDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphLegend.h"
#include "Headers\Hs\Graph.hs"
#include "Client\FormElements\fldfontn.h"
#include "Headers\Htp\Ilwismen.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



GraphDrawer::GraphDrawer(GraphView* gv)
: gvw(gv)
{
  grleg = new GraphLegend(this);
  CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
  fnt->GetLogFont(&lfTitle);
}

GraphDrawer::~GraphDrawer()
{
  for (unsigned int i=0; i < agl.iSize(); i++) {
		delete agl[i];
	}
  delete grleg;
}

void GraphDrawer::SetView(GraphView* gv)
{
  gvw = gv;
}

void GraphDrawer::draw(CDC* cdc)
{
	// do nothing
}

int GraphDrawer::iSpaceNeededX() const
{ 
	return 0; 
}

int GraphDrawer::iSpaceNeededY() const
{ 
	return 0; 
}

bool GraphDrawer::fConfigure()
{
	return false;
}

String GraphDrawer::sText(CPoint p) const
{
	return String();
}

bool GraphDrawer::fAddGraph(bool fConfigLayer)
{
	return false;
}

bool GraphDrawer::fAddFormulaGraph(bool fConfigLayer)
{
	return false;
}

bool GraphDrawer::fAddLsfGraph(bool fConfigLayer)
{
	return false;
}

bool GraphDrawer::fAddSmvGraph(bool fConfigLayer)
{
	return false;
}

void GraphDrawer::Init()
{
	CalcAxisSizes();
}

void GraphDrawer::CalcAxisSizes()
{
	// do nothing
}

GraphDrawer::Area GraphDrawer::aPoint(CPoint pt) const
{
  if (grleg && grleg->fShow && grleg->rect.PtInRect(pt))
    return aLEGEND;
  if (pt.y < gvw->iCharHeight + 10)
    return aTITLE;
  return aGRAPH;
}

bool GraphDrawer::fConfigureTitle()
{
  class ConfigForm : public FormWithDest 
  {
  public:
    ConfigForm(GraphDrawer* gd, const String& sTitle)
    : FormWithDest(0, sTitle)
    {
      FieldString* fs = new FieldString(root, SGPUiTitle, &gd->sTitle);
      fs->SetWidth(90);
			new FieldLogFont(root, &gd->lfTitle);
      SetMenHelpTopic("ilwismen\\graph_window_options_title.htm");
      create();      
    }
  };
  ConfigForm frm(this, SGPTitleGraphTitle);
  return frm.fOkClicked();
}

void GraphDrawer::SaveSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::WriteElement(sSection.scVal(), "Title", fn, sTitle);
	ObjectInfo::WriteElement(sSection.scVal(), "Font", fn, lfTitle.lfFaceName);
	long iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::WriteElement(sSection.scVal(), "LogFont", fn, (char*)&lfTitle, iLen);
}

void GraphDrawer::LoadSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::ReadElement(sSection.scVal(), "Title", fn, sTitle);
	String sFont;
	ObjectInfo::ReadElement(sSection.scVal(), "Font", fn, sFont);
	lstrcpy(lfTitle.lfFaceName, sFont.scVal());
	int iLen = sizeof(LOGFONT) - LF_FACESIZE;
	int iRes = ObjectInfo::ReadElement(sSection.scVal(), "LogFont", fn, (char*)&lfTitle, iLen);
  if (0 == iRes || "" == sFont) 
  {
    CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
  	fnt->GetLogFont(&lfTitle);
  }
}

void GraphDrawer::CalcTitleSize()
{
  if ("" == sTitle)
    m_iTitleSize = 0;
  else 
  {
    CDC dc;
    dc.CreateCompatibleDC(0);
    CFont fntTitle;
    fntTitle.CreateFontIndirect(&lfTitle);
    CFont* fntOld = dc.SelectObject(&fntTitle);  
    CSize siz = dc.GetTextExtent(sTitle.scVal());
    m_iTitleSize = siz.cy;
    dc.SelectObject(fntOld);
  }
}


//// CartesianGraphDrawer

CartesianGraphDrawer::CartesianGraphDrawer(GraphView* gv)
: GraphDrawer(gv)
{
	gaxX = new GraphAxis(GraphAxis::gapX, this);
	gaxYLeft = new GraphAxis(GraphAxis::gapYLeft, this);
	gaxYRight = new GraphAxis(GraphAxis::gapYRight, this);
}

CartesianGraphDrawer::~CartesianGraphDrawer()
{
	delete gaxX;
	delete gaxYLeft;
	delete gaxYRight;
}

void CartesianGraphDrawer::SetView(GraphView* gv)
{
  GraphDrawer::SetView(gv);
  gaxX->gvw = gv;
  gaxYLeft->gvw = gv;
  gaxYRight->gvw = gv;
}

void CartesianGraphDrawer::draw(CDC* cdc)
{
	unsigned int i;
	bool fDrawYLeft = false, fDrawYRight = false; 
  for (i=0; i < agl.iSize(); i++) {
		CartesianGraphLayer* cgl = dynamic_cast<CartesianGraphLayer*>(agl[i]);
		if (cgl->fYAxisLeft)
			fDrawYLeft = true;
		else
			fDrawYRight = true;
	}
	gaxX->CalcSize();
  gaxYLeft->CalcSize();
	gaxYRight->CalcSize();
	gaxX->drawGrid(cdc);
	gaxYLeft->drawGrid(cdc);
	gaxYRight->drawGrid(cdc);
  for (i=0; i < agl.iSize(); i++)
	  agl[i]->draw(cdc);
  drawTitle(cdc);
	gaxX->draw(cdc);
	gaxYLeft->draw(cdc);
	gaxYRight->draw(cdc);
  grleg->draw(cdc);
}

void CartesianGraphDrawer::drawTitle(CDC* cdc)
{
  if ("" == sTitle)
    return;
  CPoint pt;
 	pt.x = gvw->dimGraph().cx / 2 + gvw->iBorder;
	if (gaxYLeft->fVisible)
		pt.x += gaxYLeft->iSize();
  pt.y = 5;
  CFont fntTitle;
  fntTitle.CreateFontIndirect(&lfTitle);
	CFont* fntOld =	cdc->SelectObject(&fntTitle);
  UINT ta = cdc->SetTextAlign(TA_CENTER);
  cdc->TextOut(pt.x, pt.y, sTitle.scVal());
  cdc->SetTextAlign(ta);
	cdc->SelectObject(fntOld);
}

bool CartesianGraphDrawer::fConfigure()
{
	return fConfigure(0, true);
}

bool CartesianGraphDrawer::fConfigure(CartesianGraphLayer* cgl, bool fLastGraph)
{
  CartesianGraphOptionsForm frm(this);
	if (0 == cgl && fLastGraph)
		frm.SetActivePage(frm.iGraphPropPages+agl.iSize()-1);
	else if (0 != cgl)
		for (unsigned int i=0; i < agl.iSize(); i++)
			if (agl[i] == cgl) {
				frm.SetActivePage(frm.iGraphPropPages+i);
				break;
			}
  bool fRet = IDOK == frm.DoModal();
	return fRet;
}


CPoint CartesianGraphDrawer::ptPos(double rRow, double rCol) const
{
	zPoint p = gvw->pntPos(rRow, rCol);
	p.x += gvw->iBorder;
	p.y += gvw->iBorder;
	if (gaxYLeft->fVisible)
		p.x += gaxYLeft->iSize();
  if ("" != sTitle)
    p.y += m_iTitleSize;
	return p;
}
  
void CartesianGraphDrawer::Pt2XY(const CPoint& p, double& rX, double &rY) const
{
	zPoint pt = p;
	pt.x -= gvw->iBorder;
	pt.y -= gvw->iBorder;
	if (gaxYLeft->fVisible)
		pt.x -= gaxYLeft->iSize();
  if ("" != sTitle)
    pt.y -= gvw->iCharHeight;
	gvw->Pnt2RowCol(pt, rY, rX);
}

int CartesianGraphDrawer::iSpaceNeededX() const
{ 
	int iSpace = 0;
	if (gaxX->rShift() > 0) {
		double r = gaxX->rConv(gaxX->rMin());
		CPoint p1 = gvw->pntPos(0, r);
		r = gaxX->rConv(gaxX->rMin()+2*gaxX->rShift());
		CPoint p2 = gvw->pntPos(0, r);
		iSpace += p2.x - p1.x;
	}
	if (gaxYLeft->fVisible)
		iSpace += gaxYLeft->iSize();
	if (gaxYRight->fVisible)
		iSpace += gaxYRight->iSize();
	return iSpace; 
}

int CartesianGraphDrawer::iSpaceNeededY() const
{ 
  int iSpace = 0;
  iSpace += m_iTitleSize;
  if (gaxX->fVisible)
		iSpace += gaxX->iSize();
	return iSpace;
}

String CartesianGraphDrawer::sText(CPoint point) const
{
	double rX, rY;
	Pt2XY(point, rX, rY);
	String sX = gaxX->sText(gaxX->rConvInv(rX)-gaxX->rShift(), true).sTrimSpaces();
	String sY = gaxYLeft->sText(gaxYLeft->rConvInv(rY), true).sTrimSpaces();
	return String("%S, %S", sX, sY);
}

int CartesianGraphDrawer::iNrBarGraphs() const
{
	int iBarGraphs = 0;
	for (unsigned int i=0; i < agl.iSize(); i++)
		if (cgl(i)->fShow && cgl(i)->cgt == cgtBar)
			iBarGraphs++;
	return iBarGraphs;
}

int CartesianGraphDrawer::iBarGraph(CartesianGraphLayer* gl) const
{
	int iBar = 0;
	for (unsigned int i=0; i < agl.iSize(); i++) {
	  if (agl[i] == gl)
		 return iBar;
		if (cgl(i)->fShow && cgl(i)->cgt == cgtBar)
			iBar++;
	}
	return iBar;
}

void CartesianGraphDrawer::SaveSettings(const FileName& fn, const String& sSection)
{
  GraphDrawer::SaveSettings(fn, sSection);
	String sSec("%S X", sSection);
	gaxX->SaveSettings(fn, sSec);
	sSec = String("%S YLeft", sSection);
	gaxYLeft->SaveSettings(fn, sSec);
	sSec = String("%S YRight", sSection);
	gaxYRight->SaveSettings(fn, sSec);
	sSec = String("%S Legend", sSection);
  grleg->SaveSettings(fn, sSec);
	ObjectInfo::WriteElement(sSection.scVal(), "NrGraphs", fn, (long)agl.iSize());
	for (unsigned int i=0; i  < agl.iSize(); i++) {
  	sSec = String("%S %i", sSection, i);
		agl[i]->SaveSettings(fn, sSec);
	}
}

void CartesianGraphDrawer::LoadSettings(const FileName& fn, const String& sSection)
{
  GraphDrawer::LoadSettings(fn, sSection);
	String sSec("%S X", sSection);
	gaxX->LoadSettings(fn, sSec);
	sSec = String("%S YLeft", sSection);
	gaxYLeft->LoadSettings(fn, sSec);
	sSec = String("%S YRight", sSection);
	gaxYRight->LoadSettings(fn, sSec);
	sSec = String("%S Legend", sSection);
  grleg->LoadSettings(fn, sSec);
	int iGraphs = 0;
	ObjectInfo::ReadElement(sSection.scVal(), "NrGraphs", fn, iGraphs);
	for (int i=0; i  < iGraphs; i++) {
		String sSec("%S %i", sSection, i);
		CartesianGraphLayer* cgl = CartesianGraphLayer::create(fn, sSec, this);
		if (0 != cgl)
		  agl.push_back(cgl);
	}
}

void CartesianGraphDrawer::ClearSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::WriteElement(sSection.scVal(), "Title", fn, 0);
	String sSec("%S X", sSection);
	ObjectInfo::WriteElement(sSec.scVal(), 0, fn, 0);
	sSec = String("%S YLeft", sSection);
	ObjectInfo::WriteElement(sSec.scVal(), 0, fn, 0);
	sSec = String("%S YRight", sSection);
	ObjectInfo::WriteElement(sSec.scVal(), 0, fn, 0);
	int iGraphs = 0;
	ObjectInfo::ReadElement(sSection.scVal(), "NrGraphs", fn, iGraphs);
	for (int i=0; i  < iGraphs; i++) {
		String sSec("%S %i", sSection, i);
		ObjectInfo::WriteElement(sSec.scVal(), 0, fn, 0);
	}
}

void CartesianGraphDrawer::CalcAxisSizes()
{
	gaxX->CalcSize();
	gaxYLeft->CalcSize();
	gaxYRight->CalcSize();
  CalcTitleSize();
}

GraphDrawer::Area CartesianGraphDrawer::aPoint(CPoint pt) const
{
  Area area = GraphDrawer::aPoint(pt);
  if (area != aGRAPH)
    return area;
	pt.x -= gvw->iBorder;
	pt.y -= gvw->iBorder;
	if (gaxYLeft->fVisible)
		pt.x -= gaxYLeft->iSize();
  if ("" != sTitle)
    pt.y -= gvw->iCharHeight;
  if (pt.x < 0)
    return aYAXISLEFT;
  if (pt.x > gvw->dimGraph().cx)
    return aYAXISRIGHT;
  if (pt.y > gvw->dimGraph().cy)
    return aXAXIS;

  return aGRAPH;
}

