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
/* $Log: /ILWIS 3.0/Import_Export/imptif.h $
 * 
 * 9     27-10-04 15:07 Willem
 * Added support for multi-page geo-tiff files. If a tiff file contains
 * more than one image the images are all imported into an
 * ObjectCollection.
 * The images can have:
 *     - the differing size
 *     - different bits per pixel
 *     - different georeference/ccordinate system
 * 
 * 8     29-08-00 11:28 Hendrikse
 * added several members needed for the import of Geographic Csy's (latlon
 * variants) without projections (incl datum with ellipsoid and ellipsoid
 * only)
 * 
 * 7     15/06/00 15:12 Willem
 * GetPrivateProfileString has been replaced. The *.def files are now read
 * through an ElementContainer. This done to circumvent an (undocumented)
 * limitation in GetPrivateProfileString (Windows 98): it is not able to
 * read all key from section with a large number of key/value pairs. (in
 * case of geotiff.def, section "pcs_inv" the last line in this section
 * read was 195
 * 
 * 6     15/06/00 12:10 Willem
 * Completely rewritten. Addtional functionality:
 * - UTM coordsystems are recognized and the projection is set in CSY
 * - 16 bit IKONOS images are now imported
 * 
 * 5     9/05/00 9:24 Willem
 * Added support for 16 bit gray import; does not yet work properly
 * 
 * 4     10/02/00 17:16 Willem
 * Comments (revisited)
 * 
 * 3     10/02/00 17:15 Willem
 * Comments //
 * 
 * 2     10/02/00 16:15 Willem
 * - Added "#pragma pack" to properly align datastructures
 * - removed bugs related to high endian format data swapping.
 * - Added code to erase newly created IlwisObjects in case the user
 * aborts the import
 */
// Revision 1.1  1998/09/17 09:12:27  Wim
// 22beta2
//
// Revision 1.5  1997/12/12 20:34:38  Willem
// Now correctly interpretes zeroIsBlack and zeroIsWhite flags (PhotoMetricInterpretation)
// if zeroIsWhite image-pixels are inverted first.
//
// Revision 1.4  1997/12/12 14:44:56  Dick
// Log message has a change
//
// Revision 1.3  1997/09/18 17:23:08  Willem
// Status variables were reset too often. They are implemented as member variables.
//
// Revision 1.2  1997/09/08 14:56:28  Willem
// Solved a problem for high-endian Tiff with more than 256 colours: A long address
// was loaded into an int. This address, an offset into the file, became truncated.
// All possible truncations are now removed.
//
/* imptiff.h
   Import Ilwis Segment List
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WK   29 May 98    4:04 pm
*/

#ifndef IMPORTTIF_DEF
#define IMPORTTIF_DEF

#include "Engine\DataExchange\Convloc.h"
#include "Engine\Representation\Rprclass.h"

class TiffElementMap : public map<string, map<string, String> >
{
public:
	String getValue(const string& sSection, const string& sKey);
	String getValueEx(const string& sSection, string& sKeyOrSubstring);
	void Serialize(CArchive&);
};

#define CSTypeProjected  1
#define CSTypeGeographic  2
#define CSTypeGeocentric  3

unsigned int undef( const unsigned int& ) { return 0x8000; }

#pragma pack(1)
struct MachineTIFF_HeaderStruct
{
  unsigned char Order[2];
  unsigned short Version;
  unsigned long iOffset;
};

struct MachineTiffTagField    // Image File Directory Entry
{         
	unsigned short iTag;        // IFD field identifier
	unsigned short iType;       // type of the values
	unsigned long iCount;       // number of values
	unsigned long iOffset;      // file offset to get the values (offset is even)
                              // offset stores value directly if it fits in 4 bytes or less
};

struct ModelPixelScales {
  double rX;
  double rY;
  double rZ;
};                  
struct ModelTiePoint {
  double rCol;
  double rRow;
  double rH; 
  double rX;
  double rY;
  double rZ;
}; 

struct MachineGeoKeyDir
{
	unsigned short iKeyID;
	unsigned short iTiffTagLoc;
	unsigned short iCount;
	unsigned short iValue;
};

class SwappableField
{
public:
	SwappableField(bool fSwap = false);   // default Intel byte ordering
	virtual bool ReadFrom(File& filTiff) = 0;
	virtual void SwapField() = 0;

protected:
	bool m_fSwap;
};

class TiffTagField : public SwappableField
{
public:
	TiffTagField(bool fSwap = false);
	virtual bool ReadFrom(File& filTiff);
	virtual void SwapField();
	
	unsigned short iTag();        // IFD field identifier
	unsigned short iType();       // type of the values
	unsigned long iCount();       // number of values
	unsigned long iOffset();      // file offset to get the values (offset is even)

private:
	enum st
	{
		tagBYTE = 1, tagASCII, tagSHORT, tagLONG, tagRATIONAL, 
		tagSBYTE, tagUNDEFINED, tagSSHORT, tagSLONG, tagSRATIONAL,
		tagFLOAT, tagDOUBLE
	};
	MachineTiffTagField m_mtf;
};

class GeoKeyDir : public SwappableField
{
public:
	GeoKeyDir(bool fSwap = false);
	virtual bool ReadFrom(File& filTiff);
	virtual void SwapField();

	unsigned short iKeyID();
	unsigned short iTiffTagLoc();
	unsigned short iCount();
	unsigned short iValue();

private:
	MachineGeoKeyDir m_mgk;
};

