#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "SpaceTimePathDrawer.h"
#include "TimeProfileTool.h"
#include "Engine\Map\Segment\Seg.h"
#include "Drawers\DrawingColor.h"
#include "Client\Mapwindow\InfoLine.h"
#include "geos\headers\geos\algorithm\distance\DistanceToPoint.h"
#include "geos\headers\geos\algorithm\distance\PointPairDistance.h"
#include "geos\headers\geos\linearref\LengthIndexedLine.h"
#include "geos\headers\geos\operation\linemerge\LineMerger.h"

using namespace ILWIS;

DrawerTool *createTimeProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new TimeProfileTool(zv, view, drw);
}

TimeProfileTool::TimeProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool("TimeProfileTool",zv, view, drw)
, stpdrw(0)
{
}

TimeProfileTool::~TimeProfileTool()
{
}

bool TimeProfileTool::isToolUseableFor(ILWIS::DrawerTool *tool) { 

	LayerDrawerTool *ldrwt = dynamic_cast<LayerDrawerTool *>(tool);
	if ( !ldrwt )
		return false;
	stpdrw = dynamic_cast<SpaceTimePathDrawer *>(drawer);
	if ( !stpdrw)
		return false;

	return true;
}

HTREEITEM TimeProfileTool::configure( HTREEITEM parentItem)
{
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setDoubleCickAction(this, (DTDoubleClickActionFunc)&TimeProfileTool::startTimeProfileForm);
	htiNode =  insertItem(TR("Time Profile"),"TimeProfile",item);
	DrawerTool::configure(htiNode);
	return htiNode;
}

String TimeProfileTool::getMenuString() const
{
	return TR("Time Profile");
}

void TimeProfileTool::startTimeProfileForm()
{
	new TimeProfileForm(tree, stpdrw);
}

BEGIN_MESSAGE_MAP(ProfileGraphWindow, SimpleGraphWindowWrapper)
	//{{AFX_MSG_MAP(ProfileGraphWindow)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ProfileGraphWindow::ProfileGraphWindow(FormEntry *f)
: SimpleGraphWindowWrapper(f)
, m_gridXN(false)
, m_gridXT(false)
, m_gridYT(false)
, info(0)
, mousePos(CPoint(-1, -1))
, m_bmMemoryGraph(0)
, m_dcMemoryGraph(0)
, fDrawAxes(false)
{
}

ProfileGraphWindow::~ProfileGraphWindow()
{
	delete info;
}

void ProfileGraphWindow::OnDestroy() 
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

BOOL ProfileGraphWindow::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	info = new InfoLine(this);
	return SimpleGraphWindowWrapper::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void ProfileGraphWindow::StartDrag(CPoint point)
{
	m_fDragging = true;
	mousePos = point;
	fDrawAxes = true;
	SetDirty();
}

void ProfileGraphWindow::Drag(CPoint point)
{
	if (m_fDragging)
	{
		mousePos = point;
		fDrawAxes = true;
		SetDirty();
	}
}

void ProfileGraphWindow::EndDrag(CPoint point)
{
	if (m_fDragging) {
		mousePos = point;
		fDrawAxes = true;
		SetDirty();
		m_fDragging = false;
	}
}

void ProfileGraphWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	SimpleGraphWindowWrapper::OnLButtonDown(nFlags, point);
	//String txt = getInfo(point);
	//info->text(point, txt);
}

void ProfileGraphWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	//info->text(point, "");
	SimpleGraphWindowWrapper::OnLButtonUp(nFlags, point);
}

void ProfileGraphWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	SimpleGraphWindowWrapper::OnMouseMove(nFlags, point);
	if (m_fDragging) {
		//String txt = getInfo(point);
		//info->text(point, txt);
	}
}

void ProfileGraphWindow::SetFunctions(SimpleFunction * funPtr, int _iNrFunctions)
{
	iNrFunctions = _iNrFunctions;
	SetFunction(funPtr);
}

void ProfileGraphWindow::SetGridTicks(vector<double> & gridXNodes, vector<double> & gridXTicks, vector<double> & gridYTicks)
{
	m_gridXNodes = gridXNodes;
	m_gridXTicks = gridXTicks;
	m_gridYTicks = gridYTicks;
}

