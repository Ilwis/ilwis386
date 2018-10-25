#pragma once

ILWIS::DrawerTool *createBooleanColorsTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class BooleanColorsTool : public DrawerTool {
public:
	BooleanColorsTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~BooleanColorsTool();
	String getMenuString() const;
protected:
	void displayOptionColorTrue();
	void displayOptionColorFalse();

};

class SetColorFormTrue : public DisplayOptionsForm {
public:
	SetColorFormTrue(CWnd *wPar, LayerDrawer *dr);
	void apply();
private:
	int setTransparency(Event *ev);
	FieldColor *fc;
	Color c;

};

class SetColorFormFalse : public DisplayOptionsForm {
public:
	SetColorFormFalse(CWnd *wPar, LayerDrawer *dr);
	void apply();
private:
	int setTransparency(Event *ev);
	FieldColor *fc;
	Color c;

};

}