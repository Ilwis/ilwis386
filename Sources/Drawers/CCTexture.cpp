// Texture.cpp: implementation of the Texture class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "CCTexture.h"
#include "Drawers\RasterLayerDrawer.h"
#include "TextureHeap.h" // for Palette
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

CCTexture::CCTexture(const MapList & _mpl, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, RasterSetData *_data, unsigned int zoomFactor, const RangeReal & rrMinMaxMap)
: Texture(_mpl->map(0),drawColor, drm, offsetX,offsetY, sizeX, sizeY,_data->width, _data->height, zoomFactor, 0, rrMinMaxMap,0),
mpl(_mpl)
{
	data = _data ;
}

CCTexture::~CCTexture()
{
}

void CCTexture::CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	fValue = 0 != mpl[0]->dm()->pdvi() || 0 != mpl[0]->dm()->pdvr();
	fAttTable = false;
	texture_data = new char [(sizeX / zoomFactor) * (sizeY / zoomFactor) * 4];
	this->valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);

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
	glPixelTransferf(GL_MAP_COLOR, 0);
	RangeReal rrFromR = data->ccMaps[0].rr;
	RangeReal rrToR = mpl[data->ccMaps[0].index]->rrMinMax();
	if (rrFromR.fValid() && rrToR.fValid()) {
		float fFactR = (rrToR.rWidth()) / (rrFromR.rWidth());
		float fOffR = (rrToR.rLo() - rrFromR.rLo()) / rrFromR.rWidth();
		glPixelTransferf(GL_RED_SCALE, fFactR);
		glPixelTransferf(GL_RED_BIAS, fOffR);
	}

	RangeReal rrFromG = data->ccMaps[1].rr;
	RangeReal rrToG = mpl[data->ccMaps[1].index]->rrMinMax();
	if (rrFromG.fValid() && rrToG.fValid()) {
		float fFactG = (rrToG.rWidth()) / (rrFromG.rWidth());
		float fOffG = (rrToG.rLo() - rrFromG.rLo()) / rrFromG.rWidth();
		glPixelTransferf(GL_GREEN_SCALE, fFactG);
		glPixelTransferf(GL_GREEN_BIAS, fOffG);
	}

	RangeReal rrFromB = data->ccMaps[2].rr;
	RangeReal rrToB = mpl[data->ccMaps[2].index]->rrMinMax();
	if (rrFromB.fValid() && rrToB.fValid()) {
		float fFactB = (rrToB.rWidth()) / (rrFromB.rWidth());
		float fOffB = (rrToB.rLo() - rrFromB.rLo()) / rrFromB.rWidth();
		glPixelTransferf(GL_BLUE_SCALE, fFactB);
		glPixelTransferf(GL_BLUE_BIAS, fOffB);
	}

	glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	glPixelTransferf(GL_RED_SCALE, 1);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_SCALE, 1);
	glPixelTransferf(GL_GREEN_BIAS, 0);
	glPixelTransferf(GL_BLUE_SCALE, 1);
	glPixelTransferf(GL_BLUE_BIAS, 0);
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	fRepresentationChanged = false;
	if (fInThread)
		drawerContext->ReleaseContext();
}

