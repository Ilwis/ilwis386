#pragma once

class MapCompositionDoc;
class BaseMap;

DrawerTool *createLineSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);

namespace ILWIS {
	class Segment;

	class _export LineSetEditor : public FeatureSetEditor {
	public:
		LineSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		~LineSetEditor();
		bool fPasteOk();
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
	private:
		String getMenuString() const;
		void getSettings();
		bool insertFeature(UINT nFlags, CPoint point);
		bool insertVertex(UINT nFlags, CPoint point) ;
		void OnCopy();
		HTREEITEM configure( HTREEITEM parentItem);
		void OnUpdateMode(CCmdUI* pCmdUI);
		bool OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);
		void OnRemoveRedundantNodes();
		void OnCheckClosedSegments();
		void OnCheckCodeConsistency();
		void OnCheckIntersects();
		void OnCheckConnected();
		void OnCheckSelf();

		void setMode(BaseMapEditor::Mode m) ;
		bool fCheckClosedSegments(Tranquilizer& trq, long iStartSeg);
		bool fCheckCodeConsistency(Tranquilizer& trq, long iStartSeg);
		bool fCheckIntersects(Tranquilizer& trq, long iStartSeg); // returns true if ok;
		bool fCheckConnected(Tranquilizer& trq, long iStartSeg); // returns true if ok
		bool fCheckSelf(Tranquilizer& trq, long iStartSeg);
		void SetActNode(Coord crd);
		void ZoomInOnError();
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		int AskSplit(ILWIS::Segment *seg, long iAfter, Coord crdAt);
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;
		void findSnapPoint();

		vector<Coord *> coords;
		int iSnapPixels;
		bool fAutoSnap,fShowNodes;
     	zCursor curSegSplit, curSegSplitting;
		String sMask;
		ILWIS::Segment * segment;

		DECLARE_MESSAGE_MAP()
	};
}