void ProfileGraphWindow::SetGrid(bool gridXN, bool gridXT, bool gridYT)
{
	m_gridXN = gridXN;
	m_gridXT = gridXT;
	m_gridYT = gridYT;
	fDrawAxes = true;
	SetDirty();
}

String ProfileGraphWindow::getInfo(CPoint point)
{
	int iX = point.x;
	int iY = point.y;
	double rX = rScreenToX(iX);
	double rY = rScreenToY(iY);

	CString sVal;
	sVal.Format("%.02f", rX);
	sVal = sVal + ", " + ILWIS::Time(rY).toString().c_str();

	return sVal;
}

void ProfileGraphWindow::SetDirty(bool fRedraw)
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

void ProfileGraphWindow::DrawFunction(CDC* pDC, const SimpleFunction * pFunc)
{
	Color currentColor(0, 0, 255);

	// create and select a thin, blue pen
	CPen penTemp;
	penTemp.CreatePen(PS_SOLID, 0, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
	CPen* pOldPen = pDC->SelectObject(&penTemp);

	// Draw the functions here
	if (pFunc)
	{
		for (int i = 0; i < iNrFunctions; ++i)
		{
			ProfileGraphFunction * pFunction = &((ProfileGraphFunction*)pFunc)[i];
			CRect functionPlotRect (GetFunctionPlotRect());
			int iY = functionPlotRect.top;
			double rY = rScreenToY(iY);
			double rX = pFunction->rGetFx(rY);
			Color color = pFunction->GetColor(rY);
			if (color != currentColor) {
				currentColor = color;
				pDC->SelectObject(pOldPen);
				penTemp.DeleteObject();
				penTemp.CreatePen(PS_SOLID, 0, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
				CPen* pOldPen = pDC->SelectObject(&penTemp);
			}
			int iX = iXToScreen(rX);
			pDC->MoveTo(iX, iY);
			for (iY = functionPlotRect.top + 1; iY <= functionPlotRect.bottom; ++iY)
			{
				rY = rScreenToY(iY);
				rX = pFunction->rGetFx(rY);
				color = pFunction->GetColor(rY);
				if (color != currentColor) {
					currentColor = color;
					pDC->SelectObject(pOldPen);
					penTemp.DeleteObject();
					penTemp.CreatePen(PS_SOLID, 0, RGB(currentColor.red(), currentColor.green(), currentColor.blue()));
					CPen* pOldPen = pDC->SelectObject(&penTemp);
				}
				iX = iXToScreen(rX);
				if (rY < pFunction->rFirstX() || rY > pFunction->rLastX())
					pDC->MoveTo(iX, iY);
				else
					pDC->LineTo(iX, iY);
			}
			if (rY >= pFunction->rFirstX() && rY <= pFunction->rLastX())
				pDC->LineTo(iX, iY); // include the last point in the plot
		}
	}

	// put back the old objects

	pDC->SelectObject(pOldPen);
}

void ProfileGraphWindow::DrawAxes(CDC* pDC)
{
	// draw the axes with a thin black pen
	CPen penBlack(PS_SOLID, 0, RGB(0, 0, 0));
	CPen penGray(PS_SOLID, 0, RGB(210, 210, 210));
	CPen penDarkGray(PS_SOLID, 0, RGB(140, 140, 140));
	CPen* pOldPen = pDC->SelectObject(&penGray);

	CRect functionPlotRect (GetFunctionPlotRect());
	// draw the grid
	if (m_gridXT)
		for (int i = 0; i < m_gridXTicks.size(); ++i) {
			pDC->MoveTo(iXToScreen(m_gridXTicks[i]), functionPlotRect.top);
			pDC->LineTo(iXToScreen(m_gridXTicks[i]), functionPlotRect.bottom);
		}
	if (m_gridYT)
		for (int i = 0; i < m_gridYTicks.size(); ++i) {
			pDC->MoveTo(functionPlotRect.left, iYToScreen(m_gridYTicks[i]));
			pDC->LineTo(functionPlotRect.right, iYToScreen(m_gridYTicks[i]));
		}
	pDC->SelectObject(&penDarkGray);
	if (m_gridXN)
		for (int i = 0; i < m_gridXNodes.size(); ++i) {
			pDC->MoveTo(iXToScreen(m_gridXNodes[i]), functionPlotRect.top);
			pDC->LineTo(iXToScreen(m_gridXNodes[i]), functionPlotRect.bottom);
		}
	// draw the axes
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(functionPlotRect.left, functionPlotRect.top);
	pDC->LineTo(functionPlotRect.left, functionPlotRect.bottom);
	pDC->LineTo(functionPlotRect.right, functionPlotRect.bottom);

	// draw ticks on the axes		
	int iTickThickness = 3; // nr of pixels for drawing a tick
	for (int i = 0; i < m_gridXTicks.size(); ++i) {
		pDC->MoveTo(iXToScreen(m_gridXTicks[i]), functionPlotRect.bottom);
		pDC->LineTo(iXToScreen(m_gridXTicks[i]), functionPlotRect.bottom + iTickThickness);
	}
	for (int i = 0; i < m_gridYTicks.size(); ++i) {
		pDC->MoveTo(functionPlotRect.left, iYToScreen(m_gridYTicks[i]));
		pDC->LineTo(functionPlotRect.left - iTickThickness, iYToScreen(m_gridYTicks[i]));
	}

	// draw axis values
	// x-axis values
	CFont * oldFont = 0;
	if (m_fnt)
		oldFont = pDC->SelectObject(m_fnt);
	UINT iPreviousAlignment = pDC->SetTextAlign(TA_CENTER | TA_TOP);
	COLORREF clrPreviousColor = pDC->SetTextColor(RGB(0, 0, 0)); // black text
	CString sVal;
	if (m_XYFunctionDomain.fValid()) {
		sVal.Format("%.02f", m_XYFunctionDomain.left);
		pDC->TextOut(functionPlotRect.left, functionPlotRect.bottom + iTickThickness, sVal);
		sVal.Format("%.02f", m_XYFunctionDomain.right);
		pDC->TextOut(functionPlotRect.right, functionPlotRect.bottom + iTickThickness, sVal);
	// y-axis values
		pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
		sVal = ILWIS::Time(m_XYFunctionDomain.top).toString().c_str();
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.top, sVal);
		sVal = ILWIS::Time(m_XYFunctionDomain.bottom).toString().c_str();
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.bottom, sVal);
	}

	pDC->SetTextColor(clrPreviousColor);
	pDC->SetTextAlign(iPreviousAlignment);
	if (m_fnt) // take care not to change this: m_fnt determines whether SelectObject should be called or not (in order to "release" m_fnt)
		pDC->SelectObject(oldFont);
	
	// Put back the old objects.
	pDC->SelectObject(pOldPen);
}

