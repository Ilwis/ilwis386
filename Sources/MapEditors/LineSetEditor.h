#pragma once

class MapCompositionDoc;
class BaseMap;

ILWIS::BaseMapEditor *createLineSetEditor(MapCompositionDoc *doc, const BaseMap& bm);

namespace ILWIS {
	class Segment;

	class _export LineSetEditor : public FeatureSetEditor {
	public:
		LineSetEditor(MapCompositionDoc *doc, const SegmentMap& mp);
		~LineSetEditor();
		bool fPasteOk();
	private:
		bool insert(UINT nFlags, CPoint point);
		void OnCopy();
		void OnInsertMode();
		void OnUpdateMode(CCmdUI* pCmdUI);
		void OnMoveMode();
		void OnSelectMode();
		void OnSplitMode();
		bool OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);
		void OnRemoveRedundantNodes();
		void OnCheckClosedSegments();
		void OnCheckCodeConsistency();
		void OnCheckIntersects();
		void OnCheckConnected();
		void OnCheckSelf();

		void Mode(BaseMapEditor::Mode m) ;
		bool fCheckClosedSegments(Tranquilizer& trq, long iStartSeg);
		bool fCheckCodeConsistency(Tranquilizer& trq, long iStartSeg);
		bool fCheckIntersects(Tranquilizer& trq, long iStartSeg); // returns true if ok;
		bool fCheckConnected(Tranquilizer& trq, long iStartSeg); // returns true if ok
		bool fCheckSelf(Tranquilizer& trq, long iStartSeg);
		void SetActNode(Coord crd);
		void ZoomInOnError();
		int AskSplit(ILWIS::Segment *seg, long iAfter, Coord crdAt);
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;

		vector<Coord *> coords;
		int iSnapPixels;
		bool fAutoSnap,fShowNodes;
     	zCursor curEdit, curSegEdit, curSegMove, curSegMoving,
          curSegSplit, curSegSplitting;
		String sMask;


		DECLARE_MESSAGE_MAP()
	};
}