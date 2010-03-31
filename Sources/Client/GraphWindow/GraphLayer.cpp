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
// GraphLayer.cpp: implementation of the GraphLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblview.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Scripting\Calc.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Engine\Base\Algorithm\Lsf.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Hs\Table.hs"
#include "Headers\Htp\Ilwismen.htp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//// GraphLayer

GraphLayer::GraphLayer(GraphDrawer* grd)
: gd(grd), fRprColor(true), fShow(true)
{
}

GraphLayer::GraphLayer(GraphDrawer* grd, const FileName& fn, const String& sSection)
: gd(grd), fRprColor(true), fShow(true)
{
	ObjectInfo::ReadElement(sSection.scVal(), "Name", fn, sTitle);
  line.Read(sSection.scVal(), "Line", fn);
	ObjectInfo::ReadElement(sSection.scVal(), "Color", fn, color);
	ObjectInfo::ReadElement(sSection.scVal(), "RprColor", fn, fRprColor);
	String sSymbolDescr;
	ObjectInfo::ReadElement(sSection.scVal(), "Symbol", fn, sSymbolDescr);
	sscanf(sSymbolDescr.scVal(), "%i %i %i %i %i", &smb.iSize, &smb.iWidth, &smb.col, &smb.fillCol, &smb.smb);
}

GraphLayer::~GraphLayer()
{
}

void GraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
	ObjectInfo::WriteElement(sSection.scVal(), "Name", fn, sTitle);
  line.Write(sSection.scVal(), "Line", fn);
	ObjectInfo::WriteElement(sSection.scVal(), "Color", fn, color);
	ObjectInfo::WriteElement(sSection.scVal(), "RprColor", fn, fRprColor);
	String sSymbolDescr("%i %i %i %i %i", smb.iSize, smb.iWidth, (int)smb.col, (int)smb.fillCol, smb.smb);
	ObjectInfo::WriteElement(sSection.scVal(), "Symbol", fn, sSymbolDescr);
}

bool GraphLayer::fConfig()
{
	return false;
}

void GraphLayer::draw(CDC* cdc)
{
  // do nothing
}

int GraphLayer::iNrPoints() 
{
	return iUNDEF;
}

double GraphLayer::rX(int i)
{
	return rUNDEF;
}

double GraphLayer::rY(int i)
{
	return rUNDEF;
}

String GraphLayer::sName()
{
	return sUNDEF;
}

GraphLayerOptionsForm* GraphLayer::frmOptions()
{
	return 0;
}

/// CartesianGraphlayer

CartesianGraphLayer::CartesianGraphLayer(CartesianGraphDrawer* gd)
: GraphLayer(gd), cgd(gd), fYAxisLeft(true), cgt(cgtContineous) 
{
}

CartesianGraphLayer::CartesianGraphLayer(CartesianGraphDrawer* gd, const FileName& fn, const String& sSection)
: GraphLayer(gd, fn, sSection), cgd(gd), fYAxisLeft(true), cgt(cgtContineous)
{
	ObjectInfo::ReadElement(sSection.scVal(), "YAxisLeft", fn, fYAxisLeft);
	String sGraphType;
	ObjectInfo::ReadElement(sSection.scVal(), "GraphType", fn, sGraphType);
	cgt = cgtContineous;
	if (fCIStrEqual(sGraphType, "Contineous"))
		cgt = cgtContineous;
	else if(fCIStrEqual(sGraphType, "Step"))
		cgt = cgtStep;
	else if (fCIStrEqual(sGraphType, "Bar"))
		cgt = cgtBar;
	else if (fCIStrEqual(sGraphType, "Needle"))
		cgt = cgtNeedle; 
	else if (fCIStrEqual(sGraphType, "Point"))
		cgt = cgtPoint; 
}

CartesianGraphLayer::~CartesianGraphLayer()
{
}

void CartesianGraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
  GraphLayer::SaveSettings(fn, sSection);
	ObjectInfo::WriteElement(sSection.scVal(), "YAxisLeft", fn, fYAxisLeft);
  
	String sGraphType;
  switch (cgt) {
		case cgtContineous: sGraphType = "Contineous";	break;
		case cgtStep:       sGraphType = "Step";break; 
		case cgtBar:        sGraphType = "Bar";break; 
		case cgtNeedle:     sGraphType = "Needle";break; 
		case cgtPoint:      sGraphType = "Point";break; 
		default: break;
	}
	if (sGraphType.length()>0)
  	ObjectInfo::WriteElement(sSection.scVal(), "GraphType", fn, sGraphType);
}

bool CartesianGraphLayer::fConfig()
{
	return cgd->fConfigure(this);
}

namespace {
	struct XY 
	{ 
		XY(double x, double y)
			: rX(x), rY(y) {}
		double rX, rY;
		friend bool operator < (const XY& a, const XY& b)
		{
			return a.rX < b.rX;
		}
	};
};


void CartesianGraphLayer::draw(CDC* cdc)
{
  if (!fShow)
    return;
  if (iNrPoints() == 0)
		return;
	GraphAxis* gaxisX = cgd->gaxX;
	GraphAxis* gaxisY = fYAxisLeft ? cgd->gaxYLeft : cgd->gaxYRight;
  double rMinX, rMaxX, rMinY, rMaxY;
  rMinX = gaxisX->rMin();
  rMaxX = gaxisX->rMax();
  rMinY = gaxisY->rMin();
  rMaxY = gaxisY->rMax();
//CRgn reg;
//	reg.CreateRectRgnIndirect(&cgd->rectClip());
//  cdc->SelectClipRgn(&reg);
	switch (cgt) {
		case cgtContineous: 
			{
				set<XY> sxy;
				double x, y;
				CPoint p;
				for (int i=1; i <= iNrPoints(); i++) 
				{
					double rx = rX(i);
          if (rx < rMinX || rx > rMaxX)
            continue;
 			    bool fValidPoint = rx != rUNDEF;
					if (fValidPoint) {
  					x = gaxisX->rConv(rx + gaxisX->rShift());
						double r = rY(i);
 						fValidPoint = r != rUNDEF;
            if (r < rMinY || r > rMaxY)
              fValidPoint = false;
						if (fValidPoint) 
							y = gaxisY->rConv(r);
						else
							y = rUNDEF;
						sxy.insert(XY(x,y));
					}
				}
				bool fValidPoint;
				bool fPrevValidPoint = false;
        zPoint* ap = new zPoint[iNrPoints()];
        int iPnt = 0;
				for (set<XY>::const_iterator iter = sxy.begin(); iter != sxy.end(); ++iter)
				{
					XY xy = *iter;
					x = xy.rX;
					y = xy.rY;
					fValidPoint = y != rUNDEF;
					if (fValidPoint) {
            ap[iPnt++] = cgd->ptPos(y, x);
					}
          else {
            if (iPnt > 0)
              line.drawLine(cdc,ap,iPnt);
            iPnt = 0;
          }
				}
        if (iPnt > 0)
          line.drawLine(cdc,ap,iPnt);
        delete ap;
			}
			break;
		case cgtNeedle: 
			{
				double x, y;
				CPoint p;
				CPen pen(PS_SOLID, 1, color);
				CPen* penOld = cdc->SelectObject(&pen);
				for (int i=1; i <= iNrPoints(); i++) {
					double rx = rX(i);
					if (rx == rUNDEF)
						continue;
          if (rx < rMinX || rx > rMaxX)
            continue;
  				x = gaxisX->rConv(rx + gaxisX->rShift());
					y = gaxisY->rConv(gaxisY->rMin());
  				p = cgd->ptPos(y, x);
	  			cdc->MoveTo(p);
					double r = rY(i);
					if (r == rUNDEF)
						continue;
          if (r < rMinY || r > rMaxY)
            continue;
					y = gaxisY->rConv(r);
  				p = cgd->ptPos(y, x);
					cdc->LineTo(p);
				}
				cdc->SelectObject(penOld);
			}
			break;
		case cgtPoint: 
			{
				double x, y;
				CPoint p;
				for (int i=1; i <= iNrPoints(); i++) {
					double rx = rX(i);
					if (rx == rUNDEF)
						continue;
          if (rx < rMinX || rx > rMaxX)
            continue;
  				x = gaxisX->rConv(rx + gaxisX->rShift());
					double r = rY(i);
					if (r == rUNDEF)
						continue;
          if (r < rMinY || r > rMaxY)
            continue;
					y = gaxisY->rConv(r);
  				CPoint p = cgd->ptPos(y, x);
					smb.drawSmb(cdc, 0, p);
				}
			}
			break;
		case cgtStep: 
			{
				set<XY> sxy;
				double x, y;
				for (int i=1; i <= iNrPoints(); i++) 
				{
					double rx = rX(i);
          if (rx < rMinX || rx > rMaxX)
            continue;
 			    bool fValidPoint = rx != rUNDEF;
					if (fValidPoint) {
  					x = gaxisX->rConv(rx + gaxisX->rShift());
						double r = rY(i);
 						fValidPoint = r != rUNDEF;
            if (r < rMinY || r > rMaxY)
              fValidPoint = false;
						if (fValidPoint) 
							y = gaxisY->rConv(r);
						else
							y = rUNDEF;
						sxy.insert(XY(x,y));
					}
				}

				CPoint p, pOld;
				CPen pen(PS_SOLID, 1, color);
				CPen* penOld = cdc->SelectObject(&pen);
				bool fValidPoint;
				bool fPrevValidPoint = false;
				for (set<XY>::const_iterator iter = sxy.begin(); iter != sxy.end(); ++iter)
				{
					XY xy = *iter;
					x = xy.rX;
					y = xy.rY;
					fValidPoint = y != rUNDEF;
  				p = cgd->ptPos(y, x);
					if (fPrevValidPoint) {
						cdc->LineTo(p.x, pOld.y);
						cdc->LineTo(p);
					}
					else
						cdc->MoveTo(p);
					pOld = p;
					fPrevValidPoint = true;
				}
				cdc->SelectObject(penOld);
			}
			break;
		case cgtBar: 
			{
				double x, y;
				CPoint p1, p2;
				CPen pen(PS_SOLID, 1, Color(0,0,0));
				CBrush br(color);
				CPen* penOld = cdc->SelectObject(&pen);
				CBrush* brOld = cdc->SelectObject(&br);
				int iBars = cgd->iNrBarGraphs();
				int iCurBar = cgd->iBarGraph(this);
				for (int i=1; i <= iNrPoints(); i++) {
					double rx = rX(i);
					if (rx == rUNDEF)
						continue;
          if (rx < rMinX || rx > rMaxX) 
            continue;
					x = gaxisX->rConv(rx);
					double r = rY(i);
					if (r == rUNDEF)
						continue;
          if (r < rMinY || r > rMaxY) 
            continue;
					y = gaxisY->rConv(r);
  				p1 = cgd->ptPos(y, x);
  				x = gaxisX->rConv(rx+1);
					y = gaxisY->rConv(gaxisY->rMin());
					p2 = cgd->ptPos(y, x);
					if (iBars > 1) {
  					int iW = p2.x - p1.x + 1;
	  				p1.x += double(iCurBar * iW) / iBars;
		  			p2.x = p1.x + double(iW) / iBars;
					}
					if (p2.x < p1.x + 2) // prevent empty rectangles
						p2.x = p1.x + 2;
					if (fRprColor && rpr.fValid()) {
				    cdc->SelectObject(brOld);
						Color col;
						if (0 != gaxisX->ds)
						  col = rpr->clrRaw(gaxisX->ds->iKey(rx));
						else
						  col = rpr->clr(r);
    				CBrush br(col);
				    brOld = cdc->SelectObject(&br);
  				  cdc->Rectangle(p1.x, p1.y, p2.x, p2.y);
					}
					else
  				  cdc->Rectangle(p1.x, p1.y, p2.x, p2.y);
				}
				cdc->SelectObject(penOld);
				cdc->SelectObject(brOld);
			}
			break;
	}
//  CRect rect = cgd->rectClip();
//	cgd->ClipAfterwards(cdc, rect);
}

