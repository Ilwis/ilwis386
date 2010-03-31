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
// MatrixView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\MatrixDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\Editors\MatrixView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MatrixView

IMPLEMENT_DYNCREATE(MatrixView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(MatrixView, BaseTablePaneView)
	//{{AFX_MSG_MAP(MatrixView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MatrixView::MatrixView()
{
}

MatrixView::~MatrixView()
{
}

MatrixDoc* MatrixView::GetDocument()
{
	return dynamic_cast<MatrixDoc*>(BaseTablePaneView::GetDocument());
}

const MatrixDoc* MatrixView::GetDocument() const
{
	return dynamic_cast<const MatrixDoc*>(BaseTablePaneView::GetDocument());
}

void MatrixView::OnInitialUpdate() 
{
	MatrixDoc* md = GetDocument();
	if (0 == md || !md->mat().fValid())
		return;

	md->MakeUsable();

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
  iColWidth = new int[iCols()];
  iColPix = new int[iCols()+2];
//	const MatrixDoc* md = GetDocument();
	int iWidth = md->mat()->iWidth();
  int i;
  for (i = 0; i < iCols(); ++i)
    iColWidth[i] = iWidth;
  iButtonWidth = 12;
}

void MatrixView::InitColPix(CDC* cdc)
{
	BaseTablePaneView::InitColPix(cdc);
	if (0 == iColPix)
		return;
	MatrixDoc* md = GetDocument();
	if (0 == md)
		return;
  int iRecWidth = iCharWidth * iButtonWidth + 6;
  iColPix[0] = iRecWidth;
  for (int i = 1; i <= iFirstVisibleColumn(); ++i)
    iColPix[i] = iColPix[i-1];
  for (int i = iFirstVisibleColumn(); i < iCols(); ++i) {
    iColPix[i+1] = iColPix[i] + iCharWidth * iColWidth[i] + 5;
  }
  iColPix[iCols()+1] = 32000;
}

int MatrixView::iCols() const
{
	const MatrixDoc* md = GetDocument();
	if (0 == md || !md->mat().fValid())
		return 1;
  return md->mat()->iCols();
}

long MatrixView::iRows() const
{
	const MatrixDoc* md = GetDocument();
	if (0 == md || !md->mat().fValid())
		return 1;
  return md->mat()->iRows();
}

String MatrixView::sColButton(int iCol) const
{
	const MatrixDoc* md = GetDocument();
	if (0 == md)
		return "";
	return md->sCol(iCol);
}

String MatrixView::sRowButton(long iRow) const
{
	const MatrixDoc* md = GetDocument();
	if (0 == md)
		return "";
  return md->sRow(iRow-1);
}

String MatrixView::sField(int iCol, long iRow) const
{
	const MatrixDoc* md = GetDocument();
	if (0 == md)
		return "";
	int iWidth = iColWidth[iCol];
  return md->mat()->sValue(iRow-1, iCol, iWidth);
}


