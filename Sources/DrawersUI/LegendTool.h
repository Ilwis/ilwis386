#pragma once

ILWIS::DrawerTool *createLegendTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class ColorSelector;

namespace ILWIS {

class LineLayerDrawer;

class LegendTool : public DrawerTool {
public:
	LegendTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	~LegendTool();
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	void update();
	String getMenuString() const;
private:
	void insertLegendItemsValue(const Representation& rpr, const DomainValueRangeStruct& dvs);
	void insertLegendItemsClass(const Representation& rpr);
	void displayOptionRprClass();
	HTREEITEM htiLeg;
	IlwisObject::iotIlwisObjectType mapType;
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

class PolRprForm : public DisplayOptionsForm {
	public:
	PolRprForm(CWnd *wPar, LayerDrawer *dr, RepresentationClass* rc, long raw);
	void apply(); 
private:
	int setTransparency(Event *ev);
  RepresentationClass* rcl;  
  long iRaw;
  Color col;
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
  String hatching;

};
}