int CartesianGraphLayer::iNrPoints() 
{
	return 101;
}

double CartesianGraphLayer::rX(int i)
{
	if (cgd->gaxX->ds)
		return i;
	else
		return cgd->gaxX->rMin() + (i-1) * (cgd->gaxX->rMax()-cgd->gaxX->rMin())/100;
}

String CartesianGraphLayer::sName()
{
	return sUNDEF;
}

CartesianGraphLayer* CartesianGraphLayer::create(const FileName& fn, const String& sSection, CartesianGraphDrawer* gd)
{
	String sType;
	ObjectInfo::ReadElement(sSection.scVal(), "Type", fn, sType);
	if (fCIStrEqual(sType, "Column"))
		return new ColumnGraphLayer(gd, fn, sSection);
	if (fCIStrEqual(sType, "Formula"))
		return new FormulaGraphLayer(gd, fn, sSection);
	if (fCIStrEqual(sType, "LeastSquareFit"))
		return new LsfGraphLayer(gd, fn, sSection);
	if (fCIStrEqual(sType, "SemiVariogram"))
		return new SmvGraphLayer(gd, fn, sSection);
	return 0;
}


//// ColumnGraphLayer
ColumnGraphLayer::ColumnGraphLayer(CartesianGraphDrawer* cgd, const Table& t, const Column& cX, const Column& cY)
: CartesianGraphLayer(cgd), tbl(t), colX(cX), colY(cY)
{
	Domain dm;
	if (cX.fValid())
		dm = cX->dm();
	else
		dm = tbl->dm();
	if (dm->pdc()) {
		cgt = cgtBar;
		rpr = dm->rpr();
	}	
	else if (dm->pdid())
		cgt = cgtPoint;
	else 
		cgt = cgtPoint;
  sTitle = sName();
}

