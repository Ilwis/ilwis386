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
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\messages.h"
#include "Client\Base\ZappToMFC.h"
#include "Client\Base\Res.h"
#include "Client\ilwis.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Base\BaseView2.h"
#include "Client\Editors\Utils\ColorCB.h"
#include "Client\Editors\Utils\ColorGrid.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\dmsort.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Representation\RprClassLB.h"
#include "Client\Editors\Utils\colorScroll.h"
#include "Client\Editors\Utils\colorIntensity.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Editors\Representation\ColorIntensityBar.h"
#include "Client\Editors\Representation\ColorGridBar.h"
#include "Client\Editors\Representation\RepresentationClassView.h"
#include "Client\Base\Framewin.h"
#include "Engine\Table\Rec.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\SimpleMapPaneView.h" // for iSIZE_FACTOR

static const int ID_RPRCLASS_LB=6788;

BEGIN_MESSAGE_MAP(MapRprStyleTab, CTabCtrl)
	ON_NOTIFY_REFLECT( NM_CLICK, OnTabPressed)
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP() 

int MapRprStyleTab::Create(RepresentationClassView *v)
{
	CFrameWnd *frm = v->GetParentFrame();
	CRect rctClient;
	frm->GetClientRect(&rctClient);

	int ir = CTabCtrl::Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rctClient, v, 1032);
	rprClassLB.Create(this, v);
	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium);
	SetFont(fnt);

	// We need to make a copy of the icons needed for the images on the tab pages.
	// The global ILWIS imagelist does not use transparent colors or a masking
	// color. The background color for the global imagelist does not match
	// the color of the tabs.
	// 1. Copy the required icons to a local imagelist
	// 2. Set the background to the appropriate color.
	m_il.Create(16, 16, TRUE, 0, 4);
	CImageList *pilSrc = &(IlwWinApp()->ilSmall);

	HICON hIcon = pilSrc->ExtractIcon(IlwWinApp()->iImage("TabRasMap"));
	m_il.Add(hIcon);
	hIcon = pilSrc->ExtractIcon(IlwWinApp()->iImage("TabPolMap"));
	m_il.Add(hIcon);
	hIcon = pilSrc->ExtractIcon(IlwWinApp()->iImage("TabSegMap"));
	m_il.Add(hIcon);
	hIcon = pilSrc->ExtractIcon(IlwWinApp()->iImage("TabPntMap"));
	m_il.Add(hIcon);

	Color clr(GetSysColor(COLOR_BTNFACE));
	m_il.SetBkColor(clr);
	SetImageList(&m_il);
	InsertItem(0, "Raster", 0);
	InsertItem(1, "Polygon", 1);
	InsertItem(2, "Segment", 2);
	InsertItem(3, "Point", 3);

	int iOpt = v->GetDocument()->rpr()->iReadElement("DomainClass", "DrawMode");
	iOpt = max(0, iOpt);
	SetCurSel(iOpt);

	return 1;
}

void MapRprStyleTab::OnSysColorChange( )
{
	Color clr(GetSysColor(COLOR_BTNFACE));
	m_il.SetBkColor(clr);

	CTabCtrl::OnSysColorChange();
}

MapRprStyleTab::~MapRprStyleTab()
{
	// remove the resources of the imagelist
  if (0 != m_il.m_hImageList)
  {
  	IMAGEINFO ifo;
  	for (int i=0; i < m_il.GetImageCount(); ++i)
  	{
  		m_il.GetImageInfo(i, &ifo); // Find the bitmap
  		m_il.Remove(i); // First detach it from GDI
  		UnloadIlwisButtonBitmap(ifo.hbmImage); // Then delete it as required
  	}
  }
}

void MapRprStyleTab::OnSize( UINT nType, int cx, int cy )
{
	if ( rprClassLB.GetSafeHwnd() != NULL)
  {
		MoveWindow(0,0,cx, cy);
		CRect rct, rct2;
		GetItemRect(0, &rct2);
		GetClientRect(&rct);
		rprClassLB.MoveWindow(rct.left + 6, rct.top + rct2.Height() + 8, rct.Width() - 13, rct.Height() - rct2.Height() - 15);
	}
}

