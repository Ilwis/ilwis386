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
// CartesianGraphDoc.cpp: implementation of the CartesianGraphDoc class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\CartesianGraphDoc.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Engine\Table\GraphObject.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Client\FormElements\flddat.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Hs\Table.hs"
#include "Headers\constant.h"
#include "Headers\Htp\Ilwismen.htp"

IMPLEMENT_DYNCREATE(CartesianGraphDoc, GraphDoc)

BEGIN_MESSAGE_MAP(CartesianGraphDoc, GraphDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND(ID_GRPH_ADD, OnAddColumnGraph)
  ON_COMMAND(ID_GRPH_ADD_COLUMN, OnAddColumnGraph)
	ON_COMMAND(ID_GRPH_ADD_FORMULA, OnAddFormulaGraph)
	ON_UPDATE_COMMAND_UI(ID_GRPH_ADD_FORMULA, OnUpdateAddFormulaGraph)
	ON_COMMAND(ID_GRPH_ADD_LSF, OnAddLsfGraph)
	ON_UPDATE_COMMAND_UI(ID_GRPH_ADD_LSF, OnUpdateAddFormulaGraph)
	ON_COMMAND(ID_GRPH_ADD_SVM, OnAddSvmGraph)
	ON_UPDATE_COMMAND_UI(ID_GRPH_ADD_SVM, OnUpdateAddFormulaGraph)
END_MESSAGE_MAP()

CartesianGraphDoc::CartesianGraphDoc()
: cgd(0)
{
}

CartesianGraphDoc::~CartesianGraphDoc()
{
}

void CartesianGraphDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		GraphObject go;
		ElementContainer& en = const_cast<ElementContainer&>(go->fnObj);
		if (0 == en.em)
			en.em = new ElementMap;
		go->Updated();
		go->Store();
    if (0 != cgd)
    {
  		ObjectInfo::WriteElement("Graph", "Type", en, "StandardGraph");
      cgd->SaveSettings(en, "Graph");
    }
    ObjectDependency objdep;
    for (int i = 0; i < iLayers(); ++i)
    {
      ColumnGraphLayer* cgl = dynamic_cast<ColumnGraphLayer*>(gl(i));
      if (0 != cgl) 
        objdep.Add(cgl->tbl);
    }      
    objdep.Store(go.pointer());
		en.em->Serialize(ar);
	}
	else
	{
		ElementContainer en;
		en.em = new ElementMap;
		en.em->Serialize(ar);
    String sType;
		ObjectInfo::ReadElement("Graph", "Type", en, sType);
    if ("StandardGraph" == sType)
    {
      grdr = cgd = new CartesianGraphDrawer(0); 
      cgd->LoadSettings(en, "Graph");
      for (int i = 0; i < iLayers(); ++i)
      {
        ColumnGraphLayer* cgl = dynamic_cast<ColumnGraphLayer*>(gl(i));
        if (0 != cgl) 
        {
          tbl = cgl->tbl;
          colX = cgl->colX;
          break;
        }
      }
    }
	}
}

namespace 
{
  class NewGraphForm: public FormWithDest
  {
	public:
		NewGraphForm(CWnd* wParent, const Table& table)
		: FormWithDest(wParent, SGPTitleGraph)
    , tbl(table)
		{
      if (tbl.fValid())
        sTbl = tbl->fnObj.sRelative();
      ftbl = new FieldTable(root, SGPUiTable, &sTbl);
      ftbl->SetCallBack((NotifyProc)&NewGraphForm::TableCallBack);
			fColX = true;
			CheckBox* cb = 0;
			cb = new CheckBox(root, SGPUiXAxis, &fColX);
			fcolX = new FieldColumn(cb, "", tbl, &sColX, dmVALUE | dmIMAGE | dmCLASS | dmIDENT);
			fcolY = new FieldColumn(root, SGPUiYAxis, tbl, &sColY, dmVALUE | dmIMAGE | dmBOOL);
 			fcolY->Align(cb, AL_UNDER);
      SetMenHelpTopic(htpCreateGraph);
			create();
		}
		bool fColX;
		String sTbl, sColX, sColY;	
    Table tbl;
  private:
    FieldTable* ftbl;
    FieldColumn *fcolX, *fcolY;
    int TableCallBack(Event*)
    {
      try {
        ftbl->StoreData();
        tbl = Table(sTbl);
        fcolX->FillWithColumns(tbl.ptr());
        fcolY->FillWithColumns(tbl.ptr());
      }
      catch (ErrorObject&)
      {
      }
      return 0;
    }
  };
}

void CartesianGraphDoc::OnFileOpen()
{
	if (!SaveModified())
		return;
	class OpenForm: public FormWithDest
  {
  public:
    OpenForm(CWnd* parent, String* sName)
    : FormWithDest(parent, SGPTitleOpenGraph)
    {
			new FieldDataTypeLarge(root, sName, ".grh", new GraphLister(grhGRAPH));
      SetMenHelpTopic(htpOpenGraph);
      create();
    }
  };
  String sGraph;
  OpenForm frm(wndGetActiveView(), &sGraph);
	if (frm.fOkClicked()) {
		OnOpenDocument(sGraph.scVal());
		SetModifiedFlag(FALSE);
  	UpdateAllViews(0);
	}
}

