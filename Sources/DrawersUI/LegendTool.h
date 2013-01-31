#pragma once

ILWIS::DrawerTool *createLegendTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class ColorSelector;

namespace ILWIS {

class LineLayerDrawer;

class LegendTool : public DrawerTool {
public:
	enum DrawerType{dtPOLYGON, dtPOINT, dtSEGMENT};
	LegendTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	~LegendTool();
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	void update();
	String getMenuString() const;
	DomainValueRangeStruct dvs() const { return dvrs; }
	void addValueItems(bool force=false);
private:
	void insertLegendItemsValue(const Representation& rpr);
	void insertLegendItemsClass(const Representation& rpr);
	void displayOptionLegend();
	void displayOptionRprClass();
	HTREEITEM htiLeg;
	DrawerType  drawerType;
	RangeReal vrr;
	double step;
	DomainValueRangeStruct dvrs;
};

class LineRprForm : public DisplayOptionsForm {
	public:
	LineRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw);
	void apply(); 
private:
  RepresentationClass* rcl;  
  ColorSelector* cs;
  int CustomColor(Event *);
  long iRaw;
  Color col;
  Line line;
  double transparency;

};

class LegendValueForm : public DisplayOptionsForm {
	public:
	LegendValueForm(CWnd *wPar, LayerDrawer *dr, LegendTool *tl, RangeReal& rnge, double& step);
	void apply(); 
private:
	double rmin,rmax;
	double& rstep;
	RangeReal& range;
	FieldReal *fstep, *fmax, *fmin;
	LegendTool *tool;

};

class PolRprForm : public DisplayOptionsForm {
	public:
	PolRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw);
	void apply(); 
private:
	int setTransparency(Event *ev);
  RepresentationClass* rcl;  
  long iRaw;
  Color col;
  Color col2;
  int transparency;
  String hatching;

};

class PointRprForm : public DisplayOptionsForm {
	public:
	PointRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw);
	void apply(); 
private:
  RepresentationClass* rcl;  
  long iRaw;
  Color col;
  String symbol;
  double scale;
};
}
