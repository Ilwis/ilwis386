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
// CriteriaTreeView.cpp: implementation of the CriteriaTreeView class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\constant.h"
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\CriteriaTreeView.h"
#include "Client\Editors\SMCE\CriteriaTreeDoc.h"
#include "Client\Editors\SMCE\CriteriaTreeItem.h"
#include "Client\Base\Framewin.h"

IMPLEMENT_DYNCREATE(CriteriaTreeView, TreeListView)

BEGIN_MESSAGE_MAP(CriteriaTreeView, TreeListView)
	ON_WM_LBUTTONDOWN() // receiving by TreeListView::PreTranslateMessage
	ON_WM_LBUTTONUP() // receiving by TreeListView::PreTranslateMessage
	ON_WM_RBUTTONDOWN() // receiving by TreeListView::PreTranslateMessage
	ON_WM_LBUTTONDBLCLK() // receiving by TreeListView::PreTranslateMessage
	ON_WM_MOUSEMOVE() // receiving by TreeListView::PreTranslateMessage
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	// ON_COMMAND(ID_CT_INSERT, OnInsert) // will not receive without accel (menu), so we use OnKeyDown
	ON_WM_KEYDOWN() // receiving by TreeListView::PreTranslateMessage
	ON_COMMAND(ID_CT_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_CT_EDIT, OnUpdateEdit)
	ON_COMMAND(ID_CT_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_CT_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_CT_SHOW, OnShow)
	ON_UPDATE_COMMAND_UI(ID_CT_SHOW, OnUpdateShow)
	ON_COMMAND(ID_CT_SHOW_STANDARDIZED, OnShowStandardized)
	ON_UPDATE_COMMAND_UI(ID_CT_SHOW_STANDARDIZED, OnUpdateShowStandardized)
	ON_COMMAND(ID_CT_SHOW_SLICED, OnShowSliced)
	ON_UPDATE_COMMAND_UI(ID_CT_SHOW_SLICED, OnUpdateShowSliced)
	ON_COMMAND(ID_CT_PROPERTIES, OnProperties)
	ON_UPDATE_COMMAND_UI(ID_CT_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_CT_HISTOGRAM, OnHistogram)
	ON_UPDATE_COMMAND_UI(ID_CT_HISTOGRAM, OnUpdateHistogram)
	ON_COMMAND(ID_CT_AGGREGATE_VALUES, OnAggregateValues)
	ON_UPDATE_COMMAND_UI(ID_CT_AGGREGATE_VALUES, OnUpdateAggregateValues)
	ON_COMMAND(ID_CT_MAPAGGREGATION, OnAggregateMap)
	ON_UPDATE_COMMAND_UI(ID_CT_MAPAGGREGATION, OnUpdateAggregateMap)
	ON_COMMAND(ID_CT_NOMAPAGGREGATION, OnCancelAggregateMap)
	ON_UPDATE_COMMAND_UI(ID_CT_NOMAPAGGREGATION, OnUpdateCancelAggregateMap)
	ON_COMMAND(ID_CT_SLICE, OnSlice)
	ON_UPDATE_COMMAND_UI(ID_CT_SLICE, OnUpdateSlice)
	ON_COMMAND(ID_CT_HISTOGRAM_SLICED, OnHistogramSliced)
	ON_UPDATE_COMMAND_UI(ID_CT_HISTOGRAM_SLICED, OnUpdateHistogramSliced)
	ON_COMMAND(ID_CT_AGGREGATE_VALUES_SLICED, OnAggregateValuesSliced)
	ON_UPDATE_COMMAND_UI(ID_CT_AGGREGATE_VALUES_SLICED, OnUpdateAggregateValuesSliced)
	ON_COMMAND(ID_CT_SELECTEDMAP, OnGenerateSelectedMap)
	ON_UPDATE_COMMAND_UI(ID_CT_SELECTEDMAP, OnUpdateGenerateSelectedMap)
	ON_COMMAND(ID_CT_GENSELECTEDMAP, OnGenerateSelectedMap)
	ON_UPDATE_COMMAND_UI(ID_CT_GENSELECTEDMAP, OnUpdateGenerateSelectedMap)
	ON_COMMAND(ID_CT_GENERATE_CONTOUR_MAPS, OnGenerateContourMaps)
	ON_UPDATE_COMMAND_UI(ID_CT_GENERATE_CONTOUR_MAPS, OnUpdateGenerateContourMaps)
	ON_COMMAND(ID_CT_SHOW_CONTOUR_MAPS, OnShowContourMaps)
	ON_UPDATE_COMMAND_UI(ID_CT_SHOW_CONTOUR_MAPS, OnUpdateShowContourMaps)
	ON_NOTIFY(LVN_ENDLABELEDIT, ID_TREE_LIST_CTRL, OnEndLabelEdit) // NOTE::should be TVN_ but this is what InPlaceNameEdit sends us
	ON_NOTIFY(HDN_ITEMCLICK, ID_TREE_LIST_HEADER, OnHeaderClicked)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CriteriaTreeView::CriteriaTreeView()
