#pragma once

#include <GL/gl.h>
#include "TextureHeap.h"
#define RSELECTDRAWER 323

ILWIS::NewDrawer *createRasterLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldOneSelectTextOnly;
class ValueSlicerSlider;

namespace ILWIS {

	struct BandInfo {
		int index;
		RangeReal rr;
	};
	class RasterSetData {
	public:
		RasterSetData() :
			init(false)
		{
			for(int i=0; i < 3; ++i)
				ccMaps[i].index = 2 - i;
			exceptionColor = colorUNDEF;
		};
		~RasterSetData()
		{
		}

		GLint maxTextureSize;
		unsigned long imageWidth, imageHeight; // actual image size
		unsigned long width, height; // power of 2
		bool init;
		BandInfo ccMaps[3];
		Color exceptionColor;
	};

	class DEMTriangulator;

	class _export RasterLayerDrawer : public LayerDrawer {
	public:
		RasterLayerDrawer(DrawerParameters *parms);
		RasterLayerDrawer(DrawerParameters *parms, const String& name);
		virtual ~RasterLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		virtual void setDrawMethod(DrawMethod method);
		void addDataSource(void *bmap, int options=0);
		bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const;
		void setThreaded(bool yesno);
		void setMinMax(const RangeReal & rrMinMax);
		Palette * SetPaletteOwner();
		void SetPalette(Palette * palette);
		Representation getRepresentation() const;
		String getInfo(const Coord& c) const;
		virtual void *getDataSource() const;
		
	protected:
		virtual void setData() const;
		virtual const GeoRef & gr() const;
		Map rastermap;

		void DisplayImagePortion(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const;
		void DisplayTexture(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		void DisplayTexture3D(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		double getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const;
		virtual void init() const;
		virtual void setup();
		String store(const FileName& fnView, const String& section) const;
		void load(const FileName& fnView, const String& currentSection);
		
		RasterSetData * data;
		RangeReal rrMinMax;
		bool isThreaded;
		bool isThreadedBeforeOffscreen;
		bool fLinear;
		bool fUsePalette;
		bool fPaletteOwner; // for maplist animation: all have same palette, just use the one of the first band
		Palette * palette;
		TextureHeap * textureHeap;
		TextureHeap * textureHeapBeforeOffscreen;
		DEMTriangulator * demTriangulator;
		bool fSetupDone;
	};
}