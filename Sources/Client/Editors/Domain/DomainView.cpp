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
// DomainView.cpp : implementation file
//
#include "Client\Headers\formelementspch.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"

#include "Client\Editors\Domain\classItem.h"
#include "Engine\Domain\dmsort.h"  // for error class only
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Domain\DomDoc.h"
#include "Client\Editors\Domain\DomainSortDoc.h"
#include "Client\Editors\Domain\DomainSortListCtrl.h"
#include "Client\Editors\Domain\DomainView.h"
#include "Client\FormElements\formbase.h"
#include "winuser.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Headers\Hs\DAT.hs"
#include "Headers\Hs\DOMAIN.hs"
#include "Headers\stdafx.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\objlist.h"

using namespace std;

const COLORREF clrMask = RGB(192, 192, 192);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DomainSortView::MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}

/////////////////////////////////////////////////////////////////////////////
// DomainSortView

IMPLEMENT_DYNCREATE(DomainSortView, BaseView2)

BEGIN_MESSAGE_MAP(DomainSortView, BaseView2)
	//{{AFX_MSG_MAP(DomainSortView)
	ON_COMMAND(ID_DOMEDIT,			OnEdit)
	ON_COMMAND(ID_DOMADD,			OnAdd)
	ON_COMMAND(ID_DOMDEL,			OnDelete)
	ON_COMMAND(ID_CLEAR,			OnDelete)
	ON_COMMAND(ID_DOMMERGE,			OnMerge)
	ON_COMMAND(ID_DOMPREFIX,		OnChangePrefix)
	ON_COMMAND(ID_DOMSRTALPH,		OnAlphaSort)
	ON_COMMAND(ID_DOMSRTMAN,		OnManualSort)
	ON_COMMAND(ID_DOMSRTALPHNUM,	OnAlphaNumSort)
	ON_COMMAND(ID_DOMSRTCODEALPH,	OnCodeAlphaSort)
	ON_COMMAND(ID_DOMSRTCODEALNUM,	OnCodeAlphaNumSort)
//	ON_COMMAND(ID_EDIT_CUT,			OnCut)
	ON_COMMAND(ID_EDIT_COPY,		OnCopy)
	ON_COMMAND(ID_EDIT_PASTE,		OnPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL,	OnSelectAll)
	ON_COMMAND(ID_DESCRIPTIONBAR,	OnDescriptionBar)
	ON_COMMAND(ID_DOMSORTBUTTONBAR,	OnButtonBar)
	ON_UPDATE_COMMAND_UI(ID_DOMEDIT,			OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_DOMADD,				OnUpdateAdd)
	ON_UPDATE_COMMAND_UI(ID_DOMMERGE,			OnUpdateMerge)
	ON_UPDATE_COMMAND_UI(ID_DOMPREFIX,			OnUpdateChangePrefix)
	ON_UPDATE_COMMAND_UI(ID_DOMSRTMAN,			OnUpdateSortType)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT,			OnUpdateAdd)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE,			OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_DESCRIPTIONBAR,		OnUpdateDescriptionBar)
	ON_UPDATE_COMMAND_UI(ID_DOMSORTBUTTONBAR,	OnUpdateDescriptionBar)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCHANGING, 0, OnHeaderSize)
	ON_NOTIFY(HDN_ENDTRACK,	0, OnHeaderTrack)
	ON_NOTIFY(LVN_GETDISPINFO,    AFX_IDW_PANE_FIRST, OnGetDispInfo)
	ON_NOTIFY(LVN_BEGINLABELEDIT, AFX_IDW_PANE_FIRST, OnBeginLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT,	  AFX_IDW_PANE_FIRST, OnEndLabelEdit)
	ON_NOTIFY(LVN_BEGINDRAG,	  AFX_IDW_PANE_FIRST, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_FILE_PRINT,				OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,		OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_DOMDEL,				OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_DOMSRTALPH,			OnUpdateSortType)
	ON_UPDATE_COMMAND_UI(ID_DOMSRTALPHNUM,		OnUpdateSortType)
	ON_UPDATE_COMMAND_UI(ID_DOMSRTCODEALPH,		OnUpdateSortType)
	ON_UPDATE_COMMAND_UI(ID_DOMSRTCODEALNUM,	OnUpdateSortType)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY,			OnUpdateCopy)
	ON_WM_CREATE()
	ON_MESSAGE(ILWM_DESCRIPTIONTEXT,			OnDescriptionChanged)
	ON_MESSAGE(ILWM_VIEWSETTINGS,               OnViewSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////
//	DomainSortView Controls and Forms used by the DomainEditor

class EditDomainItemForm: public FormWithDest
{
public:
	EditDomainItemForm(CWnd* wPar, const String& sTitle, const ClassItem& gli, const String& help)
		: FormWithDest(wPar, sTitle)
	{
		fInCallBack = false;
		if (gli.sName().length() != 0)
			SetItem(gli);

		fsName = new FieldString(root, TR("&Name"), &sName, Domain(), false);
		fsName->SetCallBack((NotifyProc)&EditDomainItemForm::CheckEmpty);
		fsCode = new FieldString(root, TR("&Code"), &sCode, Domain());
		fsCode->SetCallBack((NotifyProc)&EditDomainItemForm::CheckDuplicate);
		StaticText* st = new StaticText(root, TR("&Description:"));
		st->SetIndependentPos();
		st->psn->SetBound(0,0,0,0);

		FieldString* fs = new FieldString(root, "", &sDescr, Domain());
		fs->SetIndependentPos();
		fs->SetWidth(120);

		String sRemark('X', 30);
		stRemark = new StaticText(root, sRemark);
		stRemark->SetIndependentPos();
		stRemark->SetVal("");
		SetMenHelpTopic(help);
		create();
	}
	FormEntry* feDefaultFocus()
	{
		fsName->SelectAll();
		return fsName;
	}
	ClassItem cliItem() {
		return ClassItem(sName.sTrimSpaces(), sCode.sTrimSpaces(), sDescr.sTrimSpaces());
	}
	void SetItem(const ClassItem& gli) {
		sName = gli.sName().sTrimSpaces();
		sCode = gli.sCode().sTrimSpaces();
		sDescr = gli.sDescription().sTrimSpaces();
	}
private:
	int CheckEmpty(Event*)
	{
		if (fInCallBack)
			return 0;

		fsName->StoreData();
		sName = sName.sTrimSpaces();
		fIsEmpty = sName == "";
		if (fIsEmpty)
		{
			fInCallBack = true;
			fsName->SetVal(sName);
			fInCallBack = false;
			stRemark->SetVal("");
			DisableOK();
		}
		else
		{
			CheckDuplicate(0);
			if (!fIsDuplicate) EnableOK();
		}

		return 0;
	}
	int CheckDuplicate(Event*)
	{
		fsName->StoreData();
		fsCode->StoreData();
		sName = sName.sTrimSpaces();
		sCode = sCode.sTrimSpaces();
		if (sName.length() == 0 && sCode.length() == 0)
			return 0;
		
		fIsDuplicate = fCIStrEqual(sCode, sName);
		if (fIsDuplicate)
		{
			stRemark->SetVal(String(TR("%S name and Code should be different").c_str(), String("Class/ID")));
			DisableOK();
		}
		else
		{
			stRemark->SetVal("");
			if (!fIsEmpty) EnableOK();
		}
		return 0;
	}
	FieldString  *fsName, *fsCode;
	StaticText   *stRemark;
	String       sName, sCode, sDescr;
	bool         fInCallBack;
	bool         fIsEmpty, fIsDuplicate;
};

class EditDomainGroupItemForm: public FormWithDest
{
public:
	EditDomainGroupItemForm(CWnd* wPar, const String& sTitle, const GroupItem& gli, const String& htp)
		: FormWithDest(wPar, sTitle)
	{
		if (gli.sName().length() != 0)
			SetItem(gli);
		else
			rVal = rUNDEF;

		frBound = new FieldReal(root, TR("&Upper Bound"), &rVal);
		fsName = new FieldString(root, TR("&Name"), &sName, Domain(), false);
		fsName->SetCallBack((NotifyProc)&EditDomainGroupItemForm::CheckDuplicate);
		fsCode = new FieldString(root, TR("&Code"), &sCode, Domain());
		fsCode->SetCallBack((NotifyProc)&EditDomainGroupItemForm::CheckDuplicate);
		StaticText* st = new StaticText(root, TR("&Description:"));
		st->SetIndependentPos();
		st->psn->SetBound(0,0,0,0);

		FieldString* fs = new FieldString(root, "", &sDescr, Domain());
		fs->SetIndependentPos();
		fs->SetWidth(120);

		String sRemark('X', 30);
		stRemark = new StaticText(root, sRemark);
		stRemark->SetIndependentPos();
		stRemark->SetVal("");
		
		SetMenHelpTopic(htp);
		create();
	}
	FormEntry* feDefaultFocus()
	{
		return frBound;
	}
	GroupItem cliItem() {
		return GroupItem(sName.sTrimSpaces(), sCode.sTrimSpaces(), sDescr.sTrimSpaces(), rVal);
	}
	void SetItem(const GroupItem& gli) {
		sName = gli.sName().sTrimSpaces();
		sCode = gli.sCode().sTrimSpaces();
		sDescr = gli.sDescription().sTrimSpaces();
		rVal = gli.rBound();
	}
	int CheckDuplicate(Event*)
	{
		fsName->StoreData();
		fsCode->StoreData();
		sName = sName.sTrimSpaces();
		sCode = sCode.sTrimSpaces();
		if (sName.length() == 0 && sCode.length() == 0)
			return 0;

		fIsEmpty = sName.length() == 0;
		fIsDuplicate = fCIStrEqual(sCode, sName);
		if (fIsDuplicate)
		{
			stRemark->SetVal(String(TR("%S name and Code should be different").c_str(), String("Group")));
			DisableOK();
		}
		else
		{
			stRemark->SetVal("");
			if (!fIsEmpty) EnableOK();
			else DisableOK();
		}
		return 0;
	}
private:
	FieldReal    *frBound;
	FieldString  *fsName, *fsCode;
	StaticText   *stRemark;
	String       sName, sCode, sDescr;
	double       rVal;
	bool         fIsDuplicate, fIsEmpty;
};

class EditDomainMultiForm: public FormWithDest
{
public:
	EditDomainMultiForm(CWnd* wPar, const String& sTitle, 
		String* sDescr, int iSelCount, const String& htp)
		: FormWithDest(wPar, sTitle)
	{
		new StaticText(root, String(TR("%i items are selected").c_str(), iSelCount));
		StaticText* st = new StaticText(root, TR("&Description:"));
		st->SetIndependentPos();
		st->psn->SetBound(0,0,0,0);

		fsDescr = new FieldString(root, "", sDescr, Domain());
		fsDescr->SetIndependentPos();
		fsDescr->SetWidth(120);

		SetMenHelpTopic(htp);
		create();
	}
	FormEntry* feDefaultFocus()
	{
		fsDescr->SelectAll();
		return fsDescr;
	}
private:
	FieldString* fsDescr;  
};

class MergeDomainForm: public FormWithDest 
{
public:
	MergeDomainForm(CWnd* wPar, String* sDomain)
		: FormWithDest(wPar, TR("Merge Domain"))
	{
		new FieldDataType(root, TR("&Domain"), sDomain, new DomainLister(dmCLASS|dmIDENT), true);
		SetMenHelpTopic("ilwismen\\domain_class_id_editor_merge_domains.htm");
		create();
	}
};

class ChangePrefixForm: public FormWithDest 
{
public:
	ChangePrefixForm(CWnd* wPar, const String& sPrefix)
		: FormWithDest(wPar, TR("Change Prefix"))
	{
		m_sPrefix = sPrefix;

		fsPrefix = new FieldString(root, TR("&Prefix"), &m_sPrefix, Domain());
		fsPrefix->SetIndependentPos();
		fsPrefix->SetWidth(120);
		fsPrefix->SetCallBack((NotifyProc)&ChangePrefixForm::CheckPrefix);
		String sRem('x', 50);
		stRemarks = new StaticText(root, sRem);
		SetMenHelpTopic("ilwismen\\domain_class_id_editor_change_prefix.htm");
		create();
	}
	String sGetPrefix()
	{
		return m_sPrefix;
	}
private:
	FieldString* fsPrefix;
	StaticText* stRemarks;
	String m_sPrefix;
	int CheckPrefix(Event*)
	{
		fsPrefix->StoreData();
		if (m_sPrefix.find(':') != string::npos)
		{
			String sRemark = String(TR("The '%c' character is not allowed in the prefix").c_str(), ':');
			stRemarks->SetVal(sRemark);
			DisableOK();
		}
		else if (m_sPrefix.length() == 0)
		{
			stRemarks->SetVal("");
			DisableOK();
		}
		else
		{
			stRemarks->SetVal("");
			EnableOK();
		}
		return 0;
	}
};

//////////////////////////////////////////////
// DomainSortView

DomainSortView::DomainSortView()
{
    m_psPool[0] =
	m_psPool[1] =
	m_psPool[2] = NULL;
	m_iNextFree = 0;

// Initialize drag/drop
	m_pDragImage = new CImageList();
	m_fDragging = false;
	m_fAbortDrag = false;
}

DomainSortView::~DomainSortView()
{
	if (m_pDragImage != 0)
		delete m_pDragImage;
}

BOOL DomainSortView::PreCreateWindow(CREATESTRUCT& cs)
{
	return BaseView2::PreCreateWindow(cs);
}

CListCtrl& DomainSortView::GetListCtrl()
{
	return m_ListCtrl;
}

CHeaderCtrl& DomainSortView::GetHeaderCtrl()
{
	return m_HeaderCtrl;
}

/////////////////////////////////////////////////////////////////////////////
// DomainSortView drawing

void DomainSortView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// DomainSortView diagnostics

#ifdef _DEBUG
void DomainSortView::AssertValid() const
{
	BaseView2::AssertValid();
}

void DomainSortView::Dump(CDumpContext& dc) const
{
	BaseView2::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DomainSortView message handlers

DomainSortDoc* DomainSortView::GetDocument()
{
    return (DomainSortDoc*)m_pDocument;
}

/*
	OnInitialUpdate() is shared for  Class/ID and Group domains and does the following:
	- Add a menu (via the BuildMenu() function)
	- Add a Description bar
	- Setup the listview in report mode and add the columns
	- Restore the window size from a previous session (global for all domain editors)
*/
void DomainSortView::OnInitialUpdate() 
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	AddToolBars();

	DWORD defstyle = WS_CHILD|WS_VISIBLE | WS_HSCROLL;
	DWORD style = LVS_REPORT | LVS_EDITLABELS | LVS_OWNERDATA;
	style &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	m_ListCtrl.Create(defstyle | style, CRect(0,0,260,260), this, AFX_IDW_PANE_FIRST);
	m_ListCtrl.SetParentView(this);

	BaseView2::OnInitialUpdate();

	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CFont *fnt = const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));

	lvCtrl.SetFont(fnt);

	BuildMenu();

	FrameWindow *frmw = static_cast<FrameWindow *>(GetParentFrame());
	frmw->SetAcceleratorTable();

	if (pdocDom->fIsGroup())
		frmw->SetWindowName("DomainGroupEditor");
	else if (pdocDom->fIsID())
		frmw->SetWindowName("DomainIDEditor");
	else if (pdocDom->fIsClass())
		frmw->SetWindowName("DomainClassEditor");

	m_HeaderCtrl.SubclassWindow(::GetDlgItem(m_ListCtrl.m_hWnd, 0) );
