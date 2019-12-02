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
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void uncheckTool();
		void displayOptionAddList();
		void updateCbStretch();
		void setCustomRange();
		vector<IlwisObject> sources;
	protected:
		CrossSectionGraphFrom *graphForm;
		DisplayOptionTreeItem *checkItem;
		bool isUnique(const FileName& fn);
		ProbeMarkers *markers;
		bool working;
		bool fDown;
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
		friend class CrossSectionTool;
	public:
		CrossSectionGraphFrom(CWnd *wPar, LayerDrawer *dr, vector<IlwisObject>& sources, CrossSectionTool *t);
		//void apply(); 
		void addCoord(const Coord& crd);
		void setLastCoord(const Coord& crd);
		void addSourceSet(const IlwisObject& obj);
		void reset();
		void updateCbStretch();
		void setCustomRange();
		virtual void shutdown(int iReturn);
		int saveAsTable(Event *ev);
		int saveAsSpectrum(Event *ev);
	private:
		RadioGroup* rgStretch;
		CheckBox* cbStretch;
		int stretchClicked(Event *);
		int scaleChanged(Event *);
		CrossSectionGraphEntry *graph;
		CrossSectionTool *tool;
		bool yStretch;
		enum ScaleMethod {MAP,PROBE,CUSTOM} iScaleMethod;
	};

	class ProbeMarkers : public ComplexDrawer {
	public:
		ProbeMarkers(ILWIS::DrawerParameters *parms);
		~ProbeMarkers();
		//virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void addMarker(const Coord& crd);
		void setLastMarkerCoord(const Coord& crd);
		private:
	};


}