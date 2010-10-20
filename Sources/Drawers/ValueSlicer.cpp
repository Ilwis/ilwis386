#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\FormElements\fldcolor.cpp"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h"
#include "Client\ilwis.h"
#include "Engine\Representation\Rprvalue.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Drawers\ValueSlicer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\AnimationDrawer.h"



BEGIN_MESSAGE_MAP(ValueSlicer, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	//ON_NOTIFY(TTN_GETDISPINFOA,0, OnToolTipNotify)
END_MESSAGE_MAP()

#define UNDEFPOINT 100000

ValueSlicer::ValueSlicer(ValueSlicerSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldslicer = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create Slicer"));
	activePoint = CPoint(100000, 100000);
	EnableToolTips(TRUE);
}


void ValueSlicer::drawRprBase(LPDRAWITEMSTRUCT lpDIS, const CRect rct) {
	RepresentationGradual *rpg = rprBase->prg();
	double yscale = rct.Height();
	double y = 0;
	for(int i = 1; i < rpg->iLimits(); ++i) {
		
		double v1 = rpg->rGetLimitValue(i-1);
		double v2 = rpg->rGetLimitValue(i);
		double rStep = (v2 - v1) / rpg->iGetStretchSteps();
		for(int j=0; j < rpg->iGetStretchSteps(); ++j) {
			double cv = v1 + j * rStep;
			Color c = rpg->clr(cv + rStep / 2.0);
			CBrush brush(c);
			CPen pen(PS_SOLID,1,c);
			HGDIOBJ open = SelectObject(lpDIS->hDC, pen);
			HGDIOBJ prevBrush = SelectObject(lpDIS->hDC, brush);
			int yup = rct.bottom - y - rStep * yscale;
			int ydown = rct.bottom - y;
			::Rectangle(lpDIS->hDC, rct.left, ydown, rct.right, yup);
			y += rStep * yscale;
			SelectObject(lpDIS->hDC, prevBrush);
			SelectObject(lpDIS->hDC, open);
		}
	}
}

void ValueSlicer::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	if ( !fldslicer->valrange.fValid() || !fldslicer)
		return;

	HGDIOBJ oldPen, oldBrush;
	CRect crct;
	GetClientRect(crct);
	CRect rct = makeDrawRect();
	Color bkColor = GetBkColor(lpDIS->hDC);
	Color c( GetSysColor(COLOR_3DFACE));
	CBrush brushBk(c);
	CPen pen(PS_SOLID,1,GetSysColor(COLOR_3DFACE));
	oldBrush = SelectObject(lpDIS->hDC, brushBk);
	oldPen = SelectObject(lpDIS->hDC, pen);
	::Rectangle(lpDIS->hDC, crct.left, crct.top, crct.right, crct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, oldPen);
	ylimits.clear();
	ylimits.resize(fldslicer->bounds.size());

	ylimits[0] = 0;
	double yscale = rct.Height() / fldslicer->valrange->rrMinMax().rWidth();
	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	HGDIOBJ fntOld = SelectObject(lpDIS->hDC, fnt);
	if ( rprBase.fValid())
		drawRprBase(lpDIS, rct);

	double y = 0;
	String txt = fldslicer->valrange->vri() ? String("%d",(int)fldslicer->bounds[0]) : String("%.03f", fldslicer->bounds[0]);
	::TextOut(lpDIS->hDC,rct.right, rct.bottom - y - 8, txt.scVal(),txt.size());
	for(int i = 1; i < fldslicer->bounds.size(); ++i) {
		
		double v1 =  fldslicer->bounds[i-1];
		double v2 = fldslicer->bounds[i];
		double cv = (v1 + v2) / 2.0;
		Color c = fldslicer->drawingcolor->clrVal(cv);
		if ( c != colorUNDEF) {
			CBrush brush(c);
			HGDIOBJ prevBrush = SelectObject(lpDIS->hDC, brush);
			int yup = rct.bottom - y - (v2 - v1) * yscale;
			int ydown = rct.bottom - y;
			::Rectangle(lpDIS->hDC, rct.left, ydown, rct.right, yup);
			//TRACE(String("%f %f %f %d %f\n",v2,v1, v2-v1,(int)((v2 - v1) * yscale),yscale).scVal());
			SelectObject(lpDIS->hDC, prevBrush);
		}
		y += (v2 - v1) * yscale;
		String txt = fldslicer->valrange->vri() ? String("%d",(int)v2) : String("%.03f", v2);
		::TextOut(lpDIS->hDC,rct.right, rct.bottom - y - 8, txt.scVal(),txt.size());
		ylimits[i] = rct.bottom - y;
		
	}
	if ( activePoint.x != UNDEFPOINT) {
		CPen redPen(PS_SOLID,2, RGB(255,0,0));
		HGDIOBJ oldPen = SelectObject(lpDIS->hDC, redPen);
		::MoveToEx(lpDIS->hDC, rct.left + 1, activePoint.y,0);
		::LineTo(lpDIS->hDC, rct.right - 2, activePoint.y);
		SelectObject(lpDIS->hDC, oldPen);
	}


	SelectObject(lpDIS->hDC, fntOld);
	SelectObject(lpDIS->hDC, oldBrush);
}

