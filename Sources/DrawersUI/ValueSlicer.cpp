#include "Client\Headers\formelementspch.h"
#include <gdiplus.h>
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\FormElements\fldcolor.cpp"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h"
#include "Client\ilwis.h"
#include "Engine\Representation\Rprvalue.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "ValueSlicer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\SetDrawer.h"


BEGIN_MESSAGE_MAP(ValueSlicer, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	//ON_NOTIFY(TTN_GETDISPINFOA,0, OnToolTipNotify)
END_MESSAGE_MAP()

#define UNDEFPOINT 100000
#define SMALL_FACTOR 0.005

ValueSlicer::ValueSlicer(ValueSlicerSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
: BaseZapp(f)
, iDragIndex(iUNDEF)
{
	fldslicer = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create Slicer"));
	activePoint = CPoint(100000, 100000);
	EnableToolTips(TRUE);
}


void ValueSlicer::drawRprBase(LPDRAWITEMSTRUCT lpDIS, const CRect rct) {
	Gdiplus::Graphics *graphics = new Gdiplus::Graphics(lpDIS->hDC);
	RepresentationGradual *rpg = rprBase->prg();
	const double delta = fldslicer->valrange->rrMinMax().rWidth() * SMALL_FACTOR;
	double yscale = (double)rct.Height() / ((rpg->iLimits() - 1.0) * rpg->iGetStretchSteps());
	double y = 0;
	for(int i = 1; i < rpg->iLimits(); ++i) {		
		double v1 = rpg->rGetLimitValue(i-1);
		double v2 = rpg->rGetLimitValue(i);
		double rStep = (v2 - v1) / rpg->iGetStretchSteps();
		for(int j=0; j < rpg->iGetStretchSteps(); ++j) {
			double cv = v1 + j * rStep;
			Color c = rpg->clr(cv + rStep - delta);
			Gdiplus::SolidBrush brush(Gdiplus::Color(255 - c.transparency(), c.red(), c.green(), c.blue()));
			int yup = rct.bottom - y - yscale;
			int ydown = rct.bottom - y;
			Gdiplus::Rect rectangle(rct.left, yup, rct.Width(), ydown - yup);
			Gdiplus::Status status = graphics->FillRectangle(&brush, rectangle);
			y += yscale;
		}
	}
}

void ValueSlicer::drawRpr(LPDRAWITEMSTRUCT lpDIS, const CRect rct) {
	Gdiplus::Graphics *graphics = new Gdiplus::Graphics(lpDIS->hDC);
	RepresentationGradual *rpg = fldslicer->rprgrad;
	const double delta = fldslicer->valrange->rrMinMax().rWidth() * SMALL_FACTOR;
	double yscale = (double)rct.Height() / ((rpg->iLimits() - 1.0) * rpg->iGetStretchSteps());
	double y = 0;
	for(int i = 1; i < fldslicer->bounds.size(); ++i) {		
		double v1 = fldslicer->bounds[i-1];
		double v2 = fldslicer->bounds[i];
		double rStep = (v2 - v1) / rpg->iGetStretchSteps();
		double f = (v2 - v1) / fldslicer->valrange->rrMinMax().rWidth(); // fldslicer->bounds[fldslicer->bounds.size() - 1];
		double yStep = rct.Height() * f / rpg->iGetStretchSteps();
		int transp = fldslicer->transparency[i-1];
		for(int j=0; j < rpg->iGetStretchSteps(); ++j) {
			double cv = v1 + j * rStep;
			Color c = rpg->clr(cv + rStep - delta, fldslicer->valrange->rrMinMax());
			Gdiplus::SolidBrush brush(Gdiplus::Color(255 - transp, c.red(), c.green(), c.blue()));
			int yup = rct.bottom - y - yStep;
			int ydown = rct.bottom - y;
			Gdiplus::Rect rectangle(rct.left, yup, rct.Width(), ydown - yup);
			Gdiplus::Status status = graphics->FillRectangle(&brush, rectangle);
			y += yStep;
		}
	}
}

void ValueSlicer::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	if ( !fldslicer->valrange.fValid() || !fldslicer)
		return;

	CRect crct;
	GetClientRect(crct);
	CRect rct = makeDrawRect();

	Gdiplus::Graphics *graphics = new Gdiplus::Graphics(lpDIS->hDC);
	Color bkColor = GetBkColor(lpDIS->hDC);
	//Gdiplus::SolidBrush brushBK(bkColor);
	Gdiplus::SolidBrush brushBK(Gdiplus::Color(255, bkColor.red(), bkColor.green(), bkColor.blue()));
	Gdiplus::Rect rectangle( crct.left, crct.top, crct.Width()-1, crct.Height()-1 );
	graphics->FillRectangle(&brushBK, rectangle);

	ylimits.clear();
	ylimits.resize(fldslicer->bounds.size());

	ylimits[0] = 0;
	double yscale = rct.Height() / fldslicer->valrange->rrMinMax().rWidth();
	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	HGDIOBJ fntOld = SelectObject(lpDIS->hDC, fnt);
	if ( rprBase.fValid())
		drawRprBase(lpDIS, rct);
	drawRpr(lpDIS, rct);

	double y = 0;
	String txt = fldslicer->valrange->vri() ? String("%d",(int)fldslicer->bounds[0]) : String("%.03f", fldslicer->bounds[0]);
	::TextOut(lpDIS->hDC,rct.right, rct.bottom - y - 8, txt.c_str(),txt.size());
	for(int i = 1; i < fldslicer->bounds.size(); ++i) {
	
		double v1 =  fldslicer->bounds[i-1];
		double v2 = fldslicer->bounds[i];
		y += (v2 - v1) * yscale;
		String txt = fldslicer->valrange->vri() ? String("%d",(int)v2) : String("%.03f", v2);
		::TextOut(lpDIS->hDC,rct.right, rct.bottom - y - 8, txt.c_str(),txt.size());
		ylimits[i] = rct.bottom - y;
		
	}
	if ( activePoint.x != UNDEFPOINT) {
		Gdiplus::Pen redPen(Gdiplus::Color(255,255,0,0),2);
		graphics->DrawLine(&redPen,rct.left + 1, activePoint.y,  rct.right - 2, activePoint.y);
	}

	delete graphics;
}

CRect ValueSlicer::makeDrawRect() {
	CRect rct;
	GetClientRect(rct);
	return CRect(rct.left, rct.top +10,rct.left + rct.Width() / 2, rct.bottom - 10);
}

void ValueSlicer::OnLButtonDown(UINT nFlags, CPoint point) {
	CRect rct = makeDrawRect();
	fldslicer->selectedIndex = -1;
	activePoint = CPoint(UNDEFPOINT,UNDEFPOINT);
	iDragIndex = iUNDEF;
	double fract = (double)( point.y - rct.top) / rct.Height();
	double v = ( 1.0 - fract) * fldslicer->valrange->rrMinMax().rWidth() + fldslicer->valrange->rrMinMax().rLo();
	for(int i = 0; i < fldslicer->bounds.size(); ++i) {
		double limitVal = fldslicer->bounds[i];
		double delta = abs(limitVal - v);
		if ( limitVal >= v ) {
			fldslicer->selectedIndex = i - 1;
		}
		if ( delta < abs(fldslicer->valrange->rrMinMax().rWidth() * 0.04)) {
			double yscale = rct.Height() / fldslicer->valrange->rrMinMax().rWidth();
			activePoint = CPoint(point.x, rct.bottom - yscale * fldslicer->bounds[i]); 
		}
		if ( fldslicer->selectedIndex != -1)
			break;
	}
}

void ValueSlicer::OnMouseMove(UINT nFlags, CPoint point) {
	if ( activePoint.x != UNDEFPOINT) {
		CRect rct = makeDrawRect();
		double fract = (double)( point.y - rct.top) / rct.Height();
		double v = ( 1.0 - fract) * fldslicer->valrange->rrMinMax().rWidth() + fldslicer->valrange->rrMinMax().rLo();
		int index = 0;
		if (iDragIndex != iUNDEF)
			index = iDragIndex;
		else {
			for(; index < ylimits.size(); ++index) {
				int delta = abs(ylimits[index] - point.y);
				int tolerance = (double)rct.Height() * 0.04; // NB this hit value is a bit processor dependent
				if (  delta < tolerance) {
					iDragIndex = index;
					SetCapture();
					break;
				}
			}
		}
		if ( index >= ylimits.size() - 1)
			return;
		if (moveValue(index, v)) {
			activePoint.y = point.y;
			updateRepresentations();
		}
	}
}

void ValueSlicer::OnLButtonUp(UINT nFlags, CPoint point) {
	activePoint = CPoint(UNDEFPOINT, UNDEFPOINT);
	iDragIndex = iUNDEF;
	ReleaseCapture();
	Invalidate();
	fProcess(MouseLBUpEvent(nFlags,point)); 
}

void ValueSlicer::OnLButtonDblClk(UINT nFlags, CPoint point) {
	Color clr1, clr2;
	clr1 = fldslicer->rprgrad->GetColor(fldslicer->selectedIndex*2 );
	clr2 = fldslicer->rprgrad->GetColor(fldslicer->selectedIndex*2 + 1);
	bool showForm = !rprBase.fValid() || fldslicer->selectedIndex % 2 == 1;
	if (!showForm)
		return;

	double v1 = fldslicer->bounds[fldslicer->selectedIndex];
	double v2 = fldslicer->bounds[fldslicer->selectedIndex+1];
	bool v1ChangeAllowed = fldslicer->selectedIndex > 0;
	bool v2ChangeAllowed = fldslicer->selectedIndex < fldslicer->bounds.size() - 2;
	SlicingStepColor frm(this,&clr1, &v1, v1ChangeAllowed, &clr2, &v2, v2ChangeAllowed);
	if ( frm.fOkClicked()) {

		clr2.transparency() = clr1.transparency();
		int index = fldslicer->selectedIndex * 2;
		fldslicer->rprgrad->SetLimitColor(index, clr1);
		fldslicer->rprgrad->SetLimitColor(index + 1, clr2);
		fldslicer->transparency[fldslicer->selectedIndex] = clr2.transparency();
		RangeReal rr = fldslicer->valrange->rrMinMax();
		if (v1 < v2) {
			if (v1 >= rr.rLo())
				moveValue(fldslicer->selectedIndex, v1);
			if (v2 <= rr.rHi())
				moveValue(fldslicer->selectedIndex + 1, v2);
		}
		Invalidate();
		updateRepresentations();
		fldslicer->rprgrad->Store();
	}
}

bool ValueSlicer::moveValue(int index, double v) {
	const double delta = fldslicer->valrange->rrMinMax().rWidth() * SMALL_FACTOR;
	if ((index > 0) && (index < fldslicer->bounds.size() - 1) && (v > (fldslicer->bounds[index - 1] + 2 * delta)) && (v < (fldslicer->bounds[index + 1] - 2 * delta))) {
		fldslicer->bounds[index] = v;
		if (index == 0) // first two cases (index == 0 and index == fldslicer->bounds.size() - 1) are blocked by "if" above for now, thus not allowing the user to set the total min/max
			fldslicer->rprgrad->SetLimitValue(0, v);
		else if (index == fldslicer->bounds.size() - 1)
			fldslicer->rprgrad->SetLimitValue(index*2 - 1, v);
		else {
			fldslicer->rprgrad->SetLimitValue(index*2 - 1, v);
			fldslicer->rprgrad->SetLimitValue(index*2, v + delta);
		}
		return true;
	} else
		return false;
}

void ValueSlicer::updateRepresentations() {
	ILWIS::SpatialDataDrawer *parentDrw = (ILWIS::SpatialDataDrawer *)fldslicer->drawer->getParentDrawer();
	for(int i =0; i < parentDrw->getDrawerCount(); ++i) {
		ILWIS::LayerDrawer *setdrw;
		if ( rprBase.fValid()){
			setdrw = (ILWIS::LayerDrawer *)parentDrw->getDrawer(i);
			setdrw = (ILWIS::LayerDrawer *)setdrw->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
		}
		else
			setdrw = (ILWIS::LayerDrawer *)parentDrw->getDrawer(i);
		if (!setdrw)
			return;

		Representation rpr;
		rpr.SetPointer(fldslicer->rprgrad);
		setdrw->setRepresentation(rpr);
		PreparationParameters pp(NewDrawer::ptRENDER, 0);
		setdrw->prepareChildDrawers(&pp);
	}
	Invalidate();
	fldslicer->drawer->getRootDrawer()->getDrawerContext()->doDraw();
}

void ValueSlicer::setRprBase(const Representation& rprB) {
	rprBase = rprB;
}
//----------------------------------------------------
ValueSlicerSlider::ValueSlicerSlider(FormEntry* par, ILWIS::LayerDrawer *sdrw) :
	FormEntry(par,0,true),
    valueslicer(0),
	drawingcolor(0),
	drawer(0),
	rprgrad(0)
{
  psn->iMinWidth = psn->iWidth = 80;
  psn->iMinHeight = psn->iHeight = 250;
  SetIndependentPos();
  setNumberOfBounds(2);
  LayerDrawer(sdrw);
  RangeReal rr = sdrw->getStretchRangeReal();
  setValueRange(ValueRange(rr,0));
  highColor = Color(70,255,30);
  lowColor = Color(20, 60,0);
  nrBounds = 0;
  init();
}

ValueSlicerSlider::~ValueSlicerSlider() {
	delete drawingcolor;
}

void ValueSlicerSlider::create()
{
  zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
  valueslicer = new ValueSlicer(this, WS_VISIBLE |  WS_CHILD | WS_TABSTOP,
              CRect(pntFld, dimFld) , frm()->wnd() , Id());
  valueslicer->SetFont(frm()->fnt);
  valueslicer->setRprBase(rprBase);
  CreateChildren();
}

void ValueSlicerSlider::setValueRange(const ValueRange& rng) {
	if ( bounds.size() == 0)
		setNumberOfBounds(2);
	valrange = rng;
}

void ValueSlicerSlider::setBound(int index, double v){
	if ( v > valrange->rrMinMax().rHi() || v  < valrange->rrMinMax().rLo() )
		return;
	if ( bounds.size() < index)
		return;
	double before = -1e200;
	double after = 1e200;
	if ( index + 1 < bounds.size())
		after = bounds[index+1];
	if ( index - 1 > 0)
		before = bounds[0];
	if ( v > before && v < after)
		bounds[index] = v;
	init();	
}

void ValueSlicerSlider::LayerDrawer(ILWIS::LayerDrawer *dr) {
	drawer = dr;
}

void ValueSlicerSlider::setNumberOfBounds(int n) {
	bounds.clear();
	transparency.clear();
	nrBounds = n;
	rpr.SetPointer(0);
	rprgrad = 0;
	init();
}

bool ValueSlicerSlider::isValid() const {
	return valrange.fValid() && drawer!= 0;
}

void ValueSlicerSlider::init() {
	if ( isValid()) {
		bool rprCreated = false;
		if ( nrBounds == 0)
			return;
		//delete drawingcolor;

		if ( !rprgrad ) {
			rprgrad = new RepresentationValue(FileName(), Domain("value"));
			rpr.SetPointer(rprgrad);
			rprgrad->reset();
			rprgrad->SetLimitValue(0,valrange->rrMinMax().rLo());
			rprgrad->SetLimitValue(1, valrange->rrMinMax().rHi());
			rprgrad->SetLimitColor(0, lowColor);
			rprgrad->SetLimitColor(1, highColor);
			rprgrad->SetStretchSteps(15);
			bounds.push_back(valrange->rrMinMax().rLo());
			bounds.push_back(valrange->rrMinMax().rHi());
			transparency.push_back(0);
			transparency.push_back(0);
			Color clrPrev = lowColor;
			for(int i=1; i < nrBounds; ++i) {
				double limit = valrange->rrMinMax().rLo() +  i * (valrange->rrMinMax().rWidth() / nrBounds);
				Color clrNext = getColor(i);
				insertLimit(limit, clrPrev, clrNext);
				clrPrev = clrNext;				
			}
			rprCreated = true;
			rpr->Store();
		}
	
		ILWIS::SpatialDataDrawer *parentDrw = (ILWIS::SpatialDataDrawer *)drawer->getParentDrawer();
		for(int i =0; i < parentDrw->getDrawerCount(); ++i) {
			ILWIS::LayerDrawer *setdrw = (ILWIS::LayerDrawer *)parentDrw->getDrawer(i);
			if ( rprBase.fValid()){
				setdrw->setRepresentation(rprBase);
				setdrw = (ILWIS::LayerDrawer *)setdrw->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
			}
			else
				setdrw = (ILWIS::LayerDrawer *)parentDrw->getDrawer(i);
			if ( setdrw)
				setdrw->setRepresentation(rpr);
		}

		if (valueslicer)
			valueslicer->updateRepresentations();

		if ( rprBase.fValid()) {
			ILWIS::LayerDrawer *dr = (ILWIS::LayerDrawer *)drawer->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
			if ( dr)
				drawingcolor = new ILWIS::DrawingColor(dr);
		}
		else {
			drawingcolor = new ILWIS::DrawingColor(drawer);
		}
	}
}

void ValueSlicerSlider::insertLimit(double rValue, const Color& clrPrev, const Color& clrNext) {
	int index = rprgrad->insert(rValue, clrPrev, RepresentationGradual::crUPPER);
	if ( index == iUNDEF)
		return;
	index = insertBound(rValue);
	double delta = valrange->rrMinMax().rWidth() * SMALL_FACTOR;
	rprgrad->insert(bounds[index] + delta, clrNext, RepresentationGradual::crSTRETCH);
}

int ValueSlicerSlider::insertBound(double rValue) {
	int index = 0;
	for(int i = 0; i < bounds.size(); ++i) {
		if ( bounds[i] > rValue) {
			index = i;
			break;
		}
	}
	bounds.push_back(0);
	for(int i=index; i < bounds.size() - 1; ++i) {
		bounds[i+1] = bounds[i];
	}
	bounds[index] = rValue;
	transparency.push_back(0);

	return index;
}

Color ValueSlicerSlider::getColor(int i) {
	if ( highColor.fEqual(colorUNDEF) || lowColor.fEqual(colorUNDEF))
		return colorUNDEF;
	int deltar = highColor.red() - lowColor.red();
	int deltag = highColor.green() - lowColor.green();
	int deltab = highColor.blue() - lowColor.blue();
	int deltat = highColor.transparency() - lowColor.transparency();
	int rstep = deltar / (nrBounds - 1);
	int gstep = deltag / (nrBounds - 1);
	int bstep = deltab / (nrBounds - 1);
	int tstep = deltat / (nrBounds - 1);
	return Color(lowColor.red() + rstep * i, lowColor.green() + gstep * i, lowColor.blue() + bstep * i, lowColor.transparency() + tstep * i);
}


void ValueSlicerSlider::setHighColor(Color c){
	highColor = c;
}
void ValueSlicerSlider::setLowColor(Color c){
	lowColor = c;
}

void ValueSlicerSlider::setRprBase(const Representation& rprB) {
	rprBase = rprB;
}

Representation ValueSlicerSlider::getRpr() const {
	return rpr;
}

void ValueSlicerSlider::setFileNameRpr(const FileName& fn) {
	if ( rpr.fValid()) {
		rpr->SetFileName(fn);
		rpr->Store();
	}
}

//----------------------------------------------------------
SlicingStepColor::SlicingStepColor(CWnd* parent, Color* clrLow, double *v1, bool _fv1Enabled, Color*clrHigh, double *v2, bool _fv2Enabled) :
FormWithDest(parent, TR("Step Color"))
, fv1Enabled(_fv1Enabled)
, fv2Enabled(_fv2Enabled)
{
	FieldColor *fc2 = new FieldColor(root,TR("Step Color high"), clrHigh, false);
	fr2 = new FieldReal(root,"",v2);
	fr2->Align(fc2, AL_AFTER);
	FieldColor *fc1 = new FieldColor(root,TR("Step Color low"), clrLow, true);
	fr1 = new FieldReal(root,"",v1);
	fr1->Align(fr2, AL_UNDER);
	fc1->Align(fc2, AL_UNDER);
	create();
}

BOOL SlicingStepColor::OnInitDialog()
{
	FormWithDest::OnInitDialog();
	if (!fv1Enabled)
		fr1->Disable();
	if (!fv2Enabled)
		fr2->Disable();
	return TRUE;
}