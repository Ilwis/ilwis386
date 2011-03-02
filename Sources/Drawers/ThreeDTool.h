#pragma once

ILWIS::DrawerTool *createThreeDTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

class AnimationDrawer;

class ThreeDTool : public DrawerTool {
public:
	ThreeDTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~ThreeDTool();
	String getMenuString() const;
protected:
	void displayZOption3D();
	void displayZScaling();
	void setExtrusion(void *value);
	void extrusionOptions() ;


};

class DisplayZDataSourceForm : public DisplayOptionsForm {
	public:
	DisplayZDataSourceForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	void updateDrawer(SetDrawer *sdrw);
	RadioGroup *rg;
	FieldMap *fmap;

	String colName;
	Table  attTable;
	String mapName;
	int sourceIndex;
	BaseMap bmp;
};

class ZDataScaling : public DisplayOptionsForm {
public:
	ZDataScaling(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	int settransforms(Event *);
	FieldRealSliderEx *sliderScale;
	FieldRealSliderEx *sliderOffset;

	double zoffset;
	double zscale;
};

class ExtrusionOptions : public DisplayOptionsForm {
public:
	ExtrusionOptions(CWnd *p, ComplexDrawer *fsd);
	void apply();
private:
	void setFSDOptions(SetDrawer *fsd);
	int setTransparency(Event *ev);
	int line;
	int transparency;
	RadioGroup *rg;
	FieldIntSliderEx *slider;

};

}