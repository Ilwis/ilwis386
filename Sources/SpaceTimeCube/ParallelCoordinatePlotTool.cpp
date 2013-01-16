#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\LayerDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "ParallelCoordinatePlotTool.h"
#include "ColumnTreeCtrl.h"
#include "Drawers\DrawingColor.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Domain\dmsort.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Table\tblview.h"

using namespace ILWIS;

DrawerTool *createParallelCoordinatePlotTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new ParallelCoordinatePlotTool(zv, view, drw);
}

ParallelCoordinatePlotTool::ParallelCoordinatePlotTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("ParallelCoordinatePlotTool",zv, view, drw)
, layerDrawer(0)
, tpw(0)
{
	active = false;
}

ParallelCoordinatePlotTool::~ParallelCoordinatePlotTool()
{
	closeParallelCoordinatePlotWindow();
}

bool ParallelCoordinatePlotTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 
	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	layerDrawer = dynamic_cast<LayerDrawer *>(drawer);
	if ( !layerDrawer)
		return false;
	if (!((SpatialDataDrawer *)(layerDrawer->getParentDrawer()))->getBaseMap()->fTblAtt())
		return false;
	return true;
}

HTREEITEM ParallelCoordinatePlotTool::configure( HTREEITEM parentItem)
{
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&ParallelCoordinatePlotTool::startParallelCoordinatePlotForm);
	htiNode = insertItem(TR("Parallel Coordinate Plot"),"ParallelCoordinatePlot",item);
	DrawerTool::configure(htiNode);
	return htiNode;
}

String ParallelCoordinatePlotTool::getMenuString() const
{
	return TR("Parallel Coordinate Plot");
}

void ParallelCoordinatePlotTool::startParallelCoordinatePlotForm()
{
	closeParallelCoordinatePlotWindow();
	Table & tblAttr = ((SpatialDataDrawer *)(layerDrawer->getParentDrawer()))->getBaseMap()->tblAtt();
	tpw = new ParallelCoordinatePlotWindow(layerDrawer, tblAttr, this);
}

void ParallelCoordinatePlotTool::closeParallelCoordinatePlotWindow()
{
	if (tpw)
		delete tpw;
	tpw = 0;
}

BEGIN_MESSAGE_MAP(ParallelCoordinateGraphWindow, SimpleGraphWindow)
	//{{AFX_MSG_MAP(ParallelCoordinateGraphWindow)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ParallelCoordinateGraphWindow::ParallelCoordinateGraphWindow(LayerDrawer *_layerDrawer, Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns)
: SimpleGraphWindow()
, info(0)
, mousePos(CPoint(-1, -1))
, m_bmMemoryGraph(0)
, m_dcMemoryGraph(0)
, fDrawAxes(false)
, layerDrawer(_layerDrawer)
, tblAttr(_tblAttr)
, columns(_columns)
, m_fSelectionChanged(false)
, m_fAbortSelectionThread(false)
, m_selectionThread(0)
, csColumns(_csColumns)
, tpf(0)
{
}

ParallelCoordinateGraphWindow::~ParallelCoordinateGraphWindow()
{
	if (m_selectionThread)
	{
		m_fAbortSelectionThread = true;
		m_selectionThread->ResumeThread();
		csSelectionThread.Lock(); // wait here til thread exits
		csSelectionThread.Unlock();
	}

	if (info)
		delete info;
}

void ParallelCoordinateGraphWindow::OnDestroy() 
{
	CloseThreads();
	if (m_dcMemory)
	{
		// m_dcMemory->SelectObject(m_bmOldBitmap); // this crashes
		m_dcMemory->DeleteDC();
		delete m_dcMemory;
		delete m_bmMemory;
		m_dcMemoryGraph->DeleteDC();
		delete m_dcMemoryGraph;
		delete m_bmMemoryGraph;
	}
	CWnd::OnDestroy();
}

