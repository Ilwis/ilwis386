/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)

#include "Headers\toolspch.h"
#include "Engine\Base\File\ElementMap.h"
#include "Engine\DataExchange\ExpTiff.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmident.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\Grsub.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\SpatialReference\DATUM.H"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"


TiffExporter::TiffExporter(const FileName& _fnIn, const FileName& _fnOut, bool asColors) :
	fnIn(_fnIn),
	fnOut(_fnOut),
	_asColors(asColors),
	_outFile(NULL)
{
	iIFDEntries=13;
	if (_asColors)
		transformer = new ColorTransformer(*(this));
	else
		transformer = new ValuesTransformer(*(this));
}

TiffExporter::~TiffExporter()
{
	if (transformer != NULL)
		delete transformer;

	if (_outFile != NULL)
		delete _outFile;
}

// The IFD will be written at the end of the TIFF file
void TiffExporter::WriteIFD(const TiffExportInfo& teiData)
{
	iIFDPosition = _outFile->iLoc();  // position of the Tiff tag counter

// IFD size = #entries * sizeof(entry) + sizeof(counter) + sizeof(^NextIFD)
//     with size of IFD entry = 12
//     maximum entries = 18 (icluding GeoTiff)
//     IFD entry counter occupies 2 bytes
//     pointer to the next IFD is a long
	const short iNrClear = MAX_IFD_ENTRIES * SIZEOF_TAG + IFD_TABLE_EXTRA; 
	byte acClear[iNrClear];
	for (long i = 0; i < iNrClear; i++)
		acClear[i] = 0;
	_outFile->Write(iNrClear, acClear);
	_outFile->Seek(iIFDPosition);   // back to the start of the IFD

	RowCol rcSize = _inMap->rcSize();
	Array<long> iVals;

	iIFDEntries = 0;   // number of written tags is not known yet
	_outFile->Write(sizeof(iIFDEntries), &iIFDEntries);

	WriteField(tagImageWidth,  tiffLong, 1, rcSize.Col);
	WriteField(tagImageLength, tiffLong, 1, rcSize.Row);

	if (teiData.iBitsPerSample == 24) {
		for (int j = 0; j < 3; ++j) iVals &= 8;
		WriteField(tagBitsPerSample, tiffShort, iVals);
	}
	else //if (teiData.iBitsPerSample == 8)
		WriteField(tagBitsPerSample, tiffShort, 1, teiData.iBitsPerSample);

	WriteField(tagCompression, tiffShort, 1, VCompression);

	WriteField(tagPhotometricInterpretation, tiffShort, 1,  teiData.iPhotoMI);

	WriteField(tagStripOffset, tiffLong, 1, iHeaderLength); 
	if (teiData.iBitsPerSample == 24)
		WriteField(tagSamplesPerPixel, tiffShort, 1, VSamplesPerPixel);
	WriteField(tagRowsPerStrip, tiffLong, 1, rcSize.Row);

	long iRowSize = (teiData.iBitsPerSample * rcSize.Col) / 8;

	if (teiData.iBitsPerSample == 0)  // true for maps with 1 bit/pixel
		iRowSize = (rcSize.Col + 8) / 8;

	WriteField(tagStripByteCount, tiffLong, 1, rcSize.Row * iRowSize);
	WriteField(tagXResolution, tiffRational, 1, 300, 1);
	WriteField(tagYResolution, tiffRational, 1, 300, 1);   
	WriteField(tagResolutionUnit ,tiffShort, 1, 2);
	WriteField(tagSoftware ,tiffASCII, "Ilwis 3");

	// mark the position to add GeoTiff info
	long iGeoKeyPosition = _outFile->iLoc();

	// mark next IFD as non-existent
	long iNextIFD = 0;
	_outFile->Write(sizeof(long), &iNextIFD);

	// Update the Tiff tag counter to the actual number,
	// this number does not include GeoTiff, because that is added later
	_outFile->Seek(iIFDPosition);
	_outFile->Write(sizeof(iIFDEntries), &iIFDEntries);
	_outFile->Seek(iGeoKeyPosition);
}

void TiffExporter::Export(Tranquilizer& trq)
{
	const unsigned long VIFDOffset=0; //dummy real value will be added later
	_inMap = Map(fnIn);
	FileName fnOutPut = FileName::fnUnique(FileName(String("%S.tif",fnOut.sFullPath(false))));
	_outFile = new File(fnOutPut, facCRT);
	_outFile->SetErase(true);  // remove the file in case of errors.

	// write header
	trq.SetText(SCVTextTiffHeader);
	_outFile->Write(sizeof(VByteOrder), &VByteOrder);
	_outFile->Write(sizeof(VHgtg), &VHgtg);
	_outFile->Write(sizeof(VIFDOffset), &VIFDOffset);

	// write data
	trq.SetText(SCVTextProcessing);

	TiffExportInfo teiData = transformer->transform(trq);

	//write IFD
	trq.SetText(SCVTextTiffIFD);

	WriteIFD(teiData);
	long iEndTable=_outFile->iLoc();

	_outFile->Seek(4);
	_outFile->Write(sizeof(long), &iIFDPosition);  // write file offset of IFD

	_outFile->Seek(iEndTable); // reset filepointer to the end of the IFD to make adding possible
}

