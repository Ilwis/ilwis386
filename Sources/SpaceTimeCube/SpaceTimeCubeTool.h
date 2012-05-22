#pragma once

ILWIS::DrawerTool *createSpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldOneSelectTextOnly;

namespace ILWIS {

	class LayerData {
	public:
		LayerData(NewDrawer *drw);
		NewDrawer * getDrawer() {return drawer;};
		bool getPlotOption();
		bool getSizeOption();
		void setPlotOption(bool _plotOption) {plotOption = _plotOption;};
		void setSizeOption(bool _sizeOption) {sizeOption = _sizeOption;};
		void setDrawer(NewDrawer * _drawer) {drawer = _drawer;};
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
		NewDrawer *drawer;
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

	class SpaceTimeCubeTool : public DrawerTool {
	public:
		SpaceTimeCubeTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~SpaceTimeCubeTool();
		String getMenuString() const;
		void refreshDrawerList();
		void setFormAutoDeleted();
	protected:
		void makeActive(void *v, HTREEITEM);
		TimePositionBar * timePosBar;
	private:
		void update();
		void startLayerOptionsForm();
		void setSpaceTimeCube(void *v, HTREEITEM);
		void replaceDrawer(NewDrawer * oldDrw, NewDrawer * newDrw);
		HTREEITEM getLayerHandle(NewDrawer* drwFind);
		vector<LayerData> layerList;
		vector<String> ownDrawerIDs;
		TimeBounds * timeBounds;
		RangeReal sizeStretch;
		bool useSpaceTimeCube;
		PreTimeOffsetDrawer * preTimeOffset;
		PostTimeOffsetDrawer * postTimeOffset;
		LayerOptionsForm * layerOptionsForm;
	};

	class LayerOptionsForm : public DisplayOptionsForm {
	public:
		LayerOptionsForm(ComplexDrawer * drawer, CWnd *wPar, SpaceTimeCubeTool & _spaceTimeCubeTool, vector<LayerData> & layerList);
		virtual void apply();
		virtual int exec();
	private:
		int ComboCallBackFunc(Event*);
		bool fFirstTime;
		SpaceTimeCubeTool & spaceTimeCubeTool;
		vector<FieldOneSelectTextOnly*> fosPlotMethod;
		vector<String> vsPlotMethod;
		vector<FieldColumn*> fcTimeColumn;
		vector<FieldColumn*> fcSizeColumn;
		vector<String> vsTimeColumnNames;
		vector<String> vsSizeColumnNames;
		vector<LayerData> & m_layerList;
	};
}