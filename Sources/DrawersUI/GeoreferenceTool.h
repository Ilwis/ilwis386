#pragma once

ILWIS::DrawerTool *createGeoReferenceTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldGeoRefExisting;

namespace ILWIS {

	class GeoReferenceTool : public DrawerTool {
	public:
		GeoReferenceTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GeoReferenceTool();
		String getMenuString() const;
	protected:
		void displayOptionGeoReference();
	};

class GeoReferenceForm : public DisplayOptionsForm {
	public:
	GeoReferenceForm(CWnd *wPar, ComplexDrawer *dr);
	void apply(); 
private:
	int setGeoReference(Event *ev);
	FormEntry *CheckData();
	FieldGeoRefExisting *fldGrf;

	String name;
};

}