// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Texture.h"
#include "TextureHeap.h" // for Palette
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Texture::Texture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, const unsigned long imgWidth2, const unsigned long imgHeight2, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, unsigned int iPaletteSize, const RangeReal & rrMinMaxMap, const Palette * palette)
: mp(mp)
, texture_data(0)
, drawColor(drawColor)
, drm(drm)
, offsetX(offsetX)
, offsetY(offsetY)
, sizeX(sizeX)
, sizeY(sizeY)
, imgWidth2(imgWidth2)
, imgHeight2(imgHeight2)
, xMin(xMin)
, xMax(xMax)
, yMin(yMin)
, yMax(yMax)
, zoomFactor(zoomFactor)
, iPaletteSize(iPaletteSize)
, rrMinMaxMap(rrMinMaxMap)
, palette(palette)
, valid(false)
{
}

Texture::~Texture()
{
	if (valid)
		glDeleteTextures(1, &texture);
	if (texture_data)
		free(texture_data);
}

void Texture::CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	fValue = 0 != mp->dm()->pdvi() || 0 != mp->dm()->pdvr();
	fAttTable = false;
	if (palette) {
		texture_data = (char*)malloc((sizeX / zoomFactor) * (sizeY / zoomFactor) * 2);
		this->valid = DrawTexturePaletted(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
	} else {
		texture_data = (char*)malloc((sizeX / zoomFactor) * (sizeY / zoomFactor) * 4);
		this->valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
	}

	if (!valid)
		return;

	if (fInThread)
		drawerContext->TakeContext();
	glGenTextures(1, &texture);
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// The following are shared OpenGL variables .. if ever they need to change from the default of 0, restore them to the original value
	//glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
	//glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);
	boolean oldVal;
	glGetBooleanv(GL_MAP_COLOR, &oldVal);
	glPixelTransferf(GL_MAP_COLOR, palette != 0);
	if (palette != 0) {
		if (!drawerContext->isActivePalette(palette)) {
			palette->MakeCurrent();
			drawerContext->setActivePalette(palette);
		}
		glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_COLOR_INDEX, GL_UNSIGNED_SHORT, texture_data);
	}
	else {
		glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
		free(texture_data);
		texture_data = 0;
	}
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	fRepresentationChanged = false;
	if (fInThread)
		drawerContext->ReleaseContext();
}

bool Texture::fValid()
{
	return valid;
}

void Texture::BindMe(DrawerContext * drawerContext)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	double s = offsetX / (double)imgWidth2;
	double t = offsetY / (double)imgHeight2;
	glLoadIdentity();
	glScaled(imgWidth2 / (double)sizeX, imgHeight2 / (double)sizeY, 1);
	glTranslated(-s, -t, 0);
	if (palette != 0 && fRepresentationChanged) {
		boolean oldVal;
		glGetBooleanv(GL_MAP_COLOR, &oldVal);
		glPixelTransferf(GL_MAP_COLOR, true);
		if (!drawerContext->isActivePalette(palette)) {
			palette->MakeCurrent();
			drawerContext->setActivePalette(palette);
		}
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, sizeX / zoomFactor, sizeY / zoomFactor, GL_COLOR_INDEX, GL_UNSIGNED_SHORT, texture_data);
		glPixelTransferf(GL_MAP_COLOR, oldVal);
		fRepresentationChanged = false;
	}
}

void Texture::RepresentationChanged()
{
	fRepresentationChanged = true;
}

bool Texture::equals(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor)
{
	return this->xMin == xMin && this->yMin == yMin && this->xMax == xMax && this->yMax == yMax && this->zoomFactor == zoomFactor && this->palette == palette;
}

bool Texture::contains(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax)
{
	return this->xMin <= xMin && this->yMin >= yMin && this->xMax >= xMax && this->yMax <= yMax;
}

unsigned int Texture::getZoomFactor()
{
	return zoomFactor;
}

// RGB textures

