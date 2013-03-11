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
	class LayerDrawer;
	class RasterSetData;

	class Palette
	{
	public:
		Palette();
		virtual ~Palette();
		void SetData(const BaseMap & mp, const LayerDrawer * rsd, const unsigned int iPaletteSize, const RangeReal & rrMinMaxMap);
		const bool fValid() const;
		void MakeCurrent() const;
		void Refresh();
		const float rGetMaxAlpha() const;
		const bool fHasTransparentValues() const;
	private:
		float * palette_reds;
		float * palette_greens;
		float * palette_blues;
		float * palette_alphas;
		bool fRealMap;
		unsigned int iPaletteSize;
		RangeReal rrMinMaxMap;
		const LayerDrawer * rsd;
		float rMaxAlpha;
		bool fHasTransparent;
	};

	class TextureHeap  
	{
	public:
		TextureHeap();
		virtual ~TextureHeap();

		void SetData(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int iPaletteSize, ILWIS::RasterSetData * data, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext);
		void SetData(const MapList & _mpl, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int iPaletteSize, ILWIS::RasterSetData * data, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext);
		bool fValid();
		Texture * GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, const Palette * palette, bool fInThread);
		void ClearQueuedTextures();
		void RepresentationChanged();
		void ReGenerateAllTextures();
		void setTransparentValues(double v);

	protected:
		Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, const Palette * palette, bool fInThread);
		void ReGenerateTexture(Texture * texture, bool fInThread);
		static UINT GenerateTexturesInThread(LPVOID pParam);
		Texture * GenerateNextTexture(bool fInThread);
		vector<Texture*> textures;
		vector<Texture*> textureRequest;
		Map mp;
		MapList mpl;
		const DrawingColor * drawColor;
		NewDrawer::DrawMethod drm;
		unsigned int iPaletteSize;
		unsigned long imgWidth2, imgHeight2;
		RangeReal rrMinMaxMap;
		bool fAbortTexGen;
		CWinThread * textureThread;
		bool fStopThread;
		DrawerContext * drawerContext;
		CCriticalSection csChangeTexCreatorList;
		CCriticalSection csThread;
		RasterSetData *data;
		Texture * workingTexture;
	};
}

#endif // !defined(AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