: ctiSelected(0)
, ctiDragSource(0)
, fLMouseButtonDown(false)
, m_CTLInitialized(FALSE)
, nEditTimer(0)
, m_curDragProhibited("DragProhibited")
, m_curNormal(Arrow)
, m_fDragImageOnScreen(false)
{
}

CriteriaTreeView::~CriteriaTreeView()
{
	if (nEditTimer)
		KillTimer(nEditTimer);
}

CriteriaTreeDoc* CriteriaTreeView::GetDocument()
{
	return (CriteriaTreeDoc*)m_pDocument;
}

void CriteriaTreeView::OnInitialUpdate()
{
	TreeListView::OnInitialUpdate();
	if (!m_CTLInitialized)
	{
		COLORREF clrMask(RGB(192, 192, 192)); // grey = transparent
		// make following list correspond with enum eIconIndexTP in CrtiertaTreeItem.h
		const char sBitmapNames[][32] = {"SMCETREE","SMCEGROUP","SMCEMAPEFFECT","SMCEMAPCONSTRAINT","SMCEAGGREGATEDMAPEFFECT","SMCEAGGREGATEDMAPCONSTRAINT","SMCESCOREEFFECT","SMCESCORECONSTRAINT"};

		m_cImageList.Create(16, 15, ILC_MASK, 0, 10);

		CBitmap bmp;
		for (int i=0; i<8; ++i)
		{
			LoadIlwisButtonBitmap(sBitmapNames[i], bmp);
			m_cImageList.Add(&bmp, clrMask);  // the Add function makes a copy of the bitmap (see ButtonBar)
			UnloadIlwisButtonBitmap(bmp);
		}
		// m_cImageList.Create(IDB_TREE_PICTURES, 16, 10, RGB(255, 255, 255)); // MFC way
		SetImageList(&m_cImageList, TVSIL_NORMAL);

		// Initial population: one column
		InsertColumn(0, "Criteria Tree", LVCFMT_LEFT, 200);
		m_CTLInitialized = TRUE;
	}
}

void CriteriaTreeView::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	PostMessage(WM_COMMAND, ID_CT_SETALTERNATIVES);
}

void CriteriaTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	fLMouseButtonDown = true;
	// remember the previous selected item .. if we select it twice, execute edit!!
	CriteriaTreeItem* oldCtiSelected = ctiSelected;
	int iOldActCol = iActiveColumn();

	TreeListView::OnLButtonDown(nFlags, point); // will get us the newly selected item (if any)
	HTREEITEM hSelectedItem = GetSelectedItem();
	if (hSelectedItem)
		ctiSelected = (CriteriaTreeItem*) GetItemData(hSelectedItem);
	else
		ctiSelected = 0;

	bool fEditing = false;
	if (GetDocument())
		fEditing = (GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE);

	int iXpos = point.x - iOffset(); // in case we scrolled!
	int iFirstColumnWidth = 0;
	if (iColumns() > 0)
		iFirstColumnWidth = iGetColumnWidth(0);

	if (fEditing && (iXpos <= iFirstColumnWidth) && ctiSelected && (ctiSelected->GetParent() != 0))
		ctiDragSource = ctiSelected;
	else
		ctiDragSource = 0; // do not even start drag/drop if we're not editing

	// Now check if we should do "OnEdit"
	if ((nEditTimer == 0) && (ctiSelected!=0) && (ctiSelected == oldCtiSelected) && (iOldActCol == iActiveColumn()))
		nEditTimer = SetTimer(1, GetDoubleClickTime(), 0);
}

