#pragma once

ILWIS::NewDrawer *createCollectionDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{
	class LayerDrawer;
	class CollectionDrawer;

	class _export CollectionDrawer : public SetDrawer {
	public:
		CollectionDrawer(DrawerParameters *parms, const String& title);
		virtual ~CollectionDrawer();
		virtual void prepare(PreparationParameters *pp);
		bool draw(int drawerIndex, const DrawLoop drawLoop, const CoordBounds& cbArea) const;
		String description() const;
		RangeReal getRange() const { return rrMinMax; }
		void addDataSource(void *bmap, int options=0);

	protected:
		virtual String iconName(const String& subtype="?") const;
		//void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const;

	};

}