void MapRprStyleTab::OnTabPressed( NMHDR * pNotifyStruct, LRESULT* result )
{
	int iActive = GetCurSel();
	rprClassLB.SetDrawMode((RepresentationClassLB::DrawMode) iActive);
}

//--------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(RepresentationClassLB, CListBox)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDoubleClick)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_RPREDITITEMS, OnEditMultiple)
	ON_COMMAND(ID_EDIT_UNDO_COLOR, OnUndo)
	ON_COMMAND(ID_CONFIGURE, OnCustomize)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
END_MESSAGE_MAP()   

RepresentationClassLB::RepresentationClassLB() : 
	info(NULL),
	opt(opRAS),
	view(NULL),
	ds(NULL),
	iOldIndex(iUNDEF),
	iUndoIndex(iUNDEF),
	ndNameState(ndName)
{}

RepresentationClassLB::~RepresentationClassLB()
{
	delete info;
}

BOOL RepresentationClassLB::Create(CTabCtrl *parent, RepresentationClassView *v)
{
	view = v;
	_rpr = view->GetDocument()->rpr();
	CRect rct;
	v->GetClientRect(&rct);
	rct.top = rct.top + 40;
	info  = new zFontInfo(&zDisplay(view));

	DWORD iStyle = WS_HSCROLL|LBS_NOINTEGRALHEIGHT|LBS_OWNERDRAWFIXED| LBS_EXTENDEDSEL|LBS_MULTIPLESEL|
                 LBS_MULTICOLUMN|LBS_NOTIFY|LBS_HASSTRINGS|LBS_WANTKEYBOARDINPUT | WS_VISIBLE | WS_CHILD;
	BOOL iRet = CListBox::Create(iStyle, rct, parent, ID_RPRCLASS_LB);
	ASSERT_VALID(this);

	ds = _rpr->dm()->pdsrt();
	Refresh();
	SetSel(0);

	return TRUE;
}

void RepresentationClassLB::OnEditMultiple()
{
	view->OnEditMultiple();
}

void RepresentationClassLB::OnUndo()
{
	view->OnUndo();
}

void RepresentationClassLB::OnCustomize()
{
	view->OnCustomize();
}

void RepresentationClassLB::OnSelectAll()
{
	view->OnEditSelectAll();
}

void RepresentationClassLB::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();

	unsigned int uFlagsRO = 0;
	unsigned int uFlagsSelect = 0;
	if (view->GetDocument()->fReadOnly())
		uFlagsRO = MF_GRAYED;
	int iSelCount = GetSelCount();
	if (iSelCount == 0)
		uFlagsSelect = MF_GRAYED;

	String sText = String(ILWSF("men", ID_RPREDITITEMS));
	String sText1 = String(ILWSF("men", ID_EDIT_UNDO_COLOR));
	String sText2 = String(ILWSF("men", ID_CONFIGURE));
	String sText3 = String(ILWSF("men", ID_EDIT_SELECT_ALL));
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_RPREDITITEMS, sText.c_str());
	if ( fCanUndo())
		menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_EDIT_UNDO_COLOR, sText1.c_str());
	else
		menu.AppendMenu(MF_STRING | MF_GRAYED | uFlagsSelect, ID_EDIT_UNDO_COLOR, sText1.c_str());
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_CONFIGURE, sText2.c_str());
	menu.AppendMenu(MF_STRING | uFlagsRO | uFlagsSelect, ID_EDIT_SELECT_ALL, sText3.c_str());

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.Detach();
}

