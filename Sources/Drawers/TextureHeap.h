// TextureHeap.h: interface for the TextureHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
#define AFX_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client\MapWindow\Drawers\Drawer_n.h" // otherwise DrawingColor can't compile
#include "Drawers\DrawingColor.h"

#include <GL/gl.h>

namespace ILWIS {

	class Texture;

	class TextureCreator
	{
	public:
		TextureCreator(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, char * scrap_data_mipmap, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, DrawerContext * drawerContext, unsigned int zoomFactor);
		virtual ~TextureCreator();
		Texture * CreateTexture(volatile bool * fDrawStop);
	private:
		const Map mp;
		const DrawingColor * drawColor;
		const NewDrawer::DrawMethod drm;
		const unsigned int offsetX;
		const unsigned int offsetY;
		const unsigned int sizeX;
		const unsigned int sizeY;
		DrawerContext * drawerContext;
		char * scrap_data_mipmap;
		GLdouble xMin;
		GLdouble yMin;
		GLdouble xMax;
		GLdouble yMax;
		unsigned int zoomFactor;
	};

	class TextureHeap  
	{
	public:
		TextureHeap(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, DrawerContext * drawerContext);
		virtual ~TextureHeap();

		Texture * GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor);
		void ClearQueuedTextures();

	private:
		void GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor);
		static UINT GenerateTexturesInThread(LPVOID pParam);
		//vector<Texture*> textures;
		Texture* textures [10000];
		TextureCreator * textureCreators [1000];
		const Map mp;
		const DrawingColor * drawColor;
		const NewDrawer::DrawMethod drm;
		char * scrap_data_mipmap;
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