void ValueSlicer::OnLButtonUp(UINT nFlags, CPoint point) {
	activePoint = CPoint(UNDEFPOINT, UNDEFPOINT);
	Invalidate();
	fProcess(MouseLBUpEvent(nFlags,point)); 

}

CRect ValueSlicer::makeDrawRect() {
	CRect rct;
	GetClientRect(rct);
	return CRect(rct.left, rct.top +10,rct.left + rct.Width() / 2, rct.bottom - 10);
}
void ValueSlicer::OnMouseMove(UINT nFlags, CPoint point) {
	if ( activePoint.x != UNDEFPOINT) {
		CRect rct = makeDrawRect();
		double fract = (double)( point.y - rct.top) / rct.Height();
		double v = ( 1.0 - fract) * fldslicer->valrange->rrMinMax().rWidth() + fldslicer->valrange->rrMinMax().rLo();
		int index = 0;
		for(; index < ylimits.size(); ++index) {
			int delta = abs(ylimits[index] - point.y);
			int tolerance = (double)rct.Height() * 0.04; // NB this hit value is a bit processor dependent

			if (  delta < tolerance) {
				break;
			}
		}
		if ( index >= ylimits.size() - 1)
			return;
		fldslicer->bounds[index] = v;
		fldslicer->rprgrad->SetLimitValue(index, v);
	    activePoint.y = point.y;
		updateRepresentations();

	}

}

void ValueSlicer::updateRepresentations() {
	ILWIS::AbstractMapDrawer *parentDrw = (ILWIS::AbstractMapDrawer *)fldslicer->drawer->getParentDrawer();
	for(int i =0; i < parentDrw->getDrawerCount(); ++i) {
		ILWIS::SetDrawer *setdrw;
		if ( rprBase.fValid()){
			setdrw = (ILWIS::SetDrawer *)parentDrw->getDrawer(i);
			setdrw = (ILWIS::SetDrawer *)setdrw->getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
		}
		else
			setdrw = (ILWIS::SetDrawer *)parentDrw->getDrawer(i);
		if (!setdrw)
			return;

		Representation rpr;
		rpr.SetPointer(fldslicer->rprgrad);
		setdrw->setRepresentation(rpr);
	}
	Invalidate();
	fldslicer->drawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->Invalidate();
}
void ValueSlicer::OnLButtonDown(UINT nFlags, CPoint point) {
	CRect rct = makeDrawRect();
	fldslicer->selectedIndex = -1;
	activePoint = CPoint(UNDEFPOINT,UNDEFPOINT);
	double fract = (double)( point.y - rct.top) / rct.Height();
	double v = ( 1.0 - fract) * fldslicer->valrange->rrMinMax().rWidth() + fldslicer->valrange->rrMinMax().rLo();
	for(int i = 0; i < fldslicer->bounds.size(); ++i) {
		double limitVal = fldslicer->bounds[i];
		double delta = abs(limitVal - v);
		if ( limitVal >= v ) {
			fldslicer->selectedIndex = i - 1;
		}
		if ( delta < abs(fldslicer->valrange->rrMinMax().rWidth() * 0.02)) {
			double yscale = rct.Height() / fldslicer->valrange->rrMinMax().rWidth();
			activePoint = CPoint(point.x, rct.bottom - yscale * fldslicer->bounds[i]); 
		}
		if ( fldslicer->selectedIndex != -1)
			break;
	}
}

void ValueSlicer::OnLButtonDblClk(UINT nFlags, CPoint point) {
	Color clr = fldslicer->rprgrad->GetColor(fldslicer->selectedIndex );
	SlicingStepColor frm(this,&clr);
	if ( frm.fOkClicked()) {
		fldslicer->rprgrad->SetLimitColor(fldslicer->selectedIndex  , clr);
		if ( fldslicer->selectedIndex == fldslicer->bounds.size() -2)
			fldslicer->rprgrad->SetLimitColor(fldslicer->selectedIndex+1  , clr);
		Invalidate();
		updateRepresentations();
	}
}

