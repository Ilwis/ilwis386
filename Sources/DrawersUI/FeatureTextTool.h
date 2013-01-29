#pragma once

ILWIS::DrawerTool *createFeatureTextTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class _export FeatureTextTool : public DrawerTool {
public:
	FeatureTextTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *drw);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~FeatureTextTool();
	String getMenuString() const;
	void makeActive(void *v, HTREEITEM );
protected:
	void setScaling();
	Table tbl;


};

class FeatureTextToolForm : public DisplayOptionsForm {
	public:
	FeatureTextToolForm(CWnd *wPar, FeatureLayerDrawer *dr, const Table& tbl);
	void apply(); 
private:
	Table tbl;
	String colName;
	double fscale;
	bool useAttrib;
	Color clr;
	FieldReal *fontScale;
	FieldColumn *fcolumns;
	FieldColor *fcolor;
	CheckBox *cb;
};


}