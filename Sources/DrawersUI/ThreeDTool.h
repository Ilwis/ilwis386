#pragma once

ILWIS::DrawerTool *createThreeDTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldMapList;
class FieldRealSliderEx;

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
	void setExtrusion(void *value, HTREEITEM);
	void extrusionOptions() ;

};

class DisplayZDataSourceForm : public DisplayOptionsForm {
	public:
	DisplayZDataSourceForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	int initForm(Event *ev);
	void updateDrawer(LayerDrawer *sdrw, const BaseMap& basemap);
	RadioGroup *rg;
	FieldMap *fmap;
	FieldMapList *fmaplist;
	RadioButton *rbTable;
	RadioButton *rbMaplist;
	RadioButton *zCoord;

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
	int updateOffset(Event *ev);
	int settransforms(Event *);
	FieldRealSliderEx *sliderScale;
	FieldRealSliderEx *sliderOffset;
	FieldRangeReal *frr;

	double zoffset;
	double zscale;
	RangeReal range;
};

class ExtrusionOptions : public DisplayOptionsForm {
public:
	ExtrusionOptions(CWnd *p, ComplexDrawer *fsd);
	void apply();
private:
	void setFSDOptions(LayerDrawer *fsd);
	int setTransparency(Event *ev);
	int line;
	int transparency;
	RadioGroup *rg;
	FieldIntSliderEx *slider;

};

}