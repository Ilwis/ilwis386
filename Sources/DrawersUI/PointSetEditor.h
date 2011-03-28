#pragma once

class MapCompositionDoc;
class BaseMap;

DrawerTool *createPointSetEditor(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);

namespace ILWIS {
	class CoordForm;
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
		//void removeSelectedFeatures();
		void OnCopy();
		//void OnInsertMode();
		void OnUpdateMode(CCmdUI* pCmdUI);
		//void OnMoveMode();
		//void OnSelectMode();
		void OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);
		void OnLButtonDblClk(UINT nFlags, CPoint point);


		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;
		zCursor curEdit, curPntEdit, curPntMove, curPntMoving;

		vector<Coord *> coords;
		CoordForm *crdFrm;

		DECLARE_MESSAGE_MAP()
	};

	class CoordForm : public DisplayOptionsForm {
	public:
		CoordForm(CWnd *wPar, ComplexDrawer *dr, Feature *f);
		void apply(); 
	private:
		Coord crd;
		ILWIS::Point *pnt;
		FieldCoord *fc;
	};
}