//	m_HeaderCtrl.SendMessage(HDM_SETUNICODEFORMAT, FALSE);
	BuildColumns();

	if (pdocDom->iCount() > 0)
		SetSelectedItem(0, true); // Select the first item always
}

void DomainSortView::AddToolBars()
{
	CFrameWnd *frm = GetParentFrame();

	m_bbTools.Create(frm, "dmsedit.but", TR("Domain editor").c_str(), ID_DOMSORTBUTTONBAR);

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);
	BOOL fRet = descBar.Create(frm, pdocDom);
	ISTRUE(fINotEqual, fRet, FALSE);

	descBar.SetReadOnly(pdocDom->fReadOnly());
	frm->EnableDocking(CBRS_ALIGN_ANY);
	frm->DockControlBar(&descBar);
	frm->DockControlBar(&m_bbTools, AFX_IDW_DOCKBAR_TOP);
}

void DomainSortView::BuildColumns()
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	int iCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

	int iTotalWidth = 0;
	int iCurCol = 0;
	CSize isHeader;
	CString sCurrent;

	if (pdocDom->fIsClass())
		sCurrent = TR("Class Name").c_str();
	else
		sCurrent = TR("ID Name").c_str();
	CString sDummy('x', 30);  // used to set the class column to a fix size

	HDITEM hdi;
	isHeader = lvCtrl.GetStringWidth(sDummy);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT | HDF_OWNERDRAW;
	hdi.cxy = iCheckWidth + isHeader.cx;
	hdi.pszText = sCurrent.GetBuffer(sCurrent.GetLength());
	hdi.cchTextMax = sCurrent.GetLength();
	hdi.fmt = HDF_LEFT;
	m_HeaderCtrl.SetItem(iCurCol, &hdi);

	iTotalWidth += iCheckWidth + isHeader.cx;

	sCurrent = TR("Code").c_str(); 
	sDummy = CString('x', 10);
	isHeader = lvCtrl.GetStringWidth(sDummy);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT | HDF_OWNERDRAW;
	hdi.cxy = iCheckWidth + isHeader.cx;
	hdi.pszText = sCurrent.GetBuffer(sCurrent.GetLength());
	hdi.cchTextMax = sCurrent.GetLength();
	hdi.fmt = HDF_LEFT;
	m_HeaderCtrl.SetItem(iCurCol, &hdi);

	iTotalWidth += iCheckWidth + isHeader.cx;

	sCurrent = TR("Description").c_str(); 
	isHeader = lvCtrl.GetStringWidth(sCurrent);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx * 4);
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT;
	hdi.cxy = iCheckWidth + isHeader.cx * 4;
	hdi.pszText = sCurrent.GetBuffer(sCurrent.GetLength());
	hdi.cchTextMax = sCurrent.GetLength();
	hdi.fmt = HDF_LEFT;
	m_HeaderCtrl.SetItem(iCurCol, &hdi);

	iTotalWidth += iCheckWidth + isHeader.cx * 4;

	CalcSetDefaultSize(iTotalWidth);
	UpdateHeaderImages();
}

void DomainSortView::CalcSetDefaultSize(int iTotWidth)
{
	//CWindowDC dc(this);
  CRect rectWorkArea;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);
  int iH = rectWorkArea.bottom; 
  int iW = rectWorkArea.right; 
	//int iW = dc.GetDeviceCaps(HORZRES);
	//int iH = dc.GetDeviceCaps(VERTRES);

	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	CRect rect;
	int  iItemHeight = 16;
	if (lvCtrl.GetItemRect(0, rect, LVIR_BOUNDS))
		iItemHeight = rect.Height();

	int iCalcH = (pdocDom->iCount() + 1) * iItemHeight;  // +1 to have some breathing space
	if (iCalcH > iH * 0.3)
		iCalcH = (int)(min(iH * 0.35, iCalcH));
	else
		iCalcH = (int)(min(iCalcH, iH * 0.3));

	CRect rectClient, rectHeader;

	CFrameWnd *parentFrame = GetParentFrame();
	ISTRUE(fINotEqual, parentFrame, (CFrameWnd*)0);
	parentFrame->GetWindowRect(rect);

	m_HeaderCtrl.GetWindowRect(rectHeader);

	// Set default window size, in case no settings are stored in the registry
	GetClientRect(rectClient);
	rect.SetRect(rect.left, rect.top, 
		rect.left + rect.Width() - rectClient.Width() + iTotWidth,
		rect.top + rect.Height() - rectClient.Height() + iCalcH + rectHeader.Height());
	parentFrame->MoveWindow(rect);
}

