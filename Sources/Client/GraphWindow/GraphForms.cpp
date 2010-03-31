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
// GraphView.cpp: implementation of the GraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphView.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\FormElements\fentvalr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Base\Round.h"
#include "Engine\Base\Algorithm\Lsf.h"
#include "Client\GraphWindow\GraphForms.h"


#include "Headers\Hs\Graph.hs"
#include "Headers\Hs\Table.hs"
#include "Engine\Domain\dmsort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL GraphPropertyPage::OnApply()
{
	BOOL fRet = FormBasePropertyPage::OnApply();
	if (fRet) {
		GraphOptionsForm* ps = (GraphOptionsForm*)GetParent();
		ps->gd->gvw->Invalidate();
	}
	return fRet;
}

GraphLayerOptionsForm::GraphLayerOptionsForm(GraphLayer* grl)
: GraphPropertyPage(grl->sName()), gl(grl), gd(grl->gd)
{
	CartesianGraphLayer* cgl = dynamic_cast<CartesianGraphLayer*>(gl);
	RoseDiagramLayer* rdl = dynamic_cast<RoseDiagramLayer*>(gl);

  RadioGroup* rgType;
	if (0 != cgl)
		rgType = new RadioGroup(root, "", (int*)&cgl->cgt);
	else
		rgType = new RadioGroup(root, "", (int*)&rdl->rdt);
  RadioButton* rbLine = new RadioButton(rgType, SGPUiLine);
  RadioButton* rbStep = new RadioButton(rgType, SGPUiStep);
	RadioButton* rbBar = 0;
	if (0 != cgl) {
    rbBar = new RadioButton(rgType, SGPUiBar);
	}
  RadioButton* rbNeedle = new RadioButton(rgType, SGPUiNeedle);
	RadioButton* rbPoint = 0;
	if (0 != cgl) {
    rbPoint = new RadioButton(rgType, SGPUiPoint);
	}

  FieldGroup* fg = new FieldGroup(rbLine, true);
  new FieldColor(fg, SGPUiColor, &gl->color);

  fg = new FieldGroup(rbStep, true);
  new FieldColor(fg, SGPUiColor, &gl->color);

  fg = new FieldGroup(rbNeedle, true);
  new FieldColor(fg, SGPUiColor, &gl->color);
   
	if (0 != rbBar) {
		fg = new FieldGroup(rbBar, true);
		iColor = !gl->fRprColor ? 0 : 1;
		CartesianGraphDrawer* cgd = cgl->cgd;
		if (0 != cgd->gaxX->ds && 0 != cgd->gaxX->ds->pdc()) {
			RadioGroup *rgCol = new RadioGroup(fg,"", &iColor);
			rgCol->Align(rbBar, AL_AFTER);
			RadioButton *rbSingle = new RadioButton(rgCol, SGPUiSingleColor);
			RadioButton *rbFollow = new RadioButton(rgCol, SGPUiFollowsRepr);
			rbSingle->SetIndependentPos();
			new FieldColor(rbSingle, "", &gl->color);
		}
		else {
			iColor = 0;
			new FieldColor(fg, SGPUiColor, &gl->color);
		}
	}

  if (0 != rbPoint) {
		fg = new FieldGroup(rbPoint, true);
		fg->Align(rbLine, AL_AFTER);
		smb = &gl->smb;
		fsmb = new FieldSymbol(fg, SGPUiSmbType, (long*)&smb->smb, &smb->hIcon);
		fsmb->SetCallBack((NotifyProc)&GraphLayerOptionsForm::FieldSymbolCallBack);
		new FieldInt(fg, SGPUiSize, &smb->iSize, ValueRangeInt(1L,250L));
		ffc = new FieldFillColor(fg, SGPUiColor, &smb->fillCol);
		new FieldInt(fg, SGPUiLineWidth, &smb->iWidth, ValueRangeInt(1L,100L));
		new FieldColor(fg, SGPUiLineColor, &smb->col);
	}
	RadioGroup* rgax = 0;
  if (0 != cgl) {
  	iYAxis = !cgl->fYAxisLeft;
		rgax = new RadioGroup(root, SGPUiUseYAxis, &iYAxis, true);
		rgax->Align(rbPoint , AL_UNDER);
		new RadioButton(rgax, SGPUiLeft);
		new RadioButton(rgax, SGPUiRight);
		rgax->SetIndependentPos();
	}
  PushButton* pbRemove = new PushButton(root, SGPUiRemoveGraph, (NotifyProc)&GraphLayerOptionsForm::Remove);
	if (0 != rgax)
    pbRemove->Align(rgax, AL_UNDER);
  pbRemove->SetIndependentPos();
}

