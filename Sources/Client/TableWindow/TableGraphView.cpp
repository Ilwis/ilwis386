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
// TableGraphView.cpp: implementation of the TableGraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Client\TableWindow\TableGraphView.h"
#include "Engine\Table\tblview.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fldcol.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Graph.hs"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Headers\constant.h"
#include "Client\Editors\Utils\GeneralBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(TableGraphView, GraphView)
  ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_GRPH_ADD_COLUMN, OnGraphAddColumn)
	ON_COMMAND(ID_GRPH_ADD_FORMULA, OnGraphAddFormula)
	ON_COMMAND(ID_GRPH_ADD_LSF, OnGraphAddLsf)
	ON_COMMAND(ID_GRPH_ADD_SVM, OnGraphAddSemivariogram)
END_MESSAGE_MAP()

		class ConfigInitForm: public FormWithDest
		{
		public:
			ConfigInitForm(CWnd* wParent, const Table& tbl, bool fXAxis)
				: FormWithDest(wParent, SGPTitleGraph)
			{
				fColX = true;
				CheckBox* cb = 0;
				tvw = new TableView(tbl);
				if (fXAxis) {
					cb = new CheckBox(root, STBUiColX, &fColX);
					new FieldColumn(cb, "", tvw, &sColX, dmVALUE | dmIMAGE | dmCLASS | dmIDENT);
				}
				FieldColumn* fc = new FieldColumn(root, STBUiColY, tvw, &sColY, dmVALUE | dmIMAGE | dmBOOL);
				if (0 != cb)
  				fc->Align(cb, AL_UNDER);
				create();
			}
			~ConfigInitForm() {
				delete tvw;
			}
			bool fColX;
			String sColX, sColY;	
			TableView* tvw;
		};

class ColumnGraphDrawer : public CartesianGraphDrawer
{
public:
	ColumnGraphDrawer(TableGraphView*);
  virtual ~ColumnGraphDrawer();
  virtual bool fAddGraph(bool fConfigLayer);
	virtual bool fAddFormulaGraph(bool fConfigLayer);
	virtual bool fAddLsfGraph(bool fConfigLayer);
	virtual bool fAddSmvGraph(bool fConfigLayer);
	TableGraphView* tgv;
};

ColumnGraphDrawer::ColumnGraphDrawer(TableGraphView* tgvw)
: CartesianGraphDrawer(tgvw), tgv(tgvw)
{
}

ColumnGraphDrawer::~ColumnGraphDrawer()
{
}

bool ColumnGraphDrawer::fAddGraph(bool fConfigLayer)
{
	bool fFirstGraph = tgv->cgd->agl.iSize()==0;
	ConfigInitForm frm(tgv, tgv->GetDocument()->table(), fFirstGraph);
	bool fOk = frm.fOkClicked();
	if (fOk)
		fOk = tgv->NewGraphLayer(frm.fColX, frm.sColX, frm.sColY, fConfigLayer, !fFirstGraph);
	return fOk;
}

		class AddGraphFormulaForm: public FormWithDest
		{
		public:
			AddGraphFormulaForm(CWnd *parent, const String& sTitle)
			: FormWithDest(parent, sTitle)
			{
				new StaticText(root, SGPUiExpression);
				sExpr = "x";
				FieldString* fs = new FieldString(root, "y =", &sExpr, Domain(), false);
				fs->SetWidth(120);
				fs->SetIndependentPos();
				SetMenHelpTopic("ilwismen\\graph_window_add_graph_from_formula.htm");
				create();
			}
			String sExpr;
		};

