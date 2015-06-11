// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "WMSTexture.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GrcWMS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WMSTexture::WMSTexture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const CoordBounds & cb, const CoordBounds & _cbImageBounds)
: Texture(mp, drawColor, drm, 0 ,0 , 0, 0, 0, 0, 0, 0, RangeReal(), 0)
, cbBounds(cb)
, cbImageBounds(_cbImageBounds)
{
}

void WMSTexture::CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	int textureSize = drawerContext->getMaxTextureSize();
	bool fOpenStreetMap = mp->gr()->pgOSM();
	if (fOpenStreetMap)
		textureSize = 256;
	try {
		texture_data = new char [textureSize * textureSize * 4];
	} catch (CMemoryException * err) {
		char msg[512];
		err->GetErrorMessage(msg, sizeof(msg));
		err->Delete();
		throw ErrorObject(String("%s", msg));
	}
	this->valid = DrawTexture(textureSize, texture_data, fDrawStop);

	if (!valid)
		return;

	if (fInThread)
		drawerContext->TakeContext();
	glGenTextures(1, &texture);
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	if (fOpenStreetMap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// The following are shared OpenGL variables .. if ever they need to change from the default of 0, restore them to the original value
	//glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
	//glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);
	GLboolean oldVal;
	glGetBooleanv(GL_MAP_COLOR, &oldVal);
	glPixelTransferf(GL_MAP_COLOR, false);
	glTexImage2D( GL_TEXTURE_2D, 0, 4, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	delete [] texture_data;
	texture_data = 0;
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	fRepresentationChanged = false;
	if (fInThread)
		drawerContext->ReleaseContext();
}

void WMSTexture::ReCreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	int textureSize = drawerContext->getMaxTextureSize();
	texture_data = new char [textureSize * textureSize * 4];
	this->dirty = !DrawTexture(textureSize, texture_data, fDrawStop);

	if (dirty)
		return;

	if (fInThread)
		drawerContext->TakeContext();
	glBindTexture( GL_TEXTURE_2D, texture );
	GLboolean oldVal;
	glGetBooleanv(GL_MAP_COLOR, &oldVal);
	glPixelTransferf(GL_MAP_COLOR, false);
	glTexImage2D( GL_TEXTURE_2D, 0, 4, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	delete [] texture_data;
	texture_data = 0;
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	fRepresentationChanged = false;
	if (fInThread)
		drawerContext->ReleaseContext();
}

void WMSTexture::BindMe(DrawerContext * drawerContext)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	double s = (cbBounds.cMin.x - cbImageBounds.cMin.x) / cbImageBounds.width();
	double t = (cbImageBounds.cMax.y - cbBounds.cMax.y) / cbImageBounds.height();
	glLoadIdentity();
	glScaled(cbImageBounds.width() / cbBounds.width(), cbImageBounds.height() / cbBounds.height(), 1);
	glTranslated(-s, -t, 0);
}

bool WMSTexture::DrawTexture(int textureSize, char * outbuf, volatile bool* fDrawStop)
{
	if (*fDrawStop)
		return false;
	mp->KeepOpen(true);
	// 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	// set georef->rcSize to texureSize x textureSize and georef->cb to cb
	GeoRefCornersWMS *grcWMS = mp->gr()->pgWMS();
	grcWMS->Lock();
	grcWMS->SetCBWMSRequest(cbBounds);
	grcWMS->SetRCWMSRequest(RowCol(textureSize, textureSize));
	bool valid = grcWMS->retrieveImage();
	if( valid ) {
		LongBuf bufIn(textureSize);
		LongBuf bufColor(textureSize);
		for (long iDataInYPos = 0; iDataInYPos < textureSize; ++iDataInYPos) 
		{
			//if (*fDrawStop) {
			//	(*mp)->KeepOpen(false);
			//	return false;
			//}
			mp->GetLineRaw(iDataInYPos, bufIn, 0, textureSize);
			ConvLine(bufIn, bufColor);
			PutLine(bufIn, bufColor, iDataInYPos, textureSize, outbuf);
		}
	}
	
	grcWMS->Unlock();
	mp->KeepOpen(false);
	return valid;
}