GraphLayerOptionsForm::~GraphLayerOptionsForm() 
{
 
}

int GraphLayerOptionsForm::Remove(Event* Evt)
{
 	GraphOptionsForm* ps = (GraphOptionsForm*)GetParent();
	for(vector<GraphLayer*>::iterator cur = gd->agl.begin(); cur != gd->agl.end(); ++cur)
		if (*cur == gl) {
    	gd->agl.erase(cur);
			break;
		}
	ps->Refill(ps->GetActiveIndex()-1);
  return 0;
}


int GraphLayerOptionsForm::FieldSymbolCallBack(Event*)
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


int GraphLayerOptionsForm::exec()
{ 
	FormBasePropertyPage::exec();
	CartesianGraphLayer* cgl = dynamic_cast<CartesianGraphLayer*>(gl);
  if (0 != cgl) {
		switch (cgl->cgt) {
			case cgtBar:
				cgl->fRprColor = iColor==1;
				break;
			case cgtPoint:
				cgl->color = cgl->smb.col;
				break;
		}
	  cgl->fYAxisLeft = iYAxis == 0;
    if (gl->fRprColor) {
			CartesianGraphDrawer* cgd = cgl->cgd;
			Domain dm = cgd->gaxX->dvrs.dm();
			if (dm.fValid()) {
				try {
					gl->rpr = dm->rpr();
				}
				catch (const ErrorObject& err) {
					err.Show();
				}
			}
		}
	}	
  return 1;
	
}

// FormulaGraphLayerOptionsForm

FormulaGraphLayerOptionsForm::FormulaGraphLayerOptionsForm(FormulaGraphLayer* gl)
: GraphLayerOptionsForm(gl), fgl(gl)
{
	sExpr = gl->sExpr;
	FieldString* fs = new FieldString(root, "y = ", &sExpr, Domain(), false);
	fs->SetIndependentPos();
}

FormulaGraphLayerOptionsForm::~FormulaGraphLayerOptionsForm()
{
}

int FormulaGraphLayerOptionsForm::exec()
{ 
	GraphLayerOptionsForm::exec();
	fgl->SetExpression(sExpr);
  return 1;
}
// LsfGraphLayerOptionsForm

LsfGraphLayerOptionsForm::LsfGraphLayerOptionsForm(LsfGraphLayer* gl)
: GraphLayerOptionsForm(gl), lsfgl(gl)
{
	sFitType = 0; 
	iTerms = gl->iTerms;
	frf = new FieldRegressionFunc(root, STBUiFunction, &sFitType, gl->sFitType);
	frf->SetCallBack((NotifyProc)&LsfGraphLayerOptionsForm::FitFuncCallBack);
	String sFill('X', 50);
	stRegr = new StaticText(root, sFill);
	stRegr->SetVal(String());
	stRegr->SetIndependentPos();
	fiTerms = new FieldInt(root, STBUiNrTerms, &iTerms, RangeInt(2,100));
	stFormula = new StaticText(root, sFill);
	if (0 != lsfgl->lsf)
		stFormula->SetVal(lsfgl->lsf->sFormula());
	else
		stFormula->SetVal(String());
	stFormula->SetIndependentPos();
}

LsfGraphLayerOptionsForm::~LsfGraphLayerOptionsForm()
{
}

