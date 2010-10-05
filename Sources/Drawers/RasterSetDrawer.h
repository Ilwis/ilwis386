#pragma once

#include <GL/gl.h>
#include "TextureHeap.h"

ILWIS::NewDrawer *createRasterSetDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS{

	class RasterSetData {
	public:
		RasterSetData() :
			textureHeap(0),
			palette(0),
			init(false)
		{
		};
		~RasterSetData()
		{
			if (textureHeap)
				delete textureHeap;
		}

		TextureHeap * textureHeap;
		Palette * palette;
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
		virtual void setDrawMethod(DrawMethod method);
		virtual void setRepresentation(const Representation& rp);
		void addDataSource(void *bmap, int options=0);
		HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion , const CoordBounds& cbArea) const;
		void setThreaded(bool yesno);
		void setMinMax(const RangeReal & rrMinMax);
		
	protected:
		Map rastermap;

	private:

		void DisplayImagePortion(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const;
		void DisplayTexture(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const;
		double getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const;
		void init() const;

		RasterSetData * data;
		RangeReal rrMinMax;
		bool isThreaded;
		bool sameCsy;
		bool fUsePalette;
	};
}