void TiffExporter::WriteField(unsigned short tTag, tiffType tType, String sVersion)
{
	long iCount = sVersion.length() + 1; //includes \0;
	_outFile->Write(sizeof(unsigned short), &tTag);
	_outFile->Write(sizeof(unsigned short), &tType);
	_outFile->Write(sizeof(long), &iCount);

	long iCurr = _outFile->iLoc();
	_outFile->Seek(0,true);
	long iEnd = _outFile->iLoc();

	_outFile->Write(sVersion);
	_outFile->Seek(iCurr);
	_outFile->Write(sizeof(long), &iEnd);
	iIFDEntries++;
}

// Write TIFF tag into IFD:
//   this one contains a rational value, consisting of two long
//   integers. They are not written into the IFD itself, but at the 
//   current end of the file.
// After the TIFF tag is written, the IFD file position has shifted
// to the next IFD entry.
void TiffExporter::WriteField(unsigned short tTag, tiffType tType, long iCount, long iNum, long iDenum)
{
	_outFile->Write(sizeof(unsigned short), &tTag);
	_outFile->Write(sizeof(unsigned short), &tType);
	_outFile->Write(sizeof(long), &iCount);

	long iCurr = _outFile->iLoc();
	_outFile->Seek(0, true);
	long iEnd = _outFile->iLoc();

	_outFile->Write(sizeof(long), &iNum);
	_outFile->Write(sizeof(long), &iDenum);

	// update the file offset for the rational in the IFD entry
	_outFile->Seek(iCurr);
	_outFile->Write(sizeof(long), &iEnd);
	iIFDEntries++;
}

// Write TIFF tag into IFD:
//   this one contains a series of integer type values in aiVals, which
//   are not written into the IFD itself, but at the current end of the
//   file.
// After the TIFF tag is written, the IFD file position has shifted
// to the next IFD entry.
void TiffExporter::WriteField(unsigned short tTag, tiffType tType, Array<long>& aiVals)
{
	long iCount = aiVals.iSize();
	_outFile->Write(sizeof(unsigned short), &tTag);
	_outFile->Write(sizeof(unsigned short), &tType);
	_outFile->Write(sizeof(long), &iCount);

	long iCurr = _outFile->iLoc();    // retain position in IFD
	_outFile->Seek(0, true);          // position to end of file...
	long iEnd = _outFile->iLoc();     // ...and store this position

	// write all values in the aiVals array, taking into
	// account the type of the values (short, or long)
	for (long i = 0; i < iCount; ++i) {
		switch(tType)  {
			case tiffShort: {
				short iTemp = (short)aiVals[i];
				_outFile->Write(sizeof(short), &iTemp);
				break;
			}
			case tiffLong: {
				_outFile->Write(sizeof(long), &aiVals[i]);
				break;
			}
		}
	}

	aiVals.Resize(0);
	_outFile->Seek(iCurr);

	// write file offset to the values in the current IFD entry
	_outFile->Write(sizeof(long), &iEnd);
	iIFDEntries++;
}


// Write TIFF tag into IFD:
//   this one contains a series of doubles in arVals, which
//   are not written into the IFD itself, but at the current end of the
//   file.
// After the TIFF tag is written, the IFD file position has shifted
// to the next IFD entry.
void TiffExporter::WriteField(unsigned short tTag, tiffType tType, Array<double>& arVals)
{
	long iCount = arVals.iSize();
	_outFile->Write(sizeof(unsigned short), &tTag);
	_outFile->Write(sizeof(unsigned short), &tType);
	_outFile->Write(sizeof(long), &iCount);

	long iCurr = _outFile->iLoc();   // retain position in IFD
	_outFile->Seek(0, true);         // position to end of file...
	long iEnd = _outFile->iLoc();    // ...and store this position

	// write all values in the arVals array
	for (long i = 0; i < iCount; ++i)  {
		double rVal = arVals[i];
		_outFile->Write(sizeof(double), &rVal);
	}

	arVals.Resize(0);
	_outFile->Seek(iCurr);           // return to IFD position 

	// write file offset to the values in the current IFD entry
	_outFile->Write(sizeof(long), &iEnd);
	iIFDEntries++;                  // IFD entries written has increased
}