ColumnGraphLayer::ColumnGraphLayer(CartesianGraphDrawer* cgd, const FileName& fn, const String& sSection)
: CartesianGraphLayer(cgd, fn, sSection)
{
	ObjectInfo::ReadElement(sSection.scVal(), "Table", fn, tbl);
  if (!tbl.fValid())
    return;
	String sCol;
  ObjectInfo::ReadElement(sSection.scVal(), "ColumnX", fn, sCol);
	if (sCol.length() > 0)
	  colX = tbl->col(sCol);
	sCol = "";
	ObjectInfo::ReadElement(sSection.scVal(), "ColumnY", fn, sCol);
	if (sCol.length() > 0)
  	colY = tbl->col(sCol);

	Domain dm;
	if (colX.fValid())
		dm = colX->dm();
	else
		dm = tbl->dm();
	if (dm->pdc()) {
		rpr = dm->rpr();
	}	
}

ColumnGraphLayer::~ColumnGraphLayer()
{
}

void ColumnGraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
	CartesianGraphLayer::SaveSettings(fn, sSection);
	ObjectInfo::WriteElement(sSection.scVal(), "Type", fn, "Column");
	String sTblName = tbl->fnObj.sRelative();
	ObjectInfo::WriteElement(sSection.scVal(), "Table", fn, sTblName);
	if (colX.fValid())
	  ObjectInfo::WriteElement(sSection.scVal(), "ColumnX", fn, colX);
	if (colY.fValid())
	  ObjectInfo::WriteElement(sSection.scVal(), "ColumnY", fn, colY);
}

int ColumnGraphLayer::iNrPoints() 
{
	if (colX.fValid())
  	return colX->iRecs();
	return tbl->iRecs();
}

double ColumnGraphLayer::rX(int iRec)
{
	if (colX.fValid())
		if (colX->fValues())
  		return colX->rValue(iRec);
		else 
			return cgd->gaxX->ds->iOrd(colX->iRaw(iRec));
  return iRec;
}

double ColumnGraphLayer::rY(int iRec)
{
	GraphAxis* gaxisX = cgd->gaxX;		 
	if (!colX.fValid() && 0 != gaxisX->ds)
		iRec = gaxisX->ds->iKey(iRec);
  if (colY.fValid())
	  return colY->rValue(iRec);
  return 0;
}