void ParallelCoordinateGraphWindow::SetInfoForm(ParallelCoordinatePlotForm * _tpf)
{
	tpf = _tpf;
}

BOOL ParallelCoordinateGraphWindow::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	if (SimpleGraphWindow::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext)) {
		info = new InfoLine(this);
		return TRUE;
	} else
		return FALSE;
}

void ParallelCoordinateGraphWindow::StartDrag(CPoint point)
{
	m_fDragging = true;
	mousePos = point;
	SelectionChanged();
}

void ParallelCoordinateGraphWindow::Drag(CPoint point)
{
	if (m_fDragging)
	{
		mousePos = point;
		SelectionChanged();
	}
}

void ParallelCoordinateGraphWindow::EndDrag(CPoint point)
{
	if (m_fDragging) {
		mousePos = point;
		SelectionChanged();
		m_fDragging = false;
	}
}

const unsigned int MAX_CROSSPRODUCT = 1000;
// Test if point(c) is on line (a,b)
unsigned int ParallelCoordinateGraphWindow::iPointOnLine(CPoint c, int ax, int ay, int bx, int by)
{
	unsigned int crossproduct = abs((c.y - ay) * (bx - ax) - (c.x - ax) * (by - ay));
	if (crossproduct >= MAX_CROSSPRODUCT)
		return MAX_CROSSPRODUCT;

	/*
	int dotproduct = (c.x - ax) * (bx - ax) + (c.y - ay) * (by - ay); 
	if (dotproduct < 0)
		return false;

	int squaredlengthba = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
	if (dotproduct > squaredlengthba)
		return false;
	*/
	if (!(((ax <= c.x && c.x <= bx) || (bx <= c.x && c.x <= ax)) && ((ay <= c.y && c.y <= by) || (by <= c.y && c.y <= ay))))
		return MAX_CROSSPRODUCT;
	return crossproduct;
}

double ParallelCoordinateGraphWindow::rGetFx(long iRaw, ColInfo & colInfo)
{
	if (colInfo.fValue) {
		double rValue = colInfo.column->rValue(iRaw); // test on rUNDEF
		if (rValue == rUNDEF)
			rValue = colInfo.rrMinMax.rLo();
		return (rValue - colInfo.rrMinMax.rLo()) / colInfo.rrMinMax.rWidth();
	} else {
		long iValue = colInfo.column->iRaw(iRaw); // test on iUNDEF
		if (iValue == iUNDEF)
			iValue = colInfo.rrMinMax.rLo();
		return (iValue - colInfo.rrMinMax.rLo()) / colInfo.rrMinMax.rWidth();
	}
}

void ParallelCoordinateGraphWindow::SelectionChanged()
{

	if (!m_fAbortSelectionThread)
	{
		m_fSelectionChanged = true;
		if (!m_selectionThread)
			m_selectionThread = AfxBeginThread(SelectionChangedInThread, this);
		else
			m_selectionThread->ResumeThread();
	}
}

