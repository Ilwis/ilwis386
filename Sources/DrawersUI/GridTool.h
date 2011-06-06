#pragma once

ILWIS::DrawerTool *createGridTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class GridTool : public DrawerTool {
	public:
		GridTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GridTool();
		String getMenuString() const;
		void toggleGraticule(bool yesno) { hasGraticule = yesno;}
	protected:
		void gridActive(void *v, HTREEITEM );
		void graticuleActive(void *v, HTREEITEM);
		void gridOptions();
		bool hasGraticule;


	};

	class GridForm: public DisplayOptionsForm
	{
	public:
		GridForm(CWnd *par, GridDrawer *gdr);
		void apply();
	private:
		FieldReal *fr;
		GridDrawer *gd;
		double rDist;
		int dummy;
	};

}