void ColumnGraphLayer::draw(CDC* cdc)
{
	CartesianGraphLayer::draw(cdc);
}


String ColumnGraphLayer::sName()
{
	String s = "- ";
	if (colX.fValid())
		s = String("%S x ", colX->sName());
  if (colY.fValid())
    s &= colY->sName();
	return s;
}

bool ColumnGraphLayer::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(ColumnGraphLayer* cgl)
    : FormWithDest(0, SGPTitleGraphOptionsFromCol)
    {
      iImg = IlwWinApp()->iImage("Graph");
//      new CheckBox(root, SGPUiShow, &cgl->fShow);
      FieldString* fs = new FieldString(root, SGPUiName, &cgl->sTitle);
      fs->SetWidth(120);
      RadioGroup* rgType;
  		rgType = new RadioGroup(root, "", (int*)&cgl->cgt);
      RadioButton* rbLine = new RadioButton(rgType, SGPUiLine);
      RadioButton* rbStep = new RadioButton(rgType, SGPUiStep);
    	RadioButton* rbBar = new RadioButton(rgType, SGPUiBar);
      RadioButton* rbNeedle = new RadioButton(rgType, SGPUiNeedle);
    	RadioButton* rbPoint = new RadioButton(rgType, SGPUiPoint);

      FieldGroup* fg = new FieldGroup(rbLine, true);
      new FieldLine(fg, &cgl->line, true);

      fg = new FieldGroup(rbStep, true);
      new FieldColor(fg, SGPUiColor, &cgl->color);

      fg = new FieldGroup(rbNeedle, true);
      new FieldColor(fg, SGPUiColor, &cgl->color);
       
    	fg = new FieldGroup(rbBar, true);
    	iColor = cgl->fRprColor ? 1 : 0;
    	CartesianGraphDrawer* cgd = cgl->cgd;
    	if (0 != cgd->gaxX->ds && 0 != cgd->gaxX->ds->pdc()) 
      {
    		RadioGroup *rgCol = new RadioGroup(fg,"", &iColor);
    		rgCol->Align(rbBar, AL_AFTER);
    		RadioButton *rbSingle = new RadioButton(rgCol, SGPUiSingleColor);
    		RadioButton *rbFollow = new RadioButton(rgCol, SGPUiFollowsRepr);
    		rbSingle->SetIndependentPos();
    		new FieldColor(rbSingle, "", &cgl->color);
    	}
    	else {
    		iColor = 0;
    		new FieldColor(fg, SGPUiColor, &cgl->color);
    	}

    	fg = new FieldGroup(rbPoint, true);
    	fg->Align(rbLine, AL_AFTER);
    	smb = &cgl->smb;
    	fsmb = new FieldSymbol(fg, SGPUiSmbType, (long*)&smb->smb, &smb->hIcon);
    	fsmb->SetCallBack((NotifyProc)&ConfigForm::FieldSymbolCallBack);
    	new FieldInt(fg, SGPUiSize, &smb->iSize, ValueRangeInt(1L,250L));
    	ffc = new FieldFillColor(fg, SGPUiColor, &smb->fillCol);
    	new FieldInt(fg, SGPUiLineWidth, &smb->iWidth, ValueRangeInt(1L,100L));
    	new FieldColor(fg, SGPUiLineColor, &smb->col);
      
    	RadioGroup* rgax = 0;
      if (0 != cgl) {
      	iYAxis = !cgl->fYAxisLeft;
    		rgax = new RadioGroup(root, SGPUiUseYAxis, &iYAxis, true);
    		rgax->Align(rgType , AL_UNDER);
    		new RadioButton(rgax, SGPUiLeft);
    		new RadioButton(rgax, SGPUiRight);
    		rgax->SetIndependentPos();
    	}
      SetMenHelpTopic(htpEditGraph);
      create();
    }
  	int iColor, iYAxis;
  private:    
    int FieldSymbolCallBack(Event*)
    {
      fsmb->StoreData();
      switch (SymbolType(smb->smb)) {
        case smbCircle:
        case smbSquare:
        case smbDiamond:
        case smbDeltaUp:
        case smbDeltaDown:
          ffc->Show();
          break;
        case smbPlus:
        case smbMinus:
        case smbCross:
          ffc->Hide();
          break;
      }
      return 0;
    }
  	FieldFillColor* ffc;
    FieldSymbol* fsmb;
  	Symbol* smb;
  };
  ConfigForm frm(this);
  if (!frm.fOkClicked())
    return false;
	switch (cgt) {
		case cgtBar:
			fRprColor = frm.iColor==1;
			break;
		case cgtPoint:
			color = smb.col;
			break;
	}
	fYAxisLeft = 0 == frm.iYAxis;
  if (fRprColor) 
  {
		Domain dm = cgd->gaxX->dvrs.dm();
		if (dm.fValid()) 
			rpr = dm->rpr();
	}
  return true;
}