void RepresentationClassLB::DrawItem(LPDRAWITEMSTRUCT dis)
{
	if (!(dis->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		return;

	int id = dis->itemID + 1;
	bool fSelected =  dis->itemState & ODS_SELECTED ;
	int iYShift = 10; // shift at the bottom of the rect to create some whit space

	CDC dcLoc;
	dcLoc.Attach(dis->hDC);

	CRect rectWin;
	GetClientRect(&rectWin);
	CRect rectItem;
	rectItem.IntersectRect(&dis->rcItem, &rectWin);
	CRgn rr;
	rr.CreateRectRgnIndirect(&rectItem);
	dcLoc.SelectClipRgn(&rr);

	rectItem = dis->rcItem; // use the entire rectangle, clip region is already set

	dcLoc.FillRect(rectItem, &CBrush(::GetSysColor(COLOR_WINDOW)));

	Color col = fSelected ?::GetSysColor(COLOR_HIGHLIGHTTEXT): ::GetSysColor(COLOR_WINDOWTEXT);  
 
	dcLoc.SetTextColor( col);  
	int iOldM = dcLoc.SetBkMode( TRANSPARENT);
	dcLoc.SetTextAlign(TA_LEFT|TA_TOP);
	if (fSelected) 
	{
		CPen pen(PS_DOT, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CBrush brush(::GetSysColor(COLOR_WINDOW));
		CPen *oldPen = dcLoc.SelectObject(&pen);
		CBrush *oldBrush = dcLoc.SelectObject(&brush);
		dcLoc.Rectangle(rectItem);
		dcLoc.SelectObject(oldPen);
		dcLoc.SelectObject(oldBrush);

		CRect rct2(rectItem);
		rct2.top += iYShift;
		rct2.bottom = rectItem.top + info->height() + iYShift;
		dcLoc.FillRect( rct2, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	long iRaw = ds ? ds->iKey(id): id - 1;
	ISTRUE(fIGreaterEqual, iRaw, 0L);
	String str("%3i", iRaw);

	if (ds)
	{
		switch ( ndNameState)
		{
			case ndName:
				str = ds->sNameByRaw(iRaw); break;
			case ndCode:
				str = ds->sCodeByRaw(iRaw); break;
			case ndNameCode:
				str = String("%S : %S", ds->sCodeByRaw(iRaw, 0), ds->sNameByRaw(iRaw, 0));
				break;
		};

	}

	CFont *oldFont = dcLoc.SelectObject(const_cast<CFont *>(ILWISAPP->GetFont(IlwisWinApp::sfFORM)));
	dcLoc.TextOut( rectItem.left + 5, rectItem.top + iYShift , str.c_str(), str.size());
	dcLoc.SelectObject(oldFont);
	dcLoc.SetBkMode(iOldM);

	switch (opt) 
	{
		case opPNT:
			{
				ExtendedSymbol smb(_rpr,iRaw);
				zPoint pnt( (rectItem.left + rectItem.right - 10) / 2,
							(rectItem.top + rectItem.bottom + info->height()) / 2);
				smb.Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
				smb.drawSmb(&dcLoc,0,pnt);
			 	dcLoc.Detach();
			} break;
		case opPOL: 
			{
				CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWTEXT));
				CPen *oldPen = dcLoc.SelectObject( &pen);
				Pattern pat(_rpr, iRaw);
				CRect rect( rectItem.left + 5, rectItem.top + info->height() + 10,
							rectItem.right - 5, rectItem.bottom - 5 - iYShift + 10);

				Array<zPoint> pnts(8);
				vector<Array<zPoint>> pol;
				pnts[0] = zPoint(rect.left + (rect.right - rect.left)/3,   rect.top  );
				pnts[1] = zPoint(rect.left + (rect.right - rect.left)/2,   rect.top  + (rect.bottom - rect.top)/7 );
				pnts[2] = zPoint(rect.right,                               rect.top + (rect.bottom - rect.top)/2 );
				pnts[3] = zPoint(rect.left + 2*(rect.right - rect.left)/3, rect.bottom );
				pnts[4] = zPoint(rect.left + (rect.right - rect.left)/4,   rect.top + 3 * (rect.bottom - rect.top)/4 );
				pnts[5] = zPoint(rect.left + (rect.right - rect.left)/3,   rect.top + 2 * (rect.bottom - rect.top)/3 );
				pnts[6] = zPoint(rect.left ,                               rect.top + (rect.bottom - rect.top)/2 );
				pnts[7] = zPoint(rect.left + (rect.right - rect.left)/3,   rect.top );
				pol.push_back(pnts);
				pat.drawPolygon(&dcLoc, pol);
				dcLoc.SelectObject( oldPen);
			 	dcLoc.Detach();
			} break;
		case opSEG: 
			{
				Line lin(_rpr, iRaw);
				lin.Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
				int y = (rectItem.top + 20 + rectItem.bottom - 5 ) /2 + iYShift;
				zPoint p1(rectItem.left  + 5, y);
				zPoint p2(rectItem.right - 5, y);
				lin.drawLine( dcLoc.m_hDC, p1, p2);
			}break;
		case opRAS:
			{
				Color clr = _rpr->clrRaw(iRaw);
				if ((long)clr != -2)
				{
					if ((long)clr == -1)
						clr = ::GetSysColor(COLOR_WINDOWTEXT); 
					else if ((long)clr == -2)  
						clr = ::GetSysColor(COLOR_WINDOW);
					dcLoc.FillSolidRect(rectItem.left + 5, rectItem.top + info->height() + iYShift,
										rectItem.right - rectItem.left - 10,
										rectItem.bottom - rectItem.top - 20 - iYShift, clr);
				}
			}
			break;
  }
 	dcLoc.Detach();
}

void RepresentationClassLB::MeasureItem(LPMEASUREITEMSTRUCT mi )
{
	if ( info)
	{
		mi->itemHeight = 45 + info->height() + 10;; //iYShift
		mi->itemWidth = 16 * info->width();
	}
}

void RepresentationClassLB::SetDrawMode(RepresentationClassLB::DrawMode dm)
{
	ISTRUE(fIGreaterEqual, dm, opRAS);
	ISTRUE(fILessEqual, dm, opPNT);
	opt = dm;
	_rpr->WriteElement("DomainClass", "DrawMode", (long)dm);
	Invalidate();
}

const RepresentationClassLB::DrawMode RepresentationClassLB::GetDrawMode()
{
	return opt;
}

void RepresentationClassLB::OnDoubleClick()
{
	view->OnEditSingleItem();
}

bool RepresentationClassLB::fCanUndo()
{
	return iUndoIndex != iUNDEF;
}

// Initlize undo status; Is called as the last function of RPRClasView::OninitialUpdate
// This is needed because SetColor is also called when the View is build causing
// the necessary Undo init to be corrupted
void RepresentationClassLB::InitUndo()
{
	iUndoIndex = iUNDEF;
}

void RepresentationClassLB::SetUndo(int id, Color clr)
{
	iUndoIndex = id;
	clrUndo = clr;
}

void RepresentationClassLB::Undo()
{
	if (iUndoIndex == iUNDEF ) return;

	int iS = GetCurSel();

	SetSel(iUndoIndex);

	iOldIndex = iUndoIndex;
	iUndoIndex = iUNDEF;

	view->GetParentFrame()->PostMessage(ILWM_SETCOLOR, clrUndo);
}

void RepresentationClassLB::OnSelChange()
{
	int id = GetCurSel();
	if ( id == LB_ERR  ) 
		return;

	if (iOldIndex == id)
		return;  // Selection has NOT changed

	iOldIndex = id;

	long iRaw = ds ? ds->iKey(id + 1) : id;
	ISTRUE(fINotEqual, iRaw, iUNDEF);
	Color clr;
	if (opt == opPNT)
	{
		RepresentationClass *rpc = _rpr->prc();
		Ilwis::Record rec = rpc->rec(iRaw);
		int iCol = rec.iCol("SymbolType");
		if ( iCol >=0 && rec.sValue(iCol).iVal() == 0 ) // symboltype
		   clr= rpc->clrSymbolFill(iRaw);
		else			 
			clr = rpc->clrSymbol(iRaw);	
	}
	else
		clr = Color(_rpr->clrRaw(iRaw));

	// Update all views (Intensity Bar etc) with the newly selected color
	view->GetParentFrame()->PostMessage(ILWM_SETCOLOR, clr);
}
	
void RepresentationClassLB::SetColor(Color clr)
{
	if (_rpr->fDataReadOnly())
		return;

	int id = GetCurSel();
	if ( id == LB_ERR  ) 
		return;

	long iRaw = ds ? ds->iKey(id + 1) : id;

	Color tempClr = Color(_rpr->clrRaw(iRaw));
	if (tempClr == clr)
		return;  // nothing needs to be done now

	if (opt == opPNT)
	{
		RepresentationClass *rpc = _rpr->prc();
		Ilwis::Record rec = rpc->rec(iRaw);
		int iCol = rec.iCol("SymbolType");
		if ( tempClr != clr )
		{
			if ( iCol >= 0 && rec.sValue(iCol).iVal() == 0 ) // symboltype
				rpc->PutSymbolFill(iRaw, clr);
			else
				rpc->PutSymbolColor(iRaw, clr);
		}
	}		
	else
		if ( tempClr != clr )
			_rpr->prc()->PutColor(iRaw,clr);

	Invalidate();
}

void RepresentationClassLB::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch ( nChar)
	{
		case VK_ESCAPE:
			SetSel(-1, FALSE);
			Invalidate();
			break;
		case VK_RETURN:
		case VK_F2:
			if ( GetSelCount() == 1)
				view->OnEditSingleItem();
			if ( GetSelCount() > 1 )
				view->OnEditMultiple();
			SetFocus(); // else activation shifts to the framewindow
			break;
		case '1':
			SetDrawMode(RepresentationClassLB::opRAS);
			break;
		case '2':
			SetDrawMode(RepresentationClassLB::opPOL);
			break;
		case '3':
			SetDrawMode(RepresentationClassLB::opSEG);
			break;
		case '4':
			SetDrawMode(RepresentationClassLB::opPNT);
			break;
	}
}

void RepresentationClassLB::SetNameDisplayState(RepresentationClassLB::ndNameDisplay ndState)
{
	ndNameState = ndState;
}

void RepresentationClassLB::OnMouseMove( UINT nFlags, CPoint point )
{
	CRect rct;
	GetClientRect(&rct);
	int iHeight = 45 + info->height();
	int iWidth = view->iColWidth;
	int iCols = rct.Height()/ iHeight;
	int iRows = rct.Width() / iWidth;
	int iYIndex = point.y / iHeight;
	int iIndex = point.x / iWidth * iCols + iYIndex;
	DomainSort *pdsrt = _rpr->dm()->pdsrt();
	if ( pdsrt )
	{
		if ( iIndex < pdsrt->iSize() )
		{
			FrameWindow *frm = (FrameWindow *)view->GetParentFrame();
			long iRaw = pdsrt->iKey(iIndex + 1);
			String sDesc = _rpr->dm()->pdsrt()->sDescriptionByRaw(iRaw);
			frm->status->SetWindowText(sDesc.c_str());
		}
	}
}

void RepresentationClassLB::Refresh()
{
	ResetContent();
	if ( view->csBar.GetSafeHwnd())
		view->csBar.Invalidate();
	CFont *fnt=const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	SetFont( fnt);
	if ( info) 
		delete info;
	info  = new zFontInfo(&zDisplay(view));
	int iColWidth=0;
	if (ds) 
	{
		iColWidth = _rpr->iReadElement("DomainClass", "ColumnWidth");
		int iOpt = _rpr->iReadElement("DomainClass", "DrawMode");
		opt = (DrawMode)max(0, iOpt);
		ndNameState = (ndNameDisplay)_rpr->iReadElement("DomainClass", "NameState");
		ndNameState = (ndNameDisplay) max(0, ndNameState);
		if (iColWidth < 20 || iColWidth > 250)
			iColWidth = 100;
		for (int i = 1; i <= ds->iNettoSize(); ++i) 
		{
			String s = ds->sValue(i, 0);
			AddString(s.c_str());
		}
	}
	else {
		iColWidth = 50;
		int iMax = 256;
		DomainPicture* dp = _rpr->dm()->pdp();
		if (dp) 
			iMax = dp->iColors();
		for (int i = 0; i < iMax; ++i) {
			String s("%i",i);
			AddString(s.c_str());
		}
	}  
	SetColumnWidth(iColWidth);
}

//---- Debug sectie ---------------------------------------------------------------
#ifdef _DEBUG
void RepresentationClassLB::AssertValid() const
{
	CListBox::AssertValid();
}

void RepresentationClassLB::Dump(CDumpContext& dc) const
{
	CListBox::Dump(dc);
}

#endif //_DEBUG
