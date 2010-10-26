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
// TablePaneView.cpp : implementation file
//

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
#include "Engine\Table\tblview.h"
#include "Headers\Hs\Table.hs"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\TableWindow.h"
#include "Client\FormElements\fldval.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Applications\COLVIRT.H"
//#include "Engine\Table\COLCALC.H"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblinfo.h"
//#include "ApplicationsUI\Tblforms.h"
#include "Client\TableWindow\ColumnPropForm.h"
#include "Client\TableWindow\BaseTblField.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"
#include "Client\Base\Menu.h"

#define ID_GOTORECORDFIRST ID_GOTORECORD0
#define ID_GOTORECORDLAST ID_GOTORECORD1
#define ID_GOTORECORDPREV ID_GOTORECORD2
#define ID_GOTORECORDNEXT ID_GOTORECORD3


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TablePaneView

IMPLEMENT_DYNCREATE(TablePaneView, BaseTablePaneView)

BEGIN_MESSAGE_MAP(TablePaneView, BaseTablePaneView)
	//{{AFX_MSG_MAP(TablePaneView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_EDITFIELD, OnEditField)
	ON_COMMAND(ID_CLEAR, OnEditClear)
	ON_COMMAND(ID_PASTE, OnEditPaste)
	ON_COMMAND(ID_GOTOCOLUMN, OnGotoColumn)
	ON_COMMAND(ID_SORT, OnSortOn)
	ON_COMMAND(ID_SORT_ASCENDING, OnSortOnAscending)
	ON_COMMAND(ID_SORT_DESCENDING, OnSortOnDescending)
	ON_COMMAND(ID_UPDATEALLCOLS, OnUpdateAllColumns)
	ON_COMMAND(ID_PROP, OnProp)
	ON_UPDATE_COMMAND_UI(ID_PROP, OnUpdateProp)

	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


class TblField: public BaseTblField
{
public:
	TblField(TablePaneView*, int col, long row);
	~TblField();
};

class TblReadOnlyField: public BaseTblField
{
public:
	TblReadOnlyField(TablePaneView*, int col, long row);
	~TblReadOnlyField();
};



TablePaneView::TablePaneView()
{
}

TablePaneView::~TablePaneView()
{
	// has to be destructed here, because desctructor of TblField calls FieldChanged()
	deleteField();
}


/////////////////////////////////////////////////////////////////////////////
// TablePaneView drawing

void TablePaneView::InitColPix(CDC* cdc)
{
	BaseTablePaneView::InitColPix(cdc);

	if (0 == iColPix)
		return;
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	td->cvKey().iWidth = iButtonWidth;
	int iRecWidth = iCharWidth * iButtonWidth + 6;
	iColPix[0] = iRecWidth;
	for (int i = 1; i <= iFirstVisibleColumn(); ++i)
		iColPix[i] = iColPix[i-1];
	for (int i = iFirstVisibleColumn(); i < iCols(); ++i) {
		if (td->cv(i).iWidth != iColWidth[i]) {
			td->cv(i).iWidth = iColWidth[i];
			td->cv(i).Store();
		}  
		iColPix[i+1] = iColPix[i] + iCharWidth * iColWidth[i] + 5;
	}
	iColPix[td->iCols()+1] = 32000;
}


void TablePaneView::OnDraw(CDC* cdc)
{
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	td->CheckNrRecs();

	BaseTablePaneView::OnDraw(cdc);
}

/////////////////////////////////////////////////////////////////////////////
// TablePaneView diagnostics

#ifdef _DEBUG
void TablePaneView::AssertValid() const
{
	BaseTablePaneView::AssertValid();
}

void TablePaneView::Dump(CDumpContext& dc) const
{
	BaseTablePaneView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// TablePaneView message handlers

TableDoc* TablePaneView::GetDocument()
{
	return (TableDoc*)m_pDocument;
}

const TableDoc* TablePaneView::GetDocument() const
{
	return (const TableDoc*)m_pDocument;
}

int TablePaneView::iCols() const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return 1;
	return td->iCols();
}

long TablePaneView::iRows() const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return 1;
	return td->iRows();
}

String TablePaneView::sULButton() const
{
	return " .clm";
}

