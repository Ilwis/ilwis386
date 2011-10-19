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

	if (*fDrawStop)
		return false;
	if (zoomFactor == 1) // 1:1 a pixel is a rastel; expected in outbuf: sizeX * sizeY * 4 bytes (for RGBA colors)
	{
		ByteBuf bufIn1(sizeX),bufIn2(sizeX),bufIn3(sizeX) ;
		LongBuf bufColor(sizeX), bufComposite(sizeX);
		for (long iDataInYPos = 0; iDataInYPos < sizeY; ++iDataInYPos) 
		{
			if (*fDrawStop) {
				return false;
			}
			RangeReal ranges[6];
			ranges[0] = data->ccMaps[0].rr;
			ranges[1] = mpl[data->ccMaps[0].index]->rrMinMax();
			ranges[2] = data->ccMaps[1].rr;
			ranges[3] = mpl[data->ccMaps[1].index]->rrMinMax();
			ranges[4] = data->ccMaps[2].rr;
			ranges[5] = mpl[data->ccMaps[2].index]->rrMinMax();

			byte stretchedValues[768];
			for(int i=0; i < 256; ++i) {
				stretchedValues[i] = stretch(i,ranges[1], ranges[0]);
				stretchedValues[i + 256] = stretch(i,ranges[3], ranges[2]);
				stretchedValues[i + 512] = stretch(i,ranges[5], ranges[4]);
			}

			mpl[data->ccMaps[0].index]->GetLineRaw(iDataInYPos + offsetY, bufIn1, offsetX, sizeX);
			mpl[data->ccMaps[1].index]->GetLineRaw(iDataInYPos + offsetY, bufIn2, offsetX, sizeX);
			mpl[data->ccMaps[2].index]->GetLineRaw(iDataInYPos + offsetY, bufIn3, offsetX, sizeX);

			for(int i = 0; i < sizeX; ++i) {
					byte v1 = stretchedValues[bufIn1[i]];
					byte v2 = stretchedValues[bufIn2[i] + 256];
					byte v3 = stretchedValues[bufIn3[i] + 512];
					bufComposite[i] = (v1) | (v2 << 8) | (v3 << 16);
				}

			ConvLine(bufComposite, bufColor);
			PutLine(bufComposite, bufColor, iDataInYPos, texSizeX, outbuf);
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

			ByteBuf bufIn1(sizeX),bufIn2(sizeX),bufIn3(sizeX) ;
			//long * ptrBufIn = bufIn.buf();
			LongBuf bufIntermediate(xSizeOut), bufComposite(sizeX);
			long * ptrBufIntermediate = bufIntermediate.buf();
			LongBuf bufColor(xSizeOut);

			RangeReal ranges[6];
			ranges[0] = data->ccMaps[0].rr;
			ranges[1] = mpl[data->ccMaps[0].index]->rrMinMax();
			ranges[2] = data->ccMaps[1].rr;
			ranges[3] = mpl[data->ccMaps[1].index]->rrMinMax();
			ranges[4] = data->ccMaps[2].rr;
			ranges[5] = mpl[data->ccMaps[2].index]->rrMinMax();
			byte stretchedValues[768];
			for(int i=0; i < 256; ++i) {
				stretchedValues[i] = stretch(i,ranges[1], ranges[0]);
				stretchedValues[i + 256] = stretch(i,ranges[3], ranges[2]);
				stretchedValues[i + 512] = stretch(i,ranges[5], ranges[4]);
			}

			for (long iDataOutYPos = 0, iDataInYPos = 0; iDataOutYPos < ySizeOut; ++iDataOutYPos, iDataInYPos += zoomFactor)
			{
				if (*fDrawStop) {
					return false;
				}
				mpl[data->ccMaps[0].index]->GetLineRaw(iDataInYPos + offsetY, bufIn1, offsetX, sizeX,iPyrLayer);
				mpl[data->ccMaps[1].index]->GetLineRaw(iDataInYPos + offsetY, bufIn2, offsetX, sizeX,iPyrLayer);
				mpl[data->ccMaps[2].index]->GetLineRaw(iDataInYPos + offsetY, bufIn3, offsetX, sizeX,iPyrLayer);
				
				for(int i = 0; i < sizeX; ++i) {
					byte v1 = stretchedValues[bufIn1[i]];
					byte v2 = stretchedValues[bufIn2[i] + 256];
					byte v3 = stretchedValues[bufIn3[i] + 512];
					bufComposite[i] = (v1) | (v2 << 8) | (v3 << 16);
				}

				for (long iDataOutXPos = 0, iDataInXPos = 0; iDataOutXPos < xSizeOut; ++iDataOutXPos, iDataInXPos += zoomFactor)
					ptrBufIntermediate[iDataOutXPos] = bufComposite[iDataInXPos];

				ConvLine(bufIntermediate, bufColor);
				PutLine(bufIntermediate, bufColor, iDataOutYPos, texSizeX, outbuf);
			}
              
	}
	return true;
}

double CCTexture::stretch(double v, const RangeReal& rrFrom, const RangeReal& rrTo) {
	if ( rrFrom.fValid() && rrTo.fValid()) {
		double rFact = (rrTo.rWidth()) / (rrFrom.rWidth());
		double rOff = rrTo.rLo();
		return byteConv(rFact * (v - rrFrom.rLo()) + rOff);
	}
	return v;
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
		bool fDrawStop = false;
		texture_data = new char [(sizeX / zoomFactor) * (sizeY / zoomFactor) * 4];
		DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, &fDrawStop);
		boolean oldVal;
		glGetBooleanv(GL_MAP_COLOR, &oldVal);
		glPixelTransferf(GL_MAP_COLOR, false);
		glTexImage2D( GL_TEXTURE_2D, 0, 4, sizeX / zoomFactor, sizeY / zoomFactor, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
		delete [] texture_data;
		texture_data = 0;
		glPixelTransferf(GL_MAP_COLOR, oldVal);
		fRepresentationChanged = false;
	}
}

