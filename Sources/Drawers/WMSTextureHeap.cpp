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
#include "WMSTextureHeap.h"
#include "WMSTexture.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawingColor.h"
#include "LayerDrawer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

void WMSTextureHeap::SetCBImage(const CoordBounds & cb)
{
	cbImage = cb;
}

inline bool contains(const CoordBounds & cb1, const CoordBounds & cb2) {
	return cb2.cMin.x >= cb1.cMin.x && cb2.cMax.x <= cb1.cMax.x &&
		cb2.cMin.y >= cb1.cMin.y && cb2.cMax.y <= cb1.cMax.y;
}

Texture * WMSTextureHeap::GetTexture(const CoordBounds & cb, bool fInThread)
{
	WMSTexture * tex = 0;
	if (fInThread) { // call Invalidate when done, to redraw the mapwindow
		for (int i = 0; i < textures.size(); ++i) {
			if (((WMSTexture*)textures[i])->cb() == cb) {
				if (textures[i]->fDirty())
					ReGenerateTexture(textures[i], fInThread);
				textures[i]->BindMe(drawerContext);
				return textures[i];
			} else if (contains(((WMSTexture*)textures[i])->cb(), cb)) {
				if (tex != 0) {
					if (contains(tex->cb(), ((WMSTexture*)textures[i])->cb()))
						tex = (WMSTexture*)textures[i];
				} else
					tex = (WMSTexture*)textures[i];
			}
		}
		// if it is queued already, don't add it again, just be patient as it will come
		csChangeTexCreatorList.Lock();
		bool fQueued = workingTexture && (((WMSTexture*)workingTexture)->cb() == cb);
		if (!fQueued) {
			for (vector<Texture*>::iterator it = textureRequest.begin(); it != textureRequest.end() && !fQueued; ++it)
				fQueued = ((WMSTexture*)(*it))->cb() == cb;
		}
		csChangeTexCreatorList.Unlock();
		if (!fQueued)
			GenerateTexture(cb, fInThread);
	} else { // caller is waiting for the Texture*
		for (int i = 0; i < textures.size(); ++i) {
			if (((WMSTexture*)textures[i])->cb() == cb)
				tex = (WMSTexture*)textures[i];
		}
		if (0 == tex)
			tex = (WMSTexture*)GenerateTexture(cb, fInThread);
		else if (tex->fDirty())
			ReGenerateTexture(tex, fInThread);
	}

	if (tex != 0)
		tex->BindMe(drawerContext);

	return tex;
}

Texture * WMSTextureHeap::GenerateTexture(const CoordBounds & cb, bool fInThread)
{
	csChangeTexCreatorList.Lock();
	Texture * newTexture = new WMSTexture(mp, drawColor, drm, cb, cbImage);
	textureRequest.push_back(newTexture);
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

Texture * WMSTextureHeap::GetTexture(const Coord & crd)
{
	WMSTexture * tex = 0;
	CoordBounds cb (crd, crd); // a point
	for (int i = 0; i < textures.size(); ++i) {
		if (contains(((WMSTexture*)textures[i])->cb(), cb)) {
			if (tex != 0) {
				if (contains(tex->cb(), ((WMSTexture*)textures[i])->cb())) // a better texture
					tex = (WMSTexture*)textures[i];
			} else
				tex = (WMSTexture*)textures[i];
		}
	}
	return tex;
}