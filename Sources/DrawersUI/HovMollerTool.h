#pragma once

ILWIS::DrawerTool *createHovMollerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class HovMollerGraphEntry;

namespace ILWIS {
class HovMollerGraphFrom;
class HTrackLine;
class LayerDrawer;
class HTrackMarker;

class HovMollerDataSource {
public:
	HovMollerDataSource();
	void setSource(const MapList& mplist);
	MapList getSource() const;
	void updateIndex(long ind);
private:
	long currentIndex;
	MapList mpl;
};

	class HovMollerTool : public DrawerTool {
		friend class HovMollerGraph;
	public:
		HovMollerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~HovMollerTool();
		String getMenuString() const;
		void clear();
		void setSource(const FileName& fn);
		void setcheckTool(void *w, HTREEITEM);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void setMarker(const Coord& crd);
		HovMollerDataSource source;
	protected:
		HovMollerGraphFrom *graphForm;

		void displayOptionAddList( );
		bool isUnique(const FileName& fn);
		void setActiveMode(bool yesno);
		void setCoords();
		vector<Coord> coords;
		HTrackLine *line;
		HTrackMarker *point;
		bool working;
		bool fDown;
	};

	class HTrackLine : public LineDrawer {
	public:
		HTrackLine(ILWIS::DrawerParameters *parms);
		~HTrackLine();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
	private:
	};

	class HTrackMarker : public PointDrawer {
	public:
		HTrackMarker(ILWIS::DrawerParameters *parms);
		~HTrackMarker();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		private:
	};


	class ChooseHovMollerForm : public DisplayOptionsForm2 {
		public:
		ChooseHovMollerForm(CWnd *wPar, LayerDrawer *dr, HovMollerTool *t);
		int addSource(Event *ev);
	private:
		FieldDataType *fm;
		HovMollerTool *tool;
		String name;
	};

	class HovMollerGraphFrom : public DisplayOptionsForm2 {
	public:
		HovMollerGraphFrom(CWnd *wPar, LayerDrawer *dr,HovMollerTool *t);
		//void apply(); 
		void setTrack(const vector<Coord>& crds);
		void reset();
		void addSource(const MapList& ml) ;
		void update();
	private:
		int setSource(Event *ev);
		int init(Event *ev);
		int setTimeColumn(Event *ev);
		FormEntry *CheckData();


		String name;
		Table tbl;
		String columnName;
		HovMollerGraphEntry *graph;
		FieldDataType *fm;
		FieldColumn *fcTime;
		bool initial;


	};

}