GraphLayerOptionsForm* ColumnGraphLayer::frmOptions()
{
  GraphLayerOptionsForm* glof = new GraphLayerOptionsForm(this);
	glof->create();
	return glof;
}

// Formula graph layer

FormulaGraphLayer::FormulaGraphLayer(CartesianGraphDrawer* cgd, const String& sExp)
: CartesianGraphLayer(cgd), inst(0)
{
	SetExpression(sExp);
  sTitle = sName();
}

FormulaGraphLayer::FormulaGraphLayer(CartesianGraphDrawer* cgd, const FileName& fn, const String& sSection)
: CartesianGraphLayer(cgd, fn, sSection), inst(0)
{
  String sExp;
	ObjectInfo::ReadElement(sSection.scVal(), "Expression", fn, sExp);
	SetExpression(sExp);
}

FormulaGraphLayer::~FormulaGraphLayer()
{
	if (0 != inst)
	  delete inst;
}

void FormulaGraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
	CartesianGraphLayer::SaveSettings(fn, sSection);
	ObjectInfo::WriteElement(sSection.scVal(), "Type", fn, "Formula");
	ObjectInfo::WriteElement(sSection.scVal(), "Expression", fn, sExpr);
}

double FormulaGraphLayer::rY(int i)
{
  if (0 == inst)
    return rUNDEF;
  try {
    return inst->rCalcVal(rX(i));
  }
  catch (ErrorObject&) {
    return rUNDEF;
  }
}

String FormulaGraphLayer::sName()
{
	return sExpr;
}

bool FormulaGraphLayer::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(FormulaGraphLayer* fgl)
    : FormWithDest(0, SGPTitleGraphOptionsFormula)
    {
      iImg = IlwWinApp()->iImage("Graph");
//      new CheckBox(root, SGPUiShow, &fgl->fShow);
      FieldString* fs = new FieldString(root, SGPUiName, &fgl->sTitle);
      fs->SetWidth(120);
      new FieldLine(root, &fgl->line, true);
      sExpr = fgl->sExpr;
      fs = new FieldString(root, SGPUiFormulaY, &sExpr, Domain(), false);
      fs->SetWidth(120);
      SetMenHelpTopic(htpEditGraphFormula);
      create();      
    }
  	String sExpr;
  };
  ConfigForm frm(this);
  if (!frm.fOkClicked())
    return false;
 	SetExpression(frm.sExpr);
  return true;
}

GraphLayerOptionsForm* FormulaGraphLayer::frmOptions()
{
  GraphLayerOptionsForm* glof = new FormulaGraphLayerOptionsForm(this);
	glof->create();
	return glof;
}

void FormulaGraphLayer::SetExpression(const String& sExp)
{
	if (0 == sExp.length())
		return;
	if (0 != inst)
		delete inst;
  inst = 0;
  try {
    inst = Calculator::instExprX(sExp);
  }
  catch (const ErrorObject& err) {
    err.Show();
		return;
  }
	sExpr = sExp;
}

// Lsf graph layer

LsfGraphLayer::LsfGraphLayer(CartesianGraphDrawer* cgd, const Table& t, const Column& colX, const Column& colY, const String& sFunc, int iTerms)
: ColumnGraphLayer(cgd, t, colX, colY), lsf(0)
{
	SetLsf(sFunc, iTerms);
  sTitle = sName();
  cgt = cgtContineous;
}  