int LsfGraphLayerOptionsForm::FitFuncCallBack(Event*) 
{
	if (0 != lsfgl->lsf)
		stFormula->SetVal(lsfgl->lsf->sFormula());
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

int LsfGraphLayerOptionsForm::exec()
{ 
	GraphLayerOptionsForm::exec();
	lsfgl->SetLsf(*sFitType, iTerms);
	if (0 != lsfgl->lsf)
	  stFormula->SetVal(lsfgl->lsf->sFormula());
  return 1;
}

// SmvGraphLayerOptionsForm

SmvGraphLayerOptionsForm::SmvGraphLayerOptionsForm(SmvGraphLayer* gl)
: GraphLayerOptionsForm(gl), smvgl(gl)
{
	new FieldSemiVariogram(root, SGPUiSemiVar, &gl->smv);
}

SmvGraphLayerOptionsForm::~SmvGraphLayerOptionsForm()
{
}

int SmvGraphLayerOptionsForm::exec()
{ 
	GraphLayerOptionsForm::exec();
//	fgl->SetExpression(sExpr);
  return 1;
}


// GraphLayerSelector

GraphLayerSelector::GraphLayerSelector(FormEntry* par, GraphLayerManPage* frm, GraphDrawer* grd)
  : BaseSelector(par),
    curArrow(Arrow),
    gmp(frm), gd(grd)
{
  style = WS_VSCROLL | LBS_WANTKEYBOARDINPUT;
  fDrag = false;
  cur = 0;
  psn->iMinWidth *= 2;
	SetIndependentPos();
}

GraphLayerSelector::~GraphLayerSelector()
{
}

void GraphLayerSelector::create()
{
  BaseSelector::create();
  lb->setNotify(gmp, (NotifyProc)&GraphLayerManPage::Select, Notify(LBN_SELCHANGE));
  lb->setNotify(gmp,(NotifyProc)&GraphLayerManPage::Prop, Notify(LBN_DBLCLK));
// drag and drop still not implemented:
	lb->setNotify(this,(NotifyProc)&GraphLayerSelector::mouseButtonDown, WM_LBUTTONDOWN);
	lb->setNotify(this,(NotifyProc)&GraphLayerSelector::mouseButtonUp, WM_LBUTTONUP);
  lb->setNotify(gmp,(NotifyProc)&GraphLayerManPage::OnKeyDown, WM_KEYDOWN);
  Fill();
  SendMessage(*lb,LB_SETTOPINDEX,0,0);
  lb->SetSel(0);
}


void GraphLayerSelector::Fill()
{
	if (0 == lb)
		return;
  lb->ResetContent();
	for (unsigned int i=0; i < gd->agl.iSize(); i++) {
    GraphLayer* gl = gd->agl[i];
    int ind = lb->AddString(gl->sName().scVal());
    SendMessage(*lb,LB_SETITEMDATA,ind,(LPARAM)gl);
  }
  lb->SetSel(lb->GetCount()-1);
  lb->SetFocus();
}

void GraphLayerSelector::StoreData()
{
}        


String GraphLayerSelector::sName(int id)
{
  if (id < 0)
    return "?";
  CartesianGraphLayer* gl = (CartesianGraphLayer*)SendMessage(*lb, LB_GETITEMDATA, id, 0);
  String s("%S.grp", gl->sName());
  return s;
}

int GraphLayerSelector::idSelect()
{
  return lb->GetCurSel();
}

long GraphLayerSelector::DrawItem(DRAWITEMSTRUCT* dis)
{
  int id = dis->itemID;
  if (/*dis->itemAction & ODA_SELECT ||*/ id < 0)
    return 1;
  String s;
  s = sName(id);
  idw.DrawItem(dis,s,false,true,false);
  return 1;
}

int GraphLayerSelector::mouseButtonDown(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);  
	if (0 == ev)
		return 0;
	int iTop = SendMessage(*lb,LB_GETTOPINDEX,0,0);
  int iHeight = SendMessage(*lb,LB_GETITEMHEIGHT,0,0);
  idDrag = iTop + ev->pos().y / iHeight;
  if (idDrag < lb->GetCount()) {
    if (DragDetect(*lb,ev->pos())) {
			if (cur)
				delete cur;
			fDrag = true;
			cur = new zCursor("Clm16Cur");
    	::SetCapture(*lb);
			zRect rect;
			lb->GetWindowRect(rect);
			rect.top() -= 20;
			rect.bottom() += 20;
			ClipCursor((RECT*)&rect);
			::SetCursor((HCURSOR)*cur);
		}
	}
	return 0;
}