void ProfileGraphWindow::DrawMouse(CDC* pDC)
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

		// draw axis values
		// x-axis values
		int iTickThickness = 3; // nr of pixels for drawing a tick
		CFont * oldFont = 0;
		if (m_fnt)
			oldFont = pDC->SelectObject(m_fnt);
		UINT iPreviousAlignment = pDC->SetTextAlign(TA_CENTER | TA_TOP);
		COLORREF clrPreviousColor = pDC->SetTextColor(RGB(0, 0, 0)); // black text
		CString sVal;
		if (m_XYFunctionDomain.fValid()) {
			sVal.Format("%.02f", rScreenToX(mousePos.x));
			pDC->TextOut(mousePos.x, functionPlotRect.bottom + iTickThickness, sVal);
		// y-axis values
			pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
			sVal = ILWIS::Time(rScreenToY(mousePos.y)).toString().c_str();
			pDC->TextOut(functionPlotRect.left - iTickThickness, mousePos.y, sVal);
		}

		pDC->SetTextColor(clrPreviousColor);
		pDC->SetTextAlign(iPreviousAlignment);
		if (m_fnt) // take care not to change this: m_fnt determines whether SelectObject should be called or not (in order to "release" m_fnt)
			pDC->SelectObject(oldFont); // Put back the old objects.
			pDC->SelectObject(pOldPen);
	}
}

UINT ProfileGraphWindow::PaintInThread(LPVOID pParam)
{
	ProfileGraphWindow * pObject = (ProfileGraphWindow*)pParam;
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

			while (!pObject->m_fAbortPaintThread && pObject->m_fRedraw)
			{
				pObject->m_fRedraw = false;
				pObject->m_dcMemoryGraph->FillSolidRect(rectClient, RGB(255, 255, 255)); // white background
				pObject->DrawFunction(pObject->m_dcMemoryGraph, pObject->m_pFunc);
				pObject->fDrawAxes = true;
			}

			while (!pObject->m_fAbortPaintThread && !pObject->m_fDirty && pObject->fDrawAxes)
			{
				pObject->fDrawAxes = false;
				pObject->m_dcMemory->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), pObject->m_dcMemoryGraph, 0, 0, SRCCOPY);
				pObject->DrawAxes(pObject->m_dcMemory);
				pObject->DrawMouse(pObject->m_dcMemory);
			}
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

