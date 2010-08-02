
#pragma once

namespace ILWIS	{

	class FeatureLayerDrawer;

	class _export FeatureDrawer : public SimpleDrawer {
	public:
		FeatureDrawer(DrawerParameters *parms, const String& ty);
		~FeatureDrawer();
		void addDataSource(void *p,int options=0);
		void prepare(PreparationParameters *pp);
		void draw(bool norecursion);
	protected:
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		void setColor(const Color& color);
		void setOpenGLColor() const;

		Feature *feature;
		CoordBounds cb;
		double red;
		double green;
		double blue;
	};

}