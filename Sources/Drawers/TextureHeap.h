// TextureHeap.h: interface for the TextureHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
#define AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Engine\Drawers\Drawer_n.h" // for DrawMethod

#include <GL/gl.h>

namespace ILWIS {

	class Texture;
	class DrawingColor;
	class DrawerContext;
	class SetDrawer;

	class Palette
	{
	public:
		Palette();
		virtual ~Palette();
		void SetData(const Map & mp, const SetDrawer * rsd, const unsigned int iPaletteSize, const RangeReal & rrMinMaxMap);
		bool fValid();
		void MakeCurrent();
		void SetNotCurrent();
		void Refresh();
	private:
		float * palette_reds;
		float * palette_greens;
		float * palette_blues;
		float * palette_alphas;
		bool fCurrent;
		bool fRealMap;
		unsigned int iPaletteSize;
		RangeReal rrMinMaxMap;
		const SetDrawer * rsd;
	};

	class TextureHeap  
	{
	public:
		TextureHeap();
		virtual ~TextureHeap();

		void SetData(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int iPaletteSize, const unsigned long imgWidth2, const unsigned long imgHeight2, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext);
		bool fValid();
		Texture * GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, const bool fUsePalette, bool fInThread);
		void ClearQueuedTextures();
		void PaletteChanged();

	private:
		Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, const bool fUsePalette, bool fInThread);
		static UINT GenerateTexturesInThread(LPVOID pParam);
		Texture * GenerateNextTexture(bool fInThread);
		//vector<Texture*> textures;
		Texture* textures [10000];
		static const int BUF_SIZE = 100;
		Texture * textureRequest [BUF_SIZE];
		Map mp;
		const DrawingColor * drawColor;
		NewDrawer::DrawMethod drm;
		unsigned int iPaletteSize;
		unsigned long imgWidth2, imgHeight2;
		RangeReal rrMinMaxMap;
		int texturesArraySize;
		int readpos;
		int writepos;
		bool fAbortTexGen;
		CWinThread * textureThread;
		bool fStopThread;
		DrawerContext * drawerContext;
		CCriticalSection csChangeTexCreatorList;
		CCriticalSection csThread;
	};
}

#endif // !defined(AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
