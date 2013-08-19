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
// RoseDiagramDoc.cpp: implementation of the RoseDiagramDoc class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\RoseDiagramDoc.h"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"
#include "Engine\Table\GraphObject.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\ilwis.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\flddat.h"
#include "Headers\Hs\Graph.hs"
#include "Headers\Hs\Table.hs"
#include "Headers\constant.h"
#include "Headers\Htp\Ilwismen.htp"

IMPLEMENT_DYNCREATE(RoseDiagramDoc, GraphDoc)

BEGIN_MESSAGE_MAP(RoseDiagramDoc, GraphDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND(ID_GRPH_ADD, OnAddGraph)
  ON_COMMAND(ID_GRPH_ADD_COLUMN, OnAddGraph)
END_MESSAGE_MAP()


RoseDiagramDoc::RoseDiagramDoc()
: rdd(0)
{
}

RoseDiagramDoc::~RoseDiagramDoc()
{
}

void RoseDiagramDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		GraphObject go;
		ElementContainer& en = const_cast<ElementContainer&>(go->fnObj);
		if (0 == en.em)
			en.em = new ElementMap;
		go->Updated();
		go->Store();
    if (0 != rdd)
    {
  		ObjectInfo::WriteElement("Graph", "Type", en, "RoseDiagram");
      rdd->SaveSettings(en, "Graph");
    }
    ObjectDependency objdep;
    for (int i = 0; i < iLayers(); ++i)
    {
      RoseDiagramLayer* rdl = dynamic_cast<RoseDiagramLayer*>(gl(i));
      if (0 != rdl) 
        objdep.Add(rdl->tbl);
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
    if ("RoseDiagram" == sType)
    {
      grdr = rdd = new RoseDiagramDrawer(0); 
      rdd->LoadSettings(en, "Graph");
      for (int i = 0; i < iLayers(); ++i)
      {
        RoseDiagramLayer* rdl = dynamic_cast<RoseDiagramLayer*>(gl(i));
        if (0 != rdl) 
        {
          tbl = rdl->tbl;
          colX = rdl->colX;
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
		: FormWithDest(wParent, TR("Create Rose Diagram"))
    , tbl(table)
		{
      if (tbl.fValid())
        sTbl = tbl->fnObj.sRelative();
      ftbl = new FieldTable(root, TR("&Table"), &sTbl);
      ftbl->SetCallBack((NotifyProc)&NewGraphForm::TableCallBack);
			fcolX = new FieldColumn(root, TR("&Angle"), tbl, &sColX, dmVALUE);
			fcolY = new FieldColumn(root, TR("&Value"), tbl, &sColY, dmVALUE | dmIMAGE | dmBOOL);
      SetMenHelpTopic("ilwismen\\create_a_rose_diagram.htm");
			create();
		}
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

void RoseDiagramDoc::OnFileOpen()
{
	if (!SaveModified())
		return;
	class OpenForm: public FormWithDest
  {
  public:
    OpenForm(CWnd* parent, String* sName)
    : FormWithDest(parent, TR("Open Graph"))
    {
			new FieldDataTypeLarge(root, sName, ".grh", new GraphLister(grhROSEDIAGRAM));
			SetMenHelpTopic("ilwismen\\graph_window_open_graph.htm");
      create();
    }
  };
  String sGraph;
  OpenForm frm(wndGetActiveView(), &sGraph);
	if (frm.fOkClicked()) {
		OnOpenDocument(sGraph.c_str(),0);
		SetModifiedFlag(FALSE);
  	UpdateAllViews(0);
	}
}

BOOL RoseDiagramDoc::OnNewDocument()
{
  NewGraphForm frm(wndGetActiveView(), tbl);
  if (!frm.fOkClicked())
    return FALSE;
	DeleteContents();
	if (!IlwisDocument::OnNewDocument())
		return FALSE;

  tbl = Table(frm.tbl);
  Column colY;
  colX = tbl->col(frm.sColX);
  if (!colX.fValid())
		return FALSE;
	colY = tbl->col(frm.sColY);
	if (!colY.fValid())
		return false;

  grdr = rdd = new RoseDiagramDrawer(0); 
	RoseDiagramLayer* rdl = new RoseDiagramLayer(rdd, tbl, colX, colY);
  // X-axis
  if (colX.fValid())
  {
		rdd->rdaxX->Set(DomainValueRangeStruct(ValueRange(0,180,1)));
		rdd->rdaxX->dvrsData = colX->dvrs();
		rdd->rdaxX->sTitle = colX->sName();
  }
  // Y-axis
	rdd->rdaxY->Set(colY->dvrs());
	rdd->rdaxY->dvrsData = colY->dvrs();
	rdd->rdaxY->SetMinMax(colY->rrMinMax());
	rdd->rdaxY->sTitle = colY->sName();
  grdr->sTitle = rdd->rdaxY->sTitle;
	if (colY->fValues()) {
		RangeReal rr = colY->rrMinMax();
		double rWidth = rr.rWidth();
		rr.rHi() += rWidth * 0.03;
		if (rr.rLo() >= 0 && rr.rLo() < rWidth * 0.3)
			rr.rLo() = 0;
		else 
			rr.rLo() -= rWidth * 0.1;
		rdd->rdaxY->SetMinMax(rr);
	}
  
  rdd->agl.push_back(rdl);
  rdl->color = Representation::clrPrimary(rdd->agl.iSize());

  UpdateAllViews(0);
	SetModifiedFlag();
  return TRUE;
}

void RoseDiagramDoc::DeleteContents()
{
  rdd = 0;
  GraphDoc::DeleteContents();
}

void RoseDiagramDoc::OnAddGraph()
{
  class AddGraphForm: public FormWithDest
  {
	public:
		AddGraphForm(CWnd* wParent, String* sTable, String* sColX, String* sColY)
		: FormWithDest(wParent, TR("Add Rose Diagram"))
    , sTbl(sTable)
		{
      ftbl = new FieldTable(root, TR("&Table"), sTbl);
      ftbl->SetCallBack((NotifyProc)&AddGraphForm::TableCallBack);
			fcolX = new FieldColumn(root, TR("&Angle"), 0, sColX, dmVALUE);
			fcolY = new FieldColumn(root, TR("&Value"), 0, sColY, dmVALUE | dmIMAGE | dmBOOL);
      SetMenHelpTopic("ilwismen\\graph_window_add_rose_diagram.htm");
			create();
		}
  private:
    String* sTbl;
    FieldTable* ftbl;
    FieldColumn *fcolX, *fcolY;
    int TableCallBack(Event*)
    {
      try {
        ftbl->StoreData();
        FileName fn(*sTbl);
        fcolX->FillWithColumns(fn);
        fcolY->FillWithColumns(fn);
      }
      catch (ErrorObject&)
      {
      }
      return 0;
    }
  };
  Column colY;

  String sTable = tbl->sName(true);
  String sColX = colX->sName();
  String sColY;
  AddGraphForm frm(wndGetActiveView(), &sTable, &sColX, &sColY);
  if (!frm.fOkClicked())
    return;
  tbl = Table(sTable);
  colX = tbl->col(sColX);
  colY = tbl->col(sColY);

  RoseDiagramLayer* rdl = new RoseDiagramLayer(rdd, tbl, colX, colY);
	rdd->agl.push_back(rdl);
	rdl->line.clrLine() = rdl->color = Representation::clrPrimary(rdd->agl.iSize());
  // config layer ??
	SetModifiedFlag();
  UpdateAllViews(0);
}

GraphAxis* RoseDiagramDoc::ga(GraphAxis::GraphAxisPos gp) const
{
  if (0 != rdd) 
  {
    switch (gp)
    {
      case GraphAxis::gapXRose:
        return rdd->rdaxX;
      case GraphAxis::gapYRose:
        return rdd->rdaxY;
      default:
        return 0;
    }
  }  
  return 0;
}


