#pragma once

class MapCompositionDoc;
class BaseMap;

DrawerTool *createPointSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);

namespace ILWIS {
	class _export PointSetEditor : public FeatureSetEditor {
	public:
		PointSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		~PointSetEditor();
		bool fPasteOk();
	public:
		bool insertFeature(UINT nFlags, CPoint point);
		HTREEITEM configure( HTREEITEM parentItem);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		String getMenuString() const;
		void removeSelectedFeatures();
		void OnCopy();
		//void OnInsertMode();
		void OnUpdateMode(CCmdUI* pCmdUI);
		//void OnMoveMode();
		//void OnSelectMode();
		void OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);
		//bool OnLButtonUp(UINT nFlags, CPoint point);

		void setMode(BaseMapEditor::Mode m) ;
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;
		zCursor curEdit, curPntEdit, curPntMove, curPntMoving;

		vector<Coord *> coords;

		DECLARE_MESSAGE_MAP()
	};
}