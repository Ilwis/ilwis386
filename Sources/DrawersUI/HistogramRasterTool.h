#pragma once

ILWIS::DrawerTool *createHistogramRasterTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldRealSliderEx;

namespace ILWIS {
class HistogramRasterTool : public DrawerTool {
public:
	HistogramRasterTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	String getMenuString() const;
	void update();
private:
	void displayOptionHisto();
	void setHisto(void *v, HTREEITEM);
};

class HistogramRasterToolForm : public DisplayOptionsForm {
	public:
	HistogramRasterToolForm(CWnd *wPar, NewDrawer *dr);
	void apply(); 
private:
	Color color;
	int spread;
	FieldColor *fcolor;
	FieldInt *fspread;

};
}