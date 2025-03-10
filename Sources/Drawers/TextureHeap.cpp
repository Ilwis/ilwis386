// TextureHeap.cpp: implementation of the TextureHeap class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "TextureHeap.h"
#include "Texture.h"
#include "CCTexture.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawingColor.h"
#include "LayerDrawer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

//////////////////////////////////////////////////
// Palette
//////////////////////////////////////////////////

Palette::Palette()
: rsd(0)
, iPaletteSize(0)
, palette_reds(0)
, palette_greens(0)
, palette_blues(0)
, palette_alphas(0)
, rMaxAlpha(0)
, fHasTransparent(false)
, fUseValues(true)
{
}

Palette::~Palette()
{
	if (palette_reds)
		delete [] palette_reds;
	if (palette_greens)
		delete [] palette_greens;
	if (palette_blues)
		delete [] palette_blues;
	if (palette_alphas)
		delete [] palette_alphas;
}

void Palette::SetData(const BaseMap & mp, const LayerDrawer * rsd, const unsigned int iPaletteSize, const RangeReal & rrMinMaxMap)
{
	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	bool fValue = 0 != mp->dm()->pdvi() || 0 != mp->dm()->pdvr();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;
	fUseValues = fRealMap || fValue; // This may not be "final" and could be simplified. The goal is to have the logic identical to the one where in Texture::DrawTexture() the values are read by GetLineVal() or GetLineRaw(); if there "val" is read, here "val" must be written; same for "raw".
	// in summary, double/float/int go through "val", and image/class/id/color/bool go through "raw".
	
	this->rsd = rsd;
	this->rrMinMaxMap.rLo() = rrMinMaxMap.rLo();
	this->rrMinMaxMap.rHi() = rrMinMaxMap.rHi();

	if (this->iPaletteSize != iPaletteSize && iPaletteSize > 2) {
		this->iPaletteSize = iPaletteSize;
		if (palette_reds)
			delete [] palette_reds;
		if (palette_greens)
			delete [] palette_greens;
		if (palette_blues)
			delete [] palette_blues;
		if (palette_alphas)
			delete [] palette_alphas;
		palette_reds = new float [iPaletteSize];
		palette_greens = new float [iPaletteSize];
		palette_blues = new float [iPaletteSize];
		palette_alphas = new float [iPaletteSize];
		palette_alphas[iPaletteSize - 1] = 0.0; // by definition, last index reserved for UNDEF
	}
}

const bool Palette::fValid() const
{
	return iPaletteSize > 2; // a vaild palette has at least colors for mapMin, mapMax and UNDEF.
}

void Palette::MakeCurrent() const
{
	glPixelMapfv(GL_PIXEL_MAP_I_TO_R, iPaletteSize, palette_reds);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_G, iPaletteSize, palette_greens);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_B, iPaletteSize, palette_blues);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_A, iPaletteSize, palette_alphas);
}

// Call Refresh() when: 1. Map data has changed, 2. Representation has changed
void Palette::Refresh()
{
	if (fValid()) {
		unsigned int nrMapValues = iPaletteSize - 1;
		double width = rrMinMaxMap.rWidth();
		double minMapVal = rrMinMaxMap.rLo();

		const DrawingColor * drawColor = rsd->getDrawingColor();
		long * bufColor = new long [nrMapValues];

		if (fUseValues) {
			double * buf = new double [nrMapValues];
			for (int i = 0; i < nrMapValues; ++i)
				buf[i] = minMapVal + i * width / (nrMapValues - 1);
			drawColor->clrVal(buf, bufColor, nrMapValues);
			delete [] buf;
		} else {
			const NewDrawer::DrawMethod drm = rsd->getDrawMethod();
			long * buf = new long [nrMapValues];
			for (int i = 0; i < nrMapValues; ++i)
				buf[i] = minMapVal + round(i * width / (nrMapValues - 1));
			drawColor->clrRaw(buf, bufColor, nrMapValues, drm);
			delete [] buf;
		}

		rMaxAlpha = 0;
		fHasTransparent = false;

		for (int i = 0; i < nrMapValues; ++i) {
			Color col(bufColor[i]);
			palette_reds[i] = col.redP();
			palette_greens[i] = col.greenP();
			palette_blues[i] = col.blueP();
			float alpha = col.alphaP();
			palette_alphas[i] = alpha;
			rMaxAlpha = max(rMaxAlpha, alpha);
			fHasTransparent = fHasTransparent || (alpha > 0.0 && alpha < 1.0);
		}

		delete [] bufColor;
	} else {
		rMaxAlpha = 0;
		fHasTransparent = false;
	}
}

