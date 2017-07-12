#pragma once

ILWIS::DrawerTool *createNonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class NonRepresentationToolTool : public DrawerTool {
public:
	NonRepresentationToolTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~NonRepresentationToolTool();
	String getMenuString() const;
protected:
	void displayOptionSingleColor();
	void displayOptionMultiColor();


};

class SetSingleColorForm : public DisplayOptionsForm {
public:
	SetSingleColorForm(CWnd *wPar, FeatureLayerDrawer *dr);
	void apply();
private:
	FieldColor *fc;
	Color c;

};

class SetMultipleColorForm : public DisplayOptionsForm {
public:
	SetMultipleColorForm(CWnd *wPar, LayerDrawer *dr);
	void apply();
private:
	void loadColorSets(const String& folder);
	FieldOneSelectString *fo;
	RadioGroup *rg;
	int colors;
	long choice;
	vector<String> sets;

};

}