LsfGraphLayer::LsfGraphLayer(CartesianGraphDrawer* cgd, const FileName& fn, const String& sSection)
: ColumnGraphLayer(cgd, fn, sSection), lsf(0)
{
	String sFunc;	
	int iTerms;
	ObjectInfo::ReadElement(sSection.scVal(), "Func", fn, sFunc);
	ObjectInfo::ReadElement(sSection.scVal(), "Terms", fn, iTerms);
	SetLsf(sFunc, iTerms);
}

LsfGraphLayer::~LsfGraphLayer()
{
}

void LsfGraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
	ColumnGraphLayer::SaveSettings(fn, sSection);
	ObjectInfo::WriteElement(sSection.scVal(), "Type", fn, "LeastSquareFit");
	ObjectInfo::WriteElement(sSection.scVal(), "Func", fn, sFitType);
	ObjectInfo::WriteElement(sSection.scVal(), "Terms", fn, iTerms);
}

double LsfGraphLayer::rY(int i)
{
	if (0 == lsf)
		return rUNDEF;
  return lsf->rCalc(rX(i));
}

String LsfGraphLayer::sName()
{
	String s = ColumnGraphLayer::sName();
	return String("%S - %S", s, sGName);
}

bool LsfGraphLayer::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(LsfGraphLayer* gl)
    : FormWithDest(0, SGPTitleGraphOptionsLSF)
    , lgl(gl)
    {
      iImg = IlwWinApp()->iImage("Graph");
//      new CheckBox(root, SGPUiShow, &lgl->fShow);
      FieldString* fs = new FieldString(root, SGPUiName, &lgl->sTitle);
      fs->SetWidth(120);
      new FieldLine(root, &lgl->line, true);

    	sFitType = 0; 
    	iTerms = gl->iTerms;
    	frf = new FieldRegressionFunc(root, STBUiFunction, &sFitType, gl->sFitType);
    	frf->SetCallBack((NotifyProc)&ConfigForm::FitFuncCallBack);
    	String sFill('X', 50);
    	stRegr = new StaticText(root, sFill);
    	stRegr->SetVal(String());
    	stRegr->SetIndependentPos();
    	fiTerms = new FieldInt(root, STBUiNrTerms, &iTerms, RangeInt(2,100));
      if (0 != gl->lsf) {
      	StaticText* stFormula = new StaticText(root, lgl->lsf->sFormula());
      	stFormula->SetIndependentPos();
      }
      SetMenHelpTopic(htpEditGraphLsf);
      create();      
    }
  	int iTerms;
  	String* sFitType;
  private:  
  	LsfGraphLayer* lgl;
  	FieldRegressionFunc* frf;
  	StaticText* stRegr;
  	FieldInt* fiTerms;
    int FitFuncCallBack(Event*) 
    {
    	frf->StoreData();
    	if (0 == sFitType->length())
    		return 0;
    	if (fCIStrEqual(*sFitType, "polynomial")) {
    		fiTerms->SetVal(2);
    		fiTerms->Show();
    		stRegr->SetVal("y = a + b x + c x^2 + d x^3 + ...");
    	}
    	else if (fCIStrEqual(*sFitType, "trigonometric")) {
    		fiTerms->SetVal(3);
    		fiTerms->Show();
    		stRegr->SetVal("y = a + b cos x + c sin x + ...");
    	}
    	else {
    		iTerms = 2;
    		fiTerms->Hide();
    		if (fCIStrEqual(*sFitType, "power"))
    			stRegr->SetVal("y = a x^b");
    		else if (fCIStrEqual(*sFitType, "exponential"))
    			stRegr->SetVal("y = a e^bx");
    		else if (fCIStrEqual(*sFitType, "logarithmic"))
    			stRegr->SetVal("y = a log bx");
    	}
    	return 0;
    }
  };
  ConfigForm frm(this);
  if (!frm.fOkClicked())
    return false;
	SetLsf(*frm.sFitType, frm.iTerms);
  return true;
}

GraphLayerOptionsForm* LsfGraphLayer::frmOptions()
{
  GraphLayerOptionsForm* glof = new LsfGraphLayerOptionsForm(this);
	glof->create();
	return glof;
}


