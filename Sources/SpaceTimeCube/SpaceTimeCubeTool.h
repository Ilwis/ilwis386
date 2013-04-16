#pragma once

#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "TimeMessages.h"

ILWIS::DrawerTool *createSpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldOneSelectTextOnly;

namespace ILWIS {

	class LayerData {
	public:
		LayerData(NewDrawer *drw);
		String sPlotOption();
		bool fUseSort();
		bool fUseGroup();
		bool fUseSize();
		bool fUseTime2();
		void setPlotOption(String _plotOption) {plotOption = _plotOption;};
		void setUseTime2(bool _fTime2);
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
		void setTimeColumn2(String sColName);
		void setSortColumn(String sColName);
		void setGroupColumn(String sColName);
		void setSizeColumn(String sColName);
		RangeReal rrTimeMinMax() {return m_rrTimeMinMax;};
		RangeReal rrSizeMinMax() {return m_rrSizeMinMax;};
		bool isSelfTime() {return fSelfTime;};
		Column & getTimeColumn() {return temporalColumn;};
		Column & getTimeColumn2() {return temporalColumn2;};
		Column & getSortColumn() {return sortColumn;};
		Column & getGroupColumn() {return groupColumn;};
		Column & getSizeColumn() {return sizeColumn;};
		void updateFromLayer();
	private:
		const NewDrawer *drawer;
		String drawerId;
		String plotOption;
		bool fSort;
		bool fGroup;
		bool fSize;
		bool fSelfTime;
		bool fTime2;
		bool fFeatureMap;
		bool fPointMap;
		FileName fnBaseMap;
		Table attTable;
		vector<Column> temporalColumns;
		vector<Column> sortColumns;
		vector<Column> groupColumns;
		vector<Column> sizeColumns;
		Column temporalColumn;
		Column temporalColumn2;
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

	class SpaceTimeCube : public TimeListener {
	public:
		static SpaceTimeCube * getSpaceTimeCube(ZoomableView* mpv, LayerTreeView * tree, NewDrawer *drw);
		static void deleteSpaceTimeCube(ZoomableView* mpv);
		SpaceTimeCube(ZoomableView* mpv, LayerTreeView * _tree, NewDrawer *drw);
		virtual ~SpaceTimeCube();
		void setUseSpaceTimeCube(bool yesno);
		bool fUseSpaceTimeCube();
		void refreshDrawerList(bool fFromForm);
		void setFormAutoDeleted();
		void startLayerOptionsForm();
		bool showingLayerOptionsForm();
		virtual void SetTime(double timePerc, long sender);
		TimeBounds * getTimeBoundsZoom() const;
		const TimeBounds * getTimeBoundsFullExtent() const;
		void loadMapview();
	private:
		static map<ZoomableView*, SpaceTimeCube*> spaceTimeCubes;
		void update(bool fFillForm);
		void replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw);
		bool replaceTreeItem(NewDrawer * oldDrw, SpatialDataDrawer * newDrw, int index);
		void DeleteDrawerTools(DrawerTool * tool, NewDrawer * drawer);
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
		vector<FieldColumn*> fcTimeColumn2;
		vector<FieldColumn*> fcSortColumn;
		vector<FieldColumn*> fcGroupColumn;
		vector<FieldColumn*> fcSizeColumn;
		vector<CheckBox*> cbTime2;
		vector<CheckBox*> cbSort;
		vector<CheckBox*> cbGroup;
		vector<CheckBox*> cbSize;
		vector<String> vsTimeColumnNames;
		vector<String> vsTime2ColumnNames;
		vector<String> vsSortColumnNames;
		vector<String> vsGroupColumnNames;
		vector<String> vsSizeColumnNames;
		vector<bool*> vbTime2;
		vector<bool*> vbSort;
		vector<bool*> vbGroup;
		vector<bool*> vbSize;
		vector<LayerData> & m_layerList;
	};
}