// Write one IFD value into the IFD;
// The IFD position is shifted to the next IFD entry
void TiffExporter::WriteField(unsigned short tTag, tiffType tType, long iCount, long iV)
{
	_outFile->Write(sizeof(unsigned short), &tTag);
	_outFile->Write(sizeof(unsigned short), &tType);
	_outFile->Write(sizeof(long), &iCount);
	_outFile->Write(sizeof(long), &iV);
	iIFDEntries++;
}

/// ColorTransformer class
// Fill de LUT in the teiData structure. Only needed for writing the colors
// The LUT is never actually written to the TIFF file
void ColorTransformer::setBitColormap(TiffExportInfo& teiData)
{
	teiData.iPhotoMI = VPMIRGB;
	teiData.iBitsPerSample = 24;
	teiData.iSamplesPerPixel = 3;

	// Fill palette
	teiData.aiColorMap[0]   = 255 << 8;   // zero.red
	teiData.aiColorMap[256] = 255 << 8;   // zero.green
	teiData.aiColorMap[512] = 255 << 8;   // zero.blue

	teiData.aiColorMap[1]   = 0;     // one.red
	teiData.aiColorMap[257] = 0;     // one.green
	teiData.aiColorMap[513] = 0;     // one.blue
}

// Fill de LUT in the teiData structure. Only needed for writing the colors
// The LUT is never actually written to the TIFF file
void ColorTransformer::setBoolColormap(TiffExportInfo& teiData)
{
	teiData.iPhotoMI = VPMIRGB;
	teiData.iBitsPerSample = 24;
	teiData.iSamplesPerPixel = 3;

	// Fill palette
	teiData.aiColorMap[0]   = 0;   // UNDEF.red
	teiData.aiColorMap[256] = 0;   // UNDEF.green
	teiData.aiColorMap[512] = 0;   // UNDEF.blue

	teiData.aiColorMap[1]   = 255 << 8;  // false.red
	teiData.aiColorMap[257] = 0;         // false.green
	teiData.aiColorMap[513] = 0;         // false.blue

	teiData.aiColorMap[2]   = 0;         // true.red
	teiData.aiColorMap[258] = 255 << 8;  // true.green
	teiData.aiColorMap[514] = 0;         // true.blue
}

TiffExportInfo ColorTransformer::transform(Tranquilizer& trq)
{
	TiffExportInfo teiData;
	teiData.iPhotoMI = VPMIRGB;
	teiData.iBitsPerSample = 24;
	teiData.iSamplesPerPixel = 3;

	DomainType dmt = _te.inMap()->dm()->dmt();
	switch (dmt)
	{
	case dmtIMAGE:
	case dmtVALUE:
		writeValues(trq);
		break;
	case dmtBOOL:
		setBoolColormap(teiData);
		writeAsColorPalette(teiData, trq);
		break;
	case dmtBIT:
		setBitColormap(teiData);
		writeAsColorPalette(teiData, trq);
		break;
	case dmtPICTURE:
	case dmtCLASS:
		writeAsColorRaw(trq);
		break;
	case dmtCOLOR:
		writeAsColor(trq);
		break;
	case dmtID:
		writeAsColorID(trq);
		break;
	}

	return teiData;
}

void ColorTransformer::writeValues(Tranquilizer& trq)
{
	Map& inMap = _te.inMap();
	Representation rpr = inMap->dm()->rpr();
	RepresentationValue* prv = rpr->prv();
	RangeReal rr = rrDetermineValueRange(inMap);

	RealBuf rBuf(inMap->iCols());
	Color color;

	for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
		inMap->GetLineVal(iRow, rBuf);
		if ( trq.fUpdate(iRow, inMap->iLines()))
			return;
		for (long iCol = 0; iCol < inMap->iCols(); ++iCol) {
			double rVal = rBuf[iCol];
			if (rVal == rUNDEF)
				color = Color(255, 255, 255); // white
			else if (0 == prv)
				color = rpr->clr(rBuf[iCol],rr);
			else
				color = rpr->clr(rBuf[iCol]);
			writeColor(color);
		}
	}
}

