#pragma once

#include <GL/gl.h>
#include "TextureHeap.h"
#define RSELECTDRAWER 323

ILWIS::NewDrawer *createColorCompositeDrawer(ILWIS::DrawerParameters *parms);

class FieldOneSelectTextOnly;
class ValueSlicerSlider;

namespace ILWIS {

	//struct BandInfo {
	//	int index;
	//	RangeReal rr;
	//};
	//class RasterSetData {
	//public:
	//	RasterSetData() :
	//		init(false)
	//	{
	//		for(int i=0; i < 3; ++i)
	//			ccMaps[i].index = 2 - i;
	//		exceptionColor = colorUNDEF;
	//	};
	//	~RasterSetData()
	//	{
	//	}

	//	GLint maxTextureSize;
	//	unsigned long imageWidth, imageHeight; // actual image size
	//	unsigned long width, height; // power of 2
	//	bool init;
	//	BandInfo ccMaps[3];
	//	Color exceptionColor;
	//};

	//class DEMTriangulator;

	class _export ColorCompositeDrawer : public RasterLayerDrawer {
	public:
		ColorCompositeDrawer(DrawerParameters *parms);
		virtual void prepare(PreparationParameters *pp);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		void addDataSource(void *bmap, int options=0);
		void setMinMax(const RangeReal & rrMinMax);
		bool isColorComposite() const;
		int getColorCompositeBand(int index);
		void setColorCompositeBand(int index, int maplistIndex);
		void setColorCompositeRange(int index, const RangeReal& rr);
		RangeReal getColorCompositeRange(int index);
		Color getExceptionColor() const;
		void setExceptionColor(const Color& clr);
		MapList getMapList() const;
		void setDrawMethod(DrawMethod method) ;
		String getInfo(const Coord& c) const;
		
	protected:
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);

		virtual void setData() const;
		virtual GeoRef gr() const;
		MapList mpl;



	};
}