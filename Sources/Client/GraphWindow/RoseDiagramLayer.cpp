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
// RoseDiagramLayer.cpp: implementation of the RoseDiagramLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Table\tblview.h"
#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Htp\Ilwismen.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//// RoseDiagramLayer

RoseDiagramLayer::RoseDiagramLayer(RoseDiagramDrawer* rd, const Table& t, const Column& cX, const Column& cY)
: GraphLayer(rd)
, rdd(rd), rdt(rdtNeedle), tbl(t), colX(cX), colY(cY)
{
  sTitle = sName();
}

RoseDiagramLayer::RoseDiagramLayer(RoseDiagramDrawer* rd, const FileName& fn, const String& sSection)
: GraphLayer(rd, fn, sSection)
, rdd(rd), rdt(rdtNeedle) 
{
	String sGraphType;
	ObjectInfo::ReadElement(sSection.scVal(), "GraphType", fn, sGraphType);
	rdt = rdtContineous;
	if (fCIStrEqual(sGraphType, "Contineous"))
		rdt = rdtContineous;
	else if(fCIStrEqual(sGraphType, "Step"))
		rdt = rdtStep;
	else if (fCIStrEqual(sGraphType, "Needle"))
		rdt = rdtNeedle; 
	ObjectInfo::ReadElement(sSection.scVal(), "Table", fn, tbl);
	String sCol;
  ObjectInfo::ReadElement(sSection.scVal(), "ColumnX", fn, sCol);
	if (sCol.length() > 0)
	  colX = tbl->col(sCol);
	sCol = "";
	ObjectInfo::ReadElement(sSection.scVal(), "ColumnY", fn, sCol);
	if (sCol.length() > 0)
  	colY = tbl->col(sCol);
}

void RoseDiagramLayer::SaveSettings(const FileName& fn, const String& sSection)
{
  GraphLayer::SaveSettings(fn, sSection);
 
	String sGraphType;
  switch (rdt) {
		case rdtContineous: sGraphType = "Contineous";	break;
		case rdtStep:       sGraphType = "Step";break; 
		case rdtNeedle:     sGraphType = "Needle";break; 
		default: break;
	}
	if (sGraphType.length()>0)
  	ObjectInfo::WriteElement(sSection.scVal(), "GraphType", fn, sGraphType);
	ObjectInfo::WriteElement(sSection.scVal(), "Type", fn, "Column");

	ObjectInfo::WriteElement(sSection.scVal(), "Table", fn, tbl);
	if (colX.fValid())
	  ObjectInfo::WriteElement(sSection.scVal(), "ColumnX", fn, colX);
	if (colY.fValid())
	  ObjectInfo::WriteElement(sSection.scVal(), "ColumnY", fn, colY);
}

RoseDiagramLayer::~RoseDiagramLayer()
{
}

bool RoseDiagramLayer::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(RoseDiagramLayer* rdl)
    : FormWithDest(0, SGPTitleGraphOptionsRoseDiagram)
    {
      iImg = IlwWinApp()->iImage("Graph");
//      new CheckBox(root, SGPUiShow, &rdl->fShow);
      FieldString* fs = new FieldString(root, SGPUiName, &rdl->sTitle);
      fs->SetWidth(120);
  		RadioGroup* rgType = new RadioGroup(root, "", (int*)&rdl->rdt);
      RadioButton* rbLine = new RadioButton(rgType, SGPUiLine);
      RadioButton* rbStep = new RadioButton(rgType, SGPUiStep);
      RadioButton* rbNeedle = new RadioButton(rgType, SGPUiNeedle);

      FieldGroup* fg = new FieldGroup(rbLine, true);
      new FieldColor(fg, SGPUiColor, &rdl->color);

      fg = new FieldGroup(rbStep, true);
      new FieldColor(fg, SGPUiColor, &rdl->color);

      fg = new FieldGroup(rbNeedle, true);
      new FieldColor(fg, SGPUiColor, &rdl->color);
   
      SetMenHelpTopic("ilwismen\\graph_window_options_rose_diagram.htm");
      create();
    }
  };
  ConfigForm frm(this);
  if (!frm.fOkClicked())
    return false;

  return true;
}

