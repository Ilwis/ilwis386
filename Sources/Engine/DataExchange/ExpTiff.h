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
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\Dat2.h"
#include "Engine\Base\File\File.h"
#include "Engine\Base\File\FILENAME.H"

//-----------Values-------------------------
const short VByteOrder=              18761; // "II" little endian
const short VHgtg=                   42;
const short VBitsPerSample=          8;
const short VCompression=            1;

const short VPMIBitWhite=            0;  // White is zero
const short VPMIBitBlack=            1;  // Black is zero

const short VPMIRGB=                 2;  // RGB Tiff file
const short VPMIPAL=                 3;  // Palette image
const short VSamplesPerPixel=        3;
const long iHeaderLength=            8;

const short COLOR_PAL=				1;
const short COLOR_RPR=				2;
const short COLOR_RAW=				3;
const short COLOR_COLOR=			4;
const short COLOR_ID=				5;

//-----positions-----------------------------
#define PBYTE_ORDER                 0L
#define PHGTG                       2L
#define PIFD_POS                    4L
#define MAX_IFD_ENTRIES            18
#define SIZEOF_TAG                 12
#define IFD_TABLE_EXTRA             6

//-----Tags--------------------------------
const unsigned int tagImageWidth                =256;
const unsigned int tagImageLength               =257;
const unsigned int tagBitsPerSample             =258;
const unsigned int tagCompression               =259;
const unsigned int tagPhotometricInterpretation =262;
const unsigned int tagStripOffset               =273;
const unsigned int tagSamplesPerPixel           =277;
const unsigned int tagRowsPerStrip              =278;
const unsigned int tagStripByteCount            =279;
const unsigned int tagXResolution               =282;
const unsigned int tagYResolution               =283;
const unsigned int tagResolutionUnit            =296;
const unsigned int tagSoftware                  =305;
const unsigned int tagColorMap                  =320;
const unsigned int tagModelPixelScale           =33550L;
const unsigned int tagModelTiepoint             =33922L;
const unsigned int tagGeoKeyDirectory           =34735L;
const unsigned int tagGeoDoubleParams           =34736L;
const unsigned int tagGeoAciiParams             =34737L;

//----Geotiff keys ---------------------------------------
const unsigned int keyModelType                 =1024;
const unsigned int keyRasterType                =1025;
const unsigned int keyGeographicType            =2048;
const unsigned int keyGeogCitation              =2049;
const unsigned int keyGeodeticDatumCode         =2050;
const unsigned int keyPrimeMeridian             =2051;
const unsigned int keyProjectedCSType           =3072;
const unsigned int keyCoordTransformation       =3075;
const unsigned int keyStdParallel1              =3078;
const unsigned int keyStdParallel2              =3079;
const unsigned int keyOriginLong                =3080;
const unsigned int keyOriginLat                 =3081;
const unsigned int keyFalseEasting              =3082;
const unsigned int keyFalseNorthing             =3083;
const unsigned int keyCentralLong               =3088;
const unsigned int keyCentralLat                =3089;
const unsigned int keyScaleAtNatOrigin          =3092;
const unsigned int keyScaleAtCenter             =3093;


//----Assorted types--------------------------------------
const unsigned int CSModelTypeProjected         =1;
const unsigned int CSModelTypeGeoGraphic        =2;
const unsigned int CSModelTypeGeocentric        =3;
const unsigned int RasterPixelIsArea            =1;
const unsigned int RasterPixelIsPoint           =2;
const unsigned int PmGreenwich                  =8901;

//-----Types---------------------------
typedef enum tiffType { 
                        tiffByte=1, 
                        tiffASCII, 
                        tiffShort, 
                        tiffLong, 
                        tiffRational,
                        tiffSignedByte, 
                        tiffUndefined, 
                        tiffSignedShort, 
                        tiffSignedLong,
                        tiffSignedRational, 
                        tiffFloat, 
                        tiffDouble
                      };

#pragma pack(2)
struct TiffExportInfo
{
  short iBitsPerSample;   // can be 1(bit), 8(byte), or 24(colors)
  short iSamplesPerPixel;
  short iPhotoMI;         // Photometric Interpretation
  unsigned short aiColorMap[256 * 3]; // Color LUT for 8 bit (only used for BIT and BOOL)

  short colorType;	// colors coming from LUT, RPR, RAW
};
#pragma pack()

class TiffExporter;

class RasterTransformer {

public:
	RasterTransformer(TiffExporter& te) : _te(te) { }
	virtual TiffExportInfo transform(Tranquilizer&) = 0;

protected:
	TiffExporter& _te;
	RangeReal rrDetermineValueRange(Map& ValMap);
};

class ColorTransformer : public RasterTransformer {
protected:
	void setBoolColormap(TiffExportInfo& teiData);
	void setBitColormap(TiffExportInfo& teiData);