void DomainSortView::BuildMenu()
{
//	AddToFileMenu(ID_FILE_PAGE_SETUP);
	AddToFileMenu(ID_FILE_SAVE_COPY_AS); // "Save Copy As", A
	AddToFileMenu(ID_DOMRPR);			// "Edit Representation", T
	AddToFileMenu(ID_PROP);				// "Properties", R
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_DOMMERGE);			// "Merge Domain", M
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_PRINT);		// "Print", P
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_CLOSE);

	AddToEditMenu(ID_EDIT_COPY);		// "Copy to clip"
	AddToEditMenu(ID_EDIT_PASTE);		// "Paste from clip"
	AddToEditMenu(ID_EDIT_SELECT_ALL);	// "Select all items"
	AddToEditMenu(DONT_CARE, MF_SEPARATOR);
	AddToEditMenu(ID_DOMADD);			// "Add &Item"
	AddToEditMenu(ID_DOMEDIT);			// "&Edit Item"
	AddToEditMenu(ID_DOMDEL);			// "&Delete Item"

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);
	if (pdocDom->fIsID() && pdocDom->sPrefix().length() > 0)
		AddToEditMenu(ID_DOMPREFIX);		// "Edit Prefix"

	AddToEditMenu(DONT_CARE, MF_SEPARATOR);
	AddToEditMenu(ID_DOMSRTALPHNUM);	// "Sort by Name"
	AddToEditMenu(ID_DOMSRTCODEALNUM);	// "Sort by Code"
	AddToEditMenu(ID_DOMSRTMAN);		// "Manual Sorting"

	AddToViewMenu(ID_DESCRIPTIONBAR);	// Show/Hide description bar
	AddToViewMenu(ID_DOMSORTBUTTONBAR);	// Show/Hide tool button bar
	AddToViewMenu(ID_STATUSLINE);		// Show/Hide status bar

  AddToHelpMenu(ID_HLPKEY);			// F1: Context help
  AddToHelpMenu(ID_HLPCONTENTS);	// F11: Contents
	AddToHelpMenu(DONT_CARE, MF_SEPARATOR);
	AddToHelpMenu(ID_APP_ABOUT);  //, "&About ILWIS...");
}

void DomainSortView::SaveState(IlwisSettings& settings)
{
	int iNrCols = m_HeaderCtrl.GetItemCount();
	if (iNrCols == 0)
		return;

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	CWindowDC dcScr(this);
	TEXTMETRIC tm;
	dcScr.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;

	CRect rectCol;
	for (int iCol = 0; iCol < iNrCols; iCol++)
	{
		HDITEM hdi;
		hdi.mask = HDI_WIDTH;
		m_HeaderCtrl.GetItem(iCol, &hdi);
		switch (iCol)
		{
			case 0:		settings.SetValue("Name", hdi.cxy / m_cxChar); break;
			case 1:		settings.SetValue("Code", hdi.cxy / m_cxChar); break;
			case 2:		settings.SetValue("Description", hdi.cxy / m_cxChar); break;
		}
	}
}

void DomainSortView::LoadState(IlwisSettings& settings)
{
	int iNrCols = m_HeaderCtrl.GetItemCount();
	if (iNrCols == 0)
		return;

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

    CWindowDC dcScr(this);
	TEXTMETRIC tm;
	dcScr.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;

	for (int iCol = 0; iCol < iNrCols; iCol++)
	{
		int iWid;
		switch (iCol)
		{
			case 0:		iWid = m_cxChar * settings.iValue("Name"); break;
			case 1:		iWid = m_cxChar * settings.iValue("Code"); break;
			case 2:		iWid = m_cxChar * settings.iValue("Description"); break;
		}
		if (iWid > 0)
		{
			HDITEM hdi;
			hdi.mask = HDI_WIDTH;
			hdi.cxy = iWid;
			m_HeaderCtrl.SetItem(iCol, &hdi);
		}
	}
	CRect rectCol;
	GetWindowRect(rectCol);

	OnSize(SIZE_RESTORED, rectCol.Width(), rectCol.Height());
}

void DomainSortView::OnEscape()
{
	if (m_fDragging) 
	{
		m_fAbortDrag = true;
		OnLButtonUp(0, CPoint(0, 0));
	}
	else
		OnUnSelectAll();
}

BOOL DomainSortView::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN )
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			OnEscape();
			return FALSE;
		}
	}
  return BaseView2::PreTranslateMessage(pMsg);
}

/*
	Clear() empties the List view totally
*/
void DomainSortView::Clear()
{
	CListCtrl& lvCtrl = GetListCtrl();
	lvCtrl.DeleteAllItems();

	int iColCount = m_HeaderCtrl.GetItemCount();
	int iIndex;

	for (iIndex = iColCount - 1; iIndex >= 0; iIndex--)
		m_HeaderCtrl.DeleteItem(iIndex);
}

/*
	OnUpdate()
	Refresh the contents of the ListView and of the Description bar
*/
void DomainSortView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CListCtrl& lvCtrl = GetListCtrl();
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);
	lvCtrl.SetItemCountEx(pdocDom->iCount(), LVSICF_NOSCROLL);

	SetDescription(pdocDom->sObjectDescription());
}

void DomainSortView::OnSetFocus( CWnd* wnd)
{
	GetListCtrl().SetFocus();
}

void DomainSortView::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl& lvCtrl = GetListCtrl();
	if (lvCtrl.m_hWnd != 0)
		lvCtrl.MoveWindow(0, 0, cx, cy);

	if (m_HeaderCtrl.m_hWnd != 0)
	{
		int iNrCols = m_HeaderCtrl.GetItemCount();
		if (iNrCols == 0)
			return;

		CRect rectCol;
		m_HeaderCtrl.GetItemRect(iNrCols - 1, &rectCol);

		lvCtrl.SetColumnWidth(iNrCols - 1, LVSCW_AUTOSIZE_USEHEADER);
	}
	
	BaseView2::OnSize(nType, cx, cy);
}

// margin is the difference between the window and the client areas
void DomainSortView::GetMinViewSize(CSize& size, const CSize& margin)
{
	int iNrCols = m_HeaderCtrl.GetItemCount();
	int iTotWidth = margin.cx;
	if (iNrCols != 0)
	{
		for (int iCol = 0; iCol < iNrCols - 1; iCol++)
		{
			HDITEM hdi;
			hdi.mask = HDI_WIDTH;
			m_HeaderCtrl.GetItem(iCol, &hdi);
			iTotWidth += hdi.cxy;
		}
	}
	 // allow descript column to be at least 50 pixels
	size.cx = min(max(260, iTotWidth + 50), GetSystemMetrics(SM_CXFULLSCREEN));

	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	CRect rect;
	int  iItemHeight = 16;
	if (lvCtrl.GetItemRect(0, rect, LVIR_BOUNDS))
		iItemHeight = rect.Height();

	int iCalcH = margin.cy + 10 * iItemHeight;
	size.cy = min(max(260, iCalcH), GetSystemMetrics(SM_CYFULLSCREEN));
}

// Garantee at least 50 pixels for the columns
void DomainSortView::OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* pnmh = (NMHEADER*)pNMHDR;

	if (pnmh->pitem->mask & HDI_WIDTH &&
				pnmh->pitem->cxy < 50 &&
				pnmh->pitem->cxy >= 0 )
		  pnmh->pitem->cxy = 50;
}

// Resize description column if any column in the listview is resized
void DomainSortView::OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* pnmh = (NMHEADER*)pNMHDR;

	if (pnmh->pitem->mask & HDI_WIDTH &&
				pnmh->pitem->cxy < 50 &&
				pnmh->pitem->cxy >= 0 )
		  pnmh->pitem->cxy = 50;

	if (m_HeaderCtrl.m_hWnd != 0)
	{
		int iNrCols = m_HeaderCtrl.GetItemCount();
		if (pnmh->iItem < iNrCols - 1)
	    GetListCtrl().SetColumnWidth(iNrCols - 1, LVSCW_AUTOSIZE_USEHEADER);
	}
}

void DomainSortView::UpdateHeaderImages()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);
	if (pdocDom->fIsGroup())
		return;	// No sorting alternatives for DomainGroup, so no images

	int iNrCols = m_HeaderCtrl.GetItemCount();
	if (iNrCols == 0)
		return;

	int iSortCol;
	if (pdocDom->fAlphaNumSorted())
		iSortCol = 0;
	else if (pdocDom->fCodeAlphaNumeric())
		iSortCol = 1;
	else 
		iSortCol = -1;  // remove images

	m_HeaderCtrl.SetSortImage(iSortCol);
}

void DomainSortView::OnHeaderClicked(NMHDR *pNMHDR, LRESULT *pResult)
{
    DomainSortDoc* pdocDom = GetDocument();
    ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	NMHEADER* pnmh = (NMHEADER*)pNMHDR;

	if (pnmh->iButton == 0)	// Left button
	{
		if (pdocDom->fIsGroup())
			return;	// No sorting alternatives for DomainGroup

		if (pnmh->iItem > 1)
			return;  // no sort necessary for decsription column

		if (!fWarnForSortManualChangeOK())
			return;

		if (pnmh->iItem == 0)	// Class/ID column
		{
			if (pdocDom->fAlphaNumSorted())
				return;  // already properly sorted

			pdocDom->SetSortAlphaNum();
			SortDomain();
		}
		else if (pnmh->iItem == 1)	// Code column
		{
			if (pdocDom->fCodeAlphaNumeric())
				return;  // already properly sorted

			pdocDom->SetSortCodeAlphaNum();
			SortDomain();
		}
		UpdateHeaderImages();
	}
}

void DomainSortView::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int id = lvCtrl.GetNextSelectedItem(pos);
	if (point.x == -1 && point.y == -1) 
	{
		CRect rect;
		lvCtrl.GetSubItemRect(id, 0, LVIR_LABEL, rect);
		point.x = (rect.right + rect.left) / 2;
		point.y = (rect.top + rect.bottom) / 2;
		ClientToScreen(&point);
	}
	if (id < 0)
		return;

	CMenu menu;
	menu.CreatePopupMenu();

	unsigned int uFlagsRO = 0;
	unsigned int uFlagsSelect = 0;
	if (pdocDom->fReadOnly())
		uFlagsRO = MF_GRAYED;
	if (lvCtrl.GetSelectedCount() == 0)
		uFlagsSelect = MF_GRAYED;

//	String sText(ILWSF("men", ID_EDIT_CUT));
//	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_EDIT_CUT, sText.c_str());

	String sText(ILWSF("men", ID_EDIT_COPY));
	menu.AppendMenu(MF_STRING | uFlagsSelect, ID_EDIT_COPY, sText.c_str());

	zClipboard cb(this);
	unsigned int uFlagsClip = cb.isTextAvail() == 0;

	sText = String(ILWSF("men", ID_EDIT_PASTE));
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsClip, ID_EDIT_PASTE, sText.c_str());

	sText = String(ILWSF("men", ID_EDIT_SELECT_ALL));
	menu.AppendMenu(MF_STRING, ID_EDIT_SELECT_ALL, sText.c_str());

	menu.AppendMenu(MF_SEPARATOR);

	sText = String(ILWSF("men", ID_DOMADD));
	menu.AppendMenu(MF_STRING | uFlagsRO, ID_DOMADD, sText.c_str());

	sText = String(ILWSF("men", ID_DOMEDIT));
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_DOMEDIT, sText.c_str());

	sText = String(ILWSF("men", ID_DOMDEL));
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_DOMDEL, sText.c_str());

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.Detach();
}