void CriteriaTreeView::OnMouseMove(UINT nFlags, CPoint point)
{
	TreeListView::OnMouseMove(nFlags, point);

	if ((ctiDragSource != 0) && fLMouseButtonDown)
	{
		// we're dragging .. show this to the user

		bool fDragAllowed = false;

		int iXpos = point.x - iOffset(); // in case we scrolled!
		int iFirstColumnWidth = 0;
		if (iColumns() > 0)
			iFirstColumnWidth = iGetColumnWidth(0);

		UINT uFlags;
		HTREEITEM hCandidateDropItem = HitTest(point, &uFlags);
		CriteriaTreeItem* ctiCandidateDropItem = 0;
		if (hCandidateDropItem && (iXpos <= iFirstColumnWidth))
		{
			ctiCandidateDropItem = (CriteriaTreeItem*) GetItemData(hCandidateDropItem);
			if (ctiCandidateDropItem->fMoveHereAllowed(ctiDragSource))
				fDragAllowed = true;
		}

		if (m_fDragImageOnScreen)
			m_cImageList.DragLeave(&GetTreeCtrl());

		if (ctiDragSource != ctiCandidateDropItem)
		{
			if (fDragAllowed)
			{
				SelectItem(hCandidateDropItem);
				SetCursor(m_curNormal);
			}
			else
				SetCursor(m_curDragProhibited);

			if (!m_fDragImageOnScreen)
			{
				m_cImageList.BeginDrag(ctiDragSource->iIconIndex(), CPoint(16,12));
				m_fDragImageOnScreen = true;
			}
		}
		else if (m_fDragImageOnScreen)
			SetCursor(m_curDragProhibited);

		if (m_fDragImageOnScreen)
			m_cImageList.DragEnter(&GetTreeCtrl(), point);
	}
}

void CriteriaTreeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	fLMouseButtonDown = false;

	SetCursor(m_curNormal); // restore

	if (m_fDragImageOnScreen)
	{
		m_cImageList.DragLeave(&GetTreeCtrl());
		m_cImageList.EndDrag();
		m_fDragImageOnScreen = false;
	}
	
	// check if user did drag/drop .. if yes, cancel the "Edit" timer and do it; if no, do nothing

	TreeListView::OnLButtonUp(nFlags, point); // will get us the newly selected item (if any)

	int iXpos = point.x - iOffset(); // in case we scrolled!
	int iFirstColumnWidth = 0;
	if (iColumns() > 0)
		iFirstColumnWidth = iGetColumnWidth(0);
	
	HTREEITEM hDropItem = GetSelectedItem();
	if (hDropItem && (iXpos <= iFirstColumnWidth))
	{
		CriteriaTreeItem* ctiDropped = (CriteriaTreeItem*) GetItemData(hDropItem);

		if ((ctiDropped != 0) && (ctiDropped != ctiDragSource) && (ctiDragSource != 0) && ctiDropped->fMoveHereAllowed(ctiDragSource))
		{
			// cancel the timer!! no edit wanted
			if (nEditTimer)
			{
				KillTimer(nEditTimer);
				nEditTimer = 0;
			}
			// perform drag / drop

			ctiDropped->MoveHere(ctiDragSource);
			SelectItem(ctiDragSource->hItem);
		}
		else if (ctiDragSource != 0)
			SelectItem(ctiDragSource->hItem); // restore original selection
	}
	else if (ctiDragSource != 0)
		SelectItem(ctiDragSource->hItem); // restore original selection

	ctiDragSource = 0;
}

void CriteriaTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// cancel the timer!! no edit wanted
	if (nEditTimer)
	{
		KillTimer(nEditTimer);
		nEditTimer = 0;
	}

	if (ctiSelected && (iActiveColumn()>0))
	{
		if (ctiSelected->fDataExists(iActiveColumn()))
			PostMessage(WM_COMMAND, ID_CT_SHOW);
		else
		{
			bool fPossible = false;
			{
				EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
				if (eg && eg->fCalculationPossible(iActiveColumn()))
					fPossible = true;
			}
			if (fPossible)
				PostMessage(WM_COMMAND, ID_CT_SELECTEDMAP); // user-friendlyness of dblclk
			else
				PostMessage(WM_COMMAND, ID_CT_EDIT); // inconsistency in ui behavior, but requested by customer: edit input and output maps
		}
	}
	else if (ctiSelected && (iActiveColumn() == 0))
	{
		// inconsistency in ui behavior, but requested by customer: edit node properties if treectrl wouldn't react
		if (ctiSelected->GetParent() == 0) // root - tree wouldn't react
			PostMessage(WM_COMMAND, ID_CT_EDIT);
		else
		{
			EffectGroup * eg = dynamic_cast<EffectGroup*>(ctiSelected);
			if (0 == eg) // leaf - tree wouldn't react
				PostMessage(WM_COMMAND, ID_CT_EDIT);
			else
			{
				// node - tree would react if there are children
				if (eg->itFirstChild() == eg->itLastChild()) // no children
					PostMessage(WM_COMMAND, ID_CT_EDIT);
			}
		}
	}

	TreeListView::OnLButtonDblClk(nFlags, point);
}

void CriteriaTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// cancel the timer!! no edit wanted
	if (nEditTimer)
	{
		KillTimer(nEditTimer);
		nEditTimer = 0;
	}
	
	// Now a simplified version of OnLButtonDown (no Edit!)

	TreeListView::OnRButtonDown(nFlags, point); // will get us the newly selected item (if any)
	HTREEITEM hSelectedItem = GetSelectedItem();
	if (hSelectedItem)
		ctiSelected = (CriteriaTreeItem*) GetItemData(hSelectedItem);
	else
		ctiSelected = 0;
}

void CriteriaTreeView::OnTimer(UINT nIDEvent)
{
	if ((nEditTimer != 0) && (nEditTimer == nIDEvent))
	{
		// stop the timer
		KillTimer(nEditTimer);
		nEditTimer = 0;

		// do our job
		if (!fLMouseButtonDown)
			PostMessage(WM_COMMAND, ID_CT_EDIT);
	}
	else
		TreeListView::OnTimer(nIDEvent);
}

void CriteriaTreeView::OnEdit()
{
	if (ctiSelected)
	{
		RECT rect;
		GetTreeCtrl().GetItemRect(ctiSelected->hItem, &rect, false);
		// left & right coords of rect are still wrong .. calculate them
		int i;
		int m_nWidth = 0;
		for (i=0; i<iActiveColumn(); ++i) // warning: iActiveColumn() can be -1
			m_nWidth += iGetColumnWidth(i); // left side
		rect.left = iOffset() + m_nWidth+((0==i)?rect.left:0); // leave rect.left for first column
		rect.right = iOffset() + m_nWidth+iGetColumnWidth(i)-2; // right side
		// Now call the item's Edit
		ctiSelected->Edit(max(iActiveColumn(), 0), rect, &GetTreeCtrl());
	}
}

void CriteriaTreeView::OnUpdateEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ctiSelected != 0) && (ctiSelected->fEditAllowed(max(iActiveColumn(), 0))));
}

void CriteriaTreeView::OnDelete()
{
	// call the selected item's Delete
	if (ctiSelected)
		ctiSelected->Delete(max(iActiveColumn(), 0));
}

void CriteriaTreeView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(fDeleteAllowed());
}

bool CriteriaTreeView::fDeleteAllowed()
{
	int iActCol = max(iActiveColumn(), 0);
	bool fEditing = false;
	if (GetDocument())
		fEditing = (GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE);

	return	(ctiSelected != 0) &&
					((iActCol>0) || (fEditing && (ctiSelected->GetParent() !=0 ))) &&
					(ctiSelected->fEditAllowed(iActCol));
}

void CriteriaTreeView::OnShow()
{
	if (ctiSelected)
		ctiSelected->Show(iActiveColumn());
}

void CriteriaTreeView::OnUpdateShow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && ctiSelected->fDataExists(iActiveColumn()));
}

void CriteriaTreeView::OnShowStandardized()
{
	if (ctiSelected)
		ctiSelected->ShowStandardized(iActiveColumn());
}

void CriteriaTreeView::OnUpdateShowStandardized(CCmdUI* pCmdUI)
{
	Effect* e = dynamic_cast<Effect*>(ctiSelected);
	pCmdUI->Enable(e && e->fShowStandardizedPossible(iActiveColumn()));
}

void CriteriaTreeView::OnProperties()
{
	if (ctiSelected && (iActiveColumn()>0))
		ctiSelected->Properties(iActiveColumn());
}

void CriteriaTreeView::OnUpdateProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && (iActiveColumn()>0) && ctiSelected->fDataExists(iActiveColumn()));
}

