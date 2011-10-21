// CCTexture.h: interface for the CCTexture class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/gl.h>

#include "Texture.h" // for DrawMethod

namespace ILWIS {

	class DrawingColor;
	class DrawerContext;
	class Palette;
	class RasterSetData;

	class CCTexture : public Texture
	{
	public:
		CCTexture(const MapList & _mpl, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, RasterSetData *_data, unsigned int zoomFactor, const RangeReal & rrMinMaxMap);
		virtual ~CCTexture();
		virtual void CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop);
		virtual void ReCreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop);

		//virtual bool fValid();

	private:
		double stretch(double v, const RangeReal& rrFrom, const RangeReal& rrTo);
		void BindMe(DrawerContext * drawerContext);
		bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		const MapList mpl;
		RasterSetData *data;
	};
}