void DomainSortView::DoLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (!pdocDom->fReadOnly() && lvCtrl.GetSelectedCount() != 0)
		OnEdit();
}

// ListView items are zero-based; Domain items are one-based
void DomainSortView::OnEdit()
{
	try {
		CListCtrl& lvCtrl = GetListCtrl();

		int iSelected = lvCtrl.GetSelectedCount();
		if (iSelected == 0)
			return;
		else if (iSelected == 1)
		{
			POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
			int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
			DomainSortDoc* pdocDom = GetDocument();
			ASSERT_VALID(pdocDom);

			String sName = pdocDom->sName(iItem).sVal();
			String sCode = pdocDom->sCode(iItem).sVal();
			String sDescr = pdocDom->sDescription(iItem).sVal();

			ClassItem gliOrig(sName, sCode, sDescr);
			ClassItem gli(sName, sCode, sDescr);
			bool fEditReady = false;
			bool fOkClick = true;
			while (!fEditReady && fOkClick)
			{
				EditDomainItemForm frm(GetParent(), TR("Edit Domain Item"), gli, "ilwismen\\domain_class_id_editor_edit_item.htm");
				fOkClick = frm.fOkClicked();

				if (fOkClick) 
				{
					gli = frm.cliItem();
					if (gli.sName().length() == 0)   // sName must be filled in: empty classes not allowed
						return;

					if (!fCIStrEqual(gli.sName(), gliOrig.sName()) && !fCIStrEqual(gli.sCode(), gliOrig.sCode()))
						fEditReady = (pdocDom->fCanAdd(gli));  // check if exist to avoid adding duplicates
					else
						fEditReady = true;
					if (fEditReady)  // OK to change
					{
						pdocDom->SetItem(iItem, gli);
						lvCtrl.InvalidateRect(NULL);
						SetSelectedItem(iItem - 1, true);
					}
					else
					{
						String sIDClass = pdocDom->fIsID() ? "ID" : "Class";
						String sMsg = String(TR("%S or Code already in domain, cannot change, try again?").c_str(), sIDClass);
						fEditReady = IDYES != MessageBox(sMsg.c_str(), TR("Domain warning").c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON1);
					}
				}
			}
		}
		else {
			String sDescr;

			EditDomainMultiForm frm(GetParent(), TR("Edit Domain Items"), &sDescr, iSelected, "ilwismen\\domain_class_id_editor_edit_item.htm");
			if (!frm.fOkClicked())
				return;

			DomainSortDoc* pdocDom = GetDocument();
			ASSERT_VALID(pdocDom);

			// Collect the items to be editted. They are first stored here. The SetDescription() function
			// will update all views. This interferes with the selected items, that is the selection is changed!
			vector<int> vi;
			POSITION pos = lvCtrl.GetFirstSelectedItemPosition();

			while (pos != NULL)
				vi.push_back(1 + lvCtrl.GetNextSelectedItem(pos));

			// Now set the new description for all selected items
			vector<int>::iterator iter;
			for (iter = vi.begin(); iter != vi.end(); ++iter) 
				pdocDom->SetDescription(*iter, sDescr);

			// Re-enable the selection; this has to be done separately from the setting of the
			// description, because SetDescription resets the selection to the item it is settings 
			// the description of.
			for (iter = vi.begin(); iter != vi.end(); ++iter) 
				SetSelectedItem((*iter) - 1, true);
		}
		lvCtrl.Invalidate();
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}

/*
	OnBeginLabelEdit() 
	OnEndLabelEdit() 
	These functions contain the functionality
	for the in place editting of listview items. (in any column)
*/
void DomainSortView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	*pResult = 1;

	if (pdocDom->fReadOnly())
		return;

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	CPoint posMouse;
	GetCursorPos(&posMouse);
	ScreenToClient(&posMouse);

	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;

	m_iItem = pDispInfo->item.iItem;
	m_iSubItem = -1;

	// Now find the column in which the edit should take place
	int iCol = 0;
	CRect rcSubItem;
	while (m_iSubItem == -1 && GetListCtrl().GetColumn(iCol, &lvc))
	{
		CRect rcItem;
		GetListCtrl().GetSubItemRect(m_iItem, iCol, LVIR_LABEL, rcItem);

		if (rcItem.PtInRect(posMouse))
		{
			m_iSubItem = iCol;
			rcSubItem = rcItem;
		}
		iCol++;	
	}

	if (m_iSubItem == -1)
		return;

	*pResult = 1; // No processing needed after return

	CRect rect;
	CEdit* pEdit = m_ListCtrl.GetEditControl();
	pEdit->GetWindowRect(&rect);
	int margin = (rect.Height() - rcSubItem.Height() - 2) / 2;
	if (m_iSubItem > 0)
		rcSubItem.left += 2; // columns other than column 0 are indented 2 pixels extra!
	rcSubItem.top -= margin;
	rcSubItem.bottom += margin;

	LVITEM lvi;
	lvi.iItem = m_iItem;
	GetListCtrl().GetItem(&lvi);

	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if ( frm )
		frm->HandleAccelerators(false);  // disable accelerators when InPlace editting

	sLabelText = String(GetListCtrl().GetItemText(m_iItem, m_iSubItem));
	// Create the InPlaceEdit; there is no need to delete it afterwards, it will destroy itself
	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
	m_Edit = new InPlaceEdit(m_iItem, m_iSubItem, sLabelText.c_str());
	m_Edit->Create(style, rcSubItem, &m_ListCtrl, 468);
}

void DomainSortView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FrameWindow *frm = dynamic_cast<FrameWindow *>( GetParentFrame());
	if ( frm )
		frm->HandleAccelerators(true);  // re-enable accelerators after InPlace editting
	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if (pDispInfo->item.pszText != 0)
	{
		DomainSortDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);
		
		String sField(pDispInfo->item.pszText);
		sField.sTrimSpaces();
		
		int iSub = pdocDom->fIsGroup() ? m_iSubItem - 1 : m_iSubItem;
		DomainSortFields dsf = iSub < 0 ? dsfUPPER : (DomainSortFields)iSub;
		
		GroupItem gli;
		switch (dsf)
		{
			case dsfUPPER:		gli.SetBound(sField.rVal());
								break;
			case dsfCLASSID:	gli.SetName(sField);
								break;
			case dsfCODE:		gli.SetCode(sField);
								break;
		}
		switch (dsf)
		{
			case dsfUPPER:		
			case dsfCLASSID:	
			case dsfCODE:	if (pdocDom->fCanAdd(gli))
								pdocDom->SetItemPart(pDispInfo->item.iItem + 1, (DomainSortFields)m_iSubItem, sField);
							else if (fCIStrEqual(sLabelText, sField))  // compare with the original label text
								pdocDom->SetItemPart(pDispInfo->item.iItem + 1, (DomainSortFields)m_iSubItem, sField);
							else
								MessageBeep(0xffffffff);  // -1 = standardbeep
							break;
			default:		pdocDom->SetItemPart(pDispInfo->item.iItem + 1, (DomainSortFields)m_iSubItem, sField);
							break;
		}
		
	}
	
	*pResult = 0;
}

/*
	OnGetDispInfo()
	This callback function is called by MFC whenever an item in the ListView
	needs to be displayed.
*/
void DomainSortView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		DomainSortDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);

		int iSubItem = pDispInfo->item.iSubItem;

		CString sField = pdocDom->sGetItemPart(pDispInfo->item.iItem + 1, (DomainSortFields)iSubItem).c_str();

		LPTSTR psBuffer = AddPool(&sField);
		pDispInfo->item.pszText = psBuffer;
	}

	*pResult = 0;
}

// Extra function to temporarily buffer strings for ListView's
// space is needed for at most three strings (2 extra LVN_GETDISPINFO
// notifications) See also MFC with Visual C++, Blaszczak, page 383.
LPTSTR DomainSortView::AddPool(CString* ps)
{
	LPTSTR psRetVal;
	int iOldest = m_iNextFree;

	m_sPool[m_iNextFree] = *ps;
	psRetVal = m_sPool[m_iNextFree].LockBuffer();
	m_psPool[m_iNextFree++] = psRetVal;
	m_sPool[iOldest].ReleaseBuffer();

	if (m_iNextFree == 3)
		m_iNextFree = 0;
	return psRetVal;
}

void DomainSortView::OnAdd()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	ClassItem gli;
	bool fEditReady = false;
	bool fOkClick = true;
	while (!fEditReady && fOkClick)
	{
		EditDomainItemForm frm(GetParent(), TR("Add Domain Item"), gli, "ilwismen\\domain_class_id_editor_add_item.htm");  // gli is dummy here
		fOkClick = frm.fOkClicked();
		if (fOkClick) 
		{
			gli = frm.cliItem();
			if (gli.sName().length() == 0)   // sName must be filled in: empty classes not allowed
				return;

			fEditReady = (pdocDom->fCanAdd(gli));  // check if exist to avoid adding duplicates
			if (fEditReady)  // OK to change
			{
				CWaitCursor cwait;
				long iItem = pdocDom->iCount() + 1;
				long id = pdocDom->iAddItem(gli);

				CListCtrl& lvCtrl = GetListCtrl();

				LV_ITEM lvi;
				lvi.mask = LVIF_TEXT;
				lvi.iItem = iItem;
				lvi.pszText = LPSTR_TEXTCALLBACK;

				lvCtrl.SetItemCountEx(iItem);
				OnUnSelectAll();
				SetSelectedItem(id - 1, true);
			}
			else
			{
				String sIDClass = pdocDom->fIsID() ? "ID" : "Class";
				String sMsg = String(TR("%S or Code already in domain, cannot add, try again?").c_str(), sIDClass);
				fEditReady = IDYES != MessageBox(sMsg.c_str(), TR("Domain warning").c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON1);
			}
		}
	}
}

void DomainSortView::OnUpdateAdd(CCmdUI* pCmdUI)
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	bool fGray = pdocDom->fReadOnly();
	pCmdUI->Enable(!fGray);
}

void DomainSortView::OnUpdateEdit(CCmdUI* pCmdUI)
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	bool fGray = pdocDom->fReadOnly() || lvCtrl.GetSelectedCount() == 0;
	pCmdUI->Enable(!fGray);
}