ProfileFieldGraph::ProfileFieldGraph(FormEntry* parent)
: FieldGraph(parent)
{
}

void ProfileFieldGraph::create()
{
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight);

	sgw = new ProfileGraphWindow(this);
	sgw->Create(NULL, "Graph", WS_CHILD | WS_VISIBLE, CRect(pntFld, dimFld), _frm->wnd(), Id());

  CreateChildren();
}

void ProfileFieldGraph::SetFunctions(SimpleFunction * funPtr, int iNrFunctions)
{
	if (sgw) ((ProfileGraphWindow*)sgw)->SetFunctions(funPtr, iNrFunctions);	
}

void ProfileFieldGraph::SetGridTicks(vector<double> & gridXNodes, vector<double> & gridXTicks, vector<double> & gridYTicks)
{
	if (sgw) ((ProfileGraphWindow*)sgw)->SetGridTicks(gridXNodes, gridXTicks, gridYTicks);
}

void ProfileFieldGraph::SetGrid(bool gridXN, bool gridXT, bool gridYT)
{
	if (sgw) ((ProfileGraphWindow*)sgw)->SetGrid(gridXN, gridXT, gridYT);
}

ProfileGraphFunction::ProfileGraphFunction()
: SimpleFunction(0, DoubleRect(0, 0, 1, 1))
, iSelectedAnchorNr(-1)
{
	m_rDataX.resize(0);
	m_rDataY.resize(0);
}

ProfileGraphFunction::~ProfileGraphFunction()
{
}

void ProfileGraphFunction::SetDomain(double l, double t, double r, double b)
{
	m_Domain.SetRect(l, t, r, b);
}

double ProfileGraphFunction::rFirstX()
{
	if (m_rDataX.size() > 0)
		return m_rDataX[0];
	else
		return m_Domain.left;
}

double ProfileGraphFunction::rLastX()
{
	if (m_rDataX.size() > 0)
		return m_rDataX[m_rDataX.size() - 1];
	else
		return m_Domain.right;
}

double ProfileGraphFunction::rGetFx(double x) const
{
	int iDataSize = m_rDataX.size();
	if (iDataSize > 0)
	{
		int iX = 0;
		while (iX < iDataSize && m_rDataX[iX] < x)
			++iX;
		// iX >= iDataSize || m_rDataX[iX] >= x
		if (iX == 0)
			return m_rDataY[0];
		else if (iX >= iDataSize)
			return m_rDataY[iDataSize - 1];
		else if (m_rDataX[iX] == x)
			return m_rDataY[iX];
		else // normal case, linear interpolation
		{
			double rLeftX = m_rDataX[iX - 1];
			double rRightX = m_rDataX[iX];
			double rLeftY = m_rDataY[iX - 1];
			double rRightY = m_rDataY[iX];
			double portion = (x - rLeftX) / (rRightX - rLeftX);
			double rResult = rLeftY + portion * (rRightY - rLeftY);
			return rResult;
		}
	}
	else
		return m_Domain.bottom;
}

Color ProfileGraphFunction::GetColor(double x) const
{
	int iDataSize = m_rDataX.size();
	if (iDataSize > 0)
	{
		int iX = 0;
		while (iX < iDataSize && m_rDataX[iX] < x)
			++iX;
		// iX >= iDataSize || m_rDataX[iX] >= x
		if (iX == 0)
			return m_colors[0];
		else if (iX >= iDataSize)
			return m_colors[iDataSize - 1];
		else if (m_rDataX[iX] == x)
			return m_colors[iX];
		else // normal case, linear interpolation
		{
			double rLeftX = m_rDataX[iX - 1];
			double rRightX = m_rDataX[iX];
			Color colLeft = m_colors[iX - 1];
			Color colRight = m_colors[iX];
			double portion = (x - rLeftX) / (rRightX - rLeftX);
			Color colResult (colLeft.red() + portion * (colRight.red() - colLeft.red()), colLeft.green() + portion * (colRight.green() - colLeft.green()), colLeft.blue() + portion * (colRight.blue() - colLeft.blue()));
			return colResult;
		}
	}
	else
		return Color(0, 0, 255);
}

