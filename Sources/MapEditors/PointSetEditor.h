#pragma once

class MapCompositionDoc;
class BaseMap;

ILWIS::FeatureSetEditor *createPointSetEditor(MapCompositionDoc *doc, const BaseMap& bm);

namespace ILWIS {
	class _export PointSetEditor : public FeatureSetEditor {
	public:
		PointSetEditor(MapCompositionDoc *doc, const PointMap& bmap);
		~PointSetEditor();
		bool fPasteOk();
	public:
		void OnCopy();
		void OnInsertMode();
		void OnUpdateMode(CCmdUI* pCmdUI);
		void OnMoveMode();
		void OnSelectMode();
		bool OnContextMenu(CWnd* pWnd, CPoint point);

		void Mode(BaseMapEditor::Mode m) ;
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;
		zCursor curEdit, curPntEdit, curPntMove, curPntMoving;

		vector<Coord *> coords;

		DECLARE_MESSAGE_MAP()
	};
}