BOOL CartesianGraphDoc::OnNewDocument()
{
  NewGraphForm frm(wndGetActiveView(), tbl);
  if (!frm.fOkClicked())
    return FALSE;
	DeleteContents();
	if (!IlwisDocument::OnNewDocument())
		return FALSE;

  tbl = Table(frm.tbl);
  colX = Column();
  Column colY;
  if (frm.fColX)
  {
    colX = tbl->col(frm.sColX);
		if (!colX.fValid())
			return FALSE;
  }    
	colY = tbl->col(frm.sColY);
	if (!colY.fValid())
		return false;

  grdr = cgd = new CartesianGraphDrawer(0); 
	ColumnGraphLayer* gl = new ColumnGraphLayer(cgd, tbl, colX, colY);
  // X-axis
  if (colX.fValid())
  {
		cgd->gaxX->Set(colX->dvrs());
		cgd->gaxX->dvrsData = colX->dvrs();
		if (colX->fValues()) 
		{
			RangeReal rr = colX->rrMinMax();
			double rWidth = rr.rWidth();
			rr.rHi() += rWidth * 0.03;
			if (rr.rLo() >= 0 && rr.rLo() < rWidth * 0.3)
				rr.rLo() = 0;
			else 
				rr.rLo() -= rWidth * 0.05;
			cgd->gaxX->SetMinMax(rr);
		}
		cgd->gaxX->sTitle = colX->sName();
  }
	else {
		cgd->gaxX->Set(tbl->dm());
		if (tbl->dm()->pdnone())
			cgd->gaxX->SetMinMax(RangeReal(1, tbl->iRecs()));
	}
  // Y-axis
	cgd->gaxYLeft->Set(colY->dvrs());
	cgd->gaxYLeft->dvrsData = colY->dvrs();
	cgd->gaxYRight->Set(colY->dvrs());
	cgd->gaxYRight->dvrsData = colY->dvrs();
	if (colY->fValues()) {
		RangeReal rr = colY->rrMinMax();
		double rWidth = rr.rWidth();
		rr.rHi() += rWidth * 0.03;
		if (rr.rLo() >= 0 && rr.rLo() < rWidth * 0.3)
			rr.rLo() = 0;
		else 
			rr.rLo() -= rWidth * 0.1;
		cgd->gaxYLeft->SetMinMax(rr);
		cgd->gaxYRight->SetMinMax(rr);
	}
	cgd->gaxYLeft->sTitle = colY->sName();
  grdr->sTitle = gl->sTitle;
	gl->fYAxisLeft = true;
  
	cgd->agl.push_back(gl);
	//throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	gl->smb.col = gl->line.clrLine() = gl->color = Representation::clrPrimary(cgd->agl.iSize());
  gl->smb.iSize = 5;

  UpdateAllViews(0);
	SetModifiedFlag();
  return TRUE;
}

void CartesianGraphDoc::DeleteContents()
{
  cgd = 0;
  GraphDoc::DeleteContents();
}

void CartesianGraphDoc::OnAddColumnGraph()
{
  class AddGraphForm: public FormWithDest
  {
	public:
		AddGraphForm(CWnd* wParent, const Domain& dom, String* sTable, String* sColX, String* sColY)
		: FormWithDest(wParent, SGPTitleAddGraph)
    , dm(dom)
    , sTbl(sTable)
		{
      ftbl = new FieldTable(root, SGPUiTable, sTbl, ".tbt.his.hsa.hss.hsp.rpr");
      ftbl->SetCallBack((NotifyProc)&AddGraphForm::TableCallBack);
			fcolX = new FieldColumn(root, SGPUiXAxis, 0, sColX);
			fcolY = new FieldColumn(root, SGPUiYAxis, 0, sColY, dmVALUE | dmIMAGE | dmBOOL);
      SetMenHelpTopic(htpAddGraph);
			create();
		}
  private:
    Domain dm;
    String* sTbl;
    FieldTable* ftbl;
    FieldColumn *fcolX, *fcolY;
    int TableCallBack(Event*)
    {
      try {
        ftbl->StoreData();
        FileName fn(*sTbl);
        fcolX->FillWithColumns(fn,dm);
        fcolY->FillWithColumns(fn);
      }
      catch (ErrorObject&)
      {
      }
      return 0;
    }
  };
  class AddGraphFormColY: public FormWithDest
  {
	public:
		AddGraphFormColY(CWnd* wParent, const Table& tbl, String* sColY)
		: FormWithDest(wParent, SGPTitleAddGraph)
		{
			new FieldColumn(root, SGPUiYAxis, tbl, sColY, dmVALUE | dmIMAGE | dmBOOL);
      SetMenHelpTopic(htpAddGraph);
			create();
		}
  };    
  Column colY;

  if (colX.fValid())
  {
    Domain dmCol = colX->dm();
    String sTable = tbl->sName(true);
    String sColX = colX->sName();
    String sColY;
    AddGraphForm frm(wndGetActiveView(), dmCol, &sTable, &sColX, &sColY);
    if (!frm.fOkClicked())
      return;
    tbl = Table(sTable);
    colX = tbl->col(sColX);
    colY = tbl->col(sColY);
  }
  else 
  {
    String sColY;
    AddGraphFormColY frm(wndGetActiveView(), tbl, &sColY);
    if (!frm.fOkClicked())
      return;
    colY = tbl->col(sColY);
  }
  if (!colY.fValid())
    return;
	ColumnGraphLayer* gl = new ColumnGraphLayer(cgd, tbl, colX, colY);
  // updating of axis ??
	gl->fYAxisLeft = true;
	cgd->agl.push_back(gl);
	throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->smb.col = gl->line.clrLine() = gl->color	= BaseDrawer::clrPrimary(cgd->agl.iSize());
  gl->smb.iSize = 5;
  // config layer ??
	SetModifiedFlag();
  UpdateAllViews(0);
}