/*
	OnDelete() is the function called from the UI (menu and DEL key) It displays
	a confirmation dialog, before delegating the actual removal to:
	OnDeleteNoAsk().
*/
void DomainSortView::OnDelete()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	CListCtrl& lvCtrl = GetListCtrl();

	int iSelected = lvCtrl.GetSelectedCount();
	if (iSelected == 0)
		return;

	String sMsg;
	String sTitle;
	if (iSelected == 1)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
		DomainSortDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);

		GroupItem gli;
		pdocDom->GetItem(iItem, gli);
		String sNC;
		if (gli.sCode().length() > 0)
			sNC = String("%S:%S", gli.sCode(), gli.sName());
		else
			sNC = gli.sName();
		if (pdocDom->fIsID())
			sMsg = String(TR("Delete ID '%S' from the domain").c_str(), sNC);
		else
			sMsg = String(TR("Delete class '%S' from the domain").c_str(), sNC);
		sTitle = TR("Delete Domain Item");
	}
	else 
	{
		if (pdocDom->fIsID())
			sMsg = String(TR("%i ID's selected for deletion, proceed?").c_str(), iSelected);
		else
			sMsg = String(TR("%i Classes selected for deletion, proceed?").c_str(), iSelected);
		sTitle = TR("Delete Domain Items");
	}

	int iOk = MessageBox(sMsg.c_str(), sTitle.c_str(), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);

	if (iOk == IDYES)
		OnDeleteNoAsk();
}

/*
	OnDeleteNoAsk()
	This function is called from two places:
	- The Delete option
	- The Cut menu option
	In the last case no UI confirmation is displayed
*/
void DomainSortView::OnDeleteNoAsk()
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	int iSelected = lvCtrl.GetSelectedCount();
	if (iSelected == 0)
		return;
	else if (iSelected == 1)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
		DomainSortDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);

		GroupItem gli;
		pdocDom->GetItem(iItem, gli);

		pdocDom->DeleteItem(gli);
		if (iItem > pdocDom->iCount())
			iItem--;
		SetSelectedItem(iItem - 1, true);
	}
	else {
		GroupItem gli;
		vector<GroupItem> vgi;
		int iItem;

		// Collect the items to be deleted. They are first stored here. After one deletion
		// the domain is changed and the order of the items as well.
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		while (pos != NULL)
		{
			iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of next selected item

			pdocDom->GetItem(iItem, gli);
			vgi.push_back(gli);
		}

		// Now delete all collected items one by one
		vector<GroupItem>::iterator iter;
		for (iter = vgi.begin(); iter != vgi.end(); ++iter) 
			pdocDom->DeleteItem(*iter);
		vgi.clear();
		if (iItem > pdocDom->iCount())
			iItem--;
		SetSelectedItem(iItem - 1, true);
	}
	lvCtrl.SetItemCountEx(pdocDom->iCount());
}

void DomainSortView::OnMerge()
{
	try {
		String sDom;
		MergeDomainForm frm(GetParent(), &sDom);
		if (frm.fOkClicked()) {
			DomainSortDoc* pdocDom = GetDocument();
			ASSERT_VALID(pdocDom);

			CWaitCursor cWait;
			pdocDom->AddFromDomain(sDom);
		}
	}
	catch (SelfMergeError&) {
		MessageBox(TR("Not possible to merge with itself").c_str(), TR("Error").c_str());
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}

void DomainSortView::OnUpdateMerge(CCmdUI* pCmdUI)
{
	OnUpdateAdd(pCmdUI);
}

void DomainSortView::OnChangePrefix()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	String sPrefix = pdocDom->sPrefix();

	ChangePrefixForm frm(GetParent(), sPrefix);

	try
	{
		CWaitCursor cWait;
		if (frm.fOkClicked())
			pdocDom->SetPrefix(frm.sGetPrefix());
	}
	catch (const WarningPrefixNotRenamed& err)
	{
		MessageBox(err.sWhat().c_str(), TR("Domain warning").c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
}

void DomainSortView::OnUpdateChangePrefix(CCmdUI* pCmdUI)
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	bool fGray = pdocDom->fReadOnly() || !pdocDom->fIsID();
	pCmdUI->Enable(!fGray);
}

void DomainSortView::SetSelectedItem(long iItem, bool fSelected)
{
	LV_ITEM lvi;
	CListCtrl& lvCtrl = GetListCtrl();

	lvi.mask = LVIF_STATE;
	lvi.state = fSelected ? LVIS_SELECTED | LVIS_FOCUSED: 0;
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvCtrl.SetItemState(iItem, &lvi);
}

void DomainSortView::OnUpdateSortType(CCmdUI* pCmdUI)
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	bool fGray = pdocDom->fReadOnly();
	pCmdUI->Enable(!fGray);
	switch (pCmdUI->m_nID) {
		case ID_DOMSRTALPH:
			pCmdUI->SetRadio(pdocDom->fAlphaSorted());
			break;
		case ID_DOMSRTMAN:
			pCmdUI->SetRadio(pdocDom->fManualSorted());
			break;
		case ID_DOMSRTALPHNUM:
			pCmdUI->SetRadio(pdocDom->fAlphaNumSorted());
			break;
		case ID_DOMSRTCODEALPH:
			pCmdUI->SetRadio(pdocDom->fCodeAlphabetic());
			break;
		case ID_DOMSRTCODEALNUM:
			pCmdUI->SetRadio(pdocDom->fCodeAlphaNumeric());
			break;
	}
}

// Display a warning when the sort type is changed and the old sort type
// is Manual Ordering. This is the only ordering that cannot be undone
// The function returns true when it is OK to change
bool DomainSortView::fWarnForSortManualChangeOK()
{
	int iOk = IDYES;
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fManualSorted())
		iOk = MessageBox(TR("Changes in the domain caused by changing from Manual Ordering cannot be undone, continue?").c_str(), 
			             TR("Selecting new domain sort type").c_str(), 
						 MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);
	return (iOk == IDYES);
}

void DomainSortView::OnAlphaSort()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fAlphaSorted())
		return;  // already properly sorted

	if (fWarnForSortManualChangeOK())
	{
		pdocDom->SetSortAlphabetic();
		SortDomain();
	}
}

void DomainSortView::OnCodeAlphaSort()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fCodeAlphabetic())
		return;  // already properly sorted

	if (fWarnForSortManualChangeOK())
	{
		pdocDom->SetSortCodeAlpha();
		SortDomain();
	}
}

void DomainSortView::OnAlphaNumSort()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fAlphaNumSorted())
		return;  // already properly sorted

	if (fWarnForSortManualChangeOK())
	{
		pdocDom->SetSortAlphaNum();
		SortDomain();
	}
}

void DomainSortView::OnCodeAlphaNumSort()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fCodeAlphaNumeric())
		return;  // already properly sorted

	if (fWarnForSortManualChangeOK())
	{
		pdocDom->SetSortCodeAlphaNum();
		SortDomain();
	}
}

void DomainSortView::OnManualSort()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	pdocDom->SetSortManual();
	UpdateHeaderImages();
}

void DomainSortView::SortDomain()
{
	UpdateHeaderImages();
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	int iSelected = lvCtrl.GetSelectedCount();
	ClassItem gli;
	if (iSelected == 1)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item

		pdocDom->GetItem(iItem, gli);
		SetSelectedItem(iItem - 1, false);
	}

	CWaitCursor cWait;

	// Do the sort
	pdocDom->Sort();

	if (iSelected == 1)
		SetSelectedItem(pdocDom->iGetItemIndexOf(gli) - 1, true);
}

/*
void DomainSortView::OnCut()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	OnCopy();
	OnDeleteNoAsk();
}
*/

void DomainSortView::OnCopy()
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	int iSelected = lvCtrl.GetSelectedCount();
	GroupItem gli;
	
	zClipboard cb(this);
	cb.clear();
	String sClip;

	if (iSelected > 0)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item

		while (iSelected > 0)
		{
			pdocDom->GetItem(iItem, gli);
			sClip &= pdocDom->sMakeClipLine(gli);

			iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
			iSelected--;
		}
	}
	cb.add(sClip.sVal());
}

void DomainSortView::OnPaste()
{
	zClipboard cb(this);
	if (cb.isTextAvail() == 0)  // false
		return;

	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	char *sClp = cb.getText();
	String sClip(sClp);
	delete [] sClp;
	string sItem;
	unsigned int iBegin = 0;

	// Define storage for accepted new elements. This is kept here to be able to set them
	// to a selected state. The (temporary) storage is needed to remember which items have
	// been added; Using the position in the ListView does not keep record of the correct 
	// items, because this information is lost when a sorting method is selected.
	vector<GroupItem> vgi;
	lvCtrl.SetRedraw(FALSE);
	while (iBegin < sClip.size())
	{
		size_t iEnd = sClip.find("\r\n", iBegin);
		if (iEnd == string::npos)
			iEnd = sClip.size();

		sItem = sClip.substr(iBegin, iEnd - iBegin);
		int iField = dsfCLASSID;
		int iInBegin = 0;
		size_t iLen = sItem.size();
		GroupItem gli;
		while (iInBegin < iLen)
		{
			size_t iInEnd = sItem.find('\t', iInBegin);
			if (iInEnd == string::npos)
			{
				if (iInBegin < iLen) 
					iInEnd = iLen;
				else
					break;
			}

			pdocDom->MakeItemPart((DomainSortFields)iField, gli, sItem.substr(iInBegin, iInEnd - iInBegin));
			iInBegin = iInEnd + 1;
			iField++;
		}
		if (pdocDom->iMergeAddItem(gli) != iUNDEF)
			vgi.push_back(gli);
		iBegin = iEnd + 2;
	}
	lvCtrl.SetItemCountEx(pdocDom->iCount());
	lvCtrl.SetRedraw(TRUE);

	// Now visibly select all new domain items in the ListView, They are listed in the vgi vector.
	// First clear any active selection
	OnUnSelectAll();
	vector<GroupItem>::iterator iter;
	for (iter = vgi.begin(); iter != vgi.end(); ++iter) 
		SetSelectedItem(pdocDom->iGetItemIndexOf(*iter) - 1, true);
	vgi.clear();
}

void DomainSortView::OnUnSelectAll()
{
	LV_ITEM lvi;

	lvi.mask		= LVIF_STATE;
	lvi.iItem		= -1;
	lvi.iSubItem	= 0;
	lvi.state		= ~LVIS_SELECTED;
	lvi.stateMask	= LVIS_SELECTED;

	GetListCtrl().SetItemState(-1, &lvi);

	return;
}

void DomainSortView::OnSelectAll()
{
	LV_ITEM lvi;

	lvi.mask		= LVIF_STATE;
	lvi.iItem		= -1;
	lvi.iSubItem	= 0;
	lvi.state		= LVIS_SELECTED;
	lvi.stateMask	= LVIS_SELECTED;

	GetListCtrl().SetItemState(-1, &lvi);

	return;
}