void ProfileGraphFunction::SetData(vector<double> dataX, vector<double> dataY)
{
	m_rDataX = dataX;
	m_rDataY = dataY;
	iSelectedAnchorNr = -1;
}

void ProfileGraphFunction::SetColors(vector<Color> colors)
{
	m_colors = colors;
}

void ProfileGraphFunction::SetDefaultAnchors()
{
	// no anchors to set
	SolveParams();
}

void ProfileGraphFunction::SolveParams()
{
	// nothing to solve
}

void ProfileGraphFunction::SetAnchor(DoublePoint pAnchor)
{
}

int ProfileGraphFunction::iGetAnchorNr()
{
	return iSelectedAnchorNr;
}

TimeProfileForm::TimeProfileForm(CWnd* mw, SpaceTimePathDrawer *stp)
: FormWithDest(mw, TR("Time Profile of Point Map"), fbsSHOWALWAYS|fbsBUTTONSUNDER|fbsNOCANCELBUTTON|fbsOKHASCLOSETEXT, WS_MINIMIZEBOX)
, stpdrw(stp)
, m_functions(0)
, m_gridXN(false)
, m_gridXT(false)
, m_gridYT(false)
, m_fUseRadiusThreshold(false)
, m_radiusThreshold(50)
{
	fsm = new FieldSegmentMap(root, TR("Profile Segment Map"), &sSegmentMapProfile);
	fsm->SetCallBack((NotifyProc)&TimeProfileForm::CallBackSegmentMapChanged);
	fgFunctionGraph = new ProfileFieldGraph(root);
	fgFunctionGraph->SetWidth(500);
	fgFunctionGraph->SetHeight(500);
	fgFunctionGraph->SetIndependentPos();
	fgFunctionGraph->SetFunctions(0, 0);
	fgFunctionGraph->SetCallBack((NotifyProc)&TimeProfileForm::CallBackAnchorChangedInGraph);
	cbXNgrid = new CheckBox(root, TR("X-Nodes"), &m_gridXN);
	cbXNgrid->SetCallBack((NotifyProc)&TimeProfileForm::CallBackXNGrid);
	cbXTgrid = new CheckBox(root, TR("X-Grid"), &m_gridXT);
	cbXTgrid->SetCallBack((NotifyProc)&TimeProfileForm::CallBackXTGrid);
	cbXTgrid->Align(cbXNgrid, AL_AFTER);
	cbYTgrid = new CheckBox(root, TR("Y-Grid"), &m_gridYT);
	cbYTgrid->SetCallBack((NotifyProc)&TimeProfileForm::CallBackYTGrid);
	cbYTgrid->Align(cbXTgrid, AL_AFTER);
	cbRadiusThreshold = new CheckBox(root, TR("Radius Threshold"), &m_fUseRadiusThreshold);
	cbRadiusThreshold->Align(cbXTgrid, AL_UNDER);
	cbRadiusThreshold->SetCallBack((NotifyProc)&TimeProfileForm::CallBackSegmentMapChanged);
	frRadiusThreshold = new FieldReal(cbRadiusThreshold, "", &m_radiusThreshold, ValueRange(0, 100000, 0.01));
	frRadiusThreshold->Align(cbRadiusThreshold, AL_AFTER);
	frRadiusThreshold->SetCallBack((NotifyProc)&TimeProfileForm::CallBackSegmentMapChanged);

	create();
	
	fgFunctionGraph->SetBorderThickness(130, 20, 30, 20);
	fgFunctionGraph->Replot();
}

TimeProfileForm::~TimeProfileForm()
{
	fgFunctionGraph->SetFunctions(0, 0);
	if (m_functions)
		delete [] m_functions;
}

int TimeProfileForm::CallBackAnchorChangedInGraph(Event*)
{
	//if (m_function) {
	//	int index = m_function->iGetAnchorNr();
	//}
	return 0;
}

int TimeProfileForm::CallBackSegmentMapChanged(Event*)
{
	fsm->StoreData();
	cbRadiusThreshold->StoreData();
	frRadiusThreshold->StoreData();
	if (sSegmentMapProfile.length() > 0)
		ComputeGraphs();
	return 0;
}