void LsfGraphLayer::SetLsf(const String& sFunc, int iNrTrms)
{
	if (0 != lsf)
		delete lsf;
  int ic = colX->iOffset();
  iNrValidPnts = 0;
  for (int i = 0; i < tbl->iRecs(); ++i, ++ic) {
    if (colX->rValue(ic) == rUNDEF || colY->rValue(ic) == rUNDEF)
      continue;
    iNrValidPnts++;     // count all valid X,Y points
  }

  CVector cvX(iNrValidPnts), cvY(iNrValidPnts);
  long j = 0;
  ic = colX->iOffset();
  for (int i = 0; i < tbl->iRecs(); ++i, ++ic) {
    if (colX->rValue(ic) == rUNDEF || colY->rValue(ic) == rUNDEF)
      continue;         // skip non_valid points
    cvX(j) = colX->rValue(ic);
    cvY(j) = colY->rValue(ic);
    j++;
  }

  try {
    if ("polynomial" == sFunc) {
      lsf = new LeastSquaresFitPolynomial(cvX,cvY,iNrTrms);
      sGName = String("Polynomial (%i)", iNrTrms);
    }
    else if ("trigonometric" == sFunc) {
      lsf = new LeastSquaresFitTrigonometric(cvX,cvY,iNrTrms);
      sGName = String("Trigonometric (%i)", iNrTrms);
    }
    else if ("power" == sFunc) {
      lsf = new LeastSquaresFitPower(cvX,cvY);
      sGName = "Power";
    }
    else if ("exponential" == sFunc) {
      lsf = new LeastSquaresFitExponential(cvX,cvY);
      sGName = "Exponential";
    }
    else if ("logarithmic" == sFunc) {
      lsf = new LeastSquaresFitLogarithmic(cvX,cvY);
      sGName = "Logarithmic";
    }
  }
  catch (ErrorObject& err) {
    err.Show();
    lsf = 0;
		return;
  }
	sFitType = sFunc;
	iTerms = iNrTrms;
}

// Semivariogram Graph layer

SmvGraphLayer::SmvGraphLayer(CartesianGraphDrawer* cgd, const SemiVariogram& semivar)
: CartesianGraphLayer(cgd)
, smv(semivar)
{
  sTitle = sName();
}

SmvGraphLayer::SmvGraphLayer(CartesianGraphDrawer* cgd, const FileName& fn, const String& sSection)
: CartesianGraphLayer(cgd, fn, sSection)
{
	smv = SemiVariogram(fn, sSection.scVal());
}

SmvGraphLayer::~SmvGraphLayer()
{
}

void SmvGraphLayer::SaveSettings(const FileName& fn, const String& sSection)
{
	CartesianGraphLayer::SaveSettings(fn, sSection);
	ObjectInfo::WriteElement(sSection.scVal(), "Type", fn, "SemiVariogram");
	smv.Store(fn, sSection.scVal());
}

double SmvGraphLayer::rY(int i)
{
	return smv.rCalc(rX(i));
}

String SmvGraphLayer::sName()
{
	return String(smv.sModel());
}

bool SmvGraphLayer::fConfig()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(SmvGraphLayer* sgl)
    : FormWithDest(0, SGPTitleGraphOptionsSMV)
    {
      iImg = IlwWinApp()->iImage("Graph");
//      new CheckBox(root, SGPUiShow, &sgl->fShow);
      FieldString* fs = new FieldString(root, SGPUiName, &sgl->sTitle);
      fs->SetWidth(120);
      new FieldLine(root, &sgl->line, true);
    	new FieldSemiVariogram(root, SGPUiSemiVar, &sgl->smv);
      SetMenHelpTopic(htpEditGraphSemiVar);
      create();      
    }
  };
  ConfigForm frm(this);
  return frm.fOkClicked();
}

GraphLayerOptionsForm* SmvGraphLayer::frmOptions()
{
  GraphLayerOptionsForm* glof = new SmvGraphLayerOptionsForm(this);
	glof->create();
	return glof;
}