void Texture::PutLine(const RealBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = bufColor.iSize();
	double * ptrBufOriginal = bufOriginal.buf();
	long * ptrBufColor = bufColor.buf();
	char *c = &outbuf[iLine * texSizeX * 4];
	memcpy(c, ptrBufColor, iLen * 4);
	c += 3; // point to the last byte of the Color struct
	for (long i = 0; i < iLen; ++i) {
		(rUNDEF == ptrBufOriginal[i])?*c&=0:*c|=255; // alpha = 0 or alpha = max
		c += 4;
	}
}

void Texture::PutLine(const LongBuf& bufOriginal, const LongBuf& bufColor, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = bufColor.iSize();
	long * ptrBufOriginal = bufOriginal.buf();
	long * ptrBufColor = bufColor.buf();
	char *c = &outbuf[iLine * texSizeX * 4];
	memcpy(c, ptrBufColor, iLen * 4);
	c += 3; // point to the last byte of the Color struct
	for (long i = 0; i < iLen; ++i) {
		(iUNDEF == ptrBufOriginal[i])?*c&=0:*c|=255; // alpha = 0 or alpha = max
		c += 4;
	}
}

void Texture::ConvLine(const RealBuf& buf, LongBuf& bufColor)
{
	drawColor->clrVal(buf.buf(), bufColor.buf(), buf.iSize());
}

void Texture::ConvLine(const LongBuf& buf, LongBuf& bufColor)
{
	drawColor->clrRaw(buf.buf(), bufColor.buf(), buf.iSize(), drm);
}

bool Texture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop)
{
	RowCol rcSize = mp->rcSize();
	long imageWidth = rcSize.Col;
	long imageHeight = rcSize.Row;
	long sizeX = texSizeX;
	long sizeY = texSizeY;
	if (offsetX + sizeX > imageWidth)
		sizeX = imageWidth - offsetX;
	if (offsetY + sizeY > imageHeight)
		sizeY = imageHeight - offsetY;
	if (sizeX == 0 || sizeY == 0)
		return false;

	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;

	if (*fDrawStop)
		return false;
	mp->KeepOpen(true);
	if (zoomFactor == 1) // 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	{
		if (fRealMap) 
		{
			RealBuf bufIn(sizeX);
			LongBuf bufColor(sizeX);
			memset(bufIn.buf(), 0, sizeX * 8); // to prevent NAN values in bufIn.
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				ConvLine(bufIn, bufColor);
				PutLine(bufIn, bufColor, iDataInYPos, texSizeX, outbuf);
			}
		}
		else 
		{ // !fRealMap
			LongBuf bufIn(sizeX);
			LongBuf bufColor(sizeX);
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos) 
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				ConvLine(bufIn, bufColor);
				PutLine(bufIn, bufColor, iDataInYPos, texSizeX, outbuf);
			}
		}                                                                         
	}
	else // zoomFactor > 1; expected in outbuf: sizeX * sizeY * 4 / (zoomFactor * zoomFactor) bytes (for RGB colors)
	{
		const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor));
		const long ySizeOut = (long)ceil((double)sizeY / ((double)zoomFactor));
		texSizeX /= zoomFactor;
		texSizeY /= zoomFactor;

		// If pyramid layers are available, they will handle part of the zoomFactor, and GetLineRaw/GetLineVal will read shorter lines
		// We only have to accomodate for the remaining zoomFactor
		int iPyrLayer = (int)(mp->fHasPyramidFile() ? max(0, log10((double)zoomFactor) / log10(2.0)) : 0);	
		if (iPyrLayer > mp->iNoOfPyramidLayers())
			iPyrLayer = mp->iNoOfPyramidLayers();
		long zoomByPyramid = (long)pow(2.0, iPyrLayer);
		if (iPyrLayer > 0) {
			offsetX /= zoomByPyramid;
			offsetY /= zoomByPyramid;
			sizeX = (long)ceil((double)sizeX / (double)zoomByPyramid);
			sizeY = (long)ceil((double)sizeY / (double)zoomByPyramid);
			zoomFactor /= zoomByPyramid;
		}
		
		// Note that offsetX, offsetY, texSizeX, texSizeY are ^2, so they're alvays perfectly divisible by pow(2)
		// For division of sizeX and sizeY we take the ceiling, otherwise the last line / column is missing in a non-perfect division.

		if (fRealMap) 
		{
			RealBuf bufIn(sizeX);
			double * ptrBufIn = bufIn.buf();
			RealBuf bufIntermediate(xSizeOut);
			double * ptrBufIntermediate = bufIntermediate.buf();
			LongBuf bufColor(xSizeOut);
			memset(ptrBufIn, 0, sizeX * 8); // to prevent NAN values in bufIn.

			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = ptrBufIn[iDataInXPos];
				ConvLine(bufIntermediate, bufColor);
				PutLine(bufIntermediate, bufColor, iDataOutYPos, texSizeX, outbuf);
			}
		}
		else // !fRealMap
		{
			LongBuf bufIn(sizeX);
			long * ptrBufIn = bufIn.buf();
			LongBuf bufIntermediate(xSizeOut);
			long * ptrBufIntermediate = bufIntermediate.buf();
			LongBuf bufColor(xSizeOut);
			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = ptrBufIn[iDataInXPos];
				ConvLine(bufIntermediate, bufColor);
				PutLine(bufIntermediate, bufColor, iDataOutYPos, texSizeX, outbuf);
			}
		}                 
	}
	mp->KeepOpen(false);
	return true;
}