	void writeValues(Tranquilizer& trq);
	void writeAsColor(Tranquilizer& trq);
	void writeAsColorRaw(Tranquilizer& trq);
	void writeAsColorID(Tranquilizer& trq);
	void writeAsColorPalette(TiffExportInfo& teiData, Tranquilizer& trq);

public:
	ColorTransformer(TiffExporter& te) : RasterTransformer(te) {}
	TiffExportInfo transform(Tranquilizer&);
};

class ValuesTransformer : public RasterTransformer {
protected:
	void writeByteValues(TiffExportInfo& teiData, Tranquilizer& trq);
	void writeShortValues(TiffExportInfo& teiData, Tranquilizer& trq);
	void writeLongValues(TiffExportInfo& teiData, Tranquilizer& trq);
	void writeValues(TiffExportInfo& teiData, Tranquilizer& trq);	// used for both float and double
	void writeColorValues(TiffExportInfo& teiData, Tranquilizer& trq);

public:
	ValuesTransformer(TiffExporter& te) : RasterTransformer(te) {}
	TiffExportInfo transform(Tranquilizer&);
};

class TiffExporter
{
public:
    TiffExporter(const FileName& _fnIn, const FileName& _fnOut, bool asColors = false);
    ~TiffExporter();
    virtual void Export(Tranquilizer& trq);

	Map&	inMap() { return _inMap; }
	File*	outFile() { return _outFile; }

protected:
    FileName	fnIn;
    FileName	fnOut;
    Map			_inMap;
    File*		_outFile;
	bool		_asColors;
    long		iIFDPosition;
	RasterTransformer*	transformer;

    void WriteIFD(const TiffExportInfo&);

    void ExportImagePixels(TiffExportInfo&, Tranquilizer& trq);   // Images
    void ExportBoolPixels(TiffExportInfo&, Tranquilizer& trq);    // Bool maps
    void ExportValuePixels(TiffExportInfo&, Tranquilizer& trq);   // other value maps
    void ExportPicturePixels(TiffExportInfo&, Tranquilizer& trq); // Pictures
    void ExportColorPixels(TiffExportInfo&, Tranquilizer& trq);   // Color maps
    void ExportSortPixels(TiffExportInfo&, Tranquilizer& trq);    // Class maps
    void ExportIDPixels(TiffExportInfo&, Tranquilizer& trq);      // ID maps
    void ExportBitPixels(TiffExportInfo&, Tranquilizer& trq);     // bit maps

	// functions to export values to colors
    void WriteAsColorPalette(TiffExportInfo& teiData, Tranquilizer&);
			void WriteAsColorRaw(const Representation& rpr, Tranquilizer& trq);
    void WriteAsColor(Tranquilizer&);
			void WriteAsColorID(long iNr, Tranquilizer&);
    void WriteAsColorValues(Tranquilizer&);

    void WriteColor(const Color& color);

	// functions to export values to values
	void WriteAsValues8iValues(Tranquilizer& trq);
	void WriteAsValues16iValues(Tranquilizer& trq);
	void WriteAsValues32iValues(Tranquilizer& trq);
	void WriteAsValues32fValues(Tranquilizer& trq);	// used for both float and double

	void ExportImagePixelsV(TiffExportInfo& teiData, Tranquilizer& trq);

    RangeReal rrDetermineValueRange(Map& ValMap);
   
	// Functions to write tiff tags
    void WriteField( unsigned short iTag, tiffType iType, long iCount, long iV);
    void WriteField( unsigned short tTag, tiffType tType, long iCount, long iNum, long iDenum);
    void WriteField( unsigned short tTag, tiffType tType, Array<long>& aiVals);
    void WriteField( unsigned short tTag, tiffType tType, Array<double>& arVals);
    void WriteField( unsigned short tTag, tiffType tType, String sVersion);

    short iIFDEntries;        
};

class GeoTiffExporter : public TiffExporter
{
public:
    GeoTiffExporter(const FileName& _fnIn, const FileName& _fnOut);

    virtual void Export(Tranquilizer& trq);

protected:
	ElementContainer m_ecGeoTiff;

    void MakeTiepointTable(Array<double>& rTiepoints);
    void MakeKeyDirectory(Array<long>& iKeys, String& sCitation, Array<double>& rVals);

    void CoordSystemData(String& sProj, String& sEllips, String& sDatum, bool& fNorth, long& iZone, String& sCitation);

    unsigned short iGetGeoTiffCode(String sSection, String sCode);
    unsigned short iGetUTMProjectedCSCode(String sProj, String sDatum, bool fNorth, long iZone);
    unsigned short iGetProjectedCSCode(String sProj, String sDatum);
    unsigned short iGetDatumCode(String sDatum);
    unsigned short iGetProjectionCode(String sProj);
    unsigned short iGetGCSCode(String sDatum);
    unsigned short iGetGCSECode(String sDatum);

    void SetGeoKey(unsigned short iKey, unsigned short iCode, Array<long>& iKeys);
    void SetGeoKey(unsigned short iKey, String sAscii, Array<long>& iKeys, String& sAll);
    void SetGeoKey(unsigned short iKey,double rVal,  Array<double>& rVals, Array<long>& iKeys);
};


