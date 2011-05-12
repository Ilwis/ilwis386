#pragma once

ILWIS::DrawerTool *createGridTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {

	class AnimationDrawer;

	class GridTool : public DrawerTool {
	public:
		GridTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~GridTool();
		String getMenuString() const;
	protected:
		void gridActive(void *v, HTREEITEM );
		void gridOptions();


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