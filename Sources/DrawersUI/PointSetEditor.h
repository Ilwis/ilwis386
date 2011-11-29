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
	public:
		bool insertFeature(UINT nFlags, CPoint point);
		HTREEITEM configure( HTREEITEM parentItem);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		String getMenuString() const;
		void OnPaste();
		void OnUpdatePaste(CCmdUI* pCmdUI);
		void OnCopy();
		void OnUpdateCopy(CCmdUI* pCmdUI);
		void OnUpdateMode(CCmdUI* pCmdUI);
		void OnContextMenu(CWnd* pWnd, CPoint point);
		void updateFeature(SelectedFeature *f);
		void OnLButtonDblClk(UINT nFlags, CPoint point);
		bool fCopyOk();
		bool fPasteOk();
		void OnSelectAll();
		void setActive(bool yesno);


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
		void setFeature(Feature *f);
	private:
		Coord crd;
		ILWIS::Point *pnt;
		FieldCoord *fc;
	};
}