int GraphLayerSelector::mouseButtonUp(Event* Evt)
{
  MouseLBUpEvent *ev=dynamic_cast<MouseLBUpEvent *>(Evt);  
	if (0 == ev)
		return 0;
  if (!fDrag)
    return 0;
  fDrag = false;
	::ReleaseCapture();
  ClipCursor(0);
	::SetCursor((HCURSOR)curArrow);
  if (cur) {
    delete cur;
    cur = 0;
  }
  int iTop = SendMessage(*lb,LB_GETTOPINDEX,0,0);
  int iHeight = SendMessage(*lb,LB_GETITEMHEIGHT,0,0);
  int id = iTop + ev->pos().y / iHeight;
  SendMessage(*lb,LB_SETSEL,0,MAKELPARAM(id,0));
  if (id != idDrag) {
    if (id < 0)
      id = 0;
    else if (id >= gd->agl.iSize())
      id = gd->agl.iSize() - 1;
  	GraphLayer* gl = gd->agl[idDrag];
  	gd->agl.erase(gd->agl.begin()+idDrag);
	  gd->agl.insert(gd->agl.begin()+id, gl);
   	GraphOptionsForm* gof = (GraphOptionsForm*)gmp->GetParent();
  	gof->Refill(gof->GetActiveIndex());
    Fill();
    PostMessage(*lb,LB_SETTOPINDEX,id,0);
    lb->SetSel(id);
		gmp->Select(0);
		gmp->SetModified(true);
  }
  return 0;
}


////////////// GraphLayerManPage

GraphLayerManPage::GraphLayerManPage(GraphDrawer* gdr)
: GraphPropertyPage(SGPTitleGraphManagement), gd(gdr)
{
  gls = new GraphLayerSelector(root,this,gd);
  String sFill('x', 50);

  pbAdd = new PushButton(root, SGPUiAddGraph, (NotifyProc)&GraphLayerManPage::Add);
  pbAdd->SetIndependentPos();
  pbProp = new PushButton(root, SGPUiGraphConfiguration, (NotifyProc)&GraphLayerManPage::Prop);
  pbProp->Align(pbAdd, AL_AFTER);
  pbProp->SetIndependentPos();
  pbRemove = new PushButton(root, SGPUiRemoveGraph, (NotifyProc)&GraphLayerManPage::Remove);
  pbRemove->Align(pbAdd, AL_UNDER);
  pbRemove->SetIndependentPos();
  StaticText* st = new StaticText(root, SGPRemDragToOrder); 
  st->Align(pbRemove, AL_UNDER);
  st->SetIndependentPos();
  st->SetCallBack((NotifyProc)&GraphLayerManPage::Select);
	
	create();
}

GraphLayerManPage::~GraphLayerManPage()
{
}

int GraphLayerManPage::exec()
{ 
	GraphPropertyPage::exec();

	return 0;
}

int GraphLayerManPage::OnKeyDown( Event* ev) 
{
	int nKey = ev->wParm;
   if (nKey == VK_INSERT) {
      Add(0);
      return -2;
    }
    else if (nKey == VK_DELETE) {
      Remove(0);
      return -2;
    }
  return -1;
}

int GraphLayerManPage::Select(Event* Evt)
{
  int id = gls->idSelect(); 
  if (id < 0) {
    pbProp->disable();
    pbRemove->disable();
  }
  else {
    pbProp->enable();
    pbRemove->enable();
	}
  return 1;
}

int GraphLayerManPage::Prop(Event* Evt)
{
  int id = gls->idSelect(); //Evt->wParam;
  if (id < 0) 
    return 1;
  int iDummy = -1;
	GraphOptionsForm* gof = (GraphOptionsForm*)GetParent();
  gof->SetActivePage(gof->iGraphPropPages + id);	
  return 1;
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

int GraphLayerManPage::Add(Event* Evt)
{
	bool f = false;
	CartesianGraphDrawer* cgd = dynamic_cast<CartesianGraphDrawer*>(gd);
	if (0 != cgd) {
		CMenu men;
		men.CreatePopupMenu();
		add(ID_GRPH_ADD_COLUMN);
		add(ID_GRPH_ADD_FORMULA);
		add(ID_GRPH_ADD_LSF);
		add(ID_GRPH_ADD_SVM);
		CPoint p;
		GetCursorPos(&p);
		int id = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, this);
		switch (id) {
			case ID_GRPH_ADD_COLUMN:
				f = gd->fAddGraph(false); break;
			case ID_GRPH_ADD_FORMULA:
				f = gd->fAddFormulaGraph(false); break;
			case ID_GRPH_ADD_LSF:
				f = gd->fAddLsfGraph(false); break;
			case ID_GRPH_ADD_SVM:;
				f = gd->fAddSmvGraph(false); break;
			default:;
		}
	}
	else
    f = gd->fAddGraph(false);
	if (f)
		PageAdded();
  return 0;
}