class TIFF_HeaderStruct
{
public:
	TIFF_HeaderStruct(File& filTiff);

	String sID();
	short iVersion();
	unsigned long iOffset();
	bool fIsTiff();

private:
	MachineTIFF_HeaderStruct m_mth;
};

class TiffImageInfo
{
public:
	TiffImageInfo();
	void Reset();

	long     iNrCols;
	long     iNrLines;
	short    iSamplesPerPixel;
	long     iBitsPerSample;
	long     Palette;

	long     iNrStrips;
	long     iStripOffset;
	long     iRowsPerStrip;
	unsigned long iStripByteCounts;

	short    iCompression;
	short    PhotogrInterp;

	bool     fGeoTiff;

	long     iGeoDoubleParamsAdr;
	long     iGeoAsciiParamsAdr;
};

class TiffImporter;  // forward

class GeoTiffInfo
{
public:
	GeoTiffInfo();
	void SetOwner(TiffImporter* ITC);
	~GeoTiffInfo();

	void     ReadGeoTiffInfo();

	CoordSystem csy();
	GeoRef grf(CoordSystem& csy, RowCol rc);

	long     iGeoKeyDirAdr;

	short    iNrTiePoints;
	long     iModTiePointsAdr;
	long     iModPixelScalesAdr;

	enum     DefType { defGEOTIFF, defDATUM, defELLIPSOID };

protected:
	void     InitCoordTrans();
	void     InitProjParam();

	String	    sReadProfileInfo(const DefType dt, const String& sSection, String& sKey);
	bool		fFindUTMInfo(String& sDatum, long& iUTMZone, bool& fNorthHemis);
	bool		fFindDatumInfo(String& sDatum);
	bool		fFindEllipsoidInfo(String& sEllipsoid);

private:
	TiffImporter* m_ITC;
	TiffElementMap m_temDef;
	TiffElementMap m_temDatum;
	TiffElementMap m_temEllipsoid;

	String   m_sPathGeoDef;  // path of geotiff.def file
	String   m_sPathDatDef;  // path of datum.def file
	String   m_sPathEllDef;  // path of ellips.def file

	bool     m_fCornersOfCorners;
	ModelTiePoint*   m_amtp;
	ModelPixelScales m_mpsScales;

	String sCSProj(int iCSProjID);
	String sCSGeog(int iCSGeogID);
	String sCSEGeog(int iCSEGeogID);
	void     ExtendDescription(GeoKeyDir& gkd, String& sDescr, bool fIlwisGenerated);
	String   sCoordTransMethod(int iMethod);
	ProjectionParamValue ppvParamType(int iKey);

	int      m_iCoordSystType;
	int      m_iGeographicType;

	unsigned short m_iProjCSTypeGeoKey;
	unsigned short m_iGeographicCSGeoKey;
	unsigned short m_iProjCoordTransGeoKey;
	String   m_sGTCitation;
	String   m_sGeogCitation;
	String   m_sDescr;
	String   m_sProjCS;
	String   m_sGeogCS;
	String   m_sGeogCSE;

	map<int, String> m_mpCoordTransMethods;
	map<int, ProjectionParamValue> m_mpProjParams;
};

// TiffImporter
class TiffImporter 
{
public:
	TiffImporter(Tranquilizer&, File&, const FileName& fnObject);

	void Import(); 

	void InitArray();

	bool   fReadPalette(int iNrOfColors, Array<Color>&);

	String sAsciiParams(GeoKeyDir& gkd);
	double rDoubleParams(GeoKeyDir& gkd);
	void GetPixelScales(long iModPixelScalesAdr, ModelPixelScales& mpsScales);
	void GetTiePoints(long iModTiePointsAdr, long iNrTiePoints, ModelTiePoint* amtp);

	long iReadTiffIntro();
	long iReadTiffImageInfo();
	TiffImageInfo& tii();
	GeoTiffInfo& gti();

	Domain dmDetermine();

	bool fSwap;

	FileName fnObjectName(const String& sExt);
	FileName fnBaseObject(const String& sExt = "");
	File& filTIFF;

private:
	int ReadStrip(Map& mp, long iLine);

	FileName m_fnObject;
	bool m_fMultipleImages;
	long m_iNrImages;

	byte   bRead();
	short  shRead();
	long   iRead();
	double rRead();

	ArrayLarge<int> StrArrBef;
	ArrayLarge<unsigned char> StrArrCod;
	ArrayLarge<int> StrArrLen;
	ByteBuf bufBlock;

	unsigned long m_iNrBytes;
	long m_bBlackColor;
	long m_bWhiteColor;
	byte m_bTiffVal;

	TiffImageInfo m_tii;
	GeoTiffInfo   m_gti;

private:
	void ReadPackBitsLine(ByteBuf&);
	void ReadLZWLine(ByteBuf&);
	void UnPackLine(ByteBuf&);
	bool fReadHuffLine(ByteBuf& bb);

	const int tiffCODE_CLEAR, tiffCODE_END;

	int m_iBlockNr, m_BlockLen, m_CodeMax, m_CodeOld, m_MaxCode, m_CodeSize;
	int m_iBit;
	unsigned int m_iMapLine;
	unsigned long m_iCurr;

	Tranquilizer& trq;
};
#pragma pack()

#endif // IMPORTTIF_DEF