UINT ParallelCoordinateGraphWindow::SelectionChangedInThread(LPVOID pParam)
{
	ParallelCoordinateGraphWindow * pObject = (ParallelCoordinateGraphWindow*)pParam;
	if (pObject == NULL)
		return 1;

	pObject->csSelectionThread.Lock();

	while (!pObject->m_fAbortSelectionThread)
	{
		while (!pObject->m_fAbortSelectionThread && pObject->m_fSelectionChanged)
		{
			pObject->m_fSelectionChanged = false;
			pObject->selectedRaws.clear();
			long iNettoRecs = pObject->tblAttr->iNettoRecs();
			unsigned int bestCrossproduct = MAX_CROSSPRODUCT;
			const int iCol0 = max(0, floor(pObject->rScreenToX(pObject->mousePos.x)));
			const int iCol1 = min(pObject->columns.size() - 1, iCol0 + 1);
			if (iCol1 > iCol0) {
				for (long i = 0; i < iNettoRecs; ++i) {
					int iX0 = pObject->iXToScreen(iCol0);
					int iY0 = pObject->iYToScreen(pObject->rGetFx(i + 1, *pObject->columns[iCol0]));
					int iX1 = pObject->iXToScreen(iCol1);
					int iY1 = pObject->iYToScreen(pObject->rGetFx(i + 1, *pObject->columns[iCol1]));
					unsigned int crossproduct = iPointOnLine(pObject->mousePos, iX0, iY0, iX1, iY1);
					if (crossproduct < bestCrossproduct) {
						bestCrossproduct = crossproduct;
						pObject->selectedRaws.clear();
						pObject->selectedRaws.push_back(i + 1);
					} else if (crossproduct == bestCrossproduct && bestCrossproduct < MAX_CROSSPRODUCT)
						pObject->selectedRaws.push_back(i + 1);
				}
			}

			pObject->fDrawAxes = true;
			pObject->SetDirty();

			String sInfo;

			if (pObject->selectedRaws.size() > 0) {
				long iRaw = pObject->selectedRaws[0];
				double rY0 = pObject->rGetFx(iRaw, *pObject->columns[iCol0]);
				double rY1 = pObject->rGetFx(iRaw, *pObject->columns[iCol1]);
				ColInfo & colInfo0 = *pObject->columns[iCol0];
				ColInfo & colInfo1 = *pObject->columns[iCol1];
				rY0 = rY0 * colInfo0.rrMinMax.rWidth() + colInfo0.rrMinMax.rLo();
				rY1 = rY1 * colInfo1.rrMinMax.rWidth() + colInfo1.rrMinMax.rLo();
				if (colInfo0.fValue) {
					DomainValue * pdv = colInfo0.column->dm()->pdv();
					ColumnView cv (0, colInfo0.column);
					sInfo = colInfo0.column->sName() + "=" + pdv->sValue(rY0, cv.iWidth, cv.iDec).sTrimSpaces();
				} else {
					const DomainSort * pdsrt = colInfo0.pdSort;
					sInfo = colInfo0.column->sName() + "=" + pdsrt->sValueByRaw(rY0).sTrimSpaces();
				}
				if (colInfo1.fValue) {
					DomainValue * pdv = colInfo1.column->dm()->pdv();
					ColumnView cv (0, colInfo1.column);
					sInfo += ";" + colInfo1.column->sName() + "=" + pdv->sValue(rY1, cv.iWidth, cv.iDec).sTrimSpaces();
				} else {
					const DomainSort * pdsrt = colInfo1.pdSort;
					sInfo += ";" + colInfo1.column->sName() + "=" + pdsrt->sValueByRaw(rY1).sTrimSpaces();
				}
			}

			if (pObject->m_fDragging)
				pObject->info->text(pObject->mousePos, sInfo);

			// send raws array

			if (pObject->tblAttr.fValid())
				IlwWinApp()->SendUpdateTableSelection(pObject->selectedRaws, pObject->tblAttr->dm()->fnObj, long(pObject));
		}
		if (!pObject->m_fAbortSelectionThread)
		{
			pObject->m_selectionThread->SuspendThread(); // wait here, and dont consume CPU time either
		}
	}

	pObject->m_fAbortSelectionThread = false;
	pObject->csSelectionThread.Unlock();
	return 0;
}

void ParallelCoordinateGraphWindow::SelectFeatures(RowSelectInfo & inf)
{
	if (inf.sender == (long) this)
		return;
	if (!tblAttr.fValid())
		return;

	if (tblAttr->dm()->fnObj == inf.fn)
	{
		selectedRaws = inf.raws;
		fDrawAxes = true; // replots selected functions (and axes)
		SetDirty();
	}
}

void ParallelCoordinateGraphWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	SimpleGraphWindow::OnLButtonDown(nFlags, point);
	//String txt = getInfo(point);
	//info->text(point, txt);
}

void ParallelCoordinateGraphWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	info->text(point, "");
	SimpleGraphWindow::OnLButtonUp(nFlags, point);
}

void ParallelCoordinateGraphWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	SimpleGraphWindow::OnMouseMove(nFlags, point);
	if (m_fDragging) {
		//String txt = getInfo(point);
		//info->text(point, txt);
	}
	if (tpf) {
		String txt = getInfo(point);
		tpf->SetInfo(txt);
	}
}

void ParallelCoordinateGraphWindow::SetDomain(double l, double t, double r, double b)
{
	m_XYFunctionDomain.SetRect(l, t, r, b);
	m_XYFunctionDomain.NormalizeRect();
}

String ParallelCoordinateGraphWindow::getInfo(CPoint point)
{
	CString sVal;
	if (GetFunctionPlotRect().PtInRect(point)) {
		int iX = point.x;
		int iY = point.y;
		iX = round(rScreenToX(iX));
		double rY = rScreenToY(iY);
		ColInfo & colInfo = *columns[iX];
		rY = rY * colInfo.rrMinMax.rWidth() + colInfo.rrMinMax.rLo();
		if (colInfo.fValue) {
			DomainValue * pdv = colInfo.column->dm()->pdv();
			ColumnView cv (0, colInfo.column);
			sVal = (colInfo.column->sName() + "=" + pdv->sValue(rY, cv.iWidth, cv.iDec).sTrimSpaces()).c_str();
		} else {
			const DomainSort * pdsrt = colInfo.pdSort;
			sVal = (colInfo.column->sName() + "=" + pdsrt->sValueByRaw(rY).sTrimSpaces()).c_str();
		}
	}
	return sVal;
}

void ParallelCoordinateGraphWindow::SetDirty(bool fRedraw)
{
	// fRedraw will replot the functions and the axex
	// fRedraw = false, but fDrawAxes will redraw the axes
	// fRedraw = false and fDrawAxes = false will only do BitBlt

	if (!m_fAbortPaintThread)
	{
		m_fDirty = true;
		m_fRedraw = m_fRedraw || fRedraw;
		if (!m_paintThread)
			m_paintThread = AfxBeginThread(PaintInThread, this);
		else
			m_paintThread->ResumeThread();
	}
}

