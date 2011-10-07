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

		//virtual bool fValid();

	private:
		void BindMe(DrawerContext * drawerContext);
		bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		const MapList mpl;
		RasterSetData *data;
	};
}


