#pragma once

ILWIS::DrawerTool *createHighLightTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class HighLightTool : public DrawerTool {
public:
	HighLightTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~HighLightTool();
	String getMenuString() const;
protected:
	void areaOfInterest();


};

class AnimationAreaOfInterest : public DisplayOptionsForm {
public:
	AnimationAreaOfInterest(CWnd *par, AnimationDrawer *gdr);
	~AnimationAreaOfInterest();
private:
	void apply();
	FlatIconButton *fb;
	FieldColor *fc;
	BoxDrawer *boxdrw;
	int createROIDrawer(Event*);
	void areaOfInterest(CRect rect);
	String boxId;
	Color clr;

};

}