String TablePaneView::sColButton(int iCol) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
	if (iCol <= td->iCols()) 
	{
		if ( td->cv(iCol).ptr() == NULL )
			return "";

		String s = td->cv(iCol)->sName();
		if (td->tvw->cvKey().fValid()) // sort column set
			if (td->tvw->cvKey() == td->tvw->cv(iCol)) {
				s &= " *";
				if (td->tvw->so == TableView::soASCENDING)
					s &= '>';
				else
					s &= '<';
			}
			return s;
	}
	else
		return "";  
}

String TablePaneView::sRowButton(long iRow) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
	return td->sRow(iRow);
}

String TablePaneView::sField(int iCol, long iRow) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
	td->MakeUsable(iCol);
	return td->sValue(iCol,iRow);
}

String TablePaneView::sDescrULButton() const // upper left button
{
	return STBRemColumnManagement;
}

String TablePaneView::sDescrColButton(int iCol) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
	if (iCol < td->iCols()) {
		Column col = td->cv(iCol);
		if ( col.ptr() == NULL )
			return "";
		String s = STBRemChgColProp;
		s &= col->sName();
		if ("" != col->sDescription) {
			s &= ": ";
			s &= col->sDescription;
		}  
		return s;
	}  
	else
		return STBRemAddNewCol;
}

String TablePaneView::sDescrRowButton(long iRow) const
{
	String s = sRowButton(iRow);
	return String(STBRemEditRec_s.sVal(), s.sTrimSpaces());
}

String TablePaneView::sDescrField(int iCol, long iRow) const
{
	const TableDoc* td = GetDocument();
	if (0 == td)
		return "";
	if (iCol < td->iCols()) {
		Column col = td->cv(iCol);
		if ( col.ptr() == NULL)
			return "";
		String s = col->sTypeName();
		if ("" != col->sDescription) {
			s &= ": ";
			s &= col->sDescription;
		}  
		return s;
	}  
	return "";
	//  return String(SDEditFldOfRec_ss,sColButton(iCol),sRowButton(iRow));
}

void TablePaneView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	TableDoc* td = GetDocument();
	if (0 == td)
		return;	
	CreateColPix(td);
	iButtonWidth = max(4, td->table()->dm()->iWidth()); 
	BaseTablePaneView::OnUpdate(pSender, lHint, pHint);
}

void TablePaneView::CreateColPix(TableDoc* td)
{
	if (iColWidth)
		delete [] iColWidth;
	if (iColPix)
		delete [] iColPix;  
	//  tps->SetRanges(iRows(), iCols());


	iColWidth = new int[td->iCols()];
	iColPix = new int[td->iCols()+2];
	int i;
	for (i = 0; i < td->iCols(); ++i)
		iColWidth[i] = td->cv(i).iWidth;	
}

void TablePaneView::OnInitialUpdate() 
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
	TableDoc* td = GetDocument();
	if (0 == td)
		return;	

	BaseTablePaneView::OnInitialUpdate();
	if ( td->table()->fUseAs())
	{
		Menu men(GetTopLevelFrame());
		String sItem;
		String sIt("%S,%S", ILWSF("men",ID_MEN_FILE), ILWSF("men",ID_FILE_SAVE_AS));
		for(unsigned int i=0; i < sIt.size(); ++i)
			if ( sIt[i] != '&')
				sItem += sIt[i];

		men.InsertMenuItem(ID_FILE_SAVE_COPY_AS, ID_FILE_PRINT);
		men.DeleteMenuItem(sItem);
	}		
	td->table()->LoadData();		
}

bool TablePaneView::fAllowMoveCol() const
{
	return true;
}

void TablePaneView::MoveCol(int iFrom, int iTo)
{
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	td->MoveCol(iFrom, iTo);
}


void TablePaneView::OnGotoColumn()
{
	class Form: public FormWithDest
	{
	public:
		Form(CWnd* parent, TableView* tvw, String* sCol)
			: FormWithDest(parent, STBTitleGotoCol)
		{
			new FieldBlank(root);
			new FieldColumn(root, STBUiCol, tvw, sCol);
			SetMenHelpTopic(htpTblGotoCol);
			create();
		}
	};
	int iCurr = iFirstVisibleColumn();
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	String sCol = td->cv(iCurr)->sName();
	Form frm(this, td->tvw, &sCol);
	if (frm.fOkClicked()) {
		int iCol = td->tvw->iCol(sCol);
		horzPixMove(iCol - iCurr);
	}
}