bool ColumnGraphDrawer::fAddFormulaGraph(bool fConfigLayer)
{
	AddGraphFormulaForm frm(tgv, SGPTitleAddGraphFormula);
	bool fOk = frm.fOkClicked();
	if (fOk) 
		fOk = tgv->AddFormulaGraph(frm.sExpr, fConfigLayer);
	return fOk;
}

		class AddLsfForm: public FormWithDest
		{
		public:
			AddLsfForm(CWnd *parent, const String& sTitle, const Table& tbl)
			: FormWithDest(parent, sTitle)
			{
	      tvw = new TableView(tbl);
  			if (tvw->iCols() > 0)
					sColX = tvw->cv(0)->sName();
				if (tvw->iCols() > 1)
					sColY = tvw->cv(1)->sName();
				new FieldColumn(root, STBUiColX, tvw, &sColX, dmVALUE|dmIMAGE);
				new FieldColumn(root, STBUiColY, tvw, &sColY, dmVALUE|dmIMAGE);
				sFunc = 0;
				m_sDefault = String();
				frf = new FieldRegressionFunc(root, STBUiFunction, &sFunc, m_sDefault);
				frf->SetCallBack((NotifyProc)&AddLsfForm::FuncCallBack);
				String sFill('x', 50);
				stRegr = new StaticText(root, sFill);
				stRegr->SetIndependentPos();
				fiTerms = new FieldInt(root, STBUiNrTerms, &iTerms, RangeInt(2,100));
				SetMenHelpTopic("ilwismen\\graph_window_add_graph_least_squares_fit.htm");
				create();
			}
			String sColX, sColY, *sFunc, m_sDefault;
			int iTerms;
		private:  
			int FuncCallBack(Event*) {
				frf->StoreData();
				if (0 == sFunc || -1 == (long)sFunc)
					return 0;
				if (*sFunc == "polynomial") {
					fiTerms->SetVal(2);
					fiTerms->Show();
					stRegr->SetVal("y = a + b x + c x^2 + d x^3 + ...");
				}
				else if (*sFunc == "trigonometric") {
					fiTerms->SetVal(3);
					fiTerms->Show();
					stRegr->SetVal("y = a + b cos x + c sin x + ...");
				}
				else {
					iTerms = 2;
					fiTerms->Hide();
					if (*sFunc == "power")
						stRegr->SetVal("y = a x^b");
					else if (*sFunc == "exponential")
						stRegr->SetVal("y = a e^bx");
					else if (*sFunc == "logarithmic")
						stRegr->SetVal("y = a log bx");
				}
				return 0;
			}
			FieldRegressionFunc* frf;
			StaticText* stRegr;
			FieldInt* fiTerms;
			TableView* tvw;
		};

bool ColumnGraphDrawer::fAddLsfGraph(bool fConfigLayer)
{
	AddLsfForm frm(tgv, SGPTitleAddGraphLsf, tgv->GetDocument()->table());
	bool fOk = frm.fOkClicked();
	if (fOk)
		fOk = tgv->AddLsfGraph(frm.sColX, frm.sColY, *frm.sFunc, frm.iTerms, fConfigLayer);
	return fOk;
}

		class AddSmvForm: public FormWithDest
		{
		public:
			AddSmvForm(CWnd *parent, const String& sTitle)
			: FormWithDest(parent, sTitle)
			{
				new FieldSemiVariogram(root, SGPUiSemiVar, &smv);
				SetMenHelpTopic("ilwismen\\graph_window_add_semivariogram_model.htm");
				create();
			}
			SemiVariogram smv;
		};

bool ColumnGraphDrawer::fAddSmvGraph(bool fConfigLayer)
{
	AddSmvForm frm(tgv, SGPTitleAddGraphSemiVar);
	bool fOk = frm.fOkClicked();
	if (fOk)
		fOk = tgv->AddSmvGraph(frm.smv, fConfigLayer);
	return fOk;
}

TableGraphView::TableGraphView()
{
  cgd = new ColumnGraphDrawer(this);
	grdrw = cgd;
}

TableGraphView::~TableGraphView()
{
  delete cgd;
}

TableDoc* TableGraphView::GetDocument()
{
  return (TableDoc*)m_pDocument;
}

const TableDoc* TableGraphView::GetDocument() const
{
  return (const TableDoc*)m_pDocument;
}

void TableGraphView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	GraphView::OnUpdate(pSender, lHint, pHint);

	TableDoc* td = GetDocument();
	TableView* tvw = td->tvw;
