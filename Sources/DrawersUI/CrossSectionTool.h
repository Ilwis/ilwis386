#pragma once

ILWIS::DrawerTool *createCrossSectionTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);
class CrossSectionGraphEntry;

namespace ILWIS {
class CrossSectionGraphFrom;
class ProbeMarkers;


	class CrossSectionTool : public DrawerTool {
	public:
		CrossSectionTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~CrossSectionTool();
		String getMenuString() const;
		void clear();
		void addSource(const FileName& fn);
		void setcheckTool(void *w, HTREEITEM);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT fl, CPoint point);
	protected:
		CrossSectionGraphFrom *graphForm;

		void displayOptionAddList( );
		bool isUnique(const FileName& fn);
		ProbeMarkers *markers;

		vector<IlwisObject> sources;
		bool working;
	};

	class ChooseCrossSectionForm : public DisplayOptionsForm2 {
		public:
		ChooseCrossSectionForm(CWnd *wPar, LayerDrawer *dr, CrossSectionTool *t);
		int addSource(Event *ev);
		int exec();
	private:
		FieldDataType *fm;
		CrossSectionTool *tool;
		String name;
	};

	class CrossSectionGraphFrom : public DisplayOptionsForm2 {
	public:
		CrossSectionGraphFrom(CWnd *wPar, LayerDrawer *dr, vector<IlwisObject>& sources);
		//void apply(); 
		void setSelectCoord(const Coord& crd);
		void addSourceSet(const IlwisObject& obj);
		void reset();
	private:
		CrossSectionGraphEntry *graph;

	};

	class ProbeMarkers : public ComplexDrawer {
	public:
		ProbeMarkers(ILWIS::DrawerParameters *parms);
		~ProbeMarkers();
		//virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addMarker(const Coord& crd);
		private:
	};


}