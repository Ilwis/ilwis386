#pragma once

ILWIS::DrawerTool *createGrid3DTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class Grid3DTool : public DrawerTool {
	public:
		Grid3DTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~Grid3DTool();
		String getMenuString() const;
	protected:
		void displayOptionGrid3D() ;
		void grid3D(void *v, HTREEITEM );
	

	};
	class Grid3DOptions : public DisplayOptionsForm {
	public:
		Grid3DOptions(CWnd *par, GridDrawer *gdr);
	private:
		void apply();
		int uncheckRest(Event *ev);
		CheckBox *cbgrid;
		CheckBox *cbplane; 
		CheckBox *cbcube;
		CheckBox *cbverticals;
		CheckBox *cbRetainGound;
		FieldIntSliderEx *slider;
		FieldGroup *fg;
		bool hasplane, hasgrid, iscube, hasverticals, hasaxis;
		bool threeD;

	};

}