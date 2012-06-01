#pragma once

#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"

ILWIS::DrawerTool *createSpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldOneSelectTextOnly;

namespace ILWIS {

	class LayerData {
	public:
		LayerData(NewDrawer *drw);
		bool getPlotOption();
		bool fUseSort();
		bool fUseGroup();
		bool fUseSize();
		void setPlotOption(bool _plotOption) {plotOption = _plotOption;};
		void setUseSort(bool _fSort) {fSort = _fSort;};
		void setUseGroup(bool _fGroup) {fGroup = _fGroup;};
		void setUseSize(bool _fSize) {fSize = _fSize;};
		void setDrawerId(String _drawerId) {drawerId = _drawerId;};
		String getDrawerId() {return drawerId;};
		bool hasTime();
		bool hasSort();
		bool hasGroup();
		bool hasSize();
		bool isFeatureMap() {return fFeatureMap;};
		bool isPointMap() {return fPointMap;};
		FileName fnObj() {return fnBaseMap;};
		Table & getAttTable() {return attTable;};
		void setTimeColumn(String sColName);
		void setSortColumn(String sColName);
		void setGroupColumn(String sColName);
		void setSizeColumn(String sColName);
		RangeReal rrTimeMinMax() {return m_rrTimeMinMax;};
		RangeReal rrSizeMinMax() {return m_rrSizeMinMax;};
		bool isSelfTime() {return fSelfTime;};
		Column & getTimeColumn() {return temporalColumn;};
		Column & getSortColumn() {return sortColumn;};
		Column & getGroupColumn() {return groupColumn;};
		Column & getSizeColumn() {return sizeColumn;};
	private:
		String drawerId;
		bool plotOption;
		bool fSort;
		bool fGroup;
		bool fSize;
		bool fSelfTime;
		bool fFeatureMap;
		bool fPointMap;
		FileName fnBaseMap;
		Table attTable;
		vector<Column> temporalColumns;
		vector<Column> sortColumns;
		vector<Column> groupColumns;
		vector<Column> sizeColumns;
		Column temporalColumn;
		Column sortColumn;
		Column groupColumn;
		Column sizeColumn;
		RangeReal m_rrTimeMinMax;
		RangeReal m_rrSizeMinMax;
	};

	class TimeBounds;
	class PreTimeOffsetDrawer;
	class PostTimeOffsetDrawer;
	class LayerOptionsForm;
	class TimePositionBar;

	class SpaceTimeCube {
	public:
		static SpaceTimeCube * getSpaceTimeCube(ZoomableView* mpv, LayerTreeView * tree, NewDrawer *drw);
		static void deleteSpaceTimeCube(ZoomableView* mpv);
		SpaceTimeCube(ZoomableView* mpv, LayerTreeView * _tree, NewDrawer *drw);
		virtual ~SpaceTimeCube();
		void setUseSpaceTimeCube(bool yesno);
		bool fUseSpaceTimeCube();
		void refreshDrawerList();
		void setFormAutoDeleted();
		void startLayerOptionsForm();
		bool showingLayerOptionsForm();
		void SetTime(double time, bool fShiftDown);
		double GetTime();
		TimeBounds * getTimeBoundsZoom() const;
		const TimeBounds * getTimeBoundsFullExtent() const;
	private:
		static map<ZoomableView*, SpaceTimeCube*> spaceTimeCubes;
		void update();
		void replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw);
		bool replaceTreeItem(NewDrawer * oldDrw, SpatialDataDrawer * newDrw, int index);
		void AddTimeOffsetDrawers(ComplexDrawer * drw, double * timeOffsetVariable, DrawerParameters & dp, PreparationParameters & pp);
		MapCompositionDoc * getDocument() const;
		HTREEITEM findTreeItem(NewDrawer* drwFind);
		TimePositionBar * timePosBar;
		vector<LayerData> layerList;
		vector<String> ownDrawerIDs;
		TimeBounds * timeBoundsZoom;
		TimeBounds * timeBoundsFullExtent;
		RangeReal sizeStretch;
		bool useSpaceTimeCube;
		double timePos;
		double timeOffset;
		double timeShift;
		String sTimePosText;
		LayerOptionsForm * layerOptionsForm;
		ZoomableView* mpv;
		RootDrawer* rootDrawer;
		LayerTreeView *tree;
	};

	class SpaceTimeCubeTool : public DrawerTool {
	public:
		SpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SpaceTimeCubeTool();
		String getMenuString() const;
		SpaceTimeCube * getSpaceTimeCube() const;
	protected:
		void makeActive(void *v, HTREEITEM);
	private:
		void setUseSpaceTimeCube(void *v, HTREEITEM);
		void startLayerOptionsForm();
		void addTools();
		void removeTools();
		SpaceTimeCube * stc;
		vector<HTREEITEM> htiElements;
	};

	class LayerOptionsForm : public DisplayOptionsForm {
	public:
		LayerOptionsForm(CWnd *wPar, SpaceTimeCube & _spaceTimeCube, vector<LayerData> & layerList);
		~LayerOptionsForm();
		virtual void apply();
		virtual void OnOK();
		virtual void OnCancel();
	private:
		int ComboCallBackFunc(Event*);
		bool fFirstTime;
		SpaceTimeCube & spaceTimeCube;
		vector<FieldOneSelectTextOnly*> fosPlotMethod;
		vector<String> vsPlotMethod;
		vector<FieldColumn*> fcTimeColumn;
		vector<FieldColumn*> fcSortColumn;
		vector<FieldColumn*> fcGroupColumn;
		vector<FieldColumn*> fcSizeColumn;
		vector<CheckBox*> cbSort;
		vector<CheckBox*> cbGroup;
		vector<CheckBox*> cbSize;
		vector<String> vsTimeColumnNames;
		vector<String> vsSortColumnNames;
		vector<String> vsGroupColumnNames;
		vector<String> vsSizeColumnNames;
		vector<bool*> vbSort;
		vector<bool*> vbGroup;
		vector<bool*> vbSize;
		vector<LayerData> & m_layerList;
	};
}