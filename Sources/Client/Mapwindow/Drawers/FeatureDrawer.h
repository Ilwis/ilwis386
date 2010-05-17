
#pragma once

namespace ILWIS	{

	class _export FeatureDrawer : public AbstractDrawer {
	public:
		FeatureDrawer(DrawerParameters *parms, const String& ty);
		~FeatureDrawer();
		void setDataSource(void *p);
	protected:
		Feature *feature;
		Color clr;
		Color clr0;
	};

}