
#pragma once

namespace ILWIS	{

	class _export FeatureDrawer : public AbstractDrawer {
	public:
		FeatureDrawer(DrawerContext *context, const String& ty);
		~FeatureDrawer();
		void setDataSource(void *p);
		void setCoordinateSystem(const CoordSystem& cs);
		void setDomain(const Domain& dm);
	protected:
		Feature *feature;
		CoordSystem csy;
		Domain dm;
	};
}