void RoseDiagramLayer::draw(CDC* cdc)
{
  if (!fShow)
    return;
  if (iNrPoints() == 0)
		return;
	switch (rdt) {
		case rdtContineous: 
			{
				CPoint p;
				CPen pen(PS_SOLID, 1, color);
				CPen* penOld = cdc->SelectObject(&pen);
				bool fPrevValidPoint = false;
				for (int i=1; i <= iNrPoints(); i++) {
					double r = rY(i);
 			    bool fValidPoint = r != rUNDEF;
					if (fValidPoint) {
  				  p = ptPos(rX(i), r);
					  if (!fPrevValidPoint) 
  					  cdc->MoveTo(p);
   					else 
					    cdc->LineTo(p);
					}
					fPrevValidPoint = fValidPoint;
				}
				cdc->SelectObject(penOld);
			}
			break;
		case rdtNeedle: 
			{
				CPen pen(PS_SOLID, 1, color);
				CPen* penOld = cdc->SelectObject(&pen);
 	      bool f360Deg = rdd->rdaxX->deg == RoseDiagramAxis::deg360;
      	CPoint cp = CPoint(f360Deg ? 500 : 1000, 500); // center point
        CPoint p1 = rdd->ptPos(cp.x, cp.y); // center point
				for (int i=1; i <= iNrPoints(); i++) {
  			  CPoint p2 = ptPos(rX(i), rY(i));
	  			cdc->MoveTo(p1);
	  			cdc->LineTo(p2);
				}
				cdc->SelectObject(penOld);
			}
			break;
		case rdtStep: 
			{
				CPoint p1, p2;
				CPen pen(PS_SOLID, 1, color);
				CPen* penOld = cdc->SelectObject(&pen);
				p1 = ptPos(rX(1), rY(1));
				cdc->MoveTo(p1);
				for (int i=2; i <= iNrPoints(); i++) {
  				p1 = ptPos(rX(i-1), rY(i));
				  cdc->LineTo(p1);
  				p2 = ptPos(rX(i), rY(i));
					CPoint pr1 = ptPos(0, rY(i));
					CPoint pr2 = ptPos(90, rY(i));
					CPoint pr3 = ptPos(180, rY(i));
					CRect rect(pr3.x, pr2.y, pr1.x, pr1.y + (pr1.y-pr2.y));
					if (p1 != p2)
  					cdc->Arc(rect, p1, p2);
					cdc->MoveTo(p2);
				}
				cdc->SelectObject(penOld);
			}
			break;
	}
}

int RoseDiagramLayer::iNrPoints() 
{
	if (colX.fValid())
  	return colX->iRecs();
	return tbl->iRecs();
}

double RoseDiagramLayer::rX(int iRec)
{
	if (colX.fValid())
  	return colX->rValue(iRec);
  return iRec;
}

double RoseDiagramLayer::rY(int iRec)
{
	return colY->rValue(iRec);
}

String RoseDiagramLayer::sName()
{
  String s = "- ";
	if (colX.fValid())
		s = String("%S x ", colX->sName());
  s &= colY->sName();
	return s;
}

GraphLayerOptionsForm* RoseDiagramLayer::frmOptions()
{
  GraphLayerOptionsForm* glof = new GraphLayerOptionsForm(this);
	glof->create();
	return glof;
}

CPoint RoseDiagramLayer::ptPos(double x, double y)
{
	bool f360Deg = rdd->rdaxX->deg == RoseDiagramAxis::deg360;
	CPoint cp = CPoint(f360Deg ? 500 : 1000, 500); // center point
	double rRad = rdd->rdaxX->rRad(x);
	double rL1 = rdd->rdaxY->rLen(y);
	double rL2 = rL1;
  if (!f360Deg)
		rL2 *= 2;
  return rdd->ptPos(cp.x-sin(rRad)*rL2, cp.y+cos(rRad)*rL1);
}