int TimeProfileForm::CallBackXNGrid(Event*)
{
	cbXNgrid->StoreData();
	fgFunctionGraph->SetGrid(m_gridXN, m_gridXT, m_gridYT);
	return 0;
}

int TimeProfileForm::CallBackXTGrid(Event*)
{
	cbXTgrid->StoreData();
	fgFunctionGraph->SetGrid(m_gridXN, m_gridXT, m_gridYT);
	return 0;
}

int TimeProfileForm::CallBackYTGrid(Event*)
{
	cbYTgrid->StoreData();
	fgFunctionGraph->SetGrid(m_gridXN, m_gridXT, m_gridYT);
	return 0;
}

vector<double> getTimeTicks(const TimeBounds * timeBounds)
{
	vector<double> timeTicks;
	double tMin = timeBounds->tMin();
	double tMax = timeBounds->tMax();
	double range = tMax - tMin;
	double step = 1;
	if (range < 20.0 / (24.0 * 60.0)) // 20 minutes
		step = 1 / (24.0 * 60.0); // step is 1 minute
	else if (range >= 20.0 / (24.0 * 60.0) && range < 2.0 / 24.0) // 2 hours
		step = 10.0 / (24.0 * 60.0); // step is 10 minutes
	else if (range >= 2.0 / 24.0 && range < 2) // 2 days
		step = 1.0 / 24.0; // step is 1 hour
	else if (range >= 2 && range < 15) // 2 weeks
		step = 1; // step is 1 day
	else if (range >= 15 && range < 90) // 3 months
		step = 7; // step is 1 week
	else if (range >= 90 && range < 700) // 2 years
		step = 30; // step is 1 month
	else if (range >= 700)
		step = 365; // step is 1 year
	double tick = tMin + step; // only add a tick where it would be visible (no need for t = 0)
	while (tick < tMax) {
		timeTicks.push_back(tick);
		tick += step;
	}
	return timeTicks;
}

vector<double> getDistanceTicks(double x1, double x2)
{
	vector<double> distanceTicks;
	double range = x2 - x1;
	double step = 1;
	if (range < 2) // 2 meters
		step = 0.1; // step is 10 cm
	else if (range >= 2 && range < 20) // 20 meters
		step = 1; // step is 1 meter
	else if (range >= 20 && range < 200) // 200 meters
		step = 10; // step is 10 meters
	else if (range >= 200 && range < 2000) // 2 kilometers
		step = 100; // step is 100 meters
	else if (range >= 2000 && range < 20000) // 20 kilometers
		step = 1000; // step is 1 kilometer
	else if (range >= 20000 && range < 200000) // 200 kilometers
		step = 10000; // step is 10 kilometers
	else
		step = 100000; // step is 100 kilometers
	double tick = x1 + step; // only add a tick where it would be visible (no need for x = 0)
	while (tick < x2) {
		distanceTicks.push_back(tick);
		tick += step;
	}

	return distanceTicks;
}

