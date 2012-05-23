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
		bool getSizeOption();
		void setPlotOption(bool _plotOption) {plotOption = _plotOption;};
		void setSizeOption(bool _sizeOption) {sizeOption = _sizeOption;};
		void setDrawerId(String _drawerId) {drawerId = _drawerId;};
		String getDrawerId() {return drawerId;};
		bool hasTime();
		bool isFeatureMap() {return fFeatureMap;};
		bool isPointMap() {return fPointMap;};
		FileName fnObj() {return fnBaseMap;};
		Table & getAttTable() {return attTable;};
		void setTimeColumn(String sColName);
		void setSizeColumn(String sColName);
		RangeReal rrTimeMinMax() {return m_rrTimeMinMax;};
		RangeReal rrSizeMinMax() {return m_rrSizeMinMax;};
		bool isSelfTime() {return fSelfTime;};
		bool isSelfSize() {return fSelfSize;};
		Column & getTimeColumn() {return temporalColumn;};
		Column & getSizeColumn() {return sizeColumn;};
	private:
		String drawerId;
		bool plotOption;
		bool sizeOption;
		bool fSelfTime;
		bool fSelfSize;
		bool fFeatureMap;
		bool fPointMap;
		FileName fnBaseMap;
		Table attTable;
		vector<Column> temporalColumns;
		vector<Column> sizeColumns;
		Column temporalColumn;
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
		void update();
		void setUseSpaceTimeCube(bool yesno);
		bool fUseSpaceTimeCube();
		void refreshDrawerList();
		void setFormAutoDeleted();
		void startLayerOptionsForm();
		void SetTime(double time);
		double GetTime();
	private:
		static map<ZoomableView*, SpaceTimeCube*> spaceTimeCubes;
		void replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw);
		bool replaceTreeItem(NewDrawer * oldDrw, SpatialDataDrawer * newDrw, int index);
		MapCompositionDoc * getDocument() const;
		HTREEITEM findTreeItem(NewDrawer* drwFind);
		TimePositionBar * timePosBar;
		vector<LayerData> layerList;
		vector<String> ownDrawerIDs;
		TimeBounds * timeBounds;
		RangeReal sizeStretch;
		bool useSpaceTimeCube;
		double timeOffset;
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
	protected:
		void makeActive(void *v, HTREEITEM);
	private:
		void setUseSpaceTimeCube(void *v, HTREEITEM);
		void startLayerOptionsForm();
		SpaceTimeCube * stc;
	};

	class LayerOptionsForm : public DisplayOptionsForm {
	public:
		LayerOptionsForm(CWnd *wPar, SpaceTimeCube & _spaceTimeCube, vector<LayerData> & layerList);
		virtual void apply();
		virtual int exec();
	private:
		int ComboCallBackFunc(Event*);
		bool fFirstTime;
		SpaceTimeCube & spaceTimeCube;
		vector<FieldOneSelectTextOnly*> fosPlotMethod;
		vector<String> vsPlotMethod;
		vector<FieldColumn*> fcTimeColumn;
		vector<FieldColumn*> fcSizeColumn;
		vector<String> vsTimeColumnNames;
		vector<String> vsSizeColumnNames;
		vector<LayerData> & m_layerList;
	};
}