void TablePaneView::OnULButtonPressed()
{
	TableWindow* tw = twParent();
	if (0 == tw)
		return;
	tw->SendMessage(WM_COMMAND, ID_COLUMNS, 0);
}

void TablePaneView::OnRowButtonPressed(long iRow)
{
	TableWindow* tw = twParent();
	if (0 == tw)
		return;
	long iRec = tvw()->iRec(iRow);
	if (iRec > tvw()->iRecs())
		return;
	Ilwis::Record rec = tvw()->rec(iRec);
	tw->CreateRecordBar();
	tw->recBar->view->SetRecord(rec, tvw());
	tw->ShowControlBar(tw->recBar,TRUE,FALSE);
}

void TablePaneView::OnColButtonPressed(int iCol)
{
	deleteField();
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	if (iCol < td->tvw->iCols()) {
		int iPos = iCol + 1;
		ColumnPropForm frm(this, td->tvw->cv(iCol), &iPos);
		if (frm.fOkClicked()) {
			td->SetColPos(iCol, iPos);
			td->UpdateAllViews(0);
		}
	}
	else 
		td->OnAddColumn();
}

void TablePaneView::OnFieldPressed(int iCol, long iRow, bool fLeft)
{
	deleteField();
	if (iCol < 0 || iCol >= iCols()
		|| iRow <= 0 || iRow > iRows()) 
	{
		MessageBeep(MB_ICONASTERISK);
		return;
	}

	long iRec = tvw()->iRec(iRow);
	Ilwis::Record rec = tvw()->rec(iRec);
	TableWindow* tw = twParent();
	if (tw) {
		if (0 != tw->recBar && 0 != tw->recBar->GetSafeHwnd()) {
			tw->recBar->view->SetRecord(rec, tvw());
			tw->recBar->Invalidate();
		}
	}

	if (fLeft) {
		FrameWindow* fw = fwParent();
		TableDoc* td = GetDocument();
		if (0 == td)
			return;
		if (td->tvw->fEditable(iCol)) {
			tField = new TblField(this,iCol,iRow);
			String s(STBRemEditField);
			if (fw) 
				fw->status->SetWindowText(s.scVal());
		}  
		else {
			tField = new TblReadOnlyField(this,iCol,iRow);  
			String s(STBRemFieldIsReadOnly);
			if (fw) 
				fw->status->SetWindowText(s.scVal());
		}  
	}    
}

