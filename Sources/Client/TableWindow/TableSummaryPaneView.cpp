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
// TableSummaryPaneView.cpp: implementation of the TableSummaryPaneView class.
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
#include "Engine\Table\tblview.h"
#include "Client\TableWindow\TableSummaryPaneView.h"
#include "Headers\Hs\Table.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(TableSummaryPaneView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(TableSummaryPaneView, BaseTablePaneView)
	ON_WM_CONTEXTMENU()
  ON_UPDATE_COMMAND_UI(ID_PROP, OnUpdateProp)
END_MESSAGE_MAP()

TableSummaryPaneView::TableSummaryPaneView()
{
	fShowHeading = false;
}

TableSummaryPaneView::~TableSummaryPaneView()
{
}

TableDoc* TableSummaryPaneView::GetDocument()
{
  return (TableDoc*)m_pDocument;
}

const TableDoc* TableSummaryPaneView::GetDocument() const
{
  return (const TableDoc*)m_pDocument;
}

long TableSummaryPaneView::iCols() const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return 1;
  return td->iCols();
}

long TableSummaryPaneView::iRows() const
{
	return 5;
}

void TableSummaryPaneView::InitColPix(CDC* cdc)
{
	BaseTablePaneView::InitColPix(cdc);
	if (0 == iColPix)
		return;
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	iButtonWidth = td->cvKey().iWidth;
  int iRecWidth = iCharWidth * iButtonWidth + 6;
  iColPix[0] = iRecWidth;
  for (int i = 1; i <= iFirstVisibleColumn(); ++i)
    iColPix[i] = iColPix[i-1];
  for (int i = iFirstVisibleColumn(); i < iCols(); ++i) {
    iColWidth[i] = td->cv(i).iWidth;
    iColPix[i+1] = iColPix[i] + iCharWidth * iColWidth[i] + 5;
  }
  iColPix[td->iCols()+1] = 32000;
}

void TableSummaryPaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  if (iColWidth)
    delete [] iColWidth;
  if (iColPix)
    delete [] iColPix;  
//  tps->SetRanges(iRows(), iCols());

	TableDoc* td = GetDocument();
	if (0 == td)
		return;
  iColWidth = new int[td->iCols()];
  iColPix = new int[td->iCols()+2];
  int i;
  for (i = 0; i < td->iCols(); ++i)
    iColWidth[i] = td->cv(i).iWidth;
  iButtonWidth = td->cvKey().iWidth;
	if (lHint == uhNOBUTTONS) 
    lHint = 0;
	BaseTablePaneView::OnUpdate(pSender, lHint, pHint);
}

void TableSummaryPaneView::OnInitialUpdate() 
{
	// first place scroll bars
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 10;
	si.nPage = 1;
	si.nPos = 1;
  SetScrollInfo(SB_VERT, &si);
  SetScrollInfo(SB_HORZ, &si);
	BaseTablePaneView::OnInitialUpdate();
}

String TableSummaryPaneView::sRowButton(long iRow) const
{
	String s;
	switch (iRow) 
	{
		case 1: s = TR("Min"); break;
		case 2: s = TR("Max"); break;
		case 3: s = TR("Avg"); break;
		case 4: s = TR("StD"); break;
		case 5: s = TR("Sum"); break;
		default: return "";
	}
	for (int i = s.length(); i < iButWidth(); i++)
		s &= ' ';
	return s;
}

String TableSummaryPaneView::sField(int iCol, long iRow) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
  td->MakeUsable(iCol);
  String s;
	switch (iRow) 
	{
		case 1: s = td->view()->sMinimum(iCol); break;
		case 2: s = td->view()->sMaximum(iCol); break;
		case 3: s = td->view()->sMean(iCol); break;
		case 4: s = td->view()->sStdDev(iCol); break;
		case 5: s = td->view()->sSum(iCol); break;
		default: s = "";
	}
  int iWid = td->cv(iCol).iWidth;

  if (s.sTrimSpaces() == sUNDEF)
    s = "";
  if ((iWid > 0) && (s.length() > iWid)) // too large
    s = String('*', iWid);
  return s;
}

String TableSummaryPaneView::sDescrRowButton(long iRow) const
{
	switch (iRow) 
	{
		case 1: return TR("Minimum of values in column");
		case 2: return TR("Maximum of values in column");
		case 3: return TR("Average of values in column");
		case 4: return TR("Standard deviation of values in column");
		case 5: return TR("Sum of values in column");
		default: return "";
	}
}

void TableSummaryPaneView::OnUpdateProp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void TableSummaryPaneView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men;
	men.CreatePopupMenu();
  add(ID_EDIT_COPY );
	men.EnableMenuItem(ID_EDIT_COPY, fAllowCopy() ? MF_ENABLED : MF_GRAYED);
//  add(ID_EDIT_PASTE);
//	men.EnableMenuItem(ID_EDIT_PASTE, fAllowPaste() ? MF_ENABLED : MF_GRAYED);
//  add(ID_EDIT_CLEAR);
//	men.EnableMenuItem(ID_EDIT_CLEAR, fAllowClear() ? MF_ENABLED : MF_GRAYED);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

String TableSummaryPaneView::sDescrField(int iCol, long iRow) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
  if (iCol < td->iCols()) {
		String sRow = sDescrRowButton(iRow);
    Column col = td->cv(iCol);
		if ( col.ptr() == NULL )
			return "?";
    String sCol = col->sName();
    if ("" != col->sDescription) {
      sCol &= ": ";
      sCol &= col->sDescription;
    }  
		String s("%S: %S", sRow, sCol);
    return s;
  }  
  return "";
//  return String(SDEditFldOfRec_ss,sColButton(iCol),sRowButton(iRow));
}

