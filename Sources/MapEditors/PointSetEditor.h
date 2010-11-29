#pragma once

class MapCompositionDoc;
class BaseMap;

ILWIS::BaseMapEditor *createPointSetEditor(MapCompositionDoc *doc, const BaseMap& bm);

namespace ILWIS {
	class _export PointSetEditor : public FeatureSetEditor {
	public:
		PointSetEditor(MapCompositionDoc *doc, const PointMap& bmap);
		~PointSetEditor();
		bool fPasteOk();
	public:
		bool insert(UINT nFlags, CPoint point);
		void OnCopy();
		void OnInsertMode();
		void OnUpdateMode(CCmdUI* pCmdUI);
		void OnMoveMode();
		void OnSelectMode();
		bool OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);

		void Mode(BaseMapEditor::Mode m) ;
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;
		zCursor curEdit, curPntEdit, curPntMove, curPntMoving;

		vector<Coord *> coords;

		DECLARE_MESSAGE_MAP()
	};
}