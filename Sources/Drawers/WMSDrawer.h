#pragma once

ILWIS::NewDrawer *createWMSDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	class WMSData {
	public:
		CoordBounds cbFullExtent;
	};

	class _export WMSDrawer : public RasterLayerDrawer {
	public:
		WMSDrawer(DrawerParameters *parms);
		virtual ~WMSDrawer();
		virtual bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
		virtual String getInfo(const Coord& c) const;
	protected:
		void DisplayImagePortion(CoordBounds &cb) const;
		void DisplayTexture(CoordBounds & cb) const;
		void DisplayTexture3D(CoordBounds & cb) const;
		virtual void setData() const;
		virtual void init() const;
		virtual void setup();
		virtual void *getDataSource() const;
		WMSData * wmsData;
	};
}