void CartesianGraphDoc::OnAddFormulaGraph()
{
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
			SetMenHelpTopic(htpAddLineGraph);
			create();
		}
		String sExpr;
	};
	AddGraphFormulaForm frm(wndGetActiveView(), SGPTitleAddGraphFormula);
	if (!frm.fOkClicked())
    return;
	FormulaGraphLayer* gl = new FormulaGraphLayer(cgd, frm.sExpr);
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->line.clrLine() = gl->color = BaseDrawer::clrPrimary(cgd->agl.iSize());
	SetModifiedFlag();
  UpdateAllViews(0);
}

void CartesianGraphDoc::OnAddLsfGraph()
{
	class AddLsfForm: public FormWithDest
	{
	public:
		AddLsfForm(CWnd *parent, const String& sTitle, const Table& tbl)
		: FormWithDest(parent, sTitle)
		{
			if (tbl->iCols() > 0)
				sColX = tbl->col(0)->sName();
			if (tbl->iCols() > 1)
				sColY = tbl->col(1)->sName();
			new FieldColumn(root, STBUiColX, tbl, &sColX, dmVALUE|dmIMAGE);
			new FieldColumn(root, STBUiColY, tbl, &sColY, dmVALUE|dmIMAGE);
			sFunc = 0;
			m_sDefault = String();
			frf = new FieldRegressionFunc(root, STBUiFunction, &sFunc, m_sDefault);
			frf->SetCallBack((NotifyProc)&AddLsfForm::FuncCallBack);
			String sFill('x', 50);
			stRegr = new StaticText(root, sFill);
			stRegr->SetIndependentPos();
			fiTerms = new FieldInt(root, STBUiNrTerms, &iTerms, RangeInt(2,100));
			SetMenHelpTopic(htpAddLsfGraph);
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
	};
	AddLsfForm frm(wndGetActiveView(), SGPTitleAddGraphLsf, tbl);
	if (!frm.fOkClicked())
    return;

  Column colX = tbl->col(frm.sColX);
	if (!colX.fValid())
  	return;
	Column colY = tbl->col(frm.sColY);
	if (!colY.fValid())
		return;

	LsfGraphLayer* gl = new LsfGraphLayer(cgd, tbl, colX, colY, *frm.sFunc, frm.iTerms); 
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->line.clrLine() = gl->color = BaseDrawer::clrPrimary(cgd->agl.iSize());
	SetModifiedFlag();
  UpdateAllViews(0);
}

void CartesianGraphDoc::OnAddSvmGraph()
{
	class AddSmvForm: public FormWithDest
	{
	public:
		AddSmvForm(CWnd *parent, const String& sTitle)
		: FormWithDest(parent, sTitle)
		{
			new FieldSemiVariogram(root, SGPUiSemiVar, &smv);
			SetMenHelpTopic(htpAddSemiVarGraph);
			create();
		}
		SemiVariogram smv;
	};
	AddSmvForm frm(wndGetActiveView(), SGPTitleAddGraphSemiVar);
  if (!frm.fOkClicked())
    return;

	SmvGraphLayer* gl = new SmvGraphLayer(cgd, frm.smv); 
  cgd->agl.push_back(gl);
  throw ErrorObject(String("TO DO %s %s", __LINE__, __FILE__));
	//gl->line.clrLine() = gl->color = BaseDrawer::clrPrimary(cgd->agl.iSize());
	SetModifiedFlag();
  UpdateAllViews(0);
}

GraphAxis* CartesianGraphDoc::ga(GraphAxis::GraphAxisPos gp) const
{
  if (0 != cgd) 
  {
    switch (gp)
    {
      case GraphAxis::gapX:
        return cgd->gaxX;
      case GraphAxis::gapYLeft:
        return cgd->gaxYLeft;
      case GraphAxis::gapYRight:
        return cgd->gaxYRight;
      default:
        return 0;
    }
  }  
  return 0;
}

bool CartesianGraphDoc::fEnableAddFormulaGraph() const
{
  bool fEnable = true;
  if (0 != cgd)
    fEnable = cgd->gaxX->dvrs.fValues();
  return fEnable;
}

void CartesianGraphDoc::OnUpdateAddFormulaGraph(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(fEnableAddFormulaGraph());
}