void CCTexture::ReCreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop)
{
	fValue = 0 != mpl[0]->dm()->pdvi() || 0 != mpl[0]->dm()->pdvr();
	fAttTable = false;
	this->dirty = !DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);

	if (dirty)
		return;

	if (fInThread)
		drawerContext->TakeContext();
	glBindTexture( GL_TEXTURE_2D, texture );
	fRepresentationChanged = false;
	boolean oldVal;
	glGetBooleanv(GL_MAP_COLOR, &oldVal);
	glPixelTransferf(GL_MAP_COLOR, 0);
	RangeReal rrFromR = data->ccMaps[0].rr;
	RangeReal rrToR = mpl[data->ccMaps[0].index]->rrMinMax();
	if (rrFromR.fValid() && rrToR.fValid()) {
		float fFactR = (rrToR.rWidth()) / (rrFromR.rWidth());
		float fOffR = (rrToR.rLo() - rrFromR.rLo()) / rrFromR.rWidth();
		glPixelTransferf(GL_RED_SCALE, fFactR);
		glPixelTransferf(GL_RED_BIAS, fOffR);
	}

	RangeReal rrFromG = data->ccMaps[1].rr;
	RangeReal rrToG = mpl[data->ccMaps[1].index]->rrMinMax();
	if (rrFromG.fValid() && rrToG.fValid()) {
		float fFactG = (rrToG.rWidth()) / (rrFromG.rWidth());
		float fOffG = (rrToG.rLo() - rrFromG.rLo()) / rrFromG.rWidth();
		glPixelTransferf(GL_GREEN_SCALE, fFactG);
		glPixelTransferf(GL_GREEN_BIAS, fOffG);
	}

	RangeReal rrFromB = data->ccMaps[2].rr;
	RangeReal rrToB = mpl[data->ccMaps[2].index]->rrMinMax();
	if (rrFromB.fValid() && rrToB.fValid()) {
		float fFactB = (rrToB.rWidth()) / (rrFromB.rWidth());
		float fOffB = (rrToB.rLo() - rrFromB.rLo()) / rrFromB.rWidth();
		glPixelTransferf(GL_BLUE_SCALE, fFactB);
		glPixelTransferf(GL_BLUE_BIAS, fOffB);
	}

	glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	glPixelTransferf(GL_RED_SCALE, 1);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_SCALE, 1);
	glPixelTransferf(GL_GREEN_BIAS, 0);
	glPixelTransferf(GL_BLUE_SCALE, 1);
	glPixelTransferf(GL_BLUE_BIAS, 0);
	glPixelTransferf(GL_MAP_COLOR, oldVal);
	if (fInThread)
		drawerContext->ReleaseContext();
}