// Paletted textures, thus less colors, but fast palete-swap option
// An OpenGL palette GL_PIXEL_MAP_I_TO_R GL_PIXEL_MAP_I_TO_G GL_PIXEL_MAP_I_TO_B GL_PIXEL_MAP_I_TO_A must be defined prior to drawing this texture

void Texture::PutLineData(const RealBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = bufData.iSize();
	double * ptrBufOriginal = bufOriginal.buf();
	short * ptrBufData = bufData.buf();
	char *c = &outbuf[iLine * texSizeX * 2];
	memcpy(c, ptrBufData, iLen * 2);
	for (long i = 0; i < iLen; ++i) {
		if (rUNDEF == ptrBufOriginal[i])
			((short *)c)[i] = iPaletteSize - 1;  // at index iPaletteSize - 1 there is a color with alpha = 0
	}
}

void Texture::PutLineData(const LongBuf& bufOriginal, const IntBuf& bufData, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = bufData.iSize();
	long * ptrBufOriginal = bufOriginal.buf();
	short * ptrBufData = bufData.buf();
	char *c = &outbuf[iLine * texSizeX * 2];
	memcpy(c, ptrBufData, iLen * 2);
	for (long i = 0; i < iLen; ++i) {
		if (iUNDEF == ptrBufOriginal[i])
			((short *)c)[i] = iPaletteSize - 1; // at index iPaletteSize - 1 there is a color with alpha = 0

	}
}

void Texture::StretchLine(const RealBuf& buf, IntBuf& bufData)
{
	int iLen = buf.iSize();
	double * ptrBuf = buf.buf();
	short * ptrBufData = bufData.buf();
	double width = rrMinMaxMap.rWidth();
	double minMapVal = rrMinMaxMap.rLo();
	int nrMapValues = iPaletteSize - 1;
	for (int i = 0; i < iLen; ++i)
		ptrBufData[i] = (ptrBuf[i] - minMapVal) * (nrMapValues - 1) / width; // reserve last index for UNDEF
}

void Texture::StretchLine(const LongBuf& buf, IntBuf& bufData)
{
	int iLen = buf.iSize();
	long * ptrBuf = buf.buf();
	short * ptrBufData = bufData.buf();
	double width = rrMinMaxMap.rWidth();
	double minMapVal = rrMinMaxMap.rLo();
	int nrMapValues = iPaletteSize - 1;
	for (int i = 0; i < iLen; ++i)
		ptrBufData[i] = (ptrBuf[i] - minMapVal) * (nrMapValues - 1) / width; // reserve last index for UNDEF
}

