#pragma once

ILWIS::DrawerTool *createLineStyleTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class LineStyleTool : public DrawerTool {
public:
	LineStyleTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~LineStyleTool();
	String getMenuString() const;
protected:
	void displayOptionSetLineStyle();
	LineDspType linestyle;
	double linethickness;
};

class LineStyleForm: public DisplayOptionsForm
{
public:
	LineStyleForm(CWnd *par, ComplexDrawer *gdr);
	void apply();
private:
	FieldReal *fi;
	FieldLineType *flt;
	FieldColor *fc;
	LineProperties *lprops;
	LineDspType style;
};
}