/*
	m_ccChart.ResetAll(FALSE);

	switch (eType)
	{
		case eNONE:
			m_ccChart.SetIndexMode(CF_INDEX_NAT_N);
			break;
		case eVALUE:		
			m_ccChart.SetIndexMode(CF_INDEX_USR_X);
			break;
	}
	for (int iCol = 0; iCol < vcolY.size(); ++iCol) 
	{
		const Column& colY = vcolY[iCol];
		int iRecs = td->iRecs();
		for (int iRow = 0; iRow < iRecs; ++iRow)
		{
			int iRec = tvw->iRec(iRow);
			double rX;
			double rVal = colY->rValue(iRec);
			if (rUNDEF == rVal)
				continue;
			switch (eType)
			{
				case eNONE:
					m_ccChart.AddData(iCol, rVal);
					break;
				case eVALUE:		
					rX = colX->rValue(iRec);
					if (rUNDEF == rX)
						continue;
					m_ccChart.AddData(iCol, rX, rVal);
					break;
			}
		}
	}
	m_ccChart.SetChartType(chartType);
	m_ccChart.SetDispMode(dispMode);
*/
}


bool TableGraphView::fConfigInit(CWnd* wPar)
{
	ConfigInitForm frm(wPar, GetDocument()->table(), true);
	bool fOk = frm.fOkClicked();
	if (fOk)
		fOk = NewGraphLayer(frm.fColX, frm.sColX, frm.sColY, true, false);
	return fOk;
}

bool TableGraphView::NewGraphLayer(bool fColX, const String& sColX, const String& sColY, bool fConfigLayer, bool fXAxisDefined)
{
	Table tbl = GetDocument()->table();
  Column colX;
	if (!fXAxisDefined) {
		if (fColX) {
			colX = tbl->col(sColX);
			if (!colX.fValid())
				return false;
			cgd->gaxX->Set(colX->dvrs());
			cgd->gaxX->dvrsData = colX->dvrs();
			if (colX->fValues()) 
			{
				RangeReal rr = colX->rrMinMax();
				double rWidth = rr.rWidth();
				rr.rHi() += rWidth * 0.03;
				if (rr.rLo() > 0 && rr.rLo() < rWidth * 0.3)
					rr.rLo() = 0;
				else 
					rr.rLo() -= rWidth * 0.05;
				cgd->gaxX->SetMinMax(rr);
			}
			cgd->gaxX->sTitle = colX->sName();
			if (0 != colX->sDescr().length())
				cgd->gaxX->sTitle += String(" : %S", colX->sGetDescription());
		}
		else {
			cgd->gaxX->Set(tbl->dm());
			if (tbl->dm()->pdnone())
				cgd->gaxX->SetMinMax(RangeReal(1, tbl->iRecs()));
		}
	}
	else {
		// get X column from one of the other column graphs
		for (unsigned int i=0; i < cgd->agl.iSize(); i++) {
			ColumnGraphLayer* cgl = dynamic_cast<ColumnGraphLayer*>(cgd->agl[i]);
			if (0 != cgl) {
				colX = cgl->colX;
				break;
			}
		}
	}
	Column colY = tbl->col(sColY);
	if (!colY.fValid())
		return false;

	ColumnGraphLayer* gl = new ColumnGraphLayer(cgd, tbl, colX, colY);
	// check if current graphs use left Y axis
	bool fUseLeftYAxis = false, fUseRightYAxis = false;
	for (unsigned int i=0; i < cgd->agl.iSize(); i++) {
		if (cgd->cgl(i)->fYAxisLeft)
		  fUseLeftYAxis = true;
		else 
		  fUseRightYAxis = true;
	}

	if (!fUseRightYAxis) {
		cgd->gaxYRight->Set(colY->dvrs());
		cgd->gaxYRight->dvrsData = colY->dvrs();
		if (colY->fValues()) {
			RangeReal rr = colY->rrMinMax();
			double rWidth = rr.rWidth();
			rr.rHi() += rWidth * 0.03;
			if (rr.rLo() > 0 && rr.rLo() < rWidth * 0.3)
				rr.rLo() = 0;
			else 
				rr.rLo() -= rWidth * 0.05;
			cgd->gaxYRight->SetMinMax(rr);
		}
		cgd->gaxYRight->sTitle = colY->sName();
		if (0 != colY->sDescr().length())
			cgd->gaxYRight->sTitle += String(" : %S", colY->sGetDescription());
		gl->fYAxisLeft = false;
	}
	if (!fUseLeftYAxis) { 
		cgd->gaxYLeft->Set(colY->dvrs());
		cgd->gaxYLeft->dvrsData = colY->dvrs();
		if (colY->fValues()) {
			RangeReal rr = colY->rrMinMax();
			double rWidth = rr.rWidth();
			rr.rHi() += rWidth * 0.03;
			if (rr.rLo() > 0 && rr.rLo() < rWidth * 0.3)
				rr.rLo() = 0;
			else 
				rr.rLo() -= rWidth * 0.1;
			cgd->gaxYLeft->SetMinMax(rr);
		}
		cgd->gaxYLeft->sTitle = colY->sName();
		if (0 != colY->sDescr().length())
			cgd->gaxYLeft->sTitle += String(" : %S", colY->sGetDescription());
		gl->fYAxisLeft = true;
  }

	bool fOk = true;
	cgd->agl.push_back(gl);
	throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->color	= BaseDrawer::clrPrimary(cgd->agl.iSize());
	if (fConfigLayer)
  	fOk = gl->fConfig();
	return fOk;
}


