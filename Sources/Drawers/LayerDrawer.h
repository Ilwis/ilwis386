#pragma once


namespace ILWIS{

class DrawingColor;

class _export LayerDrawer : public ComplexDrawer {
	public:
		enum StretchMethod { smLINEAR, smLOGARITHMIC };
		ILWIS::NewDrawer *createLayerDrawer(DrawerParameters *parms);

		LayerDrawer(DrawerParameters *parms, const String& name);
		virtual ~LayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		virtual Representation getRepresentation() const;
		virtual void setRepresentation(const Representation& rp);
		//bool isLegendUsefull() const;
		RangeReal getStretchRangeReal(bool useAttrb = false) const;
		void setStretchRangeReal(const RangeReal& rr);
		bool isStretched() const;
		StretchMethod getStretchMethod() const;
		void setStretchMethod(StretchMethod sm);
		ILWIS::DrawingColor *getDrawingColor() const;
		CoordSystem getCoordSystem() const;
		bool useInternalDomain() const;
		String iconName(const String& subtype="?") const ;
		String getInfo(const Coord& crd) const;
		static unsigned long test_count;
		bool draw( const CoordBounds& cbArea) const;
		//void updateLegendItem();
		Column getAtttributeColumn() const;
		void setAttributeColumn(const Column& col );
		bool useAttributeColumn() const;
		void setUseAttributeColumn(bool yesno);
		double getExtrusionTransparency() const;
		void setExtrustionTransparency(double v);
		virtual void modifyLineStyleItem(LayerTreeView  *tv, bool remove=false) {}
		void *getDataSource() { return getParentDrawer()->getDataSource(); }
		RangeReal getTransparentValues() const;
		void setTransparentValues(const RangeReal& rr);
		virtual bool isSelectable() const;
		virtual void setSelectable(bool yesno );
		void select(const CoordBounds& cbSelect);
			
	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;

		//BaseMap basemap;
		Representation rpr;
		CoordSystem csy;
		Column attColumn;
		bool useAttColumn;
		DrawingColor *drawColor;
		RangeReal rrStretch;
		RangeReal rrLegendRange;
		bool stretched;
		StretchMethod stretchMethod;
		double extrTransparency;
	};



}