int GraphLayerManPage::Remove(Event* Evt)
{
  int id = gls->idSelect();
  if (id < 0) 
    return 1;
	/// remove
	gd->agl.erase(gd->agl.begin()+id);
 	GraphOptionsForm* gof = (GraphOptionsForm*)GetParent();
	gof->Refill(gof->GetActiveIndex());
  gls->Fill();
  Select(0);
	SetModified(true);
  return 0;
}

void GraphLayerManPage::Fill()
{ 
	if (0 != gls)
		gls->Fill(); 
}


void GraphLayerManPage::PageAdded()
{
  GraphOptionsForm* ps = (GraphOptionsForm*)GetParent();
	ps->Refill(ps->GetPageCount()); // last page
	gls->Fill();
	Select(0);
	SetModified(true);
}

///// GraphAxisPage
GraphAxisPage::GraphAxisPage(GraphAxis* ga, const String& sTitle)
: GraphPropertyPage(sTitle), gaxis(ga)
{
	Init();
}

void GraphAxisPage::Init()
{ 
	if (gaxis->gap != GraphAxis::gapXRose) {
    FieldString* fs = new FieldString(root, SGPUiAxisText, &gaxis->sTitle);
    fs->SetWidth(90);
	}
  CheckBox* cb = new CheckBox(root, SGPUiShowAxis, &gaxis->fVisible);
  new CheckBox(cb, SGPUiShowGrid, &gaxis->fShowGrid);
	RoseDiagramAxis* rda = dynamic_cast<RoseDiagramAxis*>(gaxis);
	if (0 != rda && rda->gap == GraphAxis::gapXRose) {
      RadioGroup* rg = new RadioGroup(root, SGPUiRoseSize, (int*)&rda->deg, true);
			rg->Align(cb, AL_UNDER);
		  new RadioButton(rg, "1&80°");
		  new RadioButton(rg, "3&60°");
 			rg = new RadioGroup(root, SGPUiGridStep, (int*)&rda->ds, true);
	    new RadioButton(rg, "&30°");
		  new RadioButton(rg, "&45°");
		  new RadioButton(rg, "&90°");
	}
	else {
		if (gaxis->dvrs.fValues()) {
			rrMinMax = RangeReal(gaxis->rMin(), gaxis->rMax());
			rGridStep = gaxis->rGridStep();
			FieldRangeReal* frr = new FieldRangeReal(root, SGPUiMinMax, &rrMinMax, ValueRange(-1e300, 1e300, gaxis->dvrs.rStep()));
			frr->Align(cb, AL_UNDER);
			new FieldReal(root, SGPUiGridStep, &rGridStep, ValueRange(0, 1e308, gaxis->dvrs.rStep()));
		}
		else if (0 != gaxis->ds) {
			CheckBox* cbx = new CheckBox(root, SGPUiShowCodes, &gaxis->fShowCodes);
			cbx->Align(cb, AL_UNDER);
		}
		FieldInt* fi = new FieldInt(root, SGPUiTextRotation, &gaxis->iTextRot, ValueRange(0,90), true);
		if (!gaxis->dvrs.fValues() && 0 == gaxis->ds) 
			fi->Align(cb, AL_UNDER);
	}
	create();
}

int GraphAxisPage::exec()
{ 
	FormBasePropertyPage::exec();
  if (gaxis->dvrs.fValues()) {
  	gaxis->vr = ValueRange(rrMinMax, rGridStep);
  	gaxis->dvrs.SetValueRange(gaxis->vr);
	}
	RoseDiagramAxis* rda = dynamic_cast<RoseDiagramAxis*>(gaxis);
	if (0 != rda && rda->gap == GraphAxis::gapXRose) {
		int iMaxDeg = 180;
		switch (rda->deg) {
		  case RoseDiagramAxis::deg180: iMaxDeg = 180; break;
			case RoseDiagramAxis::deg360: iMaxDeg = 360; break;
		}
		gaxis->dvrsData = DomainValueRangeStruct(ValueRange(0,iMaxDeg,gaxis->dvrsData.rStep()));
	}
	return 0;
}