void ValueSlicer::setRprBase(const Representation& rprB) {
	rprBase = rprB;
}
//----------------------------------------------------
ValueSlicerSlider::ValueSlicerSlider(FormEntry* par, ILWIS::SetDrawer *sdrw) :
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
  setDrawer(sdrw);
  RangeReal rr = sdrw->getStretchRangeReal();
  setValueRange(ValueRange(rr,0));
  highColor = Color(70,255,30);
  lowColor = Color(20, 60,0);
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

void ValueSlicerSlider::setValueRange(const ValueRange& rng){
	if ( bounds.size() == 0)
		setNumberOfBounds(2);
	valrange = rng;
}

void ValueSlicerSlider::setBound(int index , double v){
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

void ValueSlicerSlider::setDrawer(ILWIS::SetDrawer *dr) {
	drawer = dr;
}

void ValueSlicerSlider::setNumberOfBounds(int n) {
	bounds.clear();
	bounds.resize(n + 1 );
	if ( isValid()) {
		for(int i=0; i < bounds.size(); ++i) {
			bounds[i] = valrange->rrMinMax().rLo() + i * (valrange->rrMinMax().rWidth() / (bounds.size() - 1));
		}
	}
	init();
}

bool ValueSlicerSlider::isValid() const {
	return valrange.fValid() && drawer!= 0;
}

void ValueSlicerSlider::setBoundColor(int index, Color c) {
	if ( index < bounds.size() -1) {
		rprgrad->SetLimitColor(index,c);
		if ( valueslicer) {
			valueslicer->Invalidate();
			valueslicer->updateRepresentations();
		}
	}
}

void ValueSlicerSlider::init() {
	if ( isValid()) {
		bool rprCreated = false;
		if ( !rprgrad) {
			for(int i=0; i < bounds.size(); ++i) {
				bounds[i] = valrange->rrMinMax().rLo() + i * (valrange->rrMinMax().rWidth() / (bounds.size() - 1));
			}
			rprgrad = new RepresentationValue(FileName(), Domain("value"));
			rprCreated = true;
		}
		rprgrad->reset();
		rprgrad->SetLimitValue(0,bounds[0]);
		rprgrad->SetLimitValue(1, bounds[bounds.size() -1]);
		rprgrad->SetLimitColor(0, lowColor);
		rprgrad->SetLimitColor(1, highColor);
		rprgrad->SetStretchSteps(1);
		rprgrad->SetColorMethod(0, RepresentationGradual::crLOWER);
		for(int i = 1; i < bounds.size()-1; ++i) {
			Color clr = nextColor(i);
			rprgrad->insert(bounds[i], clr);
			rprgrad->SetColorMethod(i,RepresentationGradual::crLOWER);
		}
		rpr.SetPointer(rprgrad);
		ILWIS::AbstractMapDrawer *parentDrw = (ILWIS::AbstractMapDrawer *)drawer->getParentDrawer();
		for(int i =0; i < parentDrw->getDrawerCount(); ++i) {
			ILWIS::SetDrawer *setdrw = (ILWIS::SetDrawer *)parentDrw->getDrawer(i);
			setdrw->setRepresentation(rpr);
		}
		drawingcolor = new ILWIS::DrawingColor(drawer);
	}
	if (valueslicer)
		valueslicer->Invalidate();
}

Color ValueSlicerSlider::nextColor(int i) {
	if ( highColor == colorUNDEF || lowColor == colorUNDEF)
		return colorUNDEF;
	int deltar = highColor.red() - lowColor.red();
	int deltag = highColor.green() - lowColor.green();
	int deltab = highColor.blue() - lowColor.blue();
	int deltat = highColor.transparency() - lowColor.transparency();
	int rstep = deltar / (bounds.size() - 2);
	int gstep = deltag / (bounds.size() - 2);
	int bstep = deltab / (bounds.size() - 2);
	int tstep = deltat / ( bounds.size() - 2);
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
//----------------------------------------------------------
SlicingStepColor::SlicingStepColor(CWnd* parent, Color* clr) :
FormWithDest(parent, TR("Step Color"))
{
	new FieldColor(root,TR("Step Color"), clr,true);
	create();
}