void DomainSortView::OnUpdateCopy(CCmdUI *pCmdUI)
{
	CListCtrl& lvCtrl = GetListCtrl();

	pCmdUI->Enable(lvCtrl.GetSelectedCount() > 0);
}

void DomainSortView::OnUpdatePaste(CCmdUI *pCmdUI)
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	CListCtrl& lvCtrl = GetListCtrl();

	zClipboard cb(this);
	pCmdUI->Enable(cb.isTextAvail() != 0 && !pdocDom->fReadOnly());
}

//////////////////////////////////////
// DomainSortView Printing support code
/*
	Standard MFC functions used:
	OnPreparePrinting()
		Called once for each print job; This function is responsible for 
		displaying the Print Dialog. The function calculates the number of
		pages required to print the entire domain, and also initializes
		the selection radio button in case some items in the ListView are selected
	OnBeginPrinting()
		Called once for each print job
	OnPrint()
		Called once for each page. This function does the actual printing
	OnEndPrinting()
		Called once for each print job. This function is used for some cleanup

	Utility functions:
	CalcPageCount()
		Calculates the number of pages needed for printing all domain items
	PrintNewLine()
		Moves the paper pointer down one text line
	PrintHeader()
		Prints the header information for the Domain
	PrintFooter
		<currently empty>
	PrintColumns()
		Print the columns in the DomainEditor in bold
	PrintItem()
		Print one domain item
	sMakeShortString()
		Test the length of a string to see if it fits into a certain area; if
		it does not fit break it and add some dots at the end.
*/
void DomainSortView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	BaseView2::OnBeginPrinting(pDC, pInfo);

	LPCTSTR pszFileName = GetDocument()->GetPathName();
	m_timeSys = CTime::GetCurrentTime();

	if (!pInfo->m_bPreview)
		return;

// Special preview code
}

void DomainSortView::PrintNewLine(CDC *pDC, CPrintInfo *pInfo)
{
	CRect& rectPage = pInfo->m_rectDraw;

	rectPage.top += m_cyChar + m_cyChar / 4;
}

void DomainSortView::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
	CListCtrl& lvCtrl = GetListCtrl();

	bool fSelected = pInfo->m_pPD->PrintSelection() > 0;
	bool fSelPages = (pInfo->GetToPage()   != pInfo->GetMaxPage()) ||
					 (pInfo->GetFromPage() != pInfo->GetMinPage());

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	String sHeader;
	if (fSelected || fSelPages)
	{
		sHeader = TR("Selection of: ").c_str();
		sHeader &= pdocDom->obj()->sTypeName();
	}
	else
		sHeader = pdocDom->obj()->sTypeName();

	string sTime = m_timeSys.Format("%d %b %Y, %H:%M    ");
	String sPageNr(sTime);
	if (fSelected)
		sPageNr &= String(TR("Page: %i").c_str(), pInfo->m_nCurPage);
	else
		sPageNr &= String(TR("Page: %i / %i").c_str(), 
					pInfo->m_nCurPage - pInfo->GetFromPage() + 1, 
					pInfo->GetToPage() - pInfo->GetFromPage() + 1);

	CRect& rectPage = pInfo->m_rectDraw;

// print the header text
	pDC->TextOut(rectPage.left, rectPage.top, sHeader.c_str());
	pDC->DrawText(sPageNr.c_str(), pInfo->m_rectDraw, DT_TOP | DT_RIGHT | DT_SINGLELINE);
	PrintNewLine(pDC, pInfo);
	rectPage.top += m_cyChar / 4;  // some extra room

	if (sHeader != pdocDom->sObjectDescription())
		pDC->TextOut(rectPage.left, rectPage.top, pdocDom->sObjectDescription().c_str());
	PrintNewLine(pDC, pInfo);
}

void DomainSortView::PrintColumns(CDC *pDC, CPrintInfo *pInfo)
{
	int iColCount = m_HeaderCtrl.GetItemCount();
	int iIndex;

	HDITEM hdi;
	char lp[256];
	int iIndent = 0;
	m_aiColOffsets.clear();
	CRect& rectPage = pInfo->m_rectDraw;
	for (iIndex = 0; iIndex < iColCount; iIndex++) {
		hdi.mask = HDI_TEXT | HDI_WIDTH;
		hdi.pszText = lp;
		hdi.cchTextMax = 255;
		m_HeaderCtrl.GetItem(iIndex, &hdi);
		size_t iSiz = strlen(hdi.pszText);
		pDC->TextOut(rectPage.left + iIndent, rectPage.top, hdi.pszText, iSiz);
		iIndent += ::MulDiv(hdi.cxy, m_iPrintDPI, m_iScreenDPI);
		m_aiColOffsets.push_back(iIndent);
	}
	PrintNewLine(pDC, pInfo);
}

void DomainSortView::PrintItem(CDC *pDC, CPrintInfo *pInfo, int iIndex)
{
	CRect& rectPage = pInfo->m_rectDraw;

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	String sField;
	CRect rect;
	rect.top = rectPage.top;
	rect.bottom = rectPage.top + m_cyChar;
	rect.left = rectPage.left;
	rect.right = rectPage.left + m_aiColOffsets[0] - m_cxChar;
	sField = sMakeShortString(pDC, pdocDom->sName(iIndex + 1), rect.Width());
	pDC->DrawText(sField.c_str(), rect, DT_LEFT | DT_SINGLELINE);

	rect.left = rectPage.left + m_aiColOffsets[0];
	rect.right = rectPage.left + m_aiColOffsets[1] - m_cxChar;
	sField = sMakeShortString(pDC, pdocDom->sCode(iIndex + 1), rect.Width());
	pDC->DrawText(sField.c_str(), rect, DT_LEFT | DT_SINGLELINE);

	pDC->TextOut(rectPage.left + m_aiColOffsets[1], rectPage.top, pdocDom->sDescription(iIndex + 1).c_str());
	PrintNewLine(pDC, pInfo);
}

void DomainSortView::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
	// nothing yet
}

void DomainSortView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	CFont* pOldFont = pDC->SelectObject(m_PrintFont);
	CFont* pBodyFont;  // place holder for returned fonts

	PrintHeader(pDC, pInfo);
	PrintNewLine(pDC, pInfo);	// empty line for spacing

	pBodyFont = pDC->SelectObject(m_PrintFontBold);  //use bold font for column header

	PrintColumns(pDC, pInfo);

	pBodyFont = pDC->SelectObject(m_PrintFont);

	// draw body text
	int iFirst = (pInfo->m_nCurPage - 1) * m_iItemsPerPage;
	int iLast = (pInfo->m_nCurPage != pInfo->GetMaxPage()) ? iFirst + m_iItemsPerPage : pdocDom->iCount();

	CListCtrl& lvCtrl = GetListCtrl();

	int iSelected = lvCtrl.GetSelectedCount();
	if (iSelected > 0 && pInfo->m_pPD->PrintSelection())
	{
		POSITION pos;
		if (pInfo->m_nCurPage == 1)
		    pos = lvCtrl.GetFirstSelectedItemPosition();
		else
			pos = m_posLast;  // continue with the last selected item

		int iCount = m_iItemsPerPage;
		while (pos != NULL && iCount > 0)
		{
			int iItem = lvCtrl.GetNextSelectedItem(pos);  // get index of next selected item
			PrintItem(pDC, pInfo, iItem);

			iCount--;
		}
		m_posLast = pos;
	}
	else
		for (int iIndex = iFirst; iIndex < iLast; iIndex++)
			PrintItem(pDC, pInfo, iIndex);

	pDC->SelectObject(pOldFont);
}

// OnPreparePrinting is called once for each print job
BOOL DomainSortView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL bResult;
	CWinApp* pApp = AfxGetApp();

	LOGFONT logFont, logFontBold;
	CFont* cf = const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	cf->GetLogFont(&logFont);
	cf->GetLogFont(&logFontBold);
	logFont.lfHeight = -::MulDiv(100, ::GetDeviceCaps(pInfo->m_pPD->m_pd.hDC, LOGPIXELSY), 720);
	m_PrintFont = new CFont;
	m_PrintFont->CreateFontIndirect(&logFont);
	logFontBold.lfWidth = 0;
	logFontBold.lfWeight = FW_BOLD;
	logFontBold.lfHeight = -::MulDiv(100, ::GetDeviceCaps(pInfo->m_pPD->m_pd.hDC, LOGPIXELSY), 720);
	m_PrintFontBold = new CFont;
	m_PrintFontBold->CreateFontIndirect(&logFontBold);

	// ask our app what the default printer is
	// if there isn't any, punt to MFC so it will generate an error

	if (!pApp->GetPrinterDeviceDefaults(&pInfo->m_pPD->m_pd) ||
		 pInfo->m_pPD->m_pd.hDevMode == NULL)
		return DoPreparePrinting(pInfo);

	HGLOBAL	hDevMode = pInfo->m_pPD->m_pd.hDevMode;
	HGLOBAL hDevNames = pInfo->m_pPD->m_pd.hDevNames;

	DEVMODE* pDevMode = (DEVMODE*) ::GlobalLock(hDevMode);
	DEVNAMES* pDevNames = (DEVNAMES*) ::GlobalLock(hDevNames);

	LPCSTR pstrDriverName = ((LPCSTR) pDevNames)+pDevNames->wDriverOffset;
	LPCSTR pstrDeviceName = ((LPCSTR) pDevNames)+pDevNames->wDeviceOffset;
	LPCSTR pstrOutputPort = ((LPCSTR) pDevNames)+pDevNames->wOutputOffset;

	CDC dcPrinter;
	if (dcPrinter.CreateDC(pstrDriverName, pstrDeviceName, pstrOutputPort, NULL))
	{
		CFont* pOldFont = dcPrinter.SelectObject(m_PrintFont);
		CalcPageCount(&dcPrinter, pInfo);
		dcPrinter.SelectObject(pOldFont);
		dcPrinter.DeleteDC();

		CListCtrl& lvCtrl = GetListCtrl();
		if (lvCtrl.GetSelectedCount() > 0)
			pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;
		if (lvCtrl.GetSelectedCount() > 1)
			pInfo->m_pPD->m_pd.Flags |= PD_SELECTION;
		bResult = DoPreparePrinting(pInfo);
	}
	else
	{
		MessageBox("Could not create printer DC");
		bResult = FALSE;
	}

	// Recalculate pages for selection
	int iHeight = 5 * m_cyChar / 4;
 	int iPages;

	int iSelected = GetListCtrl().GetSelectedCount();
	if (iSelected > 0 && pInfo->m_pPD->PrintSelection())
	{
		iPages = (iSelected * iHeight + (m_iPageHeight - 1)) / m_iPageHeight;

		pInfo->SetMinPage(1);
		pInfo->SetMaxPage(iPages);
	}

	::GlobalUnlock(hDevMode);
	::GlobalUnlock(hDevNames);

	return bResult;
}