const float Palette::rGetMaxAlpha() const {
	return rMaxAlpha;
}

const bool Palette::fHasTransparentValues() const {
	return fHasTransparent;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextureHeap::TextureHeap()
: textureThread(0)
, fAbortTexGen(false)
, fStopThread(false)
, workingTexture(0)
{
}

TextureHeap::~TextureHeap()
{
	if (textureThread)
	{
		fStopThread = true;
		textureThread->ResumeThread();
		csThread.Lock(); // wait here til thread exits
		csThread.Unlock();
	}

	ClearQueuedTextures();

	for (int i = 0; i < textures.size(); ++i)
		if (textures[i] != 0)
			delete textures[i];
}

void TextureHeap::SetData(const Map & _mp, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int iPaletteSize, RasterSetData *data, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext)
{
	this->mp.SetPointer(_mp.pointer());
	this->drawColor = drawColor;
	this->drm = drm;
	this->iPaletteSize = iPaletteSize;
	this->imgWidth2 = data->width;
	this->imgHeight2 = data->height;
	this->rrMinMaxMap.rLo() = rrMinMaxMap.rLo();
	this->rrMinMaxMap.rHi() = rrMinMaxMap.rHi();
	this->drawerContext = drawerContext;
}

void TextureHeap::SetData(const MapList & _mpl, const DrawingColor * drawColor, const NewDrawer::DrawMethod drm, const unsigned int iPaletteSize, RasterSetData *_data, const RangeReal & rrMinMaxMap, DrawerContext * drawerContext)
{
	this->mpl.SetPointer(_mpl.pointer());
	this->drawColor = drawColor;
	this->drm = drm;
	this->iPaletteSize = iPaletteSize;
	this->imgWidth2 = _data->width;
	this->imgHeight2 = _data->height;
	this->rrMinMaxMap.rLo() = rrMinMaxMap.rLo();
	this->rrMinMaxMap.rHi() = rrMinMaxMap.rHi();
	this->drawerContext = drawerContext;
	data = _data;
}

bool TextureHeap::fValid()
{
	return drawerContext != 0;
}

void TextureHeap::ClearQueuedTextures()
{
	fAbortTexGen = true;
	csChangeTexCreatorList.Lock(); // wait for TexGen thread to stop
	fAbortTexGen = false;
	for (int i = 0; i < textureRequest.size(); ++i) {
		Texture * tex = textureRequest[i];
			if (tex && !tex->fValid())
				delete tex;
	}
	textureRequest.clear();
	csChangeTexCreatorList.Unlock();
}

void TextureHeap::RepresentationChanged()
{
	for (int i = 0; i < textures.size(); ++i)
		textures[i]->RepresentationChanged();	
}

void TextureHeap::ReGenerateAllTextures()
{
	for (int i = 0; i < textures.size(); ++i)
		textures[i]->SetDirty();
}

void TextureHeap::setTransparentValues(double v)
{
	for (int i = 0; i < textures.size(); ++i)
		textures[i]->setTransparentValue(v);
}


Texture * TextureHeap::GetTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, const Palette * palette, bool fInThread)
{
	Texture * tex = 0;
	if (fInThread) { // call Invalidate when done, to redraw the mapwindow
		for (int i = 0; i < textures.size(); ++i) {
			if (textures[i]->equals(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor)) {
				if (textures[i]->fDirty())
					ReGenerateTexture(textures[i], fInThread);
				textures[i]->BindMe(drawerContext);
				return textures[i];
			} else if (textures[i]->contains(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY)) {
				if (tex != 0) {
					if (tex->getZoomFactor() > textures[i]->getZoomFactor())
						tex = textures[i];
				} else
					tex = textures[i];
			}
		}
		// if it is queued already, don't add it again, just be patient as it will come
		csChangeTexCreatorList.Lock();
		bool fQueued = workingTexture && workingTexture->equals(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
		if (!fQueued) {
			for (vector<Texture*>::iterator it = textureRequest.begin(); it != textureRequest.end() && !fQueued; ++it)
				fQueued = (*it)->equals(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor);
		}
		csChangeTexCreatorList.Unlock();
		if (!fQueued)
			GenerateTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, palette, fInThread);
	} else { // caller is waiting for the Texture*
		for (int i = 0; i < textures.size(); ++i) {
			if (textures[i]->equals(offsetX, offsetY, offsetX + sizeX, offsetY + sizeY, zoomFactor))
				tex = textures[i];
		}
		if (0 == tex)
			tex = GenerateTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, palette, fInThread);
		else if (tex->fDirty())
			ReGenerateTexture(tex, fInThread);
	}

	if (tex != 0)
		tex->BindMe(drawerContext);

	return tex;
}

