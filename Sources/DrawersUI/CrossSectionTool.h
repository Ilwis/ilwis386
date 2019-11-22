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
		void uncheckTool();
		void displayOptionAddList();
		void updateCbStretch();
		vector<IlwisObject> sources;
	protected:
		CrossSectionGraphFrom *graphForm;
		DisplayOptionTreeItem *checkItem;
		bool isUnique(const FileName& fn);
		ProbeMarkers *markers;
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
		friend class CrossSectionTool;
	public:
		CrossSectionGraphFrom(CWnd *wPar, LayerDrawer *dr, vector<IlwisObject>& sources, CrossSectionTool *t);
		//void apply(); 
		void setSelectCoord(const Coord& crd);
		void addSourceSet(const IlwisObject& obj);
		void reset();
		void updateCbStretch();
		virtual void shutdown(int iReturn);
		int saveAsTable(Event *ev);
		int saveAsSpectrum(Event *ev);
	private:
		CheckBox* cbStretch;
		int stretchClicked(Event *);
		CrossSectionGraphEntry *graph;
		CrossSectionTool *tool;
		bool yStretch;
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