void CriteriaTreeView::OnHistogram()
{
	if (ctiSelected)
		ctiSelected->Histogram(iActiveColumn());
}

void CriteriaTreeView::OnUpdateHistogram(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && ctiSelected->fDataExists(iActiveColumn()));
}

void CriteriaTreeView::OnAggregateValues()
{
	if (ctiSelected)
		ctiSelected->AggregateValues();
}

void CriteriaTreeView::OnUpdateAggregateValues(CCmdUI* pCmdUI)
{
	EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
	pCmdUI->Enable(eg && (iActiveColumn() <= 0) && eg->fDataExists(iActiveColumn()));
}

void CriteriaTreeView::OnAggregateMap()
{
	if (ctiSelected)
		ctiSelected->AggregateValues();
}

void CriteriaTreeView::OnUpdateAggregateMap(CCmdUI* pCmdUI)
{
	MapEffect* me = dynamic_cast<MapEffect*>(ctiSelected);
	bool fModeStdWeigh = false;
	if (GetDocument())
		fModeStdWeigh = (GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH);

	pCmdUI->Enable(me && (iActiveColumn() <= 0) && fModeStdWeigh);
	pCmdUI->SetRadio(me ? (me->fSpatialItem() ? FALSE : TRUE) : FALSE);
}

void CriteriaTreeView::OnCancelAggregateMap()
{
	MapEffect* me = dynamic_cast<MapEffect*>(ctiSelected);
	if (me)
		me->DoNotAggregateValues();
}

void CriteriaTreeView::OnUpdateCancelAggregateMap(CCmdUI* pCmdUI)
{
	MapEffect* me = dynamic_cast<MapEffect*>(ctiSelected);
	bool fModeStdWeigh = false;
	if (GetDocument())
		fModeStdWeigh = (GetDocument()->GetMode() == CriteriaTreeDoc::eSTDWEIGH);

	pCmdUI->Enable(me && (iActiveColumn() <= 0) && fModeStdWeigh);
	pCmdUI->SetRadio(me ? (me->fSpatialItem() ? TRUE : FALSE) : TRUE);
}

void CriteriaTreeView::OnSlice()
{
	if (ctiSelected)
		ctiSelected->Slice();
}

void CriteriaTreeView::OnUpdateSlice(CCmdUI* pCmdUI)
{
	EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
	pCmdUI->Enable(eg && (iActiveColumn() <= 0) && eg->fAllDataExists());
}

void CriteriaTreeView::OnShowSliced()
{
	if (ctiSelected)
		ctiSelected->ShowSliced(iActiveColumn());
}

void CriteriaTreeView::OnUpdateShowSliced(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && (ctiSelected->fSlicedExists(iActiveColumn())));
}

void CriteriaTreeView::OnHistogramSliced()
{
	if (ctiSelected)
		ctiSelected->HistogramSliced(iActiveColumn());
}

void CriteriaTreeView::OnUpdateHistogramSliced(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && (ctiSelected->fSlicedExists(iActiveColumn())));
}

void CriteriaTreeView::OnAggregateValuesSliced()
{
	if (ctiSelected)
		ctiSelected->AggregateValuesSliced();
}

void CriteriaTreeView::OnUpdateAggregateValuesSliced(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && (iActiveColumn() <= 0) && (ctiSelected->fSlicedExists(iActiveColumn())));
}

void CriteriaTreeView::OnGenerateContourMaps()
{
	if (ctiSelected)
		ctiSelected->GenerateContourMaps();
}

void CriteriaTreeView::OnUpdateGenerateContourMaps(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && ctiSelected->fDataExists(iActiveColumn()));
}

void CriteriaTreeView::OnShowContourMaps()
{
	if (ctiSelected)
		ctiSelected->ShowContourMaps(iActiveColumn());
}

void CriteriaTreeView::OnUpdateShowContourMaps(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ctiSelected && (ctiSelected->fContourMapExists(iActiveColumn())));
}

void CriteriaTreeView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.pszText != 0)
	{
		String sData(pDispInfo->item.pszText);
		sData.sTrimSpaces();
		CriteriaTreeItem* ctiEdited = (CriteriaTreeItem*)pDispInfo->item.iItem;
		// iItem was misused to hold a pointer to the ctiEdited .. it would be nicer if
		// InPlaceNameEdit allowed an LPARAM
		int iCol = pDispInfo->item.iSubItem;
		ctiEdited->SetData(iCol, sData);
	}
}