String DomainSortView::sMakeShortString(CDC* pDC, const String& sVal, int iColumnLen)
{
	static const CString sThreeDots = _T("...");

	int iStringLen = sVal.length();
	CString sField(sVal.c_str());

	if (iStringLen == 0 || pDC->GetTextExtent(sField).cx <= iColumnLen)
		return sVal;

	int iAddLen = pDC->GetTextExtent(sThreeDots).cx;

	int i;
	for (i = iStringLen - 1; i > 0; i--)
	{
		if (pDC->GetTextExtent(sField, i).cx + iAddLen <= iColumnLen)
			break;
	}

	String sRet = (sField.Left(i) + sThreeDots);

	return sRet;
}

void DomainSortView::CalcPageCount(CDC* pDC, CPrintInfo* pInfo)
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	m_cyChar = tm.tmHeight;	// Get the font height
	m_cxChar = tm.tmAveCharWidth;

	// figure out the page width and height
	// remember that the physical page size isn't completely
	// printable--subtract the nonprintable area from it.

	int iPageWidth = pDC->GetDeviceCaps(PHYSICALWIDTH)
			- 2 * (pDC->GetDeviceCaps(PHYSICALOFFSETX));
	m_iPageHeight = pDC->GetDeviceCaps(PHYSICALHEIGHT)
			- 2 * (pDC->GetDeviceCaps(PHYSICALOFFSETY))
			- 5 * m_cyChar;  // header size: is currently fixed

	CDC dcScr;
	dcScr.CreateDC("DISPLAY", NULL, NULL, NULL);
	m_iScreenDPI = dcScr.GetDeviceCaps(LOGPIXELSY);
	m_iPrintDPI = pDC->GetDeviceCaps(LOGPIXELSY);

	// find out how high our font is
	// figure out how many lines of that font fit on a page

	int iHeight = 5 * m_cyChar / 4;
 	int iPages;

	iPages = (pdocDom->iCount() * iHeight + (m_iPageHeight - 1)) / m_iPageHeight;
	m_iItemsPerPage = m_iPageHeight / iHeight;

	// set up printing info structure

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(iPages);
		
	return;
}

void DomainSortView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	delete m_PrintFont;
	delete m_PrintFontBold;
	m_PrintFont = NULL;
	m_PrintFontBold = NULL;
}

//////////////////////////////////////
// DomainSortView Drag and Drop code
/* 
	OnBeginDrag()
		Initializes the drag operation; it creates a drag image and initiates the drag code
		The mouse is captured and cannot be moved outside the DomainEditor during the drag
	OnMouseMove()
		Takes care of the movement of the drag image; it also takes care
		of scrolling in case the top or bottom of the ListView window is reached
		during the drag operation.
	OnLButtonUp()
		Detects the end of the drag (this is the drop action) It finishes the drsg operation
		and does some cleanup. It also tests whether the drag operation was aborted (by
		pressing the ESC key during the drag) in which case nothing will happen
	DropItemOnList()
		Moves the dragged item to the new position.
*/
void DomainSortView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	if (!pdocDom->fManualSorted())
		return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CListCtrl& lvCtrl = GetListCtrl();

	m_iDragIndex = pNMListView->iItem;

	POINT pt;
	pt.x = 8;
	pt.y = 8;
	m_pDragImage = lvCtrl.CreateDragImage(m_iDragIndex, &pt);
	m_pDragImage->BeginDrag(0, CPoint(8, 8));
	m_pDragImage->DragEnter(GetDesktopWindow(), pNMListView->ptAction);

	m_fDragging = true;

	SetCapture();
	CRect rect, rectHeader;
	lvCtrl.GetWindowRect(rect);
	m_HeaderCtrl.GetWindowRect(rectHeader);
	rect.top += rectHeader.Height();
	ClipCursor(rect);
}

void DomainSortView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_fDragging)
	{
		CListCtrl& lvCtrl = GetListCtrl();

		m_ptDropPoint = point;
		ClientToScreen(&m_ptDropPoint);

		m_pDragImage->DragMove(m_ptDropPoint);
		m_pDragImage->DragShowNolock(FALSE); // allow the dragimage to be drawn

		CWnd* pDropWnd = WindowFromPoint(m_ptDropPoint);
		pDropWnd->ScreenToClient(&m_ptDropPoint);

		// SCROLL LIST IF NECESSARY
		int iOverItem = lvCtrl.HitTest(m_ptDropPoint);
		int iTopItem = lvCtrl.GetTopIndex();
		int iBottomItem = iTopItem + lvCtrl.GetCountPerPage() - 1;
		if (iOverItem == iTopItem && iTopItem != 0)
		{
			lvCtrl.EnsureVisible(iOverItem - 1, false);
			::UpdateWindow(lvCtrl.m_hWnd);
			MoveMouse(0, 1);	// generate wm_mousemove but leave cursor in place:
			MoveMouse(0, -1);	// this is needed to detect that the cursor is on the top item
								// so the scrolling will continue
		}
		else if (iOverItem == iBottomItem && iBottomItem != (lvCtrl.GetItemCount() - 1))
		{
			lvCtrl.EnsureVisible(iOverItem + 1, false);
			::UpdateWindow(lvCtrl.m_hWnd);
			MoveMouse(0, 1);	// generate wm_mousemove but leave cursor in place:
			MoveMouse(0, -1);	// this is needed to detect that the cursor is on the bottom item;
								// so the scrolling will continue
		}
	
		m_pDragImage->DragShowNolock(TRUE);	// re-enable the draw lock
	}	
	
	BaseView2::OnMouseMove(nFlags, point);
}

void DomainSortView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_fDragging)
	{
		::ReleaseCapture();
		ClipCursor(0);
		m_fDragging = FALSE;
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();
		m_pDragImage->DeleteImageList();
		delete m_pDragImage;  // prevent memory leaks
		m_pDragImage = 0;

		CPoint pt(point);
		ClientToScreen(&pt);

		if (m_fAbortDrag)	// Drag & Drop aborted by pressing the ESC key
		{
			m_fAbortDrag = false;
			return;
		}
		else 
			DropItemOnList();
	}

	BaseView2::OnLButtonUp(nFlags, point);
}

void DomainSortView::DropItemOnList()
{
	CListCtrl& lvCtrl = GetListCtrl();

	int iDropIndex = lvCtrl.HitTest(m_ptDropPoint);
	
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (iDropIndex < 0)
		iDropIndex = pdocDom->iCount();

	if (m_iDragIndex == iDropIndex)
		return;	// item is dropped onto itself: nothing to do

	lvCtrl.SetRedraw(FALSE);

	pdocDom->MoveItem(iDropIndex + 1, m_iDragIndex + 1);

	lvCtrl.SetRedraw(TRUE);
	lvCtrl.InvalidateRect(NULL);
}

/*
	OnDdescriptionBar()
		Toggles the visibility of the Description bar
	OnUpdateControlBarMenu()
		Keeps the menu item for the description bar up to date

*/
void DomainSortView::OnDescriptionBar()
{
	GetParentFrame()->OnBarCheck(ID_DESCRIPTIONBAR);
}

void DomainSortView::OnButtonBar()
{
	GetParentFrame()->OnBarCheck(ID_DOMSORTBUTTONBAR);
}

void DomainSortView::OnUpdateDescriptionBar(CCmdUI* pCmdUI)
{
	GetParentFrame()->OnUpdateControlBarMenu(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// DomainGroupView

IMPLEMENT_DYNCREATE(DomainGroupView, DomainSortView)


BEGIN_MESSAGE_MAP(DomainGroupView, DomainSortView)
	//{{AFX_MSG_MAP(DomainGroupView)
	ON_COMMAND(ID_DOMEDIT,			OnEdit)
	ON_COMMAND(ID_DOMADD,			OnAdd)
	ON_COMMAND(ID_DOMDEL,			OnDelete)
	ON_COMMAND(ID_CLEAR,			OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////
//	DomainGroupView 

DomainGroupView::DomainGroupView()
	: DomainSortView()
{
}

DomainGroupView::~DomainGroupView()
{
}

void DomainGroupView::AddToolBars()
{
	CFrameWnd *frm = GetParentFrame();

	m_bbTools.Create(frm, "dmgedit.but", TR("Domain editor").c_str(), ID_DOMSORTBUTTONBAR);
	BOOL fRet = descBar.Create(frm, GetDocument());
	ISTRUE(fINotEqual, fRet, FALSE);

	frm->EnableDocking(CBRS_ALIGN_ANY);
	frm->DockControlBar(&descBar);
	frm->DockControlBar(&m_bbTools, AFX_IDW_DOCKBAR_TOP);
}

void DomainGroupView::BuildColumns()
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	int iCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

	int iTotalWidth = 0;
	int iCurCol = 0;
	CSize isHeader;
	CString sCurrent;

	sCurrent = TR("Upper Bound").c_str();

	isHeader = lvCtrl.GetStringWidth(sCurrent);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx);
	iTotalWidth += iCheckWidth + isHeader.cx;
	
	sCurrent = TR("Class Name").c_str();
	CString sDummy('x', 30);  // use a fixed default size for the name column
	isHeader = lvCtrl.GetStringWidth(sDummy);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx);

	iTotalWidth += iCheckWidth + isHeader.cx;

	sCurrent = TR("Code").c_str();
	sDummy = CString('x', 10);
	isHeader = lvCtrl.GetStringWidth(sDummy);
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx);

	iTotalWidth += iCheckWidth + isHeader.cx;

	sCurrent = TR("Description").c_str(); 
	isHeader = lvCtrl.GetStringWidth(sCurrent);
	iTotalWidth += iCheckWidth + isHeader.cx * 4;
	lvCtrl.InsertColumn(iCurCol++, sCurrent, LVCFMT_LEFT, iCheckWidth + isHeader.cx * 4, 1);
}

void DomainGroupView::BuildMenu()
{
//	AddToFileMenu(ID_FILE_PAGE_SETUP);
	AddToFileMenu(ID_FILE_SAVE_COPY_AS); // "Save Copy As", A
	AddToFileMenu(ID_DOMRPR);			// "Edit Representation", T
	AddToFileMenu(ID_PROP);				// "Properties", R
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_DOMMERGE);			// "Merge Domain", M
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_PRINT);		// "Print", P
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_CLOSE);

