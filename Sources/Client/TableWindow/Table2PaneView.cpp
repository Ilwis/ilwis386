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
// Table2PaneView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Table\tbl2dim.h"
#include "Client\TableWindow\Table2Doc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\Table2PaneView.h"
#include "Headers\Hs\Table.hs"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\Table2Window.h"
#include "Client\FormElements\fldval.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Applications\COLVIRT.H"
//#include "Engine\Table\COLCALC.H"
#include "Client\TableWindow\BaseTblField.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Table2PaneView

IMPLEMENT_DYNCREATE(Table2PaneView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(Table2PaneView, BaseTablePaneView)
	//{{AFX_MSG_MAP(Table2PaneView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_CLEAR, OnEditClear)
	ON_COMMAND(ID_PASTE, OnEditPaste)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


class Tbl2DimField: public BaseTblField
{
public:
  Tbl2DimField(Table2PaneView*, int col, long row);
  ~Tbl2DimField();
	Table2Dim tbl2;
};


Table2PaneView::Table2PaneView()
{
}

Table2PaneView::~Table2PaneView()
{
  if (tField) {
    // has to be destructed here, because desctructor of TblField calls FieldChanged()
    delete tField;
    tField = 0; 
  }
}


/////////////////////////////////////////////////////////////////////////////
// Table2PaneView drawing

void Table2PaneView::InitColPix(CDC* cdc)
{
	BaseTablePaneView::InitColPix(cdc);
	if (0 == iColPix)
		return;
	Table2Doc* td = GetDocument();
	if (0 == td)
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

/////////////////////////////////////////////////////////////////////////////
// Table2PaneView diagnostics

#ifdef _DEBUG
void Table2PaneView::AssertValid() const
{
	BaseTablePaneView::AssertValid();
}

void Table2PaneView::Dump(CDumpContext& dc) const
{
	BaseTablePaneView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Table2PaneView message handlers

Table2Doc* Table2PaneView::GetDocument()
{
  return (Table2Doc*)m_pDocument;
}

const Table2Doc* Table2PaneView::GetDocument() const
{
  return (const Table2Doc*)m_pDocument;
}

int Table2PaneView::iCols() const
{
	const Table2Doc* td = GetDocument();
	if (0 == td)
		return 1;
  return td->iCols();
}

long Table2PaneView::iRows() const
{
	const Table2Doc* td = GetDocument();
	if (0 == td)
		return 1;
  return td->iRecs();
}


String Table2PaneView::sColButton(int iCol) const
{
	const Table2Doc* td = GetDocument();
	if (0 == td)
		return "";
  return td->table2()->ds2()->sValue(1+iCol,iColWidth[iCol]);
}

String Table2PaneView::sRowButton(long iRow) const
{
	const Table2Doc* td = GetDocument();
	if (0 == td)
		return "";
  return td->table2()->ds1()->sValue(iRow,iButtonWidth);
}

String Table2PaneView::sField(int iCol, long iRow) const
{
	const Table2Doc* td = GetDocument();
	if (0 == td)
		return "";
	const Table2Dim& tbl2 = td->table2();
  return tbl2->sValue(tbl2->ds1()->iKey(iRow), tbl2->ds2()->iKey(1+iCol), iColWidth[iCol], tbl2->dvrsVal().iDec());
}

String Table2PaneView::sDescrULButton() const // upper left button
{
	return "";
  //return STBRemRecView;
}

String Table2PaneView::sDescrColButton(int iCol) const
{
	return "";
}

String Table2PaneView::sDescrRowButton(long iRow) const
{
	return "";
}

String Table2PaneView::sDescrField(int iCol, long iRow) const
{
	return "";
}

void Table2PaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  if (iColWidth)
    delete [] iColWidth;
  if (iColPix)
    delete [] iColPix;  
//  tps->SetRanges(iRows(), iCols());

	Table2Doc* td = GetDocument();
	if (0 == td)
		return;
  iColWidth = new int[td->iCols()];
  iColPix = new int[td->iCols()+2];
  iButtonWidth = max(4, td->table2()->ds1()->iWidth()); 
	int iWidth = max(td->table2()->dvrsVal().iWidth(), td->table2()->ds2()->iWidth());
  for (int i = 0; i < td->iCols(); ++i)
    iColWidth[i] = iWidth;
	BaseTablePaneView::OnUpdate(pSender, lHint, pHint);
}

void Table2PaneView::OnInitialUpdate() 
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
	Table2Doc* td = GetDocument();
	int iWidth = max(td->table2()->dvrsVal().iWidth(), td->table2()->ds2()->iWidth());
  int i;
  for (i = 0; i < iCols(); ++i)
    iColWidth[i] = iWidth;
  iButtonWidth = max(4, td->table2()->ds1()->iWidth());
}

void Table2PaneView::OnFieldPressed(int iCol, long iRow, bool fLeft)
{
  if (tField) delete tField;
	tField = 0;
  if (iCol < 0 || iCol >= iCols()
   || iRow <= 0 || iRow > iRows()) 
	{
    MessageBeep(MB_ICONASTERISK);
    return;
  }

  if (tField) delete tField;
  tField = 0;
  if (iCol < 0 || iRow <= 0 || iCol >= iCols() || iRow > iRows()) {
    MessageBeep(MB_ICONASTERISK);
    return;
  }
  if (fLeft)
    tField = new Tbl2DimField(this,iCol,iRow);
}


String Table2PaneView::sPrintTitle() const
{
	return GetDocument()->table2()->sTypeName();
}

bool Table2PaneView::fAllowPaste() const
{
  return !GetDocument()->table2()->fReadOnly() && fValidSelection();
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);

void Table2PaneView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men;
	men.CreatePopupMenu();

  add(ID_EDIT_COPY );
	men.EnableMenuItem(ID_EDIT_COPY, fAllowCopy() ? MF_ENABLED : MF_GRAYED);
  add(ID_EDIT_PASTE);
	men.EnableMenuItem(ID_EDIT_PASTE, fAllowPaste() ? MF_ENABLED : MF_GRAYED);
  add(ID_CLEAR);
	men.EnableMenuItem(ID_CLEAR, fAllowClear() ? MF_ENABLED : MF_GRAYED);
  add(ID_EDIT);
  int iMinCol = mmSelect.MinCol();
  int iMaxCol = mmSelect.MaxCol();
	men.EnableMenuItem(ID_EDIT, iMaxCol >= iMinCol ? MF_ENABLED : MF_GRAYED);
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void Table2PaneView::OnUpdateEdit(CCmdUI* pCmdUI)
{
	bool fEdit = fAllowPaste(); // same condition
	pCmdUI->Enable(fEdit);
}

class EditFieldForm: public FormWithDest
{
public:
  EditFieldForm(CWnd* parent, const DomainValueRangeStruct& dvrs, String& sVal)
  : FormWithDest(parent, STBTitleEditFields)
  {
    new FieldVal(root, STBUINewValue.scVal(), dvrs, &sVal);
  	new FieldBlank(root);
//		setHelpItem(htpTblEditMultiRecords);
    create();
  }
};


void Table2PaneView::OnEdit()
{
	Table2Doc* td = GetDocument();
	Table2Dim tbl2 = td->table2();
  int iMinRow = mmSelect.MinRow();
	if (iMinRow < 0) 
		iMinRow = 0;
  int iMaxRow = mmSelect.MaxRow();
	if (iMaxRow > tbl2->iMatrixRows())
		iMaxRow = tbl2->iMatrixRows();
  int iMinCol = mmSelect.MinCol();
	if (iMinCol < 0) 
		iMinCol = 0;
  int iMaxCol = mmSelect.MaxCol();
	if (iMaxCol > tbl2->iMatrixCols())
		iMaxCol = tbl2->iMatrixCols();

	String sVal;
  EditFieldForm frm(this, tbl2->dvrsVal(), sVal);
  if (frm.fOkClicked()) {
    for (int i=iMinRow; i <= iMaxRow; i++)     
      for (int j=iMinCol; j <= iMaxCol; j++)     
   		  tbl2->PutVal(tbl2->ds1()->iKey(i), tbl2->ds2()->iKey(1+j), sVal);
		Invalidate();
	}
}

void Table2PaneView::OnEditClear()
{
	Table2Doc* td = GetDocument();
	if (0 == td)
		return;
	Table2Dim tbl2 = td->table2();
  long iMinCol = mmSelect.MinCol();
  long iMaxCol = mmSelect.MaxCol();
  iMaxCol = min(iMaxCol, td->table2()->iMatrixCols()-1);
  if (iMinCol > iMaxCol)
    return;
  int iRet = MessageBox(STBMsgClearSelFields.sVal(), STBMsgClearFields.sVal(),
	    MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
  if (IDYES == iRet) {
		CWaitCursor cur;
    long iMaxRow = mmSelect.MaxRow();
    for (long c = iMinCol; c <= iMaxCol; ++c) 
      for (long r = mmSelect.MinRow(); r <= iMaxRow; ++r)
        tbl2->PutVal(tbl2->ds1()->iKey(r), tbl2->ds2()->iKey(1+c), sUNDEF);
    tbl2->Updated();
		td->UpdateAllViews(0);
  }
  BaseTablePaneView::OnEditClear();
}

void Table2PaneView::OnEditPaste()
{
	if (!IsClipboardFormatAvailable(CF_TEXT))
		return;

	Table2Doc* td = GetDocument();
	if (0 == td)
		return;
	Table2Dim tbl2 = td->table2();
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
   		tbl2->PutVal(tbl2->ds1()->iKey(r), tbl2->ds2()->iKey(1+c), str);
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
	tbl2->Updated();
	td->UpdateAllViews(0);
}

///////////////

Tbl2DimField::Tbl2DimField(Table2PaneView* pane, int col, long row)
  : BaseTblField(pane,col,row,pane->GetDocument()->table2()->dvrsVal().dm()), tbl2(pane->GetDocument()->table2())
{
  String str = tbl2->sValue(tbl2->ds1()->iKey(iRow), tbl2->ds2()->iKey(1+iCol), 0);
  init(str);
}

Tbl2DimField::~Tbl2DimField()
{
  if (fHasChanged()) 
	{
    String s = sText();
    if ("" == s)
      return;
		tbl2->PutVal(tbl2->ds1()->iKey(iRow), tbl2->ds2()->iKey(1+iCol), s);
    tbl2->Updated();
    tbpn->GetDocument()->UpdateAllViews(0);
  }
}
 