Texture * TextureHeap::GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, const Palette * palette, bool fInThread)
{
	csChangeTexCreatorList.Lock();
	if ( mp.fValid())
		textureRequest.push_back(new Texture(mp, drawColor, drm, offsetX, offsetY, sizeX, sizeY, imgWidth2, imgHeight2, zoomFactor, iPaletteSize, rrMinMaxMap, palette));
	else
		textureRequest.push_back(new CCTexture(mpl, drawColor, drm, offsetX, offsetY, sizeX, sizeY, data, zoomFactor, rrMinMaxMap));
	csChangeTexCreatorList.Unlock();
	if (fInThread) {
		if (!textureThread)
			textureThread = AfxBeginThread(GenerateTexturesInThread, this);
		else
			textureThread->ResumeThread();
	} else {
		Texture * tex = GenerateNextTexture(fInThread);
		if (tex != 0)
			return tex;
	}
	return 0;
}

void TextureHeap::ReGenerateTexture(Texture * texture, bool fInThread)
{
	csChangeTexCreatorList.Lock();
	textureRequest.push_back(texture);
	csChangeTexCreatorList.Unlock();
	if (fInThread) {
		if (!textureThread)
			textureThread = AfxBeginThread(GenerateTexturesInThread, this);
		else
			textureThread->ResumeThread();
	} else
		GenerateNextTexture(fInThread);
}

Texture * TextureHeap::GenerateNextTexture(bool fInThread)
{
	Texture * tex = 0;
	csChangeTexCreatorList.Lock();
	if (!fAbortTexGen && !fStopThread && textureRequest.size() > 0) {
		tex = textureRequest.back();
		textureRequest.pop_back();
		workingTexture = tex;
	}
	csChangeTexCreatorList.Unlock();

	if (tex != 0) {
		try {
			bool fReGenerate = tex->fValid();
			clock_t start = clock();
			if (fReGenerate)
				tex->ReCreateTexture(drawerContext, fInThread, &fAbortTexGen);
			else
				tex->CreateTexture(drawerContext, fInThread, &fAbortTexGen);
			clock_t end = clock();
			double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
			//TRACE("Texture generated in %2.2f milliseconds;\n", duration);
			if (!fReGenerate) {
				if (tex->fValid())
					textures.push_back(tex);
				else {
					delete tex;
					tex = 0;
				}
			}
			csChangeTexCreatorList.Lock();
			workingTexture = 0;
			csChangeTexCreatorList.Unlock();
		} catch (ErrorObject& err) {
			csChangeTexCreatorList.Lock();
			workingTexture = 0;
			delete tex;
			tex = 0;
			csChangeTexCreatorList.Unlock();
			throw err;
		}
	}
	return tex;
}

UINT TextureHeap::GenerateTexturesInThread(LPVOID pParam)
{
	TextureHeap * pObject = (TextureHeap*)pParam;
	if (pObject == NULL)
	{
		return 1;
	}

	bool fErrorShown = false;

	pObject->csThread.Lock();
	while (!pObject->fStopThread)
	{
		try {
			clock_t start = clock();
			Texture * tex = pObject->GenerateNextTexture(true);
			while (tex != 0) {
				clock_t end = clock();
				long duration = (end - start) * 1000 / CLOCKS_PER_SEC; // we want this in millisec
				if (duration >= 1000) { // approximately 1 sec between intermediate screen updates
					pObject->drawerContext->doDraw();
					start = end;
				}
				tex = pObject->GenerateNextTexture(true);
			}
			if (!pObject->fAbortTexGen && !pObject->fStopThread)
				pObject->drawerContext->doDraw();
			if (!pObject->fStopThread)
				pObject->textureThread->SuspendThread(); // wait here, and dont consume CPU time either
		} catch (ErrorObject& err) {
			if (!fErrorShown) {
				fErrorShown = true;
				err.Show();
			}
			pObject->ClearQueuedTextures();
			pObject->fAbortTexGen = true;
		}
	}

	pObject->fStopThread = false;
	pObject->csThread.Unlock();

	return 0;


}