bool Texture::DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop)
{
	RowCol rcSize = mp->rcSize();
	long imageWidth = rcSize.Col;
	long imageHeight = rcSize.Row;
	long sizeX = texSizeX;
	long sizeY = texSizeY;
	if (offsetX + sizeX > imageWidth)
		sizeX = imageWidth - offsetX;
	if (offsetY + sizeY > imageHeight)
		sizeY = imageHeight - offsetY;
	if (sizeX == 0 || sizeY == 0)
		return false;

	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;

	if (*fDrawStop)
		return false;
	mp->KeepOpen(true);
	if (zoomFactor == 1) // 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	{
		if (fRealMap) 
		{
			RealBuf bufIn(sizeX);
			IntBuf bufData(sizeX);
			memset(bufIn.buf(), 0, sizeX * 8); // to prevent NAN values in bufIn.
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				StretchLine(bufIn, bufData);
				PutLineData(bufIn, bufData, iDataInYPos, texSizeX, outbuf);
			}
		}
		else 
		{ // !fRealMap
			LongBuf bufIn(sizeX);
			IntBuf bufData(sizeX);
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos) 
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				StretchLine(bufIn, bufData);
				PutLineData(bufIn, bufData, iDataInYPos, texSizeX, outbuf);
			}
		}                                                                         
	}
	else // zoomFactor > 1; expected in outbuf: sizeX * sizeY * 4 / (zoomFactor * zoomFactor) bytes (for RGB colors)
	{
		const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor));
		const long ySizeOut = (long)ceil((double)sizeY / ((double)zoomFactor));
		texSizeX /= zoomFactor;
		texSizeY /= zoomFactor;

		// If pyramid layers are available, they will handle part of the zoomFactor, and GetLineRaw/GetLineVal will read shorter lines
		// We only have to accomodate for the remaining zoomFactor
		int iPyrLayer = (int)(mp->fHasPyramidFile() ? max(0, log10((double)zoomFactor) / log10(2.0)) : 0);	
		if (iPyrLayer > mp->iNoOfPyramidLayers())
			iPyrLayer = mp->iNoOfPyramidLayers();
		long zoomByPyramid = (long)pow(2.0, iPyrLayer);
		if (iPyrLayer > 0) {
			offsetX /= zoomByPyramid;
			offsetY /= zoomByPyramid;
			sizeX = (long)ceil((double)sizeX / (double)zoomByPyramid);
			sizeY = (long)ceil((double)sizeY / (double)zoomByPyramid);
			zoomFactor /= zoomByPyramid;
		}
		
		// Note that offsetX, offsetY, texSizeX, texSizeY are ^2, so they're alvays perfectly divisible by pow(2)
		// For division of sizeX and sizeY we take the ceiling, otherwise the last line / column is missing in a non-perfect division.

		if (fRealMap) 
		{
			RealBuf bufIn(sizeX);
			double * ptrBufIn = bufIn.buf();
			RealBuf bufIntermediate(xSizeOut);
			double * ptrBufIntermediate = bufIntermediate.buf();
			IntBuf bufData(xSizeOut);
			memset(ptrBufIn, 0, sizeX * 8); // to prevent NAN values in bufIn.

			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = ptrBufIn[iDataInXPos];
				StretchLine(bufIntermediate, bufData);
				PutLineData(bufIntermediate, bufData, iDataOutYPos, texSizeX, outbuf);
			}
		}
		else // !fRealMap
		{
			LongBuf bufIn(sizeX);
			long * ptrBufIn = bufIn.buf();
			LongBuf bufIntermediate(xSizeOut);
			long * ptrBufIntermediate = bufIntermediate.buf();
			IntBuf bufData(xSizeOut);
			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					mp->KeepOpen(false);
					return false;
				}
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = ptrBufIn[iDataInXPos];
				StretchLine(bufIntermediate, bufData);
				PutLineData(bufIntermediate, bufData, iDataOutYPos, texSizeX, outbuf);
			}
		}                 
	}
	mp->KeepOpen(false);
	return true;
}