void ColorTransformer::writeAsColor(Tranquilizer& trq)
{
	Map& inMap = _te.inMap();
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
	{
		LongBuf iBuf(inMap->iCols());
		inMap->GetLineRaw(iRow, iBuf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;
		for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
			writeColor(iBuf[iCol]);
	}
}

void ColorTransformer::writeAsColorRaw(Tranquilizer& trq)
{
	Map& inMap = _te.inMap();
	Representation rpr = inMap->dm()->rpr();

	for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
	{
		LongBuf iBuf(inMap->iCols());
		inMap->GetLineRaw(iRow, iBuf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;
		for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
			writeColor(rpr->clrRaw(iBuf[iCol]));
	}
}

void ColorTransformer::writeAsColorID(Tranquilizer& trq)
{
	Map& inMap = _te.inMap();

	for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
	{
		LongBuf iBuf(inMap->iCols());
		inMap->GetLineRaw(iRow, iBuf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;
		for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
			writeColor(Color::clrPrimary(1 + iBuf[iCol] % 31));
	}
}

void ColorTransformer::writeAsColorPalette(TiffExportInfo& teiData, Tranquilizer& trq)
{
	Map& inMap = _te.inMap();

	for(long iRow = 0; iRow < inMap->iLines(); ++iRow)
	{
		ByteBuf buf(inMap->iCols());
		inMap->GetLineRaw(iRow, buf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;

		for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
		{
			Color color = Color(teiData.aiColorMap[buf[iCol]] >> 8,
								teiData.aiColorMap[buf[iCol] + 256] >> 8,
								teiData.aiColorMap[buf[iCol] + 512] >> 8);
			writeColor(color);
		}
	}
}

void RasterTransformer::writeColor(const Color& color)
{
	File* outFile = _te.outFile();

	byte red=color.red();
	byte green=color.green();
	byte blue=color.blue();
	outFile->Write(1, &red);
	outFile->Write(1, &green);
	outFile->Write(1, &blue);
}

// ValuesTransformer class
TiffExportInfo ValuesTransformer::transform(Tranquilizer& trq) {
	TiffExportInfo teiData;
	teiData.iPhotoMI = VPMIBitBlack;

	DomainType dmt = _te.inMap()->dm()->dmt();
	switch (dmt)
	{
	case dmtBIT:
	case dmtBOOL:
	case dmtIMAGE:
	case dmtPICTURE:
		writeByteValues(teiData, trq);
		break;
	case dmtCLASS:
	case dmtID:
	case dmtUNIQUEID:
	case dmtVALUE:
		switch (_te.inMap()->dm()->stNeeded())
			{
			case stBYTE:
				writeByteValues(teiData, trq);
				break;
			case stINT:
				writeShortValues(teiData, trq);
				break;
			case stLONG:
				writeLongValues(teiData, trq);
				break;
			case stFLOAT:
			case stREAL:
				writeValues(teiData, trq);	// double will be converted to float
			}
		break;
	case dmtCOLOR:
		writeColorValues(teiData, trq);
		break;
	}

	return teiData;
}

void ValuesTransformer::writeByteValues(TiffExportInfo& teiData, Tranquilizer& trq)
{
	teiData.iSamplesPerPixel = 1;
	teiData.iBitsPerSample = 8;

	Map& inMap = _te.inMap();
	ByteBuf buf(inMap->iCols());
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
		inMap->GetLineRaw(iRow, buf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;

		_te.outFile()->Write(inMap->iCols(), buf.buf());
	}
}

void ValuesTransformer::writeShortValues(TiffExportInfo& teiData, Tranquilizer& trq)
{
	teiData.iSamplesPerPixel = 2;
	teiData.iBitsPerSample = 16;

	Map& inMap = _te.inMap();
	IntBuf buf(inMap->iCols());
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
		inMap->GetLineRaw(iRow, buf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;

		_te.outFile()->Write(inMap->iCols() * 2, buf.buf());
	}
}

void ValuesTransformer::writeLongValues(TiffExportInfo& teiData, Tranquilizer& trq)
{
	teiData.iSamplesPerPixel = 4;
	teiData.iBitsPerSample = 32;

	Map& inMap = _te.inMap();
	LongBuf buf(inMap->iCols());
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
		inMap->GetLineVal(iRow, buf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;

		_te.outFile()->Write(inMap->iCols() * 4, buf.buf());
	}
}

void ValuesTransformer::writeValues(TiffExportInfo& teiData, Tranquilizer& trq)	// used for both float and double
{
	teiData.iSamplesPerPixel = 4;
	teiData.iBitsPerSample = 32;

	Map& inMap = _te.inMap();
	RealBuf buf(inMap->iCols());
	FloatBuf outbuf(inMap->iCols());
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
		inMap->GetLineVal(iRow, buf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;

		for (long col = 0; col < inMap->iCols(); ++col)
			outbuf[col] = floatConv(buf[col]);
		_te.outFile()->Write(inMap->iCols() * 4, outbuf.buf());
	}
}
void ValuesTransformer::writeColorValues(TiffExportInfo& teiData, Tranquilizer& trq)
{
	teiData.iPhotoMI = VPMIRGB;
	teiData.iSamplesPerPixel = 3;
	teiData.iBitsPerSample = 24;

	Map& inMap = _te.inMap();
	for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
	{
		LongBuf iBuf(inMap->iCols());
		inMap->GetLineRaw(iRow, iBuf);
		if ( trq.fUpdate(iRow, inMap->iLines())) 
			return;
		for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
			writeColor(iBuf[iCol]);
	}
}


RangeReal RasterTransformer::rrDetermineValueRange(Map& ValMap)
{
	RangeReal rrMinMax = ValMap->rrPerc1(true);
	if (!rrMinMax.fValid())
		rrMinMax = ValMap->rrMinMax(true);
	if (!rrMinMax.fValid())
		rrMinMax = ValMap->dm()->pdv()->rrMinMax();
	return rrMinMax;
}


//-----------------------------------------------------------------------------------------------
void ImpExp::ExportGeoTiff(const FileName& fnObject, const FileName& fnFile)
{
	try
	{
		(GeoTiffExporter(fnObject, fnFile)).Export(trq);
	}
	catch (AbortedByUser& )
	{
		// just stop, no message
	}
	catch (ErrorObject& err)
	{
		err.Show();
	}
} 

GeoTiffExporter::GeoTiffExporter(const FileName& _fnIn, const FileName& _fnOut) : TiffExporter(_fnIn, _fnOut)
{
	iIFDEntries=18;

	String sPath = getEngine()->getContext()->sIlwDir();
	sPath &= "\\Resources\\Def\\geotiff.def";

	CFile cfGeotiff(sPath.scVal(), CFile::modeRead);
	CArchive ca(&cfGeotiff, CArchive::load);
	m_ecGeoTiff.em = new ElementMap;    // m_ecGeoTiff will delete the element map
	m_ecGeoTiff.em->Serialize(ca);
}

void GeoTiffExporter::Export(Tranquilizer& trq)
{
	trq.SetTitle(SCVTitleExportTiff);
	TiffExporter::Export(trq);

	if (!_inMap->gr()->fGeoRefNone())
	{
		trq.SetText(SCVTextTiffGeoKeys);
		String sAscii;

		Array<double> rVals;
		Array<long> iVals;

		GeoRefCorners *grfc = _inMap->gr()->pgc();
		GeoRefSubMap *grfSub = _inMap->gr()->pgSub();
		GeoRefFactor *grfFac = _inMap->gr()->pgFac();

		if (grfc != NULL)
		{
			RowCol rcSize = grfc->rcSize();
			if (!grfc->fCornersOfCorners) 
			{
				rcSize.Row--;
				rcSize.Col--;
			}
			rVals &= (grfc->crdMax.x - grfc->crdMin.x) / rcSize.Col;  // X pixel size
			rVals &= (grfc->crdMax.y - grfc->crdMin.y) / rcSize.Row;  // Y pixel size
			rVals &= 0.0;
			WriteField(tagModelPixelScale, tiffDouble, rVals);
		}
		else if (grfSub  != NULL )
		{
			RowCol rcSize = grfSub->rcSize();
			rcSize.Row--;// assuming !fCornersOfCorners
			rcSize.Col--;// this is Center Of Corners
			rVals &= (grfSub->cb().cMax.x - grfSub->cb().cMin.x) / rcSize.Col;  // X pixel size
			rVals &= (grfSub->cb().cMax.y - grfSub->cb().cMin.y) / rcSize.Row;  // Y pixel size
			rVals &= 0.0;
			WriteField(tagModelPixelScale, tiffDouble, rVals);
		}
		else if (grfFac  != NULL )
		{
			RowCol rcSize = grfFac->rcSize();
			rcSize.Row--;// assuming !fCornersOfCorners
			rcSize.Col--;// this is Center Of Corners
			rVals &= (grfFac->cb().cMax.x - grfFac->cb().cMin.x) / rcSize.Col;  // X pixel size
			rVals &= (grfFac->cb().cMax.y - grfFac->cb().cMin.y) / rcSize.Row;  // Y pixel size
			rVals &= 0.0;
			WriteField(tagModelPixelScale, tiffDouble, rVals);
		}

		MakeTiepointTable(rVals);
		WriteField(tagModelTiepoint, tiffDouble, rVals);

		MakeKeyDirectory(iVals, sAscii, rVals);
		WriteField(tagGeoKeyDirectory, tiffShort, iVals);

		if (rVals.iSize() > 0)
			WriteField(tagGeoDoubleParams, tiffDouble, rVals);
  
		if (sAscii.length() > 0)
			WriteField(tagGeoAciiParams, tiffASCII, sAscii); 
	}

	// mark next IFD as non-existent
	long iNextIFD = 0;
	_outFile->Write(sizeof(long), &iNextIFD);

	// Update Tiff tag counter to include the GeoTiff tags
	_outFile->Seek(iIFDPosition);
	_outFile->Write(sizeof(iIFDEntries), &iIFDEntries);
	_outFile->SetErase(false);  // No errors, keep the TIFF file
}

unsigned short GeoTiffExporter::iGetUTMProjectedCSCode(String sProj, String sDatum, bool fNorth, long iZone)
{
	String sCode = "PCS ";
	sCode &= sDatum;
	sCode &= " ";
	sCode &= sProj.toUpper();
	sCode &= String(" zone %i", iZone);
	sCode &= (fNorth ? "N" : "S");
	return iGetGeoTiffCode("PCS_INV", sCode);
}

unsigned short GeoTiffExporter::iGetProjectedCSCode(String sProj, String sDatum)
{
	String sCode = "PCS ";
	sCode &= sProj;
	return iGetGeoTiffCode("PCS_INV", sCode);
}

unsigned short GeoTiffExporter::iGetProjectionCode(String sProj)
{
	String sCode="CT ";
	sCode&=sProj;
	return iGetGeoTiffCode("CT_INV", sCode);
}

unsigned short GeoTiffExporter::iGetGCSCode(String sDatum)
{
	String sCode="GCS ";
	sCode&=sDatum;
	return iGetGeoTiffCode("GCS_INV", sCode);
}

unsigned short GeoTiffExporter::iGetGCSECode(String sDatum)
{
	String sCode="GCSE ";
	sCode&=sDatum;
	return iGetGeoTiffCode("GCSE_INV", sCode);
}

unsigned short GeoTiffExporter::iGetDatumCode(String sDatum)
{
	String sCode="Datum ";
	sCode&=sDatum;
	return iGetGeoTiffCode("Datum_INV", sCode);
}

unsigned short GeoTiffExporter::iGetGeoTiffCode(String sSection, String sCode)
{
	String sBuf = (*m_ecGeoTiff.em)(sSection, sCode);
	if (sBuf.length() > 0)
		return atoi(sBuf.scVal());

	return 0;
}

void GeoTiffExporter::CoordSystemData(String& sProj, String& sEllipse, String& sDatum, bool& fNorth, long& iZone, String& sCitation)
{
	CoordSystem cs = _inMap->gr()->cs();
	CoordSystemLatLon* csll = cs->pcsLatLon();
	CoordSystemProjection *csprj = cs->pcsProjection();
	if (0 != csprj)
	{
		Projection& prj = csprj->prj;
		if (!prj.fValid())
			return;
		sProj = prj->sName();
		sEllipse = prj->ell.sName;
		if (sProj != "") sCitation &= String("Projection=%S; ", sProj);
		if (sEllipse != "") sCitation &= String("Ellipsoide=%S; ", sEllipse);
		Datum* molDatum = csprj->datum;
		if (molDatum)
		{
				sDatum = molDatum->sName();
				String sArea = molDatum->sArea;
				if (sDatum.length() > 0) sCitation &= String("Datum=%S; ", sDatum);
				if (sArea.length() > 0)  sCitation &= String("Area=%S; ", sArea);
		}
		sCitation &= String(" ( Projection parameters: ");
		if (prj->ppParam(pvNORTH) != ppNONE) 
		{
			fNorth = prj->iParam(pvNORTH) != 0;
			sCitation &= String("Hemisphere=%s", fNorth ? "North; " : "South; ");
		}
		if (prj->ppParam(pvZONE) != ppNONE)
		{
			iZone = prj->iParam(pvZONE);
			if (iZone != 0) 
				sCitation &= String("Zone=%d; ", iZone);
		}
		double rP1 = prj->rParam(pvX0);
		double rP2 = prj->rParam(pvY0);
		if (rP1 != 0.0 && rP2 != 0.0) 
			sCitation &= String("false easting/northing=%.2f, %.2f; ", rP1, rP2);
		else if (rP1 != 0.0) 
			sCitation &= String("false easting=%.2f; ", rP1);
		else if(rP2 != 0.0) 
			sCitation &= String("false northing=%.2f; ", rP2);

		rP1 = prj->rParam(pvLON0);
		rP2 = prj->rParam(pvLAT0); 
		sCitation &= String("Central Meridian=%.8f; ", rP1);
		sCitation &= String("Central Parallel=%.8f; ", rP2);
		if (prj->ppParam(pvLAT1) != ppNONE)
		{
			rP1 = prj->rParam(pvLAT1);
			rP2 = prj->rParam(pvLAT2);
			sCitation &= String("Standard Parallels %8f, %8f; ", rP1, rP2);
		}
		rP1 = prj->rParam(pvK0);
		if (rP1 != 1)
			sCitation &= String("Scale factor at Origin=%.7f;", rP1);
		sCitation&=" )";      
	}
	else if (0 != csll)
	{
		sProj = "";
		fNorth = 0;
		iZone = 1;
		Ellipsoid ell = csll->ell;
		sCitation = String("Geographic Coordinate System ('Latlons')");
		if (ell.fSpherical())
		{
			sCitation &= String("Ellipsoide is Sphere with Radius = %.3f, ell.a");
		}
		else
		{
			sEllipse = ell.sName;
			sCitation &= String("Ellipsoide=%S; ", sEllipse);
		}

		Datum* molDatum = csll->datum;
		if (0 != molDatum)
		{
			sDatum = molDatum->sName();
			String sArea = molDatum->sArea;
			if (sDatum.length() > 0) sCitation &= String("Datum=%S; ", sDatum);
			if (sArea.length() > 0)  sCitation &= String("Area=%S; ", sArea);
		}
	}
}

void GeoTiffExporter::MakeKeyDirectory(Array<long>& iKeys, String& sCitation, Array<double>& rVals)
{
// Initialize first GeoKey, with special meaning:    
	iKeys &= 1;   // Key directory version
	iKeys &= 1;   // Key sets revision
	iKeys &= 2;   // Key codes revision
	iKeys &= 0;   // #Keys, filled in at end of routine

	GeoRefCorners *grfc = _inMap->gr()->pgc();
	if (grfc) {
		if (grfc->fCornersOfCorners)
			SetGeoKey(keyRasterType, RasterPixelIsArea, iKeys);
		else
			SetGeoKey(keyRasterType, RasterPixelIsPoint, iKeys);
	}
	GeoRefSubMap *grfSub = _inMap->gr()->pgSub();
	if (grfSub) {
		SetGeoKey(keyRasterType, RasterPixelIsPoint, iKeys);//???
	}
	GeoRefFactor *grfFac = _inMap->gr()->pgFac();
	if (grfFac) {
		SetGeoKey(keyRasterType, RasterPixelIsPoint, iKeys);//???
	}
	String sProj, sDatum, sEllipse;
	bool fNorth=true;
	long iCode = 0, iZone = iUNDEF;
	CoordSystem cs = _inMap->gr()->cs();

	if (!cs.fValid()) return;

	String sCSData;
	CoordSystemData(sProj, sEllipse, sDatum, fNorth, iZone, sCSData);
	if (sCSData.length() > 0)
		SetGeoKey(keyGeogCitation, sCSData, iKeys, sCitation);
	SetGeoKey(keyPrimeMeridian, PmGreenwich, iKeys); 

	CoordSystemLatLon* csll = cs->pcsLatLon();
	if (csll != 0)
	{
		SetGeoKey(keyModelType,CSModelTypeGeoGraphic, iKeys);
		iCode = iGetGCSCode(sDatum);
		if ( iCode!=0) SetGeoKey(keyGeographicType, iCode, iKeys);
		else
		{
			iCode=iGetGCSECode(sEllipse);
			if ( iCode!=0) SetGeoKey(keyGeographicType, iCode, iKeys);
			if (iCode==0)
			{
				iCode=iGetDatumCode(sDatum);
				if ( iCode!=0) SetGeoKey(keyGeodeticDatumCode, iCode, iKeys);
			}                
		} 
		iKeys[3] = (iKeys.iSize() - 4) / 4;
		return;
	}

	CoordSystemProjection* cspr = cs->pcsProjection();
	if (cspr != 0)
		SetGeoKey(keyModelType,CSModelTypeProjected, iKeys);

	if ( sProj=="") {
		iKeys[3] = (iKeys.iSize() - 4) / 4;
		return;
	}

	if ( fCIStrEqual(sProj,"utm"))
	{
		iCode = iGetUTMProjectedCSCode(sProj, sDatum, fNorth, iZone);
		if ( iCode != 0)
			SetGeoKey(keyProjectedCSType, iCode, iKeys);
	}
	else
	{
		iCode=iGetProjectedCSCode(sProj, sDatum);
		if ( iCode != 0)
			SetGeoKey(keyProjectedCSType, iCode, iKeys);
		iCode = 0; // projection parameters are also needed
	}
	if ( iCode == 0)
	{  
		iCode = iGetGCSCode(sDatum);
		if ( iCode!=0) SetGeoKey(keyGeographicType, iCode, iKeys);
		else
		{
			iCode=iGetGCSECode(sEllipse);
			if ( iCode!=0) SetGeoKey(keyGeographicType, iCode, iKeys);
			if (iCode==0)
			{
				iCode=iGetDatumCode(sDatum);
				if ( iCode!=0) SetGeoKey(keyGeodeticDatumCode, iCode, iKeys);
			}
		}

		iCode=iGetProjectionCode(sProj);
		if (iCode!=0)  SetGeoKey(keyCoordTransformation, iCode, iKeys);
		CoordSystem cs=_inMap->gr()->cs();
		CoordSystemProjection *csprj = cs->pcsProjection(); // ??
		if (csprj) 
		{
			Projection& prj=csprj->prj;
			double rP1=prj->rParam(pvX0);
			double rP2=prj->rParam(pvY0);
			if (rP1!=0.0 ) SetGeoKey(keyFalseEasting, rP1, rVals, iKeys);
			if (rP2!=0.0 ) SetGeoKey(keyFalseNorthing, rP2, rVals, iKeys);
			rP1=prj->rParam(pvLON0);
			rP2=prj->rParam(pvLAT0);
			if (rP1!=0.0 ) SetGeoKey(keyCentralLong, rP1, rVals, iKeys);
			if (rP2!=0.0 ) SetGeoKey(keyCentralLat , rP2, rVals, iKeys);
			rP1=prj->rParam(pvLAT1);
			rP2=prj->rParam(pvLAT2);
			//if (rP1!=0.0 ) 
			if (prj->ppParam(pvLAT1) != ppNONE)	
				SetGeoKey(keyStdParallel1, rP1, rVals, iKeys);
			//if (rP2!=0.0 ) 
			if (prj->ppParam(pvLAT2) != ppNONE)
				SetGeoKey(keyStdParallel2 ,rP2, rVals, iKeys);
		}

	}
	iKeys[3] = (iKeys.iSize() - 4) / 4;
}

void GeoTiffExporter::SetGeoKey(unsigned short iKey,double rVal, Array<double>& rVals, Array<long>& iKeys)
{
	iKeys &= iKey;
	iKeys &= tagGeoDoubleParams;
	iKeys &= 1;
	iKeys &= rVals.iSize() * sizeof(double);
	rVals &= rVal;
}

void GeoTiffExporter::SetGeoKey(unsigned short iKey, String sAscii, Array<long>& iKeys, String& sAll)
{
	iKeys &= iKey;
	iKeys &= tagGeoAciiParams;
	iKeys &= sAscii.length() + 1;
	iKeys &= sAll.length();  // index into ASCII params, written separately
	sAll &= sAscii;
	sAll &= '|';
}

void GeoTiffExporter::SetGeoKey(unsigned short iKey, unsigned short iCode, Array<long>& iKeys)
{
	iKeys &= iKey;
	iKeys &= 0;
	iKeys &= 1;
	iKeys &= iCode;
}

void GeoTiffExporter::MakeTiepointTable(Array<double>& rTiepoints)
{
	GeoRefCorners *grfc=_inMap->gr()->pgc();
	GeoRefCTP *grftp=_inMap->gr()->pgCTP();
	GeoRefSubMap *grfSub = _inMap->gr()->pgSub();
	GeoRefFactor *grfFac = _inMap->gr()->pgFac();
	if ( grfc)  // only one tiepoint (upperleft pixel) is defined
	{
		Coord c;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		if (grfc->fCornersOfCorners)
			grfc->RowCol2Coord(0, 0, c); // cannot use cConv!
		else
			grfc->RowCol2Coord(0.5, 0.5, c);  // cannot use cConv!
		rTiepoints &= c.x;
		rTiepoints &= c.y;
		rTiepoints &= 0.0;
	}
	if ( grftp)
	{
		for(int i=1; i<=grftp->iNr(); ++i)
		{
			Coord rc = grftp->crdRC(i);
			Coord c  = grftp->crd(i);
			rTiepoints &= rc.y;
			rTiepoints &= rc.x;
			rTiepoints &= 0.0;
			rTiepoints &= c.x;
			rTiepoints &= c.y;
			rTiepoints &= 0.0;
		}
	}
	if ( grfSub)// only one tiepoint (upperleft pixel center) 
	{
		Coord c;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		grfSub->RowCol2Coord(0.5, 0.5, c);  // cannot use cConv!
		rTiepoints &= c.x;
		rTiepoints &= c.y;
		rTiepoints &= 0.0;
	}
	if ( grfFac)// only one tiepoint (upperleft pixel center) 
	{
		Coord c;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		rTiepoints &= 0.0;
		grfFac->RowCol2Coord(0.5, 0.5, c);  // cannot use cConv!
		rTiepoints &= c.x;
		rTiepoints &= c.y;
		rTiepoints &= 0.0;
	}
}