void ParallelCoordinateGraphWindow::DrawFunction(CDC* pDC)
{
	Color currentColor(0, 0, 255);

	// create and select a thin, blue pen
	CPen penTemp;
	penTemp.CreatePen(PS_SOLID, 0, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
	CPen* pOldPen = pDC->SelectObject(&penTemp);

	// Draw the functions here
	DrawingColor * drawingColor = layerDrawer->getDrawingColor();
	NewDrawer::DrawMethod drawMethod = layerDrawer->getDrawMethod();
	long iNettoRecs = tblAttr->iNettoRecs();
	for (long i = 0; i < iNettoRecs; ++i)
	{
		const Color & color = drawingColor->clrRaw(i + 1, drawMethod);
		for (int j = 0; j < columns.size(); ++j) {
			double rY = rGetFx(i + 1, *columns[j]);
			int iX = iXToScreen(j);
			int iY = iYToScreen(rY);
			if (color != currentColor) {
				currentColor = color;
				pDC->SelectObject(pOldPen);
				penTemp.DeleteObject();
				penTemp.CreatePen(PS_SOLID, 0, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
				CPen* pOldPen = pDC->SelectObject(&penTemp);
			}
			if (j == 0)
				pDC->MoveTo(iX, iY);
			else
				pDC->LineTo(iX, iY);
		}
	}

	// put back the old objects

	pDC->SelectObject(pOldPen);
}

void ParallelCoordinateGraphWindow::DrawSelectedFunctions(CDC* pDC)
{
	Color currentColor(0, 0, 255);

	// create and select a thin, blue pen
	CPen penTemp;
	//penTemp.CreatePen(PS_SOLID, 4, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
	penTemp.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penTemp);

	// Draw the functions here
	DrawingColor * drawingColor = layerDrawer->getDrawingColor();
	NewDrawer::DrawMethod drawMethod = layerDrawer->getDrawMethod();
	for (long i = 0; i < selectedRaws.size(); ++i)
	{
		long iRaw = selectedRaws[i];
		const Color & color = drawingColor->clrRaw(iRaw, drawMethod);
		for (int j = 0; j < columns.size(); ++j) {
			double rY = rGetFx(iRaw, *columns[j]);
			int iX = iXToScreen(j);
			int iY = iYToScreen(rY);
			if (color != currentColor) {
				currentColor = color;
				pDC->SelectObject(pOldPen);
				penTemp.DeleteObject();
				//penTemp.CreatePen(PS_SOLID, 4, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
				penTemp.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
				CPen* pOldPen = pDC->SelectObject(&penTemp);
			}
			if (j == 0)
				pDC->MoveTo(iX, iY);
			else
				pDC->LineTo(iX, iY);
		}
	}

	// put back the old objects

	pDC->SelectObject(pOldPen);
}

void ParallelCoordinateGraphWindow::DrawAxes(CDC* pDC)
{
	// draw the axes with a thin black pen
	CPen penBlack(PS_SOLID, 0, RGB(0, 0, 0));
	CPen penGray(PS_SOLID, 0, RGB(210, 210, 210));
	CPen penDarkGray(PS_SOLID, 0, RGB(140, 140, 140));
	CPen* pOldPen = pDC->SelectObject(&penGray);

	CRect functionPlotRect (GetFunctionPlotRect());

	if (!tblAttr.fValid())
		return;

	for (int i = 0; i < columns.size(); ++i) {
		pDC->MoveTo(iXToScreen(i), functionPlotRect.top);
		pDC->LineTo(iXToScreen(i), functionPlotRect.bottom);
	}

	// draw the grid
	pDC->SelectObject(&penDarkGray);
	// draw the axes
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(functionPlotRect.left, functionPlotRect.top);
	pDC->LineTo(functionPlotRect.left, functionPlotRect.bottom);
	pDC->LineTo(functionPlotRect.right, functionPlotRect.bottom);

	// draw ticks on the axes		
	int iTickThickness = 3; // nr of pixels for drawing a tick
	for (int i = 0; i < columns.size(); ++i) {
		pDC->MoveTo(iXToScreen(i), functionPlotRect.bottom);
		pDC->LineTo(iXToScreen(i), functionPlotRect.bottom + iTickThickness);
	}

	// draw axis values
	// x-axis values
	CFont * oldFont = 0;
	if (m_fnt)
		oldFont = pDC->SelectObject(m_fnt);
	UINT iPreviousAlignment = pDC->SetTextAlign(TA_CENTER | TA_TOP);
	COLORREF clrPreviousColor = pDC->SetTextColor(RGB(0, 0, 0)); // black text
	CString sVal;
	for (int i = 0; i < columns.size(); ++i) {
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut(iXToScreen(i), functionPlotRect.bottom + iTickThickness, columns[i]->column->sName().c_str());
	}

	pDC->SetTextColor(clrPreviousColor);
	pDC->SetTextAlign(iPreviousAlignment);
	if (m_fnt) // take care not to change this: m_fnt determines whether SelectObject should be called or not (in order to "release" m_fnt)
		pDC->SelectObject(oldFont);
	
	// Put back the old objects.
	pDC->SelectObject(pOldPen);
}

void ParallelCoordinateGraphWindow::DrawMouse(CDC* pDC)
{
	CRect functionPlotRect (GetFunctionPlotRect());
	if (functionPlotRect.PtInRect(mousePos))
	{
		CPen penLightBlue(PS_SOLID, 0, RGB(180, 180, 255));
		CPen* pOldPen = pDC->SelectObject(&penLightBlue);

		// draw the cross
		pDC->MoveTo(mousePos.x, functionPlotRect.top);
		pDC->LineTo(mousePos.x, functionPlotRect.bottom);
		pDC->MoveTo(functionPlotRect.left, mousePos.y);
		pDC->LineTo(functionPlotRect.right, mousePos.y);

		pDC->SelectObject(pOldPen);
	}
}

UINT ParallelCoordinateGraphWindow::PaintInThread(LPVOID pParam)
{
	ParallelCoordinateGraphWindow * pObject = (ParallelCoordinateGraphWindow*)pParam;
	if (pObject == NULL)
		return 1;

	pObject->csThread.Lock();

	CDC* pDC = pObject->GetDC();

	CRect rectClient;
	pObject->GetClientRect(rectClient);
	if (!pObject->m_dcMemory)
	{
		pObject->m_bmMemory = new CBitmap();
		pObject->m_bmMemory->CreateCompatibleBitmap(pDC, rectClient.Width(),rectClient.Height());
		pObject->m_bmMemoryGraph = new CBitmap();
		pObject->m_bmMemoryGraph->CreateCompatibleBitmap(pDC, rectClient.Width(),rectClient.Height());

		pObject->m_dcMemory = new CDC();
		pObject->m_dcMemory->CreateCompatibleDC(pDC);
		pObject->m_dcMemoryGraph = new CDC();
		pObject->m_dcMemoryGraph->CreateCompatibleDC(pDC);

		pObject->m_bmOldBitmap = (CBitmap*)(pObject->m_dcMemory)->SelectObject(pObject->m_bmMemory);
		pObject->m_bmOldBitmapGraph = (CBitmap*)(pObject->m_dcMemoryGraph)->SelectObject(pObject->m_bmMemoryGraph);

		CRgn bounds;
		bounds.CreateRectRgnIndirect(rectClient);

		pObject->m_dcMemory->SelectClipRgn(&bounds, RGN_AND);
		pObject->m_dcMemoryGraph->SelectClipRgn(&bounds, RGN_AND);
	}

	while (!pObject->m_fAbortPaintThread)
	{
		while (pObject->m_fDirty)
		{
			pObject->m_fDirty = false;

			// resize bitmaps

			CRect rectClientNew;
			pObject->GetClientRect(rectClientNew);
			while (!pObject->m_fAbortPaintThread && rectClientNew != rectClient)
			{
				rectClient = rectClientNew;

				pObject->m_dcMemory->SelectObject(pObject->m_bmOldBitmap);
				pObject->m_dcMemoryGraph->SelectObject(pObject->m_bmOldBitmapGraph);
				delete pObject->m_bmMemory;
				delete pObject->m_bmMemoryGraph;

				pObject->m_bmMemory = new CBitmap();
				pObject->m_bmMemory->CreateCompatibleBitmap(pDC, rectClient.Width(),rectClient.Height());
				pObject->m_bmMemoryGraph = new CBitmap();
				pObject->m_bmMemoryGraph->CreateCompatibleBitmap(pDC, rectClient.Width(),rectClient.Height());

				pObject->m_bmOldBitmap = (CBitmap*)(pObject->m_dcMemory)->SelectObject(pObject->m_bmMemory);
				pObject->m_bmOldBitmapGraph = (CBitmap*)(pObject->m_dcMemoryGraph)->SelectObject(pObject->m_bmMemoryGraph);

				CRgn bounds;
				bounds.CreateRectRgnIndirect(rectClient);

				pObject->m_dcMemory->SelectClipRgn(&bounds, RGN_COPY);
				pObject->m_dcMemoryGraph->SelectClipRgn(&bounds, RGN_COPY);
				
				pObject->GetClientRect(rectClientNew);
				pObject->m_fRedraw = true;
			}

			// redraw functions

			pObject->csColumns.Lock();

			while (!pObject->m_fAbortPaintThread && pObject->m_fRedraw)
			{
				pObject->m_fRedraw = false;
				pObject->m_dcMemoryGraph->FillSolidRect(rectClient, RGB(255, 255, 255)); // white background
				pObject->DrawFunction(pObject->m_dcMemoryGraph);
				pObject->fDrawAxes = true;
			}

			// redraw axes

			while (!pObject->m_fAbortPaintThread && !pObject->m_fDirty && pObject->fDrawAxes)
			{
				pObject->fDrawAxes = false;
				pObject->m_dcMemory->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), pObject->m_dcMemoryGraph, 0, 0, SRCCOPY);
				pObject->DrawSelectedFunctions(pObject->m_dcMemory);
				pObject->DrawAxes(pObject->m_dcMemory);
				pObject->DrawMouse(pObject->m_dcMemory);
			}

			pObject->csColumns.Unlock();
		}
		if (!pObject->m_fAbortPaintThread)
		{
			pDC->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), pObject->m_dcMemory, 0, 0, SRCCOPY);
			pObject->m_paintThread->SuspendThread(); // wait here, and dont consume CPU time either
		}
	}

	pObject->ReleaseDC(pDC);
	pObject->m_fAbortPaintThread = false;
	pObject->csThread.Unlock();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ParallelCoordinatePlotForm, FormBaseWnd)
	//{{AFX_MSG_MAP(ParallelCoordinatePlotForm)
	ON_MESSAGE(MESSAGE_SELECT_ROW, OnSelectFeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ParallelCoordinatePlotForm::ParallelCoordinatePlotForm(CWnd* mw, LayerDrawer *_layerDrawer, Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns, ParallelCoordinateGraphWindow * _pgw)
: FormBaseWnd(mw, TR("Parallel Coordinate Plot of Point Map"), fbsSHOWALWAYS|fbsBUTTONSUNDER|fbsNOCANCELBUTTON|fbsNOOKBUTTON|fbsOKHASCLOSETEXT, WS_CHILD|DS_3DLOOK)
, layerDrawer(_layerDrawer)
, tblAttr(_tblAttr)
, columns(_columns)
, csColumns(_csColumns)
, pgw(_pgw)
{
	CFont * fnt = IlwWinApp()->GetFont(IlwisWinApp::sfGRAPH);
	pgw->SetAxisFont(fnt);
	fsInfo = new FieldString(root, &sDummy, ES_READONLY);
	fsInfo->SetIndependentPos();
	fsInfo->SetWidth(180);

	create();
	
	ComputePcp();

	pgw->SetBorderThickness(30, 20, 30, 20);
	//pgw->Replot();

	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)m_hWnd, 0);
}

