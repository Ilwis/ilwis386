// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\MapWindow\Drawers\DrawerContext.h"
#include "Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Texture::Texture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const long sizeX, const long sizeY, char * scrap_data_mipmap, GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor, DrawerContext * drawerContext, bool fInThread, volatile bool* fDrawStop)
: mp(mp)
, drawColor(drawColor)
, drm(drm)
, xMin(xMin)
, xMax(xMax)
, yMin(yMin)
, yMax(yMax)
, zoomFactor(zoomFactor)
, valid(false)
{
	fValue = 0 != mp->dm()->pdvi() || 0 != mp->dm()->pdvr();
	fAttTable = false;
	DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, scrap_data_mipmap, fDrawStop);
	if (*fDrawStop)
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

	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0);
	glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, scrap_data_mipmap);
	if (fInThread)
		drawerContext->ReleaseContext();
	this->valid = true;
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture);
}

bool Texture::fValid()
{
	return valid;
}

void Texture::BindMe()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::TexCoord2d(GLdouble x, GLdouble y)
{
	glTexCoord2d((x - xMin) / (xMax - xMin), (y - yMin) / (yMax - yMin));
}

bool Texture::equals(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax, unsigned int zoomFactor)
{
	return this->xMin == xMin && this->yMin == yMin && this->xMax == xMax && this->yMax == yMax && this->zoomFactor == zoomFactor;
}

bool Texture::contains(GLdouble xMin, GLdouble yMin, GLdouble xMax, GLdouble yMax)
{
	return this->xMin <= xMin && this->yMin >= yMin && this->xMax >= xMax && this->yMax <= yMax;
}

unsigned int Texture::getZoomFactor()
{
	return zoomFactor;
}

/*
void Texture::ConvLine(LongBuf& buf, int iLine, char * outbuf)
{
	long iLen = buf.iSize();
	char *c = &outbuf[iLine * iLen * 4];
	for (long i = 0; i < iLen; ++i)
		if (iUNDEF == buf[i]) {
			*c++ = 0;
			*c++ = 0;
			*c++ = 0;
			*c++ = (char)0; // alpha = 0
		} else {
			Color col (drawColor->clrRaw(buf[i], drm));
			*c++ = col.blue();
			*c++ = col.green();
			*c++ = col.red();
			*c++ = (char)255; // alpha = max
		}
}
*/

void Texture::ConvLine(LongBuf& buf, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = buf.iSize();
	long * ptrBuf = buf.buf();
	drawColor->clrRaw(ptrBuf, iLen, drm);
	char *c = &outbuf[iLine * texSizeX * 4];
	memcpy(c, ptrBuf, iLen * 4);
	c += 3; // point to the last byte of the Color struct
	for (long i = 0; i < iLen; ++i) {
		(iUNDEF == ptrBuf[i])?*c&=0:*c|=255; // alpha = 0 or alpha = max
		c += 4;
	}
}

/*
void Texture::ConvLine(const RealBuf& buf, int iLine, char * outbuf)
{
	long iLen = buf.iSize();
	char *c = &outbuf[iLine * iLen * 4];
	for (long i = 0; i < iLen; ++i)
		if (rUNDEF == buf[i]) {
			*c++ = 0;
			*c++ = 0;
			*c++ = 0;
			*c++ = (char)0; // alpha = 0
		} else {
			Color col (drawColor->clrVal(buf[i]));
			*c++ = col.blue();
			*c++ = col.green();
			*c++ = col.red();
			*c++ = (char)255; // alpha = max
		}
}
*/

void Texture::ConvLine(const RealBuf& buf, const int iLine, const long texSizeX, char * outbuf)
{
	long iLen = buf.iSize();
	LongBuf bufColor (iLen);
	long * ptrBufColor = bufColor.buf();
	drawColor->clrVal(buf.buf(), iLen, ptrBufColor);
	char *c = &outbuf[iLine * texSizeX * 4];
	memcpy(c, ptrBufColor, iLen * 4);
	c += 3; // point to the last byte of the Color struct
	for (long i = 0; i < iLen; ++i) {
		(iUNDEF == ptrBufColor[i])?*c&=0:*c|=255; // alpha = 0 or alpha = max
		c += 4;
	}
}

void Texture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop)
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
		return;

	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;

	if (*fDrawStop)
		return;
	mp->KeepOpen(true);
	if (zoomFactor == 1) // 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	{
		if (fRealMap) 
		{
			RealBuf bufIn(sizeX);
			memset(bufIn.buf(), 0, sizeX * 8); // to prevent NAN values in bufIn.
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos)
			{
				if (*fDrawStop)
					break;
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				ConvLine(bufIn, iDataInYPos, texSizeX, outbuf);
			}
		}
		else 
		{ // !fRealMap
			LongBuf bufIn(sizeX);
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos) 
			{
				if (*fDrawStop)
					break;
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX);
				ConvLine(bufIn, iDataInYPos, texSizeX, outbuf);
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
			memset(ptrBufIn, 0, sizeX * 8); // to prevent NAN values in bufIn.

			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop)
					break;
				mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = ptrBufIn[iDataInXPos];
				ConvLine(bufIntermediate, iDataOutYPos, texSizeX, outbuf);
			}
		}
		else // !fRealMap
		{
			LongBuf bufIn(sizeX);
			long * ptrBufIn = bufIn.buf();
			LongBuf bufOut(xSizeOut);
			long * ptrBufOut = bufOut.buf();
			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop)
					break;
				if (fValue && !fAttTable)
					mp->GetLineVal(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				else
					mp->GetLineRaw(iDataInYPos + offsetY, bufIn, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufOut[iDataOutXPos] = ptrBufIn[iDataInXPos];
				ConvLine(bufOut, iDataOutYPos, texSizeX, outbuf);
			}
		}                 
	}
	mp->KeepOpen(false);
}