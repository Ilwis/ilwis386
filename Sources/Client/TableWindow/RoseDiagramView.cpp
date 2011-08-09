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
// RoseDiagramView.cpp: implementation of the RoseDiagramView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\RoseDiagramView.h"
#include "Engine\Table\tblview.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fldcol.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Graph.hs"
#include "Client\GraphWindow\RoseDiagramAxis.h"
#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\RoseDiagramDrawer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(RoseDiagramView, GraphView)
	//{{AFX_MSG_MAP(RoseDiagramView)
	ON_COMMAND(ID_GRPH_ADD, OnGraphAddColumn)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

namespace {
		class ConfigInitForm: public FormWithDest
		{
		public:
			ConfigInitForm(CWnd* wParent, const Table& tbl, bool fXAxis)
				: FormWithDest(wParent, TR("Create Graph"))
			{
				fColX = true;
				CheckBox* cb = 0;
				tvw = new TableView(tbl);
				if (fXAxis) {
					cb = new CheckBox(root, TR("&X-column"), &fColX);
					new FieldColumn(cb, "", tvw, &sColX, dmVALUE);
				}
				FieldColumn* fc = new FieldColumn(root, TR("&Y-column"), tvw, &sColY, dmVALUE);
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
};

class ColumnRoseDiagramDrawer : public RoseDiagramDrawer
{
public:
	ColumnRoseDiagramDrawer(RoseDiagramView* rdvw) 
		: RoseDiagramDrawer(rdvw), rdv(rdvw)
	{
	}
  virtual ~ColumnRoseDiagramDrawer() 
	{
	}
  virtual bool fAddGraph(bool fConfigLayer);
	RoseDiagramView* rdv;
};


bool ColumnRoseDiagramDrawer::fAddGraph(bool fConfigLayer)
{
	bool fFirstGraph = rdv->rdd->agl.iSize()==0;
	ConfigInitForm frm(rdv, rdv->GetDocument()->table(), !fFirstGraph);
	bool fOk = frm.fOkClicked();
	if (fOk)
		rdv->NewGraphLayer(frm.fColX, frm.sColX, frm.sColY, fConfigLayer, !fFirstGraph);
	return fOk;
}

		
RoseDiagramView::RoseDiagramView()
{
  rdd = new ColumnRoseDiagramDrawer(this);
	grdrw = rdd;
}

RoseDiagramView::~RoseDiagramView()
{
  delete rdd;
}

TableDoc* RoseDiagramView::GetDocument()
{
  return (TableDoc*)m_pDocument;
}

const TableDoc* RoseDiagramView::GetDocument() const
{
  return (const TableDoc*)m_pDocument;
}

void RoseDiagramView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	GraphView::OnUpdate(pSender, lHint, pHint);

	TableDoc* td = GetDocument();
	TableView* tvw = td->tvw;
}


bool RoseDiagramView::fConfigInit(CWnd* wPar)
{
	ConfigInitForm frm(wPar, GetDocument()->table(), true);
	bool fOk = frm.fOkClicked();
	if (fOk)
		fOk = NewGraphLayer(frm.fColX, frm.sColX, frm.sColY, true, false);
	return fOk;
}

bool RoseDiagramView::NewGraphLayer(bool fColX, const String& sColX, const String& sColY, bool fConfigLayer, bool fXAxisDefined)
{
	Table tbl = GetDocument()->table();
	if (!fXAxisDefined) {
		rdd->rdaxX->Set(DomainValueRangeStruct(ValueRange(0,180,1)));
		if (fColX) {
			colX = tbl->col(sColX);
			if (!colX.fValid())
				return false;
			rdd->rdaxX->dvrsData = colX->dvrs();
			rdd->rdaxX->sTitle = colX->sName();
			if (0 != colX->sDescr().length())
				rdd->rdaxX->sTitle += String(" : %S", colX->sGetDescription());
		}
	}
	else {
		// get X column from one of the other column graphs
		for (unsigned int i=0; i < rdd->agl.iSize(); i++) {
			RoseDiagramLayer* rdl = rdd->rdl(i);
			if (0 != rdl) {
				colX = rdl->colX;
				break;
			}
		}
	}
	Column colY = tbl->col(sColY);
	if (!colY.fValid())
		return false;

	RoseDiagramLayer* rdl = new RoseDiagramLayer(rdd, tbl, colX, colY);
	
	rdd->rdaxY->Set(colY->dvrs());
	rdd->rdaxY->dvrsData = colY->dvrs();
	rdd->rdaxY->SetMinMax(colY->rrMinMax());
	rdd->rdaxY->sTitle = colY->sName();
	if (0 != colY->sDescr().length())
		rdd->rdaxY->sTitle += String(" : %S", colY->sGetDescription());

	bool fOk = true;
  rdd->agl.push_back(rdl);
	if (fConfigLayer)
  	fOk = rdl->fConfig();
	return fOk;
}


void RoseDiagramView::SaveSettings(const FileName& fn, int iGraphWindow)
{
	String sSection("GraphWindow%i", iGraphWindow);
	rdd->SaveSettings(fn, sSection);
}

void RoseDiagramView::LoadSettings(const FileName& fn, int iGraphWindow)
{
	String sSection("GraphWindow%i", iGraphWindow);
	rdd->LoadSettings(fn, sSection);
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void RoseDiagramView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_GRPH_OPTIONS);
  add(ID_GRPH_ADD);
	add(ID_COPY);
	add(ID_FILE_PRINT);
  men.AppendMenu(MF_SEPARATOR);
	add(ID_GRPH_FITINWINDOW);
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void RoseDiagramView::OnGraphAddColumn()
{
  rdd->fAddGraph(true);
}