ParallelCoordinatePlotForm::~ParallelCoordinatePlotForm()
{
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 0);
}

LONG ParallelCoordinatePlotForm::OnSelectFeatures(UINT wParam, LONG lParam)
{
	RowSelectInfo * inf = (RowSelectInfo *)wParam;
	pgw->SelectFeatures(*inf);
	delete inf;

	return 1;
}

void ParallelCoordinatePlotForm::SetInfo(String & sInfo)
{
	fsInfo->SetVal(sInfo);
}

void ParallelCoordinatePlotForm::ComputePcp()
{
	if (!tblAttr.fValid())
		return;

	csColumns.Lock();

	for (int i = 0; i < columns.size(); ++i)
		delete columns[i];
	columns.clear();

	for (int j = 0; j < tblAttr->iCols(); ++j) {
		RangeReal rrMinMax;
		const Column & col = tblAttr->col(j);
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
	pgw->SetDomain(0, 1, columns.size() - 1, 0);
	csColumns.Unlock();
}

BEGIN_MESSAGE_MAP(ParallelCoordinatePlotWindow, CWnd)
//{{AFX_MSG_MAP(ParallelCoordinatePlotWindow)
ON_WM_SIZE()
ON_WM_CLOSE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Setup the indicator fields for the status bar
static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

ParallelCoordinatePlotWindow::ParallelCoordinatePlotWindow(LayerDrawer * layerDrawer, Table & tblAttr, ParallelCoordinatePlotTool * _tpt)
: sgw(0)
, ctc(0)
, tpf(0)
, tpt(_tpt)
{
	// take two third of screen as default size
	int iWidth = GetSystemMetrics(SM_CXSCREEN) * 2 / 3;
	int iHeight = GetSystemMetrics(SM_CYSCREEN) * 2 / 3;

	DWORD dwStyle = WS_VISIBLE|WS_POPUP|WS_BORDER|WS_CAPTION|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_OVERLAPPEDWINDOW|WS_POPUPWINDOW|WS_SIZEBOX|WS_SYSMENU|WS_THICKFRAME|DS_3DLOOK;
	DWORD dwExStyle = 0;
	CreateEx(dwExStyle, "IlwisView", String("Parallel Coordinate Plot of Table '%S'", tblAttr->sName()).c_str(), dwStyle, CRect(0, 0, iWidth, iHeight), 0, 0);
	sgw = new ParallelCoordinateGraphWindow(layerDrawer, tblAttr, columns, csColumns);
	sgw->Create(NULL, "Graph", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0);
	tpf = new ParallelCoordinatePlotForm(this, layerDrawer, tblAttr, columns, csColumns, sgw);
	sgw->SetInfoForm(tpf);
	ctc = new ColumnTreeCtrl(tblAttr, columns, csColumns, sgw);
	ctc->Create(TVS_DISABLEDRAGDROP | TVS_CHECKBOXES | TVS_NOSCROLL /*| TVS_NOSCROLL */ /*| WS_BORDER */ | WS_TABSTOP | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0);
	ctc->ModifyStyle(TVS_CHECKBOXES, 0); // otherwise everything is unchecked when doing ctc->InitColumnTree()
	ctc->ModifyStyle(0, TVS_CHECKBOXES);
	ctc->InitColumnTree();

	OnSize(0, 0, 0);
}

void ParallelCoordinatePlotWindow::OnSize(UINT nType, int cx, int cy)
{
	if (tpf) {
		CRect rectWindow;
		GetClientRect(rectWindow);
		CRect rectForm;
		tpf->GetClientRect(rectForm);
		int newX = 0;
		int newY = rectWindow.Height() - rectForm.Height();
		rectForm.OffsetRect(newX - rectForm.TopLeft().x, newY - rectForm.TopLeft().y);
		tpf->MoveWindow(rectForm);
		CRect rectColumns;
		ctc->GetClientRect(rectColumns);
		CPoint pt1 (rectWindow.TopLeft());
		CPoint pt2(100, newY);
		pt2 += rectWindow.TopLeft();
		rectColumns.SetRect(pt1.x, pt1.y, pt2.x, pt2.y);
		CRect rectGraph;
		sgw->GetClientRect(rectGraph);
		pt1 = rectWindow.TopLeft();
		pt1.x += 100;
		pt2 = CPoint(rectWindow.Width() - 100, newY);
		pt2 += rectWindow.TopLeft();
		pt2.x += 100;
		rectGraph.SetRect(pt1.x, pt1.y, pt2.x, pt2.y);
		sgw->MoveWindow(rectGraph);
		ctc->MoveWindow(rectColumns);
		//sgw->OnSize(nType, rectGraph.Width(), rectGraph.Height());
		sgw->Replot();
	}
}

void ParallelCoordinatePlotWindow::OnClose()
{
	if (tpt)
		tpt->closeParallelCoordinatePlotWindow();
	else
		delete this;
}

ParallelCoordinatePlotWindow::~ParallelCoordinatePlotWindow()
{
	if (tpf)
		delete tpf;
	if (ctc)
		delete ctc;
	if (sgw)
		delete sgw;
	csColumns.Lock();
	for (int i = 0; i < columns.size(); ++i)
		delete columns[i];
	columns.clear();
	csColumns.Unlock();
}