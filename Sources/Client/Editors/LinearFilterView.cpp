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
// LinearFilterView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\FLTLIN.H"
#include "Client\Editors\FilterDoc.h"
#include "Client\Editors\LinearFilterDoc.h"
#include "Client\Editors\LinearFilterView.h"
#include "Headers\Hs\FILTER.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinearFilterView

IMPLEMENT_DYNCREATE(LinearFilterView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(LinearFilterView, BaseTablePaneView)
	//{{AFX_MSG_MAP(LinearFilterView)
	ON_COMMAND(ID_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LinearFilterView::LinearFilterView()
{
}

LinearFilterView::~LinearFilterView()
{
}

void LinearFilterView::OnDraw(CDC* pDC)
{
	BaseTablePaneView::OnDraw(pDC);

	CPen pen(PS_SOLID, 1, RGB(0,0,255));
	CPen* penOld = pDC->SelectObject(&pen);

	int iCol = iCols() / 2;
	int iRow = 1 + iRows() / 2;

	int x1 = iColPix[iCol];
	int x2 = iColPix[iCol+1];
	int y1 = iRow * iHeight();
	int y2 = y1 + iHeight();
	
	CRect rect;
	GetClientRect(&rect);  

	pDC->MoveTo(0, y1);
	pDC->LineTo(rect.right, y1);
	pDC->MoveTo(0, y2);
	pDC->LineTo(rect.right, y2);
	pDC->MoveTo(x1, 0);
	pDC->LineTo(x1, rect.bottom);
	pDC->MoveTo(x2, 0);
	pDC->LineTo(x2, rect.bottom);

	pDC->SelectObject(penOld);
}

LinearFilterDoc* LinearFilterView::GetDocument()
{
	return dynamic_cast<LinearFilterDoc*>(CView::GetDocument());
}

const LinearFilterDoc* LinearFilterView::GetDocument() const
{
	return dynamic_cast<const LinearFilterDoc*>(CView::GetDocument());
}

void LinearFilterView::OnInitialUpdate() 
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
	iColWidth = new int[iCols()];
	iColPix = new int[iCols()+2];
	int iWidth = 7;
	int i;
	for (i = 0; i < iCols(); ++i)
		iColWidth[i] = iWidth;
	iButtonWidth = 12;

	int iTotWidth = iWidth * iCols();
	int iPaneHeight = iHeight() * (iRows() + 1);

	CWindowDC dc(this);
	int iH = dc.GetDeviceCaps(VERTRES);
	if (iPaneHeight > iH * 0.3)
		iPaneHeight = (int)(min(iH * 0.35, iPaneHeight));
	else
		iPaneHeight = (int)(min(iPaneHeight, iH * 0.3));

	CRect rectClient, rect;

	CFrameWnd *parentFrame = GetParentFrame();
	ISTRUE(fINotEqual, parentFrame, (CFrameWnd*)0);
	parentFrame->GetWindowRect(rect);

	// Set default window size, in case no settings are stored in the registry
	GetClientRect(rectClient);
	rect.SetRect(rect.left, rect.top, 
		rect.left + rect.Width(),
		rect.top + rect.Height() - rectClient.Height() + iPaneHeight);
	
	parentFrame->MoveWindow(rect);
}

void LinearFilterView::InitColPix(CDC* cdc)
{
	BaseTablePaneView::InitColPix(cdc);
	if (0 == iColPix)
		return;
	LinearFilterDoc* fd = GetDocument();
	if (0 == fd)
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

int LinearFilterView::iCols() const
{
	const LinearFilterDoc* fd = GetDocument();
	if (0 == fd || !fd->flt().fValid())
		return 1;
  return fd->flt()->iCols();
}

long LinearFilterView::iRows() const
{
	const LinearFilterDoc* fd = GetDocument();
	if (0 == fd || !fd->flt().fValid())
		return 1;
  return fd->flt()->iRows();
}

String LinearFilterView::sColButton(int iCol) const
{
	iCol -= iCols()/2;
	String s("%i", iCol);
	return s;
}

String LinearFilterView::sRowButton(long iRow) const
{
	iRow -= 1;
	iRow -= iRows()/2;
	String s("%2i", iRow);
	return s;
}

String LinearFilterView::sField(int iCol, long iRow) const
{
	const LinearFilterDoc* fd = GetDocument();
	if (0 == fd || !fd->flt().fValid())
		return "";
	iRow -= 1;
	int iWidth = iColWidth[iCol];
	int iVal = fd->fltlin()->iValue(iRow, iCol);
	String s("%*i", iColWidth[iCol], iVal);
	return s;
}

class LinearFilterField: public BaseTblField
{
public:
  LinearFilterField(LinearFilterView* lfv, int col, long row)
  : BaseTblField(lfv,col,row)
	{
		int iVal = lfv->GetDocument()->fltlin()->iValue(iRow-1, iCol);							
		String s("%i", iVal);
		init(s);
	}
  ~LinearFilterField()
	{
		if (fHasChanged()) {
			String s = sText();
			if ("" == s)
				return;
			LinearFilterView* lfv = dynamic_cast<LinearFilterView*>(tbpn);
			if (0 == lfv)
				return;
			LinearFilterDoc* fd = lfv->GetDocument();
			fd->fltlin()->SetValue(iRow-1,iCol,s.iVal());
		}
	}
};


void LinearFilterView::OnFieldPressed(int iCol, long iRow, bool fLeft)
{
	if (tField) delete tField;
	tField = 0;
	if (iCol < 0 || iCol >= iCols()
		|| iRow <= 0 || iRow > iRows()) 
	{
//		MessageBeep(MB_ICONASTERISK);
		return;
	}
	if (fLeft) {
		FrameWindow* fw = fwParent();
		LinearFilterDoc* fd = GetDocument();
		if (0 == fd)
			return;
		if (!fd->fReadOnly()) {
			tField = new LinearFilterField(this,iCol,iRow);
		}  
		else 
			MessageBeep(MB_ICONASTERISK);
	}    
}

bool LinearFilterView::fAllowPaste() const
{
  return !GetDocument()->fReadOnly() && fValidSelection() && IsClipboardFormatAvailable(CF_TEXT);
}

void LinearFilterView::OnEditPaste()
{
	LinearFilterDoc* fd = GetDocument();
	if (0 == fd)
		return;
	FilterLinear* fl = fd->fltlin();
  char *sText, *s, *str;
  long r, c;

  bool fLine, fNull, fBreak, fRowHeader = false, fColHeader = false;

	if (!OpenClipboard())
		return;
	bool fTblFormat = 0 != IsClipboardFormatAvailable(iFmtTbl);
	HANDLE hnd;
	if (fTblFormat)
    hnd = GetClipboardData(iFmtTbl);
	else
    hnd = GetClipboardData(CF_TEXT);
  if (0 == hnd)
    return;
	sText = new char[(unsigned int)GlobalSize(hnd)];
	strcpy(sText, (char*)GlobalLock(hnd));
	GlobalUnlock(hnd);
	CloseClipboard();
	char* sTextBegin = sText;
	if (fTblFormat) {
    sscanf(sText, "rowheader=%i colheader=%i", &fRowHeader, &fColHeader);
		sTextBegin = strstr(sText, "\r\n")+2;
  	if (fColHeader) { // skip it
			char *s = strchr(sTextBegin,'\n');
			if (s) 
				sTextBegin = s+1;
		}
	}

  str = sTextBegin;
  r = mmSelect.MinRow();
  if (r < 0)
    r = 0;
	if (fColHeader)
		++r;
	bool fResortAfterPaste = false;
  for (; r <= mmSelect.MaxRow(); ++r) {
    fLine = false;
		bool fRowHeaderSkipped = !fRowHeader;
    for (c = mmSelect.MinCol(); c <= mmSelect.MaxCol(); ++c) {
			s = str;
			while (*s && *s != '\t' && *s != '\n' && *s != '\r') ++s;
			if (*s == '\r') 
				*s++ = '\0';
			fLine = *s == '\n';
			fNull = *s == '\0';
			fBreak = *s != '\t';
			*s = '\0';
			if (c == mmSelect.MinCol() && !fRowHeaderSkipped) {
				fRowHeaderSkipped = true;
			  str = s + 1;
				continue;
			}
			if (*str == '\0')
				break;
			long iVal = String(str).iVal();
   		fl->SetValue(r-1, c, iVal);
 			str = s;
			if (fBreak)
				break;
			if (fLine)
				break;  
			str = s + 1;
    }
    if (!fNull)
			++s;
    if (!fLine)
			while (*s && *s != '\n') ++s;
		if (*s == '\0')
			break;
    str = s;
  }
  delete sText;
	fl->Updated();
	fd->UpdateAllViews(0);
}

void LinearFilterView::OnUpdateEdit(CCmdUI* pCmdUI)
{
	bool fEdit = !GetDocument()->fReadOnly() && fValidSelection();
	pCmdUI->Enable(fEdit);
}

namespace{
class EditFieldForm: public FormWithDest
{
public:
  EditFieldForm(CWnd* parent, long* iVal)
  : FormWithDest(parent, TR("Edit Fields"))
  {
    new FieldInt(root, TR("&Value").c_str(), iVal);
  	new FieldBlank(root);
//		setHelpItem("ilwismen\table_window_edit_single_column.htm");
    create();
  }
};
}

void LinearFilterView::OnEdit()
{
	LinearFilterDoc* fd = GetDocument();
	if (0 == fd)
		return;
	FilterLinear* fl = fd->fltlin();
	int iMinRow = mmSelect.MinRow();
	if (iMinRow < 0) 
		iMinRow = 0;
  int iMaxRow = mmSelect.MaxRow();
	if (iMaxRow > fl->iRows())
		iMaxRow = fl->iRows();
  int iMinCol = mmSelect.MinCol();
	if (iMinCol < 0) 
		iMinCol = 0;
  int iMaxCol = mmSelect.MaxCol();
	if (iMaxCol > fl->iCols())
		iMaxCol = fl->iCols();

	long iVal = fl->iValue(iMinRow-1,iMinCol);
  EditFieldForm frm(this, &iVal);
  if (frm.fOkClicked()) {
    for (int i=iMinRow; i <= iMaxRow; i++)     
      for (int j=iMinCol; j <= iMaxCol; j++)     
    		fl->SetValue(i-1, j, iVal);
		fl->Updated();
		fd->UpdateAllViews(0);
	}
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);

void LinearFilterView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men;
	men.CreatePopupMenu();

  add(ID_EDIT_COPY );
	men.EnableMenuItem(ID_EDIT_COPY, fAllowCopy() ? MF_ENABLED : MF_GRAYED);
  add(ID_EDIT_PASTE);
	men.EnableMenuItem(ID_EDIT_PASTE, fAllowPaste() ? MF_ENABLED : MF_GRAYED);
  add(ID_EDIT);
  int iMinCol = mmSelect.MinCol();
  int iMaxCol = mmSelect.MaxCol();
	men.EnableMenuItem(ID_EDIT, iMaxCol >= iMinCol ? MF_ENABLED : MF_GRAYED);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}



 