///// RoseDiagramAxisPage

RoseDiagramAxisPage::RoseDiagramAxisPage(RoseDiagramAxis* ga, const String& sTitle)
: GraphAxisPage(ga, sTitle), rdaxis(ga)
{
}

void RoseDiagramAxisPage::Init()
{
  FieldString* fs = new FieldString(root, SGPUiAxisText, &rdaxis->sTitle);
  fs->SetWidth(90);
  CheckBox* cb = new CheckBox(root, SGPUiShowAxis, &rdaxis->fVisible);
  new CheckBox(cb, SGPUiShowGrid, &rdaxis->fShowGrid);
	rrMinMax = RangeReal(gaxis->rMin(), rdaxis->rMax());
	rGridStep = rdaxis->rGridStep();
  FieldRangeReal* frr = new FieldRangeReal(root, SGPUiMinMax, &rrMinMax, ValueRange(-1e300, 1e300, gaxis->dvrs.rStep()));
	frr->Align(cb, AL_UNDER);
	new FieldReal(root, SGPUiGridStep, &rGridStep, ValueRange(0, 1e308, rdaxis->dvrs.rStep()));
	create();
}

int RoseDiagramAxisPage::exec()
{ 
	GraphAxisPage::exec();
	return 0;
}

// GraphOptionsForm 

GraphOptionsForm::GraphOptionsForm(GraphDrawer* grdrw)
	: CPropertySheet(SGPTitleGraphOptions.scVal())
	, gd(grdrw), glmp(0), iGraphPropPages(0)
{
}

GraphOptionsForm::~GraphOptionsForm()
{
	while (!spp.empty()) {
		delete spp.top();
		spp.pop();
	}
}

void GraphOptionsForm::AddPage(CPropertyPage *pPage)
{
	CPropertySheet::AddPage(pPage);
	spp.push(pPage);
}

void GraphOptionsForm::Refill(int iActivePage)
{
  SetActivePage(0);
	int iPages = GetPageCount();
	for (int i=iGraphPropPages; i < iPages; i++)
		RemovePage(iGraphPropPages);
	while (!spp.empty()) {
		if (glmp == spp.top())
			break;
		spp.pop();
	}
	for (int i=0; i < gd->agl.iSize(); i++)
    AddPage(gd->agl[i]->frmOptions());
	if (0 != glmp)
	  glmp->Fill();
	SetActivePage(iActivePage);
}


// CartesianGraphOptionsForm

CartesianGraphOptionsForm::CartesianGraphOptionsForm(CartesianGraphDrawer* cgd, bool fAxisInfoOnly)
: GraphOptionsForm(cgd)
{
	AddPage(new GraphAxisPage(cgd->gaxX, SGPTitleXAxis));
	AddPage(new GraphAxisPage(cgd->gaxYLeft, SGPTitleYAxisLeft));
	AddPage(new GraphAxisPage(cgd->gaxYRight, SGPTitleYAxisRight));
	iGraphPropPages = 3;
	if (fAxisInfoOnly)
		return;
 	glmp = new GraphLayerManPage(gd);
  AddPage(glmp);
	iGraphPropPages = 4;
	for (int i=0; i < gd->agl.iSize(); i++) 
    AddPage(gd->agl[i]->frmOptions());
}

CartesianGraphOptionsForm::~CartesianGraphOptionsForm()
{
}

// RoseDiagramOptionsForm

RoseDiagramOptionsForm::RoseDiagramOptionsForm(RoseDiagramDrawer* rdd, bool fAxisInfoOnly)
: GraphOptionsForm(rdd)
{
	AddPage(new RoseDiagramAxisPage(rdd->rdaxX, SGPTitleXAxis));
	AddPage(new RoseDiagramAxisPage(rdd->rdaxY, SGPTitleYAxisRoseDiagram));
	iGraphPropPages = 2;
	if (fAxisInfoOnly)
		return;
 	glmp = new GraphLayerManPage(gd);
  AddPage(glmp);
	iGraphPropPages = 3;
	for (int i=0; i < gd->agl.iSize(); i++) 
    AddPage(gd->agl[i]->frmOptions());
}

RoseDiagramOptionsForm::~RoseDiagramOptionsForm()
{
}
