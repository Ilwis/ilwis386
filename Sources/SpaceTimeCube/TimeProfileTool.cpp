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

ProfileGraphWindow::ProfileGraphWindow(FormEntry *f)
: SimpleGraphWindowWrapper(f)
{
}

void ProfileGraphWindow::SetFunctions(SimpleFunction * funPtr, int _iNrFunctions)
{
	iNrFunctions = _iNrFunctions;
	SetFunction(funPtr);
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
	CPen penGray(PS_SOLID, 0, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penGray);

	if (!m_fRedraw && !m_fAbortPaintThread)
	{
		// draw the axes
		CRect functionPlotRect (GetFunctionPlotRect());
		pDC->MoveTo(functionPlotRect.left, functionPlotRect.top);
		pDC->LineTo(functionPlotRect.left, functionPlotRect.bottom);
		pDC->LineTo(functionPlotRect.right, functionPlotRect.bottom);

		// draw ticks on the axes		
		int iTicks = 5;
		int iTickThickness = 3; // nr of pixels for drawing a tick
		double rTickXSize = (double)(functionPlotRect.Width()) / (double)(iTicks - 1);
		double rTickYSize = (double)(functionPlotRect.Height()) / (double)(iTicks - 1);
		for (int i = 0; i < iTicks; ++i)
		{
			pDC->MoveTo(functionPlotRect.left, functionPlotRect.top + rTickYSize * i);
			pDC->LineTo(functionPlotRect.left - iTickThickness, functionPlotRect.top + rTickYSize * i);
			pDC->MoveTo(functionPlotRect.left + rTickXSize * i, functionPlotRect.bottom);
			pDC->LineTo(functionPlotRect.left + rTickXSize * i, functionPlotRect.bottom + iTickThickness);
		}

		// draw axis values
		// x-axis values
		CFont * oldFont = 0;
		if (m_fnt)
			oldFont = pDC->SelectObject(m_fnt);
		UINT iPreviousAlignment = pDC->SetTextAlign(TA_CENTER | TA_TOP);
		COLORREF clrPreviousColor = pDC->SetTextColor(RGB(0, 0, 0)); // black text
		CString sVal;
		sVal.Format("%g", m_XYFunctionDomain.left);
		pDC->TextOut(functionPlotRect.left, functionPlotRect.bottom + iTickThickness, sVal);
		sVal.Format("%g", m_XYFunctionDomain.right);
		pDC->TextOut(functionPlotRect.right, functionPlotRect.bottom + iTickThickness, sVal);
		// y-axis values
		pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
		sVal = ILWIS::Time(m_XYFunctionDomain.top).toString().c_str();
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.top, sVal);
		sVal = ILWIS::Time(m_XYFunctionDomain.bottom).toString().c_str();
		pDC->TextOut(functionPlotRect.left - iTickThickness, functionPlotRect.bottom, sVal);

		pDC->SetTextColor(clrPreviousColor);
		pDC->SetTextAlign(iPreviousAlignment);
		if (m_fnt) // take care not to change this: m_fnt determines whether SelectObject should be called or not (in order to "release" m_fnt)
			pDC->SelectObject(oldFont);
	}
	
	// Put back the old objects.
	pDC->SelectObject(pOldPen);
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
	sgw->Create(NULL,	"Graph", WS_CHILD | WS_VISIBLE, CRect(pntFld, dimFld), _frm->wnd(), Id());

  CreateChildren();
}

void ProfileFieldGraph::SetFunctions(SimpleFunction * funPtr, int iNrFunctions)
{
	if (sgw) ((ProfileGraphWindow*)sgw)->SetFunctions(funPtr, iNrFunctions);
		
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
	m_Anchors.clear();
	for (int i = 0; i < dataX.size(); ++i)
		m_Anchors.push_back(DoublePoint(dataX[i], dataY[i]));
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
	if (m_Anchors.size() > 0)
	{
		iSelectedAnchorNr = 0;
		for (int i = 1; i < m_Anchors.size(); ++i)
		{
			if (rUnitDist2(pAnchor, m_Anchors[i]) < rUnitDist2(pAnchor, m_Anchors[iSelectedAnchorNr]))
				iSelectedAnchorNr = i;
		}
	}
}

int ProfileGraphFunction::iGetAnchorNr()
{
	return iSelectedAnchorNr;
}

TimeProfileForm::TimeProfileForm(CWnd* mw, SpaceTimePathDrawer *stp)
: FormWithDest(mw, TR("Time Profile of Point Map"), fbsSHOWALWAYS|fbsBUTTONSUNDER|fbsNOCANCELBUTTON|fbsOKHASCLOSETEXT, WS_MINIMIZEBOX)
, stpdrw(stp)
, m_functions(0)
{
	fsm = new FieldSegmentMap(root, TR("Profile Segment Map"), &sSegmentMapProfile);
	fsm->SetCallBack((NotifyProc)&TimeProfileForm::CallBackSegmentMapChanged);
	fgFunctionGraph = new ProfileFieldGraph(root);
	fgFunctionGraph->SetWidth(500);
	fgFunctionGraph->SetHeight(500);
	fgFunctionGraph->SetBorderThickness(50, 20, 20, 20);
	fgFunctionGraph->SetIndependentPos();
	fgFunctionGraph->SetFunctions(0, 0);
	fgFunctionGraph->SetCallBack((NotifyProc)&TimeProfileForm::CallBackAnchorChangedInGraph);

	create();

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
	if (sSegmentMapProfile.length() > 0)
		ComputeGraphs();
	return 0;
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
				double length = line.indexOf(ptDist.getCoordinate(0));
				projectedFeatures.push_back(std::pair<double, Feature*>(length, feature));
			}
		}

		if (projectedFeatures.size() > 0)
			projectedFeaturesList.push_back(projectedFeatures);

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
			m_functions[i].SetDomain(minDataX, timeBounds->tMax(), maxDataX, timeBounds->tMin()); // the function domains will have the times on the Y axis and the lengths on the X axis
		fgFunctionGraph->SetFunctions(m_functions, projectedFeaturesList.size());
	}
}