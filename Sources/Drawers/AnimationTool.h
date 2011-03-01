#pragma once

ILWIS::DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

namespace ILWIS {
	class AnimationBar : public CToolBar
	{
	// Construction
	public:
		AnimationBar();
		virtual ~AnimationBar();
		virtual void OnUpdateCmdUI(CFrameWnd*, BOOL);
		void Create(CWnd* pParent); 
		void updateTime(const String& );
	protected:
		afx_msg void OnSetFocus();
		afx_msg void OnKillFocus();

		CEdit ed;
		CFont fnt;
		bool fActive;

		DECLARE_MESSAGE_MAP()
	};


	class AnimationDrawer;

	class AnimationTool : public DrawerTool {
	public:
		AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::NewDrawer *drw);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~AnimationTool();
		String getMenuString() const;
	protected:
		void animationDefaultView();
		AnimationBar animBar;


	};

}