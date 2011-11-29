#pragma once

ILWIS::DrawerTool *createAnimationTimeSelectionTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class FieldLister;

namespace ILWIS {

	class AnimationDrawer;

	class AnimationTimeSelectionTool : public DrawerTool {
	public:
		AnimationTimeSelectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationTimeSelectionTool();
		String getMenuString() const;
	protected:
		void timeSelection() ;


	};

	class TimeSelection: public DisplayOptionsForm2
	{
	public:
		TimeSelection(CWnd *par, AnimationDrawer *gdr);
		int exec();
	private:
		int setStep(Event *ev);
		void FillData();
		FieldLister *fl;
		FieldInt *fiStep;
		vector<String> data;
		vector<String> cols;
		vector<int>& activeMaps;
		long step;
	};

}