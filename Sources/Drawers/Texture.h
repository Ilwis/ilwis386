// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
#define AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GL/gl.h>

#include "Engine\Drawers\ComplexDrawer.h" // for DrawMethod

namespace ILWIS {

	class DrawingColor;
	class DrawerContext;
	class Palette;

	class Texture
	{
	public:
		Texture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, unsigned int zoomFactor, unsigned int iPaletteSize, const RangeReal & rrMinMaxMap, const Palette * palette);
		virtual ~Texture();

		void CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop);
		virtual void BindMe(DrawerContext * drawerContext); // To be called before glBegin
		bool equals(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor);
		bool contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2);
		unsigned int getZoomFactor();
		void RepresentationChanged();
		bool fValid();

	protected:
		void PutLine(const RealBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf);
		void PutLine(const LongBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf);
		void ConvLine(const RealBuf& buf, LongBuf& bufColor);
		void ConvLine(const LongBuf& buf, LongBuf& bufColor);
		virtual bool DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		void PutLineData(const RealBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf);
		void PutLineData(const LongBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf);
		void StretchLine(const RealBuf& buf, IntBuf& bufData);
		void StretchLine(const LongBuf& buf, IntBuf& bufData);
		bool DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		GLuint texture;
		char * texture_data;
		const unsigned long sizeX, sizeY;
		const long offsetX, offsetY;
		const unsigned long imgWidth2, imgHeight2;
		const unsigned int zoomFactor;
		const DrawingColor * drawColor;
		const ComplexDrawer::DrawMethod drm;
		const unsigned int iPaletteSize;
		const RangeReal rrMinMaxMap;
		bool fValue;
		bool fAttTable;
		const Palette * palette;
		bool fRepresentationChanged;
		bool valid;
	private:
		const Map mp;
	};
}

#endif // !defined(AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
