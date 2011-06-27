#pragma once

#include <GL/gl.h>
#include "TextureHeap.h"
#define RSELECTDRAWER 323

ILWIS::NewDrawer *createRasterLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldOneSelectTextOnly;
class ValueSlicerSlider;

namespace ILWIS {

	class RasterSetData {
	public:
		RasterSetData() :
			init(false)
		{
		};
		~RasterSetData()
		{
		}

		GLint maxTextureSize;
		unsigned long imageWidth, imageHeight; // actual image size
		unsigned long width, height; // power of 2
		bool init;
	};

	class DEMTriangulator;

	class _export RasterLayerDrawer : public LayerDrawer {
	public:
		RasterLayerDrawer(DrawerParameters *parms);
		virtual ~RasterLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		virtual void setDrawMethod(DrawMethod method);
		void addDataSource(void *bmap, int options=0);
		bool draw( const CoordBounds& cbArea) const;
		void setThreaded(bool yesno);
		void setMinMax(const RangeReal & rrMinMax);
		Palette * SetPaletteOwner();
		void SetPalette(Palette * palette);
		//void addSelectionDrawers(const Representation& rpr);
		
	protected:
		Map rastermap;

	private:

		void DisplayImagePortion(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const;
		void DisplayTexture(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		void DisplayTexture3D(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		double getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const;
		void init() const;
		
		RasterSetData * data;
		RangeReal rrMinMax;
		bool isThreaded;
		bool isThreadedBeforeOffscreen;
		bool fGrfLinear;
		bool sameCsy;
		bool fUsePalette;
		bool fPaletteOwner; // for maplist animation: all have same palette, just use the one of the first band
		Palette * palette;
		TextureHeap * textureHeap;
		TextureHeap * textureHeapBeforeOffscreen;
		DEMTriangulator * demTriangulator;
	};
}