//	AddToEditMenu(ID_EDIT_CUT);			// "Cut to clip"
	AddToEditMenu(ID_EDIT_COPY);		// "Copy to clip"
	AddToEditMenu(ID_EDIT_PASTE);		// "Paste from clip"
	AddToEditMenu(ID_EDIT_SELECT_ALL);	// "Select all items"
	AddToEditMenu(DONT_CARE, MF_SEPARATOR);
	AddToEditMenu(ID_DOMADD);			// "Add &Item"
	AddToEditMenu(ID_DOMEDIT);			// "&Edit Item"
	AddToEditMenu(ID_DOMDEL);			// "&Delete Item"

	AddToViewMenu(ID_DESCRIPTIONBAR);	// Show/Hide description bar
	AddToViewMenu(ID_DOMSORTBUTTONBAR);	// Show/Hide tool button bar
	AddToViewMenu(ID_STATUSLINE);		// Show/Hide status bar

  AddToHelpMenu(ID_HLPKEY);			// F1: Context help
  AddToHelpMenu(ID_HLPCONTENTS);	// F11: Contents
	AddToHelpMenu(DONT_CARE, MF_SEPARATOR);
	AddToHelpMenu(ID_APP_ABOUT);  //, "&About ILWIS...");
}

DomainGroupDoc* DomainGroupView::GetDocument()
{
    return (DomainGroupDoc*)m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// DomainGroupView diagnostics

#ifdef _DEBUG
void DomainGroupView::AssertValid() const
{
	DomainSortView::AssertValid();
}

void DomainGroupView::Dump(CDumpContext& dc) const
{
	DomainSortView::Dump(dc);
}
#endif //_DEBUG

void DomainGroupView::PrintItem(CDC *pDC, CPrintInfo *pInfo, int iIndex)
{
	CRect& rectPage = pInfo->m_rectDraw;

	DomainGroupDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	String sField;
	CRect rect;
	rect.top = rectPage.top;
	rect.bottom = rectPage.top + m_cyChar;
	rect.left = rectPage.left;
	rect.right = rectPage.left + m_aiColOffsets[0] - m_cxChar * 3;
	sField = sMakeShortString(pDC, pdocDom->sGetItemPart(iIndex + 1, (DomainSortFields)0), rect.Width());
	pDC->DrawText(sField.c_str(), rect, DT_RIGHT | DT_SINGLELINE);

	rect.left = rectPage.left + m_aiColOffsets[0];
	rect.right = rectPage.left + m_aiColOffsets[1] - m_cxChar;
	sField = sMakeShortString(pDC, pdocDom->sName(iIndex + 1), rect.Width());
	pDC->DrawText(sField.c_str(), rect, DT_LEFT | DT_SINGLELINE);

	rect.left = rectPage.left + m_aiColOffsets[1];
	rect.right = rectPage.left + m_aiColOffsets[2] - m_cxChar;
	sField = sMakeShortString(pDC, pdocDom->sCode(iIndex + 1), rect.Width());
	pDC->DrawText(sField.c_str(), rect, DT_LEFT | DT_SINGLELINE);

	pDC->TextOut(rectPage.left + m_aiColOffsets[2], rectPage.top, pdocDom->sDescription(iIndex + 1).c_str());
	PrintNewLine(pDC, pInfo);
}

void DomainGroupView::DoLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl& lvCtrl = GetListCtrl();

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (!pdocDom->fReadOnly() && lvCtrl.GetSelectedCount() != 0)
		OnEdit();
}

void DomainGroupView::OnEdit()
{
	CListCtrl& lvCtrl = GetListCtrl();

	int iSelected = lvCtrl.GetSelectedCount();
	if (iSelected == 0)
		return;
	else if (iSelected == 1)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
		DomainGroupDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);

		double rUpper = pdocDom->rBound(iItem);
		String sName = pdocDom->sName(iItem).sVal();
		String sCode = pdocDom->sCode(iItem).sVal();
		String sDescr = pdocDom->sDescription(iItem).sVal();

		GroupItem gliOrig(sName, sCode, sDescr, rUpper);
		GroupItem gli(sName, sCode, sDescr, rUpper);
		bool fEditReady = false;
		bool fOkClick = true;
		while (!fEditReady && fOkClick)
		{
			EditDomainGroupItemForm frm(GetParent(), TR("Edit Domain Item"), gli, "ilwismen\\domain_group_editor_edit_item.htm");
			fOkClick = frm.fOkClicked();

			if (fOkClick)
			{
				GroupItem gli = frm.cliItem();
				if (gli.sName().length() == 0)   // sName must be filled in: empty classes not allowed
					return;

				if (!fCIStrEqual(gli.sName(), gliOrig.sName()) && !fCIStrEqual(gli.sCode(), gliOrig.sCode()))
					fEditReady = (pdocDom->fCanAdd(gli));  // check if exist to avoid adding duplicates
				else
					fEditReady = true;

				if (fEditReady)  // OK to change
				{
					pdocDom->SetItem(iItem, gli);
					lvCtrl.InvalidateRect(NULL);
					SetSelectedItem(iItem - 1, true);
				}
				else
				{
					String sMsg = String(TR("%S or Code already in domain, cannot change, try again?").c_str(), String("Upper Bound, Class"));
					fEditReady = IDYES != MessageBox(sMsg.c_str(), TR("Domain warning").c_str(), MB_OK | MB_ICONEXCLAMATION | MB_DEFBUTTON1);
				}
			}
		}
	}
	else 
	{
		DomainSortView::OnEdit();	// edit multiple items will change the description only,
									// this is already programmed in DomainSortView
	}
}

void DomainGroupView::OnAdd()
{
	DomainGroupDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	bool fOk = false;
	GroupItem gli;

	bool fEditReady = false;
	bool fOkClick = true;
	while (!fEditReady && fOkClick)
	{
		EditDomainGroupItemForm frm(GetParent(), TR("Add Domain Item"), gli, "ilwismen\\domain_group_editor_add_item.htm");  // gli is dummy here
		fOkClick = frm.fOkClicked();
		if (fOkClick) 
		{
			gli = frm.cliItem();
			if (gli.sName().length() == 0)   // sName must be filled in: empty classes not allowed
				return;

			fEditReady = (pdocDom->fCanAdd(gli)); // check if exist to avoid adding duplicates
			if (fEditReady)  // OK to change
			{
				CWaitCursor cwait;
				long iItem = pdocDom->iCount() + 1;
				long id = pdocDom->iAddItem(gli);

				CListCtrl& lvCtrl = GetListCtrl();

				LV_ITEM lvi;
				lvi.mask = LVIF_TEXT;
				lvi.iItem = iItem;
				lvi.pszText = LPSTR_TEXTCALLBACK;

				lvCtrl.SetItemCountEx(iItem);
				OnUnSelectAll();
				SetSelectedItem(id - 1, true);
			}
			else
			{
				String sMsg = String(TR("%S or Code already in domain, cannot add, try again?").c_str(), String("Upper Bound, Class"));
				fEditReady = IDYES != MessageBox(sMsg.c_str(), TR("Domain warning").c_str(), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON1);
			}
		}
	}
}

void DomainGroupView::OnDelete()
{
	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

	if (pdocDom->fReadOnly())
		return;

	CListCtrl& lvCtrl = GetListCtrl();

	int iSelected = lvCtrl.GetSelectedCount();
	if (iSelected == 0)
		return;

	String sMsg;
	if (iSelected == 1)
	{
		POSITION pos = lvCtrl.GetFirstSelectedItemPosition();
		int iItem = 1 + lvCtrl.GetNextSelectedItem(pos);  // get index of first selected item
		DomainGroupDoc* pdocDom = GetDocument();
		ASSERT_VALID(pdocDom);

		GroupItem gli;
		pdocDom->GetItem(iItem, gli);
		String sNC;
		if (gli.sCode().length() > 0)
			sNC = String("%g: %S:%S", gli.rBound(), gli.sCode(), gli.sName());
		else
			sNC = String("%g: %S", gli.rBound(), gli.sName());
		sMsg = String(TR("Delete boundary class '%S' from the domain").c_str(), sNC);
	}
	else 
		sMsg = String(TR("%i boundary classes selected for deletion, proceed?").c_str(), iSelected);

	int iOk = MessageBox(sMsg.c_str(), TR("Delete Domain Item").c_str(), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);

	if (iOk == IDYES)
		OnDeleteNoAsk();
}

void DomainGroupView::SaveState(IlwisSettings& settings)
{
	int iNrCols = GetHeaderCtrl().GetItemCount();
	if (iNrCols == 0)
		return;

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

    CWindowDC dcScr(this);
	TEXTMETRIC tm;
	dcScr.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;

	CRect rectCol;
	for (int iCol = 0; iCol < iNrCols; iCol++)
	{
		GetHeaderCtrl().GetItemRect(iCol, &rectCol);
		switch (iCol)
		{
			case 0:	settings.SetValue("Bounds", rectCol.Width() / m_cxChar); break;
			case 1:	settings.SetValue("Name", rectCol.Width() / m_cxChar); break;
			case 2:	settings.SetValue("Code", rectCol.Width() / m_cxChar); break;
			case 3:	settings.SetValue("Description", rectCol.Width() / m_cxChar); break;
		}
	}
}

void DomainGroupView::LoadState(IlwisSettings& settings)
{
	int iNrCols = GetHeaderCtrl().GetItemCount();
	if (iNrCols == 0)
		return;

	DomainSortDoc* pdocDom = GetDocument();
	ASSERT_VALID(pdocDom);

    CWindowDC dcScr(this);
	TEXTMETRIC tm;
	dcScr.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;

	for (int iCol = 0; iCol < iNrCols; iCol++)
	{
		int iWid;
		switch (iCol)
		{
			case 0:		iWid = m_cxChar * settings.iValue("Bounds"); break;
			case 1:		iWid = m_cxChar * settings.iValue("Name"); break;
			case 2:		iWid = m_cxChar * settings.iValue("Code"); break;
			case 3:		iWid = m_cxChar * settings.iValue("Description"); break;
		}
		if (iWid > 0)
		{
			HDITEM hdi;
			hdi.mask = HDI_WIDTH;
			hdi.cxy = iWid;
			GetHeaderCtrl().SetItem(iCol, &hdi);
		}
	}
	CRect rectCol;
	GetWindowRect(rectCol);

	OnSize(SIZE_RESTORED, rectCol.Width(), rectCol.Height());
}
