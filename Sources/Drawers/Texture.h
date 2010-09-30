// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
#define AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GL/gl.h>

#include "Client\MapWindow\Drawers\ComplexDrawer.h" // for DrawMethod

namespace ILWIS {

	class DrawingColor;
	class DrawerContext;

	class Texture  
	{
	public:
		Texture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const long sizeX, const long sizeY, char * scrap_data_mipmap, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, DrawerContext * drawerContext, bool fInThread, volatile bool* fDrawStop);
		Texture(const Map & mp, const long offsetX, const long offsetY, const long sizeX, const long sizeY, char * scrap_data_mipmap, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, unsigned int iPaletteSize, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext, bool fInThread, volatile bool* fDrawStop);
		virtual ~Texture();

		void BindMe(); // To be called before glBegin
		void TexCoord2d(GLdouble x, GLdouble y); // To be called repeatedly between glBegin and glEnd // These are in world coordinates !!
		bool equals(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor);
		bool contains(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax);
		unsigned int getZoomFactor();
		bool fValid();

	private:
		void PutLine(const RealBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf);
		void PutLine(const LongBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf);
		void ConvLine(const RealBuf& buf, LongBuf& bufColor);
		void ConvLine(const LongBuf& buf, LongBuf& bufColor);
		void DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		void PutLineData(const RealBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf);
		void PutLineData(const LongBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf);
		void StretchLine(const RealBuf& buf, IntBuf& bufData);
		void StretchLine(const LongBuf& buf, IntBuf& bufData);
		void DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop);
		Map mp;
		GLuint texture;
		GLdouble xMin, yMin, xMax, yMax; // These are in world coordinates !! These are to be mapped to texture coordinates 0 to 1
		unsigned int zoomFactor;
		const DrawingColor * drawColor;
		ComplexDrawer::DrawMethod drm;
		unsigned int iPaletteSize;
		RangeReal rrMinMaxMap;
		bool fValue;
		bool fAttTable;
		const bool fUsePalette;
		bool valid;
	};
}

#endif // !defined(AFX_TEXTURE_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
