#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\FormElements\fldcolor.cpp"
#include "Drawers\DrawingColor.h"
#include "Client\ilwis.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Drawers\ValueSlicer.h"
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

ValueSlicer::ValueSlicer(ValueSlicerSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldslicer = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create Slicer"));
	activePoint = CPoint(100000, 100000);
	EnableToolTips(TRUE);
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

	double y = 0;
	for(int i = 1; i < fldslicer->bounds.size(); ++i) {
		
		double v1 =  fldslicer->bounds[i-1];
		double v2 = fldslicer->bounds[i];
		Color c = fldslicer->drawingcolor->clrVal((v1 + v2)/2.0);
		CBrush brush(c);
		oldBrush = SelectObject(lpDIS->hDC, brush);
		int yup = rct.bottom - y - (v2 - v1) * yscale;
		int ydown = rct.bottom - y;
		::Rectangle(lpDIS->hDC, rct.left, ydown, rct.right, yup);
		y += (v2 - v1) * yscale;
		String txt = fldslicer->valrange->vri() ? String("%d",(int)v2) : String("%.03f", v2);
		::TextOut(lpDIS->hDC,rct.right, rct.bottom - y - 8, txt.scVal(),txt.size());
		ylimits[i] = rct.bottom - y;
		SelectObject(lpDIS->hDC, oldBrush);
		
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
			int tolerance = (double)rct.Height() * 0.02;
			TRACE(String("delta %d, tolerance %d,index %d yl %d yp %d\n", delta, tolerance,index, ylimits[index], point.y).scVal());
			if (  delta < tolerance) {
				break;
			}
		}
		if ( index >= ylimits.size() - 1)
			return;
		fldslicer->bounds[index] = v;
		fldslicer->rprgrad->SetLimitValue(index, v);
		activePoint.y = point.y;
	
		Invalidate();

	}
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
			TRACE(String("down Index = %d\n", fldslicer->selectedIndex).scVal());
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
		Invalidate();
	}

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

void ValueSlicerSlider::init() {
	if ( isValid()) {
		bool rprCreated = false;
		if ( !rprgrad) {
			for(int i=0; i < bounds.size(); ++i) {
				bounds[i] = valrange->rrMinMax().rLo() + i * (valrange->rrMinMax().rWidth() / (bounds.size() - 1));
			}
			rprgrad = new RepresentationGradual(FileName(), Domain("value"));
			rprCreated = true;
		}
		rprgrad->reset();
		int colorStep = 255 / bounds.size();
		rprgrad->SetLimitValue(0,bounds[0]);
		rprgrad->SetLimitValue(1, bounds[bounds.size() -1]);
		rprgrad->SetLimitColor(0, Color(50,50,50));
		rprgrad->SetLimitColor(1, Color(min(50 + bounds.size() * colorStep, 255), 50,50));
		for(int i = 1; i < bounds.size()-1; ++i) {
			Color clr(50 + i * colorStep,50,50);
			rprgrad->insert(bounds[i], clr);
			rprgrad->SetColorMethod(i,RepresentationGradual::crLOWER);
		}
		if ( rprCreated) {
			rpr.SetPointer(rprgrad);
			drawer->setRepresentation(rpr);
			drawingcolor = new ILWIS::DrawingColor(drawer);
		}
	}
	if (valueslicer)
		valueslicer->Invalidate();
}

//----------------------------------------------------------
SlicingStepColor::SlicingStepColor(CWnd* parent, Color* clr) :
FormWithDest(parent, TR("Step Color"))
{
	new FieldColor(root,TR("Step Color"), clr);
	create();
}