bool CCTexture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, char * outbuf, volatile bool* fDrawStop)
{
	RowCol rcSize = mpl[0]->rcSize();
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

	ValueRange vr = mpl[0]->vr();
	if (mpl[0]->dm()->pdbool())
		vr = ValueRange();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;

	if (*fDrawStop)
		return false;

	double rFactR = 1;
	double rOffR = 0;
	double rFactG = 1;
	double rOffG = 0;
	double rFactB = 1;
	double rOffB = 0;
	RangeReal rrFromR = mpl[data->ccMaps[0].index]->rrMinMax();
	RangeReal rrFromG = mpl[data->ccMaps[1].index]->rrMinMax();
	RangeReal rrFromB = mpl[data->ccMaps[2].index]->rrMinMax();
	RangeReal rrTo = RangeReal(0, 255);
	if (rrFromR.fValid()) {
		rFactR = (rrTo.rWidth()) / (rrFromR.rWidth());
		rOffR = rrFromR.rLo();
	}
	if (rrFromG.fValid()) {
		rFactG = (rrTo.rWidth()) / (rrFromG.rWidth());
		rOffG = rrFromG.rLo();
	}
	if (rrFromB.fValid()) {
		rFactB = (rrTo.rWidth()) / (rrFromB.rWidth());
		rOffB = rrFromB.rLo();
	}

	if (zoomFactor == 1) // 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	{
		if (fRealMap) 
		{
			RealBuf bufIn1(sizeX);
			RealBuf bufIn2(sizeX);
			RealBuf bufIn3(sizeX);
			double * ptrBufIn1 = bufIn1.buf();
			double * ptrBufIn2 = bufIn2.buf();
			double * ptrBufIn3 = bufIn3.buf();
			LongBuf bufColor(sizeX);
			long * ptrBufColor = bufColor.buf();
			memset(bufIn1.buf(), 0, sizeX * 8); // to prevent NAN values in bufIn.
			memset(bufIn2.buf(), 0, sizeX * 8);
			memset(bufIn3.buf(), 0, sizeX * 8);
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos)
			{
				if (*fDrawStop) {
					return false;
				}
				mpl[data->ccMaps[0].index]->GetLineVal(iDataInYPos + offsetY, bufIn1, offsetX, sizeX);
				mpl[data->ccMaps[1].index]->GetLineVal(iDataInYPos + offsetY, bufIn2, offsetX, sizeX);
				mpl[data->ccMaps[2].index]->GetLineVal(iDataInYPos + offsetY, bufIn3, offsetX, sizeX);
				for (int i = 0; i < sizeX; ++i) {
					byte v1 = byteConv((ptrBufIn1[i] - rOffR) * rFactR);
					byte v2 = byteConv((ptrBufIn2[i] - rOffG) * rFactG);
					byte v3 = byteConv((ptrBufIn3[i] - rOffB) * rFactB);
					ptrBufColor[i] = (v1) | (v2 << 8) | (v3 << 16);
				}
				PutLine(bufIn1, bufColor, iDataInYPos, texSizeX, outbuf);
			}
		}
		else 
		{ // !fRealMap
			LongBuf bufIn1(sizeX);
			LongBuf bufIn2(sizeX);
			LongBuf bufIn3(sizeX);
			long * ptrBufIn1 = bufIn1.buf();
			long * ptrBufIn2 = bufIn2.buf();
			long * ptrBufIn3 = bufIn3.buf();
			LongBuf bufColor(sizeX);
			long * ptrBufColor = bufColor.buf();
			for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos) 
			{
				if (*fDrawStop) {
					return false;
				}
				if (fValue && !fAttTable) {
					mpl[data->ccMaps[0].index]->GetLineVal(iDataInYPos + offsetY, bufIn1, offsetX, sizeX);
					mpl[data->ccMaps[1].index]->GetLineVal(iDataInYPos + offsetY, bufIn2, offsetX, sizeX);
					mpl[data->ccMaps[2].index]->GetLineVal(iDataInYPos + offsetY, bufIn3, offsetX, sizeX);
				} else {
					mpl[data->ccMaps[0].index]->GetLineRaw(iDataInYPos + offsetY, bufIn1, offsetX, sizeX);
					mpl[data->ccMaps[1].index]->GetLineRaw(iDataInYPos + offsetY, bufIn2, offsetX, sizeX);
					mpl[data->ccMaps[2].index]->GetLineRaw(iDataInYPos + offsetY, bufIn3, offsetX, sizeX);
				}
				for (int i = 0; i < sizeX; ++i) {
					byte v1 = byteConv((ptrBufIn1[i] - rOffR) * rFactR);
					byte v2 = byteConv((ptrBufIn2[i] - rOffG) * rFactG);
					byte v3 = byteConv((ptrBufIn3[i] - rOffB) * rFactB);
					ptrBufColor[i] = (v1) | (v2 << 8) | (v3 << 16);
				}
				PutLine(bufIn1, bufColor, iDataInYPos, texSizeX, outbuf);
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
		int iPyrLayer = (int)(mpl[0]->fHasPyramidFile() ? max(0, log10((double)zoomFactor) / log10(2.0)) : 0);	
		if (iPyrLayer > mpl[0]->iNoOfPyramidLayers())
			iPyrLayer = mpl[0]->iNoOfPyramidLayers();
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
			RealBuf bufIn1(sizeX);
			RealBuf bufIn2(sizeX);
			RealBuf bufIn3(sizeX);
			double * ptrBufIn1 = bufIn1.buf();
			double * ptrBufIn2 = bufIn2.buf();
			double * ptrBufIn3 = bufIn3.buf();
			LongBuf bufColor(xSizeOut);
			long * ptrBufColor = bufColor.buf();
			memset(ptrBufIn1, 0, sizeX * 8); // to prevent NAN values in bufIn.
			memset(ptrBufIn2, 0, sizeX * 8);
			memset(ptrBufIn3, 0, sizeX * 8);

			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					return false;
				}
				mpl[data->ccMaps[0].index]->GetLineVal(iDataInYPos + offsetY, bufIn1, offsetX, sizeX, iPyrLayer);
				mpl[data->ccMaps[1].index]->GetLineVal(iDataInYPos + offsetY, bufIn2, offsetX, sizeX, iPyrLayer);
				mpl[data->ccMaps[2].index]->GetLineVal(iDataInYPos + offsetY, bufIn3, offsetX, sizeX, iPyrLayer);
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor) {
					byte v1 = byteConv((ptrBufIn1[iDataInXPos] - rOffR) * rFactR);
					byte v2 = byteConv((ptrBufIn2[iDataInXPos] - rOffG) * rFactG);
					byte v3 = byteConv((ptrBufIn3[iDataInXPos] - rOffB) * rFactB);
					ptrBufColor[iDataOutXPos] = (v1) | (v2 << 8) | (v3 << 16);
				}
				PutLine(bufColor, bufColor, iDataOutYPos, texSizeX, outbuf);
			}
		}
		else // !fRealMap
		{
			LongBuf bufIn1(sizeX);
			LongBuf bufIn2(sizeX);
			LongBuf bufIn3(sizeX);
			long * ptrBufIn1 = bufIn1.buf();
			long * ptrBufIn2 = bufIn2.buf();
			long * ptrBufIn3 = bufIn3.buf();
			LongBuf bufColor(xSizeOut);
			long * ptrBufColor = bufColor.buf();
			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					return false;
				}
				if (fValue && !fAttTable) {
					mpl[data->ccMaps[0].index]->GetLineVal(iDataInYPos + offsetY, bufIn1, offsetX, sizeX, iPyrLayer);
					mpl[data->ccMaps[1].index]->GetLineVal(iDataInYPos + offsetY, bufIn2, offsetX, sizeX, iPyrLayer);
					mpl[data->ccMaps[2].index]->GetLineVal(iDataInYPos + offsetY, bufIn3, offsetX, sizeX, iPyrLayer);
				} else {
					mpl[data->ccMaps[0].index]->GetLineRaw(iDataInYPos + offsetY, bufIn1, offsetX, sizeX, iPyrLayer);
					mpl[data->ccMaps[1].index]->GetLineRaw(iDataInYPos + offsetY, bufIn2, offsetX, sizeX, iPyrLayer);
					mpl[data->ccMaps[2].index]->GetLineRaw(iDataInYPos + offsetY, bufIn3, offsetX, sizeX, iPyrLayer);
				}
				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor) {
					byte v1 = byteConv((ptrBufIn1[iDataInXPos] - rOffR) * rFactR);
					byte v2 = byteConv((ptrBufIn2[iDataInXPos] - rOffG) * rFactG);
					byte v3 = byteConv((ptrBufIn3[iDataInXPos] - rOffB) * rFactB);
					ptrBufColor[iDataOutXPos] = (v1) | (v2 << 8) | (v3 << 16);
				}

				PutLine(bufColor, bufColor, iDataOutYPos, texSizeX, outbuf);
			}
		}                 
	}
	return true;
}