void CriteriaTreeView::OnGenerateSelectedMap()
{
	if (ctiSelected)
	{
		EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
		if (eg)
			eg->GenerateOutput(iActiveColumn());
	}
}

void CriteriaTreeView::OnUpdateGenerateSelectedMap(CCmdUI* pCmdUI)
{
	bool fPossible = false;
	if (ctiSelected)
	{
		EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
		if (eg)
			fPossible = eg->fCalculationPossible(iActiveColumn());
	}

	pCmdUI->Enable(fPossible);
}

BOOL CriteriaTreeView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (ctiSelected && ctiSelected->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return TreeListView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CriteriaTreeView::RecursiveReadDoc(HTREEITEM hInsertAt, CriteriaTreeItem* ctiReadFrom)
{
	if (0 == hInsertAt) // root
		hInsertAt = TVI_ROOT;
	EffectGroup * eg = dynamic_cast<EffectGroup*>(ctiReadFrom);
	// int iIcon = (eg!=0)?(ctiReadFrom->GetParent()!=0?1:0):2; // discriminate between root, nodes and leafs (resp. icon 0, 1 and 2)
	int iIcon = ctiReadFrom->iIconIndex();
	HTREEITEM hNew = InsertItem(ctiReadFrom->sDisplayText(0), iIcon, iIcon, hInsertAt, 0);
	if (hNew)
		SetItemData(hNew, (DWORD)ctiReadFrom); // important!!
	if (eg)
	{
		for (list <CriteriaTreeItem*>::iterator it = eg->itFirstChild(); it != eg->itLastChild(); ++it)
			RecursiveReadDoc(hNew, *it);
	}
}

void CriteriaTreeView::RecursiveReadChildren(HTREEITEM hInsertAt, CriteriaTreeItem* ctiReadFrom)
{
	if (0 == hInsertAt) // root
		hInsertAt = TVI_ROOT;
	EffectGroup * eg = dynamic_cast<EffectGroup*>(ctiReadFrom);
	if (eg)
	{
		for (list <CriteriaTreeItem*>::iterator it = eg->itFirstChild(); it != eg->itLastChild(); ++it)
			RecursiveReadDoc(hInsertAt, *it);
	}
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addBreak men.AppendMenu(MF_SEPARATOR);

void CriteriaTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (ctiSelected)
	{
		CMenu men;
		men.CreatePopupMenu();
		ctiSelected->AddContextMenuOptions(men);

		Effect* e = dynamic_cast<Effect*>(ctiSelected);
		if (e)
		{
			add(ID_CT_SHOW_STANDARDIZED);
			men.EnableMenuItem(ID_CT_SHOW_STANDARDIZED, e->fShowStandardizedPossible(iActiveColumn()) ? MF_ENABLED : MF_GRAYED);
		}
		
		addBreak;
		add(ID_CT_EDIT);
		// Extra condition for Edit: either iActCol>0 or mode==EDITTREE, otherwise it looks strange
		bool fEditing = false;
		if (GetDocument())
			fEditing = (GetDocument()->GetMode() == CriteriaTreeDoc::eEDITTREE);
		men.EnableMenuItem(ID_CT_EDIT,(((iActiveColumn()>0)||fEditing) && ctiSelected->fEditAllowed(max(iActiveColumn(), 0))) ? MF_ENABLED : MF_GRAYED);
		add(ID_CT_DELETE);
		men.EnableMenuItem(ID_CT_DELETE, fDeleteAllowed() ? MF_ENABLED : MF_GRAYED);
		EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
		if (eg)
		{
			addBreak;
			add(ID_CT_GENSELECTEDMAP);
			men.EnableMenuItem(ID_CT_GENSELECTEDMAP, eg->fCalculationPossible(iActiveColumn()) ? MF_ENABLED : MF_GRAYED);
		}
		
		add(ID_CT_SHOW);
		men.EnableMenuItem(ID_CT_SHOW, ctiSelected->fDataExists(iActiveColumn()) ? MF_ENABLED : MF_GRAYED);

		if (eg)
			addBreak;
		
		add(ID_CT_HISTOGRAM);
		men.EnableMenuItem(ID_CT_HISTOGRAM, (ctiSelected->fDataExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);

		if (eg)
		{
			if (iActiveColumn() <= 0)
			{
				add(ID_CT_AGGREGATE_VALUES);
				men.EnableMenuItem(ID_CT_AGGREGATE_VALUES, (ctiSelected->fDataExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);
				add(ID_CT_SLICE);
				men.EnableMenuItem(ID_CT_SLICE, (eg->fAllDataExists()) ? MF_ENABLED : MF_GRAYED);
			}
			add(ID_CT_SHOW_SLICED);
			men.EnableMenuItem(ID_CT_SHOW_SLICED, (ctiSelected->fSlicedExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);
			add(ID_CT_HISTOGRAM_SLICED);
			men.EnableMenuItem(ID_CT_HISTOGRAM_SLICED, (ctiSelected->fSlicedExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);
			if (iActiveColumn() <= 0)
			{
				add(ID_CT_AGGREGATE_VALUES_SLICED);
				men.EnableMenuItem(ID_CT_AGGREGATE_VALUES_SLICED, (ctiSelected->fSlicedExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);
			}
			addBreak;
			add(ID_CT_GENERATE_CONTOUR_MAPS);
			men.EnableMenuItem(ID_CT_GENERATE_CONTOUR_MAPS, (ctiSelected && ctiSelected->fDataExists(iActiveColumn())) ? MF_ENABLED : MF_GRAYED);
			add(ID_CT_SHOW_CONTOUR_MAPS);
			men.EnableMenuItem(ID_CT_SHOW_CONTOUR_MAPS, (ctiSelected && (ctiSelected->fContourMapExists(iActiveColumn()))) ? MF_ENABLED : MF_GRAYED);
		}
		if (iActiveColumn()>0)
		{
			addBreak;
			add(ID_CT_PROPERTIES);
			men.EnableMenuItem(ID_CT_PROPERTIES, ctiSelected->fDataExists(iActiveColumn()) ? MF_ENABLED : MF_GRAYED);
		}
	
		men.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, this);
		
		men.DestroyMenu();
	}
}

void CriteriaTreeView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
		// cancel the timer!! no edit wanted
	if (nEditTimer)
	{
		KillTimer(nEditTimer);
		nEditTimer = 0;
	}
	// update our "selected item"
	HTREEITEM hSelectedItem = GetSelectedItem();
	if (hSelectedItem)
		ctiSelected = (CriteriaTreeItem*) GetItemData(hSelectedItem);
	else
		ctiSelected = 0;

	CFrameWnd* cfw = GetTopLevelFrame();
	FrameWindow* fw = dynamic_cast<FrameWindow*>(cfw);
	if (fw)
	{
		if (ctiSelected)
			fw->status->SetWindowText(ctiSelected->sStatusBarText().c_str());
		else
			fw->status->SetWindowText("");
	}
}

void CriteriaTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
		// cancel the timer!! no edit wanted
	if (nEditTimer)
	{
		KillTimer(nEditTimer);
		nEditTimer = 0;
	}
	if ((nChar == VK_INSERT) && (!(nFlags & (2<<14))))
		OnInsert();
	else if ((nChar == VK_F2) && (!(nFlags & (2<<14))))
		PostMessage(WM_COMMAND, ID_CT_EDIT);
	else
		TreeListView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CriteriaTreeView::OnInsert()
{
	EffectGroup* eg = dynamic_cast<EffectGroup*>(ctiSelected);
	if (eg)
	{
		CMenu men;
		men.CreatePopupMenu();
		eg->AddContextMenuOptions(men);

		CRect rect;
		GetTreeCtrl().GetItemRect(eg->hItem, &rect, false);
		// left & right coords of rect are still wrong .. calculate them
		int i;
		int m_nWidth = 0;
		for (i=0; i<iActiveColumn(); ++i) // warning: iActiveColumn() can be -1
			m_nWidth += iGetColumnWidth(i); // left side
		rect.left = iOffset() + m_nWidth+((0==i)?rect.left:0); // leave rect.left for first column
		rect.right = iOffset() + m_nWidth+iGetColumnWidth(i)-2; // right side

		CRect treeRect;
		GetTreeCtrl().GetWindowRect(&treeRect);
	
		men.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, treeRect.left + rect.right, treeRect.top + rect.bottom, this);
		
		men.DestroyMenu();
	}
}

void CriteriaTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	TreeListView::OnUpdate(pSender, lHint, pHint);

	CriteriaTreeDoc* pDoc = GetDocument();
	// default implementation: re-generate the tree from the document
	switch (lHint)
	{
		CriteriaTreeItem* cti;
		case eNODEUPDATED:
			ctiSelected = 0;
			cti = (CriteriaTreeItem*)pHint;
			DeleteChildren(cti->hItem);
			RecursiveReadChildren(cti->hItem, cti);
			Expand(cti->hItem, TVE_EXPAND);
			break;
		case eNODEDELETED:
			cti = (CriteriaTreeItem*)pHint;
			ctiSelected = 0;
			DeleteItem(cti->hItem);
			break;
		case eITEMINSERTED:
			cti = (CriteriaTreeItem*)pHint;
			{
				HTREEITEM hParent = 0;
				if (cti->GetParent())
					hParent = cti->GetParent()->hItem;
				if (0 == hParent) // root - unlikely
					hParent = TVI_ROOT;
				int iIcon = cti->iIconIndex();
				HTREEITEM hPrevItem;
				CriteriaTreeItem* ctiPrev = cti->ctiPreviousItem();
				if (ctiPrev)
					hPrevItem = ctiPrev->hItem;
				else
					hPrevItem = TVI_FIRST;
				HTREEITEM hNew = InsertItem(cti->sDisplayText(0), iIcon, iIcon, hParent, hPrevItem);
				if (hNew)
					SetItemData(hNew, (DWORD)cti); // important!!
				if (cti->GetParent())
					Expand(cti->GetParent()->hItem, TVE_EXPAND);
			}
			break;
		case eITEMUPDATED:
			ctiSelected = 0;
			cti = (CriteriaTreeItem*)pHint;
			SetItemData(cti->hItem, (DWORD)cti); // important
			{
				int iIcon = cti->iIconIndex();
				SetItemImage(cti->hItem, iIcon, iIcon);
			}
			break;
		case eALTNRCHANGED:
			if (pDoc)
			{
				/*
				int iOldNrAlternatives = iColumns()-1;
				if (iOldNrAlternatives > pDoc->iGetNrAlternatives()) // delete some columns
				{
					for (int i = iOldNrAlternatives; i > pDoc->iGetNrAlternatives(); --i)
						DeleteColumn(i);
				}
				else if (iOldNrAlternatives < pDoc->iGetNrAlternatives()) // add some columns
				{
					for (int i = 1 + iOldNrAlternatives; i <= pDoc->iGetNrAlternatives(); ++i)
						InsertColumn(i, pDoc->sAlternative(i), LVCFMT_LEFT, 100);
				}
				*/
				// on second thought delete all and re-insert them -- there's no "rename" yet
				for (int i = iColumns()-1; i > 0; --i)
					DeleteColumn(i);
				for (int i = 1; i <= pDoc->iGetNrAlternatives(); ++i)
					InsertColumn(i, pDoc->sAlternative(i), LVCFMT_LEFT, 140);
			}
			break;
		case eWEIGHTSCHANGED:
			Invalidate();
			break;
		case eEDITMODECHANGED:
			Invalidate();
			break;
		case eLABELCHANGED:
			Invalidate();
			break;
		default:
			if (pDoc)
			{
				ctiSelected = 0; // Otherwise crash on delete
				cti = (CriteriaTreeItem*) pDoc->egRoot();
				HTREEITEM hStart = GetRootItem();
				if (hStart)
				{
					// normal mode
					DeleteChildren(hStart);
					RecursiveReadChildren(hStart, cti);
				}
				else // root also has to be created
					RecursiveReadDoc(hStart, cti);
				Expand(GetRootItem(), TVE_EXPAND);
				// Still arrange the alternatives
				for (int i = iColumns()-1; i > 0; --i)
					DeleteColumn(i);
				for (int i = 1; i <= pDoc->iGetNrAlternatives(); ++i)
					InsertColumn(i, pDoc->sAlternative(i), LVCFMT_LEFT, 140);
			}
	} // end switch

	// still update our "selected item" (follow the CTreeCtrl)
	HTREEITEM hSelectedItem = GetSelectedItem();
	if (hSelectedItem)
		ctiSelected = (CriteriaTreeItem*) GetItemData(hSelectedItem);
	else
		ctiSelected = 0;
}
