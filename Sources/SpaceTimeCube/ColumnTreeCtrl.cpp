// ColumnTreeCtrl.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "ColumnTreeCtrl.h"

#include "Engine\Drawers\ComplexDrawer.h"

#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Drawers\LayerDrawer.h"

#include "ParallelCoordinatePlotTool.h"
#include "Engine\Domain\dmsort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ILWIS;

/////////////////////////////////////////////////////////////////////////////
// ColumnTreeCtrl

ColumnTreeCtrl::ColumnTreeCtrl(Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns, ParallelCoordinateGraphWindow * _pgw)
: tblAttr(_tblAttr)
, columns(_columns)
, csColumns(_csColumns)
, pgw(_pgw)
, fLMouseButtonDown(false)
, hDragSource(NULL)
{
}

ColumnTreeCtrl::~ColumnTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(ColumnTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(ColumnTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ColumnTreeCtrl message handlers

vector <int> ColumnTreeCtrl::viColumns()
{
	vector <int> viResult;
	
	HTREEITEM hItem = GetRootItem();
	while (hItem != NULL)
	{
		if (GetCheck(hItem))
			viResult.push_back(GetItemData(hItem));
		hItem = GetNextSiblingItem(hItem);
	}

	return viResult;
}

void ColumnTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	fLMouseButtonDown = true;

	CTreeCtrl::OnLButtonDown(nFlags, point);

	pDragStartPoint = point;

	UINT uFlags;
	hDragSource = HitTest(point, &uFlags);
	if ((uFlags & TVHT_ONITEMLABEL) != TVHT_ONITEMLABEL)
		hDragSource = NULL;
}

void ColumnTreeCtrl::HandleButtonUp(UINT nFlags, CPoint point)
{
	HTREEITEM hDragTo = HitTest(point);
	SetInsertMark(NULL);

	if (fLMouseButtonDown && (hDragSource != NULL) && (hDragTo != NULL) && (hDragSource != hDragTo))
	{
		// backup old item data
		CString sLabel = GetItemText(hDragSource);
		DWORD iColumnNr = GetItemData(hDragSource);
		BOOL fChecked = GetCheck(hDragSource);
		// delete old item
		DeleteItem(hDragSource);
		// insert new item
		HTREEITEM hInsertAfter;
		if (point.y < pDragStartPoint.y) // upwards
		{
			hInsertAfter = GetPrevSiblingItem(hDragTo);
			if (hInsertAfter == NULL)
				hInsertAfter = TVI_FIRST;
		}
		else
		{
			hInsertAfter = hDragTo;
			if (hInsertAfter == NULL)
				hInsertAfter = TVI_LAST;
		}

		HTREEITEM hNewItem = InsertItem(TVIF_PARAM|TVIF_TEXT, sLabel, 0, 0, 0, 0, (LPARAM)iColumnNr, TVI_ROOT, hInsertAfter);
		if (fChecked)
			SetCheck(hNewItem);
		SelectItem(hNewItem);
	}

	fLMouseButtonDown = false;
	ComputePcp();
}

void ColumnTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	HandleButtonUp(nFlags, point);
}

void ColumnTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnMouseMove(nFlags, point);
	if (fLMouseButtonDown && (hDragSource != NULL))
	{
		HTREEITEM hCandidateDragItem = HitTest(point);
		// SelectItem(hCandidateDragItem);
		if (point.y < pDragStartPoint.y) // upwards
			SetInsertMark(hCandidateDragItem, FALSE);
		else
			SetInsertMark(hCandidateDragItem, TRUE);
	}
}


void ColumnTreeCtrl::InitColumnTree()
{
	if (GetCount() > 0)
		DeleteAllItems();

	for (int i = 0; i < tblAttr->iCols(); ++i) {
		HTREEITEM hNewItem = InsertItem(TVIF_PARAM|TVIF_TEXT, tblAttr->col(i)->sName().c_str(), 0, 0, 0, 0, (LPARAM)(i), TVI_ROOT, TVI_LAST);
		SetCheck(hNewItem);
	}
}


void ColumnTreeCtrl::ComputePcp()
{
	if (!tblAttr.fValid())
		return;

	csColumns.Lock();

	for (int i = 0; i < columns.size(); ++i)
		delete columns[i];
	columns.clear();

	HTREEITEM hItem = GetRootItem();
	while (hItem != NULL) {
		if (GetCheck(hItem)) {
			DWORD iCol = GetItemData(hItem);
			RangeReal rrMinMax;
			const Column & col = tblAttr->col(iCol);
			bool fVal = col->dvrs().fValues();
			DomainSort* pdSort = 0;
			if (fVal) {
				rrMinMax = col->rrMinMax();
				ColInfo * colInfo = new ColInfo(fVal, pdSort, col, rrMinMax);
				columns.push_back(colInfo);
			} else {
				pdSort = col->dm()->pdsrt();
				if (pdSort) {
					double rMin = 1;
					double rMax = pdSort->iSize();
					rrMinMax.rLo() = rMin;
					rrMinMax.rHi() = rMax;
					ColInfo * colInfo = new ColInfo(fVal, pdSort, col, rrMinMax);
					columns.push_back(colInfo);
				}
			}
		}
		hItem = GetNextSiblingItem(hItem);
	}

	pgw->SetDomain(0, 1, columns.size() - 1, 0);
	csColumns.Unlock();
	pgw->Replot();
}
