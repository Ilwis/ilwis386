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
, textureSize(0)
{
}

#define GL_CLAMP_TO_EDGE 0x812F

void WMSTexture::CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	textureSize = drawerContext->getMaxTextureSize();
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
		if (drawerContext->getVersion() >= 1.2) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
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
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	fRepresentationChanged = false;
	if (fInThread)
		drawerContext->ReleaseContext();
}

void WMSTexture::ReCreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
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

void WMSTexture::PutLine(const LongBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = bufColor.iSize();
	long * ptrBufOriginal = bufOriginal.buf();
	long * ptrBufColor = bufColor.buf();
	char *c = &outbuf[iLine * texSizeX * 4];
	memcpy(c, ptrBufColor, iLen * 4);
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
	bool valid = grcWMS->retrieveImage(mp->fnObj, cbBounds, RowCol(textureSize, textureSize));
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

const long WMSTexture::iRaw(Coord & crd) const {
	if (!valid)
		return iUNDEF;
	long iRow = round((double)textureSize * (cbBounds.cMax.y - crd.y) / cbBounds.height());
	long iCol = round((double)textureSize * (crd.x - cbBounds.cMin.x) / cbBounds.width());
	if (iRow >= 0 && iRow < textureSize && iCol >= 0 && iCol < textureSize) {
		long iPos = (iRow * textureSize + iCol); // *4;
		//long iRet = (texture_data[iPos]) | (texture_data[iPos + 1] << 8) | (texture_data[iPos + 2] << 16);
		long iRet = ((long*)texture_data)[iPos];
		return iRet;
	} else
		return iUNDEF;
}