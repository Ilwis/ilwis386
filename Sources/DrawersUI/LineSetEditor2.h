#pragma once

class MapCompositionDoc;
class BaseMap;

DrawerTool *createLineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);

namespace ILWIS {
	class Segment;

	struct PointClickInfo {
		PointClickInfo(Segment *s=0,NewDrawer *drw=0, long index=iUNDEF, const Coord& crd=Coord()) ;
		Segment *seg;
		NewDrawer *drawer;
		long crdIndex;
		Coord crdClick;
		bool linePoint;
	};

	struct UndoItem {
		UndoItem() : seg(0), seq(0), drawer(0), value(iUNDEF), state(0), created(false) {}
		~UndoItem();
		Segment *seg;
		NewDrawer *drawer;
		long value;
		CoordinateSequence *seq;
		int state;
		bool created;
	};

	struct SelectedSegment {
		SelectedSegment() { seg=0; drawer = 0; }
		~SelectedSegment();
		vector<int> selectedCoords;
		Segment *seg;
		NewDrawer *drawer;
	};

	class SelectedSegments : public vector<SelectedSegment *>{
	public:
		SelectedSegments() {};
		~SelectedSegments();
		void remove(Segment *seg, int index=iUNDEF);
		SelectedSegment *add(Segment *seg, NewDrawer *drw, int index, bool add);
		void empty();

	};

	class _export LineSetEditor2 : public FeatureSetEditor2 {
	public:


		LineSetEditor2(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		~LineSetEditor2();
		//bool fPasteOk();
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		void select(const CoordinateSequence *seq);
	private:
		String getMenuString() const;
		void getSettings();
		HTREEITEM configure( HTREEITEM parentItem);
		void OnUpdateMode(CCmdUI* pCmdUI);
		void OnUpdateUndo(CCmdUI* pCmdUI);
		void OnUpdateRedo(CCmdUI* pCmdUI);
		bool OnContextMenu(CWnd* pWnd, CPoint point);
		void OnUndo();
		void OnRedo();
		void updateFeature(SelectedFeature *f);
		//long getState() const;
		//void setState(long state);
		void updateState();
		void fillPointClickInfo(const Coord& crd);
		void createFeature();
		void insertVertex(bool endEdit = false);
		void selectVertex(bool add = false);
		void split();
		void updatePositions();
		int noSnap(CoordinateSequence *seq, double snap);
		void deleteVertices();
		void storeUndoState(States state, bool created = false);
		void checkSegments();
		bool hasSelection() const;


		void setMode(LineSetEditor2::States state) ;
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnMouseMove(UINT nFlags, CPoint point);
		void checkUndefined(void *v, HTREEITEM it);
		void checkAutoClose(void *v, HTREEITEM it);
		void checkSnap(void *v, HTREEITEM it);
		String sTitle() const;
		zIcon icon() const;
		int iFmtPnt, iFmtDom;

		double rSnapDistance;
		bool fAutoSnap,fShowNodes;
     	zCursor curSegSplit, curSegSplitting;
		long insertionPoint;
		SelectedSegments selectedSegments;
		Coord crdMouse;
		PointClickInfo pci;
		stack<UndoItem *> undoItems;
		bool useAutoClose;

		DECLARE_MESSAGE_MAP()
	};

	//---------------------------------------
	class CheckSegmentsForm : public DisplayOptionsForm {
	public:
		CheckSegmentsForm(CWnd *par, LayerDrawer *sdr, LineSetEditor2* ed);
	private:
		LineSetEditor2 *editor;

		int deadEnds(Event *ev);
		void apply();
	};
}