#pragma once

#include <GL/gl.h>
#include "TextureHeap.h"
#define RSELECTDRAWER 323

ILWIS::NewDrawer *createRasterSetDrawer(ILWIS::DrawerParameters *parms);

class FieldOneSelectTextOnly;
class ValueSlicerSlider;

namespace ILWIS{

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
		CoordBounds cb;
		long imageWidth, imageHeight;
		bool init;
	};

	class _export RasterSetDrawer : public SetDrawer {
	public:
		RasterSetDrawer(DrawerParameters *parms);
		virtual ~RasterSetDrawer();
		virtual void prepare(PreparationParameters *pp);
		virtual void prepareChildDrawers(PreparationParameters *pp);
		virtual void setDrawMethod(DrawMethod method);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		void setThreaded(bool yesno);
		void setMinMax(const RangeReal & rrMinMax);
		Palette * SetPaletteOwner();
		void SetPalette(Palette * palette);
		void addSelectionDrawers(const Representation& rpr);
		
	protected:
		Map rastermap;

	private:

		void DisplayImagePortion(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const;
		void DisplayTexture(double x1, double y1, double x2, double y2, Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		double getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const;
		void init() const;
		void rasterSlicing(CWnd *parent);
		void highLightSelection(CWnd *parent);

		RasterSetData * data;
		RangeReal rrMinMax;
		bool isThreaded;
		bool sameCsy;
		bool fUsePalette;
		bool fPaletteOwner; // for maplist animation: all have same palette, just use the one of the first band
		Palette * palette;
		TextureHeap * textureHeap;
	};

	class InterActiveSlicing : public DisplayOptionsForm2 {
	public:
		InterActiveSlicing(CWnd *par, RasterSetDrawer *gdr);
	private:
		FieldOneSelectTextOnly *fldSteps;
		int createSteps(Event*);
		String steps;
		ValueSlicerSlider *vs;
		int saveRpr(Event *ev);
		FileName fnRpr;

	};

	class HighLightDrawer : public DisplayOptionsForm2 {
	public:
		HighLightDrawer(CWnd *par, RasterSetDrawer *gdr);
	private:
		FieldOneSelectTextOnly *fldSteps;
		int createSteps(Event*);
		String steps;
		ValueSlicerSlider *vs;

	};

}