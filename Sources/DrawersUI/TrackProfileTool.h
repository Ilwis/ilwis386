#pragma once

ILWIS::DrawerTool *createTrackProfileTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class TrackProfileGraphEntry;

namespace ILWIS {
class TrackProfileGraphFrom;
class TrackLine;
class LayerDrawer;
class TrackMarker;

class TrackDataSource {
public:
	TrackDataSource(const IlwisObject& obj);
	void addSource(const IlwisObject& obj);
	BaseMap getMap(const Coord& crd=Coord()) const;
	IlwisObject getSource() const;
	void updateIndex(long ind);
private:
	long currentIndex;
	BaseMap bmp;
	MapList mpl;
	ObjectCollection oc;
	IlwisObject::iotIlwisObjectType type;
};

	class TrackProfileTool : public DrawerTool {
		friend class TrackProfileGraph;
	public:
		TrackProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~TrackProfileTool();
		String getMenuString() const;
		void clear();
		void addSource(const FileName& fn);
		void setcheckTool(void *w, HTREEITEM);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void setMarker(const Coord& crd);
		void timedEvent(UINT timerid);
		vector<TrackDataSource *> sources;
	protected:
		TrackProfileGraphFrom *graphForm;

		void displayOptionAddList( );
		bool isUnique(const FileName& fn);
		void setActiveMode(bool yesno);
		void setCoords();
		vector<Coord> coords;
		TrackLine *line;
		TrackMarker *point;
		bool working;
		bool fDown;
	};

	class TrackLine : public LineDrawer {
	public:
		TrackLine(ILWIS::DrawerParameters *parms);
		~TrackLine();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
	private:
	};

	class TrackMarker : public PointDrawer {
	public:
		TrackMarker(ILWIS::DrawerParameters *parms);
		~TrackMarker();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		private:
	};


	class ChooseTrackProfileForm : public DisplayOptionsForm2 {
		public:
		ChooseTrackProfileForm(CWnd *wPar, LayerDrawer *dr, TrackProfileTool *t);
		int addSource(Event *ev);
	private:
		FieldDataType *fm;
		TrackProfileTool *tool;
		String name;
	};

	class TrackProfileGraphFrom : public DisplayOptionsForm2 {
	public:
		TrackProfileGraphFrom(CWnd *wPar, LayerDrawer *dr,TrackProfileTool *t);
		//void apply(); 
		void setTrack(const vector<Coord>& crds);
		void reset();
		void addSource(const IlwisObject& bmp) ;
		void update();
	private:
		TrackProfileGraphEntry *graph;

	};

}