void TablePaneView::OnEditClear()
{
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	if (mmSelect.MinRow() < 0) {
		long cMin = mmSelect.MinCol();
		if (cMin < 0) cMin = 0;
		for (long c = mmSelect.MaxCol(); c >= cMin; --c)
			if (!td->fDelColumn(c))
				break;
		td->UpdateAllViews(0);
		td->tvw->Updated();
	}
	else if (0 != td->tvw->dm()->pdnone() && mmSelect.MinCol() < 0) {
		int iRet = MessageBox(STBMsgDelSelRows.sVal(), STBMsgDelRows.sVal(),
			MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (IDYES == iRet) 
		{
			long iMin = mmSelect.MinRow();
			if (iMin < 0) iMin = 0;
			long iMax = mmSelect.MaxRow();
			long iNr = iMax - iMin + 1;
			if (iNr > 0) {
				CWaitCursor cur;
				td->tvw->DeleteRec(iMin, iNr);
			}	
			td->UpdateAllViews(0);
			td->tvw->Updated();
		}
	}
	else {
		long iMinCol = mmSelect.MinCol();
		long iMaxCol = mmSelect.MaxCol();
		iMaxCol = min(iMaxCol, td->tvw->iCols()-1);
		if (iMinCol > iMaxCol)
			return;
		int iRet = MessageBox(STBMsgClearSelFields.sVal(), STBMsgClearFields.sVal(),
			MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (IDYES == iRet) {
			CWaitCursor cur;
			long iMaxRow = mmSelect.MaxRow();
			for (long c = iMinCol; c <= iMaxCol; ++c) {
				Column col = td->tvw->cv(c);
				if (col.fValid() && !col->fDataReadOnly()) {
					for (long r = mmSelect.MinRow(); r <= iMaxRow; ++r)
						td->tvw->PutVal((short)c,r,sUNDEF);
					col->Updated();
				}
			}    
			td->UpdateAllViews(0);
			td->tvw->Updated();
		}
	}
	BaseTablePaneView::OnEditClear();
}

void TablePaneView::OnEditPaste()
{
	if (!IsClipboardFormatAvailable(CF_TEXT))
		return;
	CWaitCursor wc;
	TableDoc* td = GetDocument();
	if (0 == td)
		return;
	Column col = td->tvw->cvKey();
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

	if (td->tvw->dm()->pdnone() && mmSelect.fUndef()) {
		mmSelect.MinCol() = -1;
		mmSelect.MaxCol() = iCols();
		mmSelect.MinRow() = iRows() + td->tvw->iOffset();
		s = sTextBegin;
		long iAdd = 0;
		while (s) {
			s = strchr(s,'\n');
			if (s) {
				++s;
				++iAdd;
			}
		}
		td->tvw->iRecNew(iAdd);
		mmSelect.MaxRow() = iRows() - 1 + td->tvw->iOffset();;
	}
	str = sTextBegin;
	r = mmSelect.MinRow();
	if (r < td->tvw->iOffset())
		r = td->tvw->iOffset();
	//	if (fColHeader)
	//		++r;
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
			if (0 == col.ptr() && c < 0)
				c = 0;
			if (c < 0) {
				if (!col->fDataReadOnly()) {
					col->PutVal(r,str);
					col->Updated();
				}
				c = -1;
			}
			else {
				ColumnView& cv = td->tvw->cv(c);
				if (cv.fValid() && !cv->fDataReadOnly()) {
					td->tvw->PutVal((short)c,r,str);
					if (td->tvw->cvKey().fValid())
						if (!fResortAfterPaste && td->tvw->cvKey() == cv)
							fResortAfterPaste = true;
				}
			}
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
		if (*s == '\n')      
			++s;
		if (*s == '\0')
			break;
		str = s;
	}
	delete sText;
	if (fResortAfterPaste)
		td->tvw->ApplySorting();
	td->UpdateAllViews(0);
}


/*
TableWindow* TabkePaneView::twParent()
{
CFrameWnd* fw = GetTopLevelFrame();
return dynamic_cast<TableWindow*>(fw);
}
*/

// TblField //////////////////////////////////////////////////////////////////

TblField::TblField(TablePaneView* pane, int col, long row)
: BaseTblField(pane,col,row,pane->tvw()->cv(col)->dm())
{
	String str = pane->tvw()->sValue(col,row,0);
	init(str);
}

TblField::~TblField()
{
	if (fHasChanged()) 
	{
		String s = sText();
		if ("" == s)
			s = "?";
		TablePaneView* pane = (TablePaneView*) tbpn;
		DomainValueRangeStruct dvs = pane->tvw()->cv(iCol)->dvrs();
		if (("?" != s) && !dvs.fValid(s)) {
			DomainSort* ds = dvs.dm()->pdsrt();
			if (0 != ds) {
				String sMsg(STBMsgNotInDomain_SS.sVal(), s, ds->sName());
				int iRet = pane->MessageBox(sMsg.sVal(), STBMsgInvalidValue.sVal(),
					MB_YESNO|MB_ICONEXCLAMATION);
				if (IDNO == iRet)
				{
					init(pane->tvw()->sValue(iCol,iRow));
					return;
				}
				if (IDYES == iRet)
					ds->iAdd(s);
			}
			else {
				String sMsg(STBMsgInvalidValue_S.sVal(), s);
				pane->MessageBox(sMsg.sVal(), STBMsgInvalidValue.sVal(),
					MB_OK|MB_ICONEXCLAMATION);
				return;
			}
		}
		pane->tvw()->PutVal(iCol,iRow,s);
		pane->tvw()->Updated();
		pane->GetDocument()->UpdateAllViews(0);
	}
}

// TblReadOnlyField ////////////////////////////////////////////////

TblReadOnlyField::TblReadOnlyField(TablePaneView* pane, int col, long row)
: BaseTblField(pane,col,row,true)
{
	String str = pane->tvw()->sValue(col,row,0);
	init(str);
}

TblReadOnlyField::~TblReadOnlyField()
{
	// never changes ;-)
}


// EditMultiRecForm

class EditMultiRecForm: public FormWithDest
{
public:
	EditMultiRecForm(CWnd* parent, TableView* tvw, int col, long rowMin, long rowMax)
		: FormWithDest(parent, "Edit"),
		view(tvw), iCol(col), iRowMin(rowMin), iRowMax(rowMax)
	{
		if (iRowMin <= 0)
			iRowMin = 1;
		sVal = view->sValue(iCol,iRowMin,0);
		Column cl = view->col(iCol);
		if (!cl.fValid())
			return;
		String sCol = cl->sName();
		if (cl->fDataReadOnly()) {
			String str(STBRemColIsReadOnly_S.scVal(), sCol);
			new StaticText(root, str);
			fbs |= fbsNOOKBUTTON;
		}
		else {
			String s(STBRemEditRecord_S.scVal(), view->sRow(iRowMin));
			StaticText* st = new StaticText(root, s);
			st->SetIndependentPos();
			if (iRowMax > iRowMin) {
				st->psn->SetBound(0,0,0,0);
				s = String(STBRemUntilRecord_S.scVal(), view->sRow(iRowMax));
				st = new StaticText(root, s);
				st->SetIndependentPos();
			}
			new FieldVal(root, sCol, cl->dvrs(), &sVal);
		}
		new FieldBlank(root);
		SetHelpTopic(htpTblEditMultiRecords);
		create();
	}
private:
	int exec() {
		FormWithDest::exec();
		for (long r = iRowMin; r <= iRowMax; ++r)
			view->PutVal(iCol,r,sVal);
		return 1;
	}
	int iCol;
	long iRowMin, iRowMax;
	TableView* view;
	String sVal;
};

class EditMultiFieldForm: public FormWithDest
{
public:
	EditMultiFieldForm(CWnd* parent, TableView* tvw, int colMin, int colMax, long row)
		: FormWithDest(parent, "Edit"),
		view(tvw), iColMin(colMin), iColMax(colMax), iRow(row)
	{
		String s(STBRemEditRecord_S.scVal(), view->sRow(iRow));
		StaticText* st = new StaticText(root, s);
		st->SetIndependentPos();

		int iCol = iColMin;
		for (int i = 0; i < 15 && iCol <= iColMax; i++, iCol++) {
			Column col = view->col(iCol);
			if (!col.fValid())
				continue;
			s = col->sName();
			sVal[i] = view->sValue(iCol,iRow,0);
			if (col->fDataReadOnly()) {
				StaticText* st = new StaticText(root, s);
				StaticText* stVal = new StaticText(root, sVal[i]);
				//        st->psn->SetBound(0,0,0,0);
				//        stVal->psn->SetBound(0,0,0,0);
				stVal->Align(st, AL_AFTER);
				FieldBlank* fb = new FieldBlank(root, 0);
				fb->Align(st, AL_UNDER);
			}
			else {
				FieldVal* fval = new FieldVal(root, s, col->dvrs(), &sVal[i]);
				if (!col->fValues()) { // this is a trick, because Fieldval makes too high value entries
					fval->psn->SetBound(0,0,2,2);
					FieldBlank* fb = new FieldBlank(root, 0);
				}
			}
		}
		SetHelpTopic(htpTblEditMultiFields);
		create();
	}
private:
	int exec() {
		FormWithDest::exec();
		int iCol = iColMin;
		for (int i = 0; i < 15 && iCol <= iColMax; i++, iCol++) {
			Column col = view->cv(iCol);
			if (col.fValid())
				col->PutVal(iRow,sVal[i]);
		}
		return 1;
	}
	int iColMin, iColMax;
	long iRow;
	TableView* view;
	String sVal[15];
};


void TablePaneView::OnEdit()
{
	deleteField();
	int iMinRow = mmSelect.MinRow();
	int iMaxRow = mmSelect.MaxRow();
	int iMinCol = mmSelect.MinCol();
	int iMaxCol = mmSelect.MaxCol();
	TableView* tv = tvw();
	TableDoc* td = GetDocument();
	if (iMaxCol >= tv->iCols())
		iMaxCol = tv->iCols() - 1;
	if (iMaxRow > tv->iRecs())
		iMaxRow = tv->iRecs();
	if (iMinCol == iMaxCol) {
		EditMultiRecForm frm(this, tv, iMinCol, iMinRow, iMaxRow);
		if (frm.fOkClicked()) {
			/*
			zRect rect = rectSelect();
			InvalidateRect(&rect);
			*/
			td->UpdateAllViews(0);
			td->tvw->Updated();
		}
	}
	else if (iMaxCol > iMinCol) {
		MinMax mm;
		mm.MinCol() = iMinCol;
		mm.MaxCol() = iMaxCol;
		for (int r = iMinRow; r <= iMaxRow; ++r) {
			EditMultiFieldForm frm(this, tv, iMinCol, iMaxCol, r);
			if (frm.fOkClicked()) {
				/*
				mm.MinRow() = r;
				mm.MaxRow() = r;
				zRect rect = rectBlock(mm);
				InvalidateRect(&rect);
				*/
				td->UpdateAllViews(0);
				td->tvw->Updated();
			}
			else
				break;
		}
	}
}

void TablePaneView::OnUpdateEdit(CCmdUI* pCmdUI)
{
	int iMinCol = mmSelect.MinCol();
	int iMaxCol = mmSelect.MaxCol();
	bool fEdit = iMaxCol >= iMinCol;
	pCmdUI->Enable(fEdit);
}

void TablePaneView::OnEditField()
{
	long iRec = iFirstVisibleRow();
	int iCol = iFirstVisibleColumn();
	PostMessage(ILW_GOTOFIELD,iCol,iRec);
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);

void TablePaneView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	deleteField();
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

	if ((iSelectedColumn() >= 0) && 0 != twParent() /* not for histograms at the moment*/ ) { 
		addBreak;
		add(ID_SORT_ASCENDING);
		add(ID_SORT_DESCENDING);
		addBreak;
		add(ID_PROP);
	}
	men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void TablePaneView::OnSortOn()
{
	deleteField();
	if (0 == twParent()) // histogram
		return;
	String sCmd("clmsort");
	twParent()->fExecute(sCmd);
}

void TablePaneView::OnSortOnAscending()
{
	deleteField();
	if (0 == twParent()) // histogram
		return;
	String sCol = "";
	int iCol = iSelectedColumn();
	if (iCol != iUNDEF) {
		ColumnView& cv = tvw()->cv(iCol);
		if (cv.fValid())
			sCol = cv->sName().sQuote();
	}
	String sCmd("sorton %S -sortorder=ascending", sCol);
	twParent()->fExecute(sCmd);
}

void TablePaneView::OnSortOnDescending()
{
	deleteField();
	if (0 == twParent()) // histogram
		return;
	String sCol = "";
	int iCol = iSelectedColumn();
	if (iCol != iUNDEF) {
		ColumnView& cv = tvw()->cv(iCol);
		if (cv.fValid())
			sCol = cv->sName().sQuote();
	}
	String sCmd("sorton %S -sortorder=descending", sCol);
	twParent()->fExecute(sCmd);
}

void TablePaneView::OnUpdateAllColumns()
{
	deleteField();
	if (0 == twParent()) // histogram
		return;
	twParent()->fExecute("updateallcolumns");
}

TableWindow* TablePaneView::twParent()
{
	CFrameWnd* fw = GetTopLevelFrame();
	return dynamic_cast<TableWindow*>(fw);
}

void TablePaneView::OnProp()
{
	deleteField();
	int iCol = iSelectedColumn();
	if (iCol != iUNDEF) 
		OnColButtonPressed(iCol);
}


int TablePaneView::iCurrRec() const
{
	if (mmSelect.MinRow() == mmSelect.MaxRow())
		return mmSelect.MinRow();
	return iUNDEF;
}

void TablePaneView::OnUpdateProp(CCmdUI* pCmdUI)
{
	bool f = iSelectedColumn() != iUNDEF;
	pCmdUI->Enable(f);
}


String TablePaneView::sPrintTitle() const
{
	return tvw()->sTypeName();
}

bool TablePaneView::fAllowPaste() const
{
	if (tvw()->fReadOnly())
		return false;
	if (fValidSelection())
		return true;
	if (tvw()->dm()->pdnone() && mmSelect.fUndef()) 
		return true;
	return false;
}