void CCTexture::BindMe(DrawerContext * drawerContext)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	double s = offsetX / (double)imgWidth2;
	double t = offsetY / (double)imgHeight2;
	glLoadIdentity();
	glScaled(imgWidth2 / (double)sizeX, imgHeight2 / (double)sizeY, 1);
	glTranslated(-s, -t, 0);
	if (fRepresentationChanged) {
		fRepresentationChanged = false;
		boolean oldVal;
		glGetBooleanv(GL_MAP_COLOR, &oldVal);
		glPixelTransferf(GL_MAP_COLOR, false);

		RangeReal rrFromR = data->ccMaps[0].rr;
		RangeReal rrToR = mpl[data->ccMaps[0].index]->rrMinMax();
		if (rrFromR.fValid() && rrToR.fValid()) {
			float fFactR = (rrToR.rWidth()) / (rrFromR.rWidth());
			float fOffR = (rrToR.rLo() - rrFromR.rLo()) / rrFromR.rWidth();
			glPixelTransferf(GL_RED_SCALE, fFactR);
			glPixelTransferf(GL_RED_BIAS, fOffR);
		}

		RangeReal rrFromG = data->ccMaps[1].rr;
		RangeReal rrToG = mpl[data->ccMaps[1].index]->rrMinMax();
		if (rrFromG.fValid() && rrToG.fValid()) {
			float fFactG = (rrToG.rWidth()) / (rrFromG.rWidth());
			float fOffG = (rrToG.rLo() - rrFromG.rLo()) / rrFromG.rWidth();
			glPixelTransferf(GL_GREEN_SCALE, fFactG);
			glPixelTransferf(GL_GREEN_BIAS, fOffG);
		}

		RangeReal rrFromB = data->ccMaps[2].rr;
		RangeReal rrToB = mpl[data->ccMaps[2].index]->rrMinMax();
		if (rrFromB.fValid() && rrToB.fValid()) {
			float fFactB = (rrToB.rWidth()) / (rrFromB.rWidth());
			float fOffB = (rrToB.rLo() - rrFromB.rLo()) / rrFromB.rWidth();
			glPixelTransferf(GL_BLUE_SCALE, fFactB);
			glPixelTransferf(GL_BLUE_BIAS, fOffB);
		}

		glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

		glPixelTransferf(GL_RED_SCALE, 1);
		glPixelTransferf(GL_RED_BIAS, 0);
		glPixelTransferf(GL_GREEN_SCALE, 1);
		glPixelTransferf(GL_GREEN_BIAS, 0);
		glPixelTransferf(GL_BLUE_SCALE, 1);
		glPixelTransferf(GL_BLUE_BIAS, 0);
		glPixelTransferf(GL_MAP_COLOR, oldVal);
	}
}