void TimeProfileForm::ComputeGraphs()
{
	SegmentMap sm (sSegmentMapProfile);

	// concatenate all loose segments of the segment map to form one large segment for which we can calculate a linear position from its start
	geos::operation::linemerge::LineMerger lm;
	for (int i=0; i<sm->iFeatures(); ++i) {
		const Geometry * geom = sm->getFeature(i);
		lm.add(geom);
	}
	vector<LineString*> * result = lm.getMergedLineStrings();
	if (result->size() > 0) {
		LineString * ls = result->at(0); // Take the first item. Should we warn the user if more items are found?
		geos::linearref::LengthIndexedLine line (ls); // index it, for use later on

		CoordSystem cs (stpdrw->getCoordSystem());
		bool fTransformCoords = stpdrw->getCoordSystem() != sm->cs();

		const vector<Feature *> & features = stpdrw->getFeatures();
		const TimeBounds * timeBounds = stpdrw->getTimeBounds();
		const bool fUseGroup = stpdrw->fGetUseGroup();
		long numberOfFeatures = features.size();
		double cubeBottom = -0.0000000005; // should be 0 .. this is a workaround for accurracy problem of rrMinMax in Talbe ODF (otherwise the first sample is unwantedly excluded)
		double cubeTop = 0.000000001 + timeBounds->tMax() - timeBounds->tMin();
		String sLastGroupValue = fUseGroup && (numberOfFeatures > 0) ? stpdrw->getGroupValue(features[0]) : "";
		vector<std::pair<double, Feature*>> projectedFeatures;
		vector<vector<std::pair<double, Feature*>>> projectedFeaturesList;
		for(long i = 0; i < numberOfFeatures; ++i) {
			Feature *feature = features[i];
			if (fUseGroup && sLastGroupValue != stpdrw->getGroupValue(feature)) {
				sLastGroupValue = stpdrw->getGroupValue(feature);
				if (projectedFeatures.size() > 0) {
					projectedFeaturesList.push_back(projectedFeatures);
					projectedFeatures.clear();
				}
			}
			ILWIS::Point *point = (ILWIS::Point *)feature;
			double z = stpdrw->getTimeValue(feature);
			if (z >= cubeBottom && z <= cubeTop) {
				Coordinate crd = *(point->getCoordinate());
				if (fTransformCoords)
					crd = sm->cs()->cConv(cs, crd);

				geos::algorithm::distance::PointPairDistance ptDist;
				geos::algorithm::distance::DistanceToPoint::computeDistance(*ls, crd, ptDist);
				if (m_fUseRadiusThreshold && ptDist.getDistance() > m_radiusThreshold)
					continue;
				double length = line.indexOf(ptDist.getCoordinate(0));
				projectedFeatures.push_back(std::pair<double, Feature*>(length, feature));
			}
		}

		if (projectedFeatures.size() > 0)
			projectedFeaturesList.push_back(projectedFeatures);
		const CoordinateSequence* streetNodes = ls->getCoordinatesRO();
		vector<double> projectedStreetNodes;
		for (int i = 0; i < streetNodes->size(); ++i) {
			double length = line.indexOf(streetNodes->getAt(i));
			projectedStreetNodes.push_back(length);
		}

		if (m_functions) {
			fgFunctionGraph->SetFunctions(0, 0);
			delete [] m_functions;
		}
		DrawingColor * drawingColor = stpdrw->getDrawingColor();
		NewDrawer::DrawMethod drawMethod = stpdrw->getDrawMethod();
		m_functions = new ProfileGraphFunction [projectedFeaturesList.size()];
		double minDataX;
		double minDataY;
		double maxDataX;
		double maxDataY;
		bool fFirst = true;
		for (int i = 0; i < projectedFeaturesList.size(); ++i) {
			projectedFeatures = projectedFeaturesList[i];
			vector<double> dataX;
			vector<double> dataY;
			vector<Color> colors;
			for (int j = 0; j < projectedFeatures.size(); ++j) {
				double X = projectedFeatures[j].first;
				dataX.push_back(X);
				Feature * feature = projectedFeatures[j].second;
				double time = stpdrw->getTimeValue(feature) + stpdrw->getTimeBounds()->tMin();
				dataY.push_back(time);
				if (fFirst) {
					minDataX = X;
					maxDataX = X;
					minDataY = time;
					maxDataY = time;
					fFirst = false;
				} else {
					if (X < minDataX)
						minDataX = X;
					else if (X > maxDataX)
						maxDataX = X;
					if (time < minDataY)
						minDataY = time;
					else if (time > maxDataY)
						maxDataY = time;
				}
				Color color (drawingColor->clrRaw(feature->iValue(), drawMethod));
				colors.push_back(color);
			}

			m_functions[i].SetData(dataY, dataX); // let the function internally have the times on the X axis and the lengths on the Y axis
			m_functions[i].SetColors(colors);
		}
		for (int i = 0; i < projectedFeaturesList.size(); ++i)
			m_functions[i].SetDomain(projectedStreetNodes[0], timeBounds->tMax(), projectedStreetNodes[projectedStreetNodes.size() - 1], timeBounds->tMin()); // the function domains will have the times on the Y axis and the lengths on the X axis
		fgFunctionGraph->SetFunctions(m_functions, projectedFeaturesList.size());
		vector<double> timeTicks = getTimeTicks(timeBounds);
		vector<double> distanceTicks = getDistanceTicks(projectedStreetNodes[0], projectedStreetNodes[projectedStreetNodes.size() - 1]);
		fgFunctionGraph->SetGridTicks(projectedStreetNodes, distanceTicks, timeTicks);
	}
}