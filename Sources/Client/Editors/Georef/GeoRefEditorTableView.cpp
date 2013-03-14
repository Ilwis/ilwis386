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
// GeoRefEditorTableView.cpp: implementation of the GeoRefEditorTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(GeoRefEditorTableView, TablePaneView)
	//{{AFX_MSG_MAP(GeoRefEditorTableView)
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	ON_COMMAND(ID_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

GeoRefEditorTableView::GeoRefEditorTableView()
{
}

GeoRefEditorTableView::~GeoRefEditorTableView()
{
}

void GeoRefEditorTableView::Create(CWnd* wPar, TiePointEditor* grfed)
{
  gre = grfed;
	CView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), wPar, 100, 0);
}

void GeoRefEditorTableView::OnULButtonPressed()
{
  if (tField) delete tField;
  tField = 0;
}


void GeoRefEditorTableView::OnColButtonPressed(int iCol)
{
  if (tField) delete tField;
  tField = 0;
//  if (iCol < iCols()) 
// always: do not forbid to add columns
    TablePaneView::OnColButtonPressed(iCol);
}

void GeoRefEditorTableView::OnRowButtonPressed(long iRow)
{
  if (tField) delete tField;
  tField = 0;
  if (iRow > iRows()) {
    gre->OnAddPoint();
  }
	else {
		gre->SelectPoint(iRow);	
	}
}

String GeoRefEditorTableView::sDescrULButton() const // upper left button
{
  return "";
}

String GeoRefEditorTableView::sDescrColButton(int) const
{
  return ""; //SDChgColProp;
}

String GeoRefEditorTableView::sDescrRowButton(long iRow) const
{
  return "";
}

BOOL GeoRefEditorTableView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (TablePaneView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	else if (gre->mpv->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	else
		return FALSE;
}

LRESULT GeoRefEditorTableView::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	return gre->mpv->OnUpdate(wParam,lParam);
}

void GeoRefEditorTableView::OnEditClear()
{
	gre->SelectPoint(0);	
  if (selection.minCol() < 0) {
    int iRet = MessageBox(TR("Delete selected tiepoints").c_str(), TR("Delete tiepoints").c_str(),
			MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
    if (IDYES == iRet) 
		{
      long iMin = selection.minRow();
      if (iMin < 0) iMin = 0;
      long iMax = selection.maxRow();
      long iNr = iMax - iMin + 1;
      if (iNr > 0) 
				gre->DelPoints(iMin, iNr);
    }
  }
  else 
		TablePaneView::OnEditClear();
}

void GeoRefEditorTableView::OnUpdate(CView* vwSender, LPARAM lHint, CObject* pHint)
{
	TablePaneView::OnUpdate(vwSender, lHint, pHint);
	if (lHint == uhPRESORT || lHint == uhPOSTSORT)
		return;
	gre->Calc();
}

String GeoRefEditorTableView::sPrintTitle() const
{
	return gre->sTitle();
}