bool TableGraphView::AddFormulaGraph(const String& sExpr, bool fConfigLayer)
{
	FormulaGraphLayer* gl = new FormulaGraphLayer(cgd, sExpr);
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->color	= BaseDrawer::clrPrimary(cgd->agl.iSize());
	if (fConfigLayer)
		return gl->fConfig();
	else 
		return true;
}


bool TableGraphView::AddLsfGraph(const String& sColX, const String& sColY, const String& sFunc, int iTerms, bool fConfigLayer)
{
	Table tbl = GetDocument()->table();
  Column colX = tbl->col(sColX);
	if (!colX.fValid())
  	return false;
	Column colY = tbl->col(sColY);
	if (!colY.fValid())
		return false;

	LsfGraphLayer* gl = new LsfGraphLayer(cgd, tbl, colX, colY, sFunc, iTerms); 
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->color	= BaseDrawer::clrPrimary(cgd->agl.iSize());
	if (fConfigLayer)
		return gl->fConfig();
	else 
		return true;
}


bool TableGraphView::AddSmvGraph(const SemiVariogram& smv, bool fConfigLayer)
{
	SmvGraphLayer* gl = new SmvGraphLayer(cgd, smv); 
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->color	= BaseDrawer::clrPrimary(cgd->agl.iSize());
	if (fConfigLayer)
		return gl->fConfig();
	else 
		return true;
}


void TableGraphView::SaveSettings(const FileName& fn, int iGraphWindow)
{
	String sSection("GraphWindow%i", iGraphWindow);
	cgd->SaveSettings(fn, sSection);
}

void TableGraphView::LoadSettings(const FileName& fn, int iGraphWindow)
{
	String sSection("GraphWindow%i", iGraphWindow);
	cgd->LoadSettings(fn, sSection);
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addSubMenu(ID) men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID)); menSub.Detach();

void TableGraphView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_GRPH_OPTIONS);
    menSub.CreateMenu();
		addSub(ID_GRPH_ADD_COLUMN);
		addSub(ID_GRPH_ADD_FORMULA);
		addSub(ID_GRPH_ADD_LSF);
		addSub(ID_GRPH_ADD_SVM);
  addSubMenu(ID_GRPH_ADD);

	add(ID_COPY);
	add(ID_FILE_PRINT);
  men.AppendMenu(MF_SEPARATOR);
	add(ID_GRPH_FITINWINDOW);
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
	GeneralBar* gb = dynamic_cast<GeneralBar*>(GetParent());
	if (gb) {
	  men.AppendMenu(MF_SEPARATOR);
		add(ID_ALLOWDOCKING);
		men.CheckMenuItem(ID_ALLOWDOCKING, gb->fAllowDocking() ? MF_CHECKED : MF_UNCHECKED);
	}
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void TableGraphView::OnGraphAddColumn()
{
  if (0 == grdrw)
    return;
  grdrw->fAddGraph(true);
}

void TableGraphView::OnGraphAddFormula()
{
  if (0 == grdrw)
    return;
  grdrw->fAddFormulaGraph(true);
}

void TableGraphView::OnGraphAddLsf()
{
  if (0 == grdrw)
    return;
  grdrw->fAddLsfGraph(true);
}

void TableGraphView::OnGraphAddSemivariogram()
{
  if (0 == grdrw)
    return;
  grdrw->fAddSmvGraph(true);
}

