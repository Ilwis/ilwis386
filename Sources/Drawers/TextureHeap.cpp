// TextureHeap.cpp: implementation of the TextureHeap class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\MapWindow\Drawers\DrawerContext.h"
#include "TextureHeap.h"
#include "Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

TextureCreator::TextureCreator(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, char * scrap_data_mipmap, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, DrawerContext * drawerContext, unsigned int zoomFactor)
: mp(_mp)
, drawColor(drawColor)
, drm(drm)
, offsetX(offsetX)
, offsetY(offsetY)
, sizeX(sizeX)
, sizeY(sizeY)
, drawerContext(drawerContext)
, scrap_data_mipmap(scrap_data_mipmap)
, xMin(xMin)
, xMax(xMax)
, yMin(yMin)
, yMax(yMax)
, zoomFactor(zoomFactor)
{
}

TextureCreator::~TextureCreator()
{

}

Texture * TextureCreator::CreateTexture(volatile bool * fDrawStop)
{
	return new Texture(mp, drawColor, drm, offsetX, offsetY, sizeX, sizeY, scrap_data_mipmap, xMin, yMin, xMax, yMax, zoomFactor, drawerContext, fDrawStop);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextureHeap::TextureHeap(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, DrawerContext * drawerContext)
: texturesArraySize(0)
, readpos(0)
, writepos(0)
, mp(_mp)
, drawColor(drawColor)
, drm(drm)
, drawerContext(drawerContext)
, textureThread(0)
, fAbortTexGen(false)
, fStopThread(false)
{
	for (int i = 0; i < 10000; ++i)
		textures[i] = 0;
	scrap_data_mipmap = (char*)malloc(drawerContext->getMaxTextureSize() * drawerContext->getMaxTextureSize() * 4 * sizeof(char));
}

TextureHeap::~TextureHeap()
{
	if (textureThread)
	{
		fStopThread = true;
		textureThread->ResumeThread();
		while (fStopThread)
			Sleep(200);
	}

	ClearQueuedTextures();

	if (scrap_data_mipmap)
		free(scrap_data_mipmap);

	for (int i = 0; i < texturesArraySize; ++i)
		if (textures[i] != 0)
			delete textures[i];
}

void TextureHeap::ClearQueuedTextures()
{
	fAbortTexGen = true;
	csChangeTexCreatorList.Lock(); // wait for TexGen thread to stop
	fAbortTexGen = false;
	while (readpos != writepos)
	{
		delete textureCreators[readpos];
		readpos = (readpos + 1) % 1000;
	}
	csChangeTexCreatorList.Unlock();
}

Texture * TextureHeap::GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor)
{
	Texture * tex = 0;
	for (int i = 0; i < texturesArraySize; ++i)
	{
		if (textures[i]->equals(xMin, yMin, xMax, yMax, zoomFactor))
		{
			textures[i]->BindMe();
			return textures[i];
		}
		else if (textures[i]->contains(xMin, yMin, xMax, yMax))
		{
			if (tex != 0)
			{
				if (tex->getZoomFactor() > textures[i]->getZoomFactor())
					tex = textures[i];
			}
			else
				tex = textures[i];
		}
	}

	GenerateTexture(offsetX, offsetY, sizeX, sizeY, xMin, yMin, xMax, yMax, zoomFactor);

	if (tex != 0)
		tex->BindMe();

	return tex;
}

void TextureHeap::GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor)
{
	if (((writepos + 1) % 1000) != readpos)
	{
		textureCreators[writepos] = new TextureCreator(mp, drawColor, drm, offsetX, offsetY, sizeX, sizeY, scrap_data_mipmap, xMin, yMin, xMax, yMax, drawerContext, zoomFactor);
		writepos = (writepos + 1) % 1000;
	}
	if (!textureThread)
		textureThread = AfxBeginThread(GenerateTexturesInThread, this);
	else
		textureThread->ResumeThread();
}

UINT TextureHeap::GenerateTexturesInThread(LPVOID pParam)
{
	TextureHeap * pObject = (TextureHeap*)pParam;
	if (pObject == NULL)
	{
		pObject->textureThread = 0;
		return 1;
	}

	while (!pObject->fStopThread)
	{
		pObject->csChangeTexCreatorList.Lock();
		if (pObject->readpos != pObject->writepos)
		{
			while (!pObject->fAbortTexGen && !pObject->fStopThread && pObject->readpos != pObject->writepos)
			{
				clock_t start = clock();
				Texture * tex = pObject->textureCreators[pObject->readpos]->CreateTexture(&pObject->fAbortTexGen);
				clock_t end = clock();
				double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
				TRACE("Texture generated in %2.2f milliseconds;\n", duration);
				if (tex->fValid())
				{
					pObject->textures[pObject->texturesArraySize++] = tex;
					delete pObject->textureCreators[pObject->readpos];
					pObject->textureCreators[pObject->readpos] = 0;
					pObject->readpos = (pObject->readpos + 1) % 1000;
				} else
					delete tex;
			}

			if (!pObject->fAbortTexGen && !pObject->fStopThread)
				pObject->drawerContext->InvalidateWindow();
		}
		pObject->csChangeTexCreatorList.Unlock();
		if (!pObject->fStopThread)
			pObject->textureThread->SuspendThread(); // wait here, and dont consume CPU time either
	}

	pObject->fStopThread = false;

	return 0;
}