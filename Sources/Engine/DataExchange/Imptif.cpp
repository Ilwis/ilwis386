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
/*	$Log: /ILWIS 3.0/Import_Export/Imptif.cpp $
 * 
 * 30    13-01-05 16:46 Retsios
 * Initialized a boolean, otherwise the release version reports "Not a
 * TIFF file" for valid TIFF files.
 * 
 * 29    27-10-04 15:07 Willem
 * Added support for multi-page geo-tiff files. If a tiff file contains
 * more than one image the images are all imported into an
 * ObjectCollection.
 * The images can have:
 *     - the differing size
 *     - different bits per pixel
 *     - different georeference/ccordinate system
 * 
 * 28    28-09-04 14:25 Hendrikse
 * Build in detection of double precision tiepoints in the GeoTiff
 * information; if so double precision tiepoints are stored into ILWIS
 * georef
 * 
 * 27    15-09-03 11:47 Hendrikse
 * dynamic_cast 's are removed since they are now redundant after giving
 * the topmost Datum-class all essential members needed (change of
 * 11-9-03)
 * 
 * 26    27-08-03 17:05 Hendrikse
 * Uses now MolodenskyDatum derived from Datum (see Datum.h)
 * 
 * 25    11/08/01 17:35 Hendrikse
 * added in import() a check on validity of gti().grf(csy, rc) and
 * gti().csy() before their use.
 * add grf(rc) in  GeoTiffInfo::grf(CoordSystem& csy, RowCol rc) to create
 * at least a GeoRefNone in case the geoTiff fill doesn't contain a valid
 * georef
 * 
 * 24    6/14/01 9:26 Willem
 * Added pragma to hide compiler warning 4503
 * 
 * 23    16-03-01 3:36p Martin
 * swapping was not correctly handled when double parms where read.
 * 
 * 22    12/03/01 15:14 Willem
 * Packbits import failed because of wrong cast result (int) instead of
 * (short)
 * 
 * 21    12/02/01 15:46 Willem
 * The background map of a grTiepoints now includes the extension
 * 
 * 20    4/01/01 16:33 Willem
 * Buffers are now (finally) prepared with enough space for all bytes in a
 * line
 * 
 * 19    31/10/00 14:30 Willem
 * A buffer was overwritten in case of bits per pixel less than 8.
 * The buffers are now created large enough
 * 
 * 18    12-10-00 14:53 Hendrikse
 * added geocode nrs for false E, false N and scale at origin 
 * 
 * 17    25-09-00 16:51 Hendrikse
 * gkd.iValue changed inside the for-loop in
 * CoordSystem GeoTiffInfo::csy()
 * I made it fixed using an extra variable iGKDvalue
 * 
 * 16    30-08-00 19:33 Hendrikse
 * added several Datum and Ellipsoid checks
 * 
 * 15    29-08-00 11:26 Hendrikse
 * added ellipsoi search paths:
 * m_sPathEllDef = sPath;
 * m_sPathEllDef &= "\\ellips.def";
 * to enable setting of ellipsoids alone (without Datum)
 * 
 * 14    22-08-00 17:18 Hendrikse
 * made sArea definition in
 * GeoTiffInfo::csy()
 * 
 * 13    18/08/00 14:01 Willem
 * The ReadStrip functions now uses buffers that are large enough
 * 
 * 12    4/08/00 11:49 Willem
 * Tiff file with more than 24 bits per pixels are not supported: an error
 * message will appear to communicate this to the user
 * 
 * 11    15/06/00 15:12 Willem
 * GetPrivateProfileString has been replaced. The *.def files are now read
 * through an ElementContainer. This done to circumvent an (undocumented)
 * limitation in GetPrivateProfileString (Windows 98): it is not able to
 * read all key from section with a large number of key/value pairs. (in
 * case of geotiff.def, section "pcs_inv" the last line in this section
 * read was 195
 * 
 * 10    15/06/00 12:10 Willem
 * Completely rewritten. Addtional functionality:
 * - UTM coordsystems are recognized and the projection is set in CSY
 * - 16 bit IKONOS images are now imported
	import .TIF format
	by Li Fei, Nov. 94
	modified by Li Fei, Sep 95
	ILWIS Department ITC
	Last change:  WK   29 May 98    4:08 pm
*/

/*
   Revision history:
   1995:     First version
   02-04-97: Fixed a problem with multiple bytes per pixel tiffs: if the tag 
             "BitsPerSample" has a "Count" field greater than zero, this is 
             now taken into account properly.
   26-06-97: Added GeoTiff support. For one or two tiepoints a GeoRefCorners is created,
             for three or more tiepoints a genuine GeoRefTiepoints is created. Projection,
             datum, ellipsoid information is only stored as description in a CoordSystem.
   04-07-97: Added a fix to GeoTiff: the projection parameters were not checked properly,
             and as a result were never added (projection range now checked: 20000-32760)
*/

#pragma warning( disable : 4786 )
#pragma warning(disable : 4503)

#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\Grctppla.h"
#include "Engine\SpatialReference\prj.h   "
#include "Engine\SpatialReference\DATUM.H"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\File\ElementMap.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include <map>
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\imptif.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

#define EPS10 1.e-10

// SwappableField
SwappableField::SwappableField(bool fSwap)
{
	m_fSwap = fSwap;
}

inline void Swap(unsigned short &iVal) 
{
	iVal = (iVal << 8) | (iVal >> 8);
}

inline void SwapLong(unsigned long &iVal)    // create intel type long
{
	iVal = ((iVal >> 24) & 0x000000ffL) |   // most significant Motorola
	       ((iVal >>  8) & 0x0000ff00L) |   //
	       ((iVal <<  8) & 0x00ff0000L) |   //
	       ((iVal << 24) & 0xff000000L);    // least significant Motorola
}

void SwapDouble(double &prVal)
{
	double &rVal = prVal;
	char *pc = (char *)&rVal;
	char c;
	for (short i = 0; i <= 3 ; i++)
	{
		c = pc[i];
		pc[i] = pc[7 - i];
		pc[7 - i] = c;
	}
}

// TiffTagField
TiffTagField::TiffTagField(bool fSwap)
	: SwappableField(fSwap)
{
}

void TiffTagField::SwapField()
{
	if (m_fSwap) Swap(m_mtf.iTag);
	if (m_fSwap) Swap(m_mtf.iType);
	if (m_fSwap) SwapLong(m_mtf.iCount);
	switch (m_mtf.iType) 
	{
		case tagASCII :
			if (m_mtf.iCount > 4)
				if (m_fSwap) SwapLong(m_mtf.iOffset);
			break;
		case tagSHORT :
			if (m_mtf.iCount > 2)
			{
				if (m_fSwap) SwapLong(m_mtf.iOffset);
			}
			else 
			{
				unsigned short iVal = m_mtf.iOffset;
				if (m_fSwap) Swap(iVal);
				m_mtf.iOffset = (unsigned long) iVal;
			}
			break;
		case tagLONG :
		case tagRATIONAL :
		case tagSRATIONAL:
		case tagDOUBLE:
			if (m_fSwap) SwapLong(m_mtf.iOffset);  // swap the addresses
			break;
		default :
			break;
	}
}

bool TiffTagField::ReadFrom(File& filTiff)
{
	size_t iRet = filTiff.Read(sizeof m_mtf, &m_mtf);
	SwapField();
	return iRet == sizeof m_mtf;
}

unsigned short TiffTagField::iTag()
{
	return m_mtf.iTag;
}

unsigned short TiffTagField::iType()
{
	return m_mtf.iType;
}

unsigned long TiffTagField::iCount()
{
	return m_mtf.iCount;
}

unsigned long TiffTagField::iOffset()
{
	return m_mtf.iOffset;
}

// GeoKeyDir
GeoKeyDir::GeoKeyDir(bool fSwap)
	: SwappableField(fSwap)
{
}

void GeoKeyDir::SwapField()
{
  if (m_fSwap) Swap(m_mgk.iKeyID);
  if (m_fSwap) Swap(m_mgk.iTiffTagLoc);
  if (m_fSwap) Swap(m_mgk.iCount);
  if (m_fSwap) Swap(m_mgk.iValue);
}

bool GeoKeyDir::ReadFrom(File& filTiff)
{
	size_t iRet = filTiff.Read(sizeof m_mgk, &m_mgk);
	SwapField();
	return iRet == sizeof m_mgk;
}

unsigned short GeoKeyDir::iKeyID()
{
	return m_mgk.iKeyID;
}

unsigned short GeoKeyDir::iTiffTagLoc()
{
	return m_mgk.iTiffTagLoc;
}

unsigned short GeoKeyDir::iCount()
{
	return m_mgk.iCount;
}

unsigned short GeoKeyDir::iValue()
{
	return m_mgk.iValue;
}

// TIFF_HeaderStruct
TIFF_HeaderStruct::TIFF_HeaderStruct(File& filTiff)
{
	filTiff.Seek(0);
	filTiff.Read(sizeof m_mth, &m_mth);
}

String TIFF_HeaderStruct::sID()
{
	String sID;
	sID &= m_mth.Order[0];
	sID &= m_mth.Order[1];
	return sID;
}

short TIFF_HeaderStruct::iVersion()
{
	unsigned short iVal = m_mth.Version;
	if (sID() == "MM")
		Swap(iVal);
	return iVal;
}

unsigned long TIFF_HeaderStruct::iOffset()
{
	unsigned long iVal = m_mth.iOffset;
	if (sID() == "MM")
		SwapLong(iVal);
	return iVal;
}

bool TIFF_HeaderStruct::fIsTiff()
{
	return (sID() == "MM" || sID() == "II");
}

TiffImageInfo::TiffImageInfo()
{
	Reset();
}

void TiffImageInfo::Reset()
{
	iNrLines = 0;
	iNrCols = 0;

	iBitsPerSample = 1;
	iSamplesPerPixel = 1;
	PhotogrInterp = 0;     //  0,1: normal; 2: RGB; 3: Palette
	Palette = 0;

	iCompression = 1;      //  1: No compression; 2: bilevel; 5: LZW; 32773: PackBits

	iNrStrips = 1;
	iStripOffset = 0;
	iRowsPerStrip = LONG_MAX;

	fGeoTiff = false;

	iGeoDoubleParamsAdr = 0;
	iGeoAsciiParamsAdr = 0;
}

GeoTiffInfo::GeoTiffInfo()
{
	iGeoKeyDirAdr = 0;

	iNrTiePoints = 0;
	iModTiePointsAdr = 0;
	iModPixelScalesAdr = 0;

	m_iCoordSystType = 0;
	m_iProjCoordTransGeoKey = 0;

	m_amtp = 0;
	String sPath = getEngine()->getContext()->sIlwDir();
	m_sPathGeoDef = sPath;
	m_sPathGeoDef &= "\\Resources\\Def\\geotiff.def";
	m_sPathDatDef = sPath;
	m_sPathDatDef &= "\\Resources\\Def\\datum.def";
	m_sPathEllDef = sPath;
	m_sPathEllDef &= "\\Resources\\Def\\ellips.def";

	InitCoordTrans();
	InitProjParam();
}

GeoTiffInfo::~GeoTiffInfo()
{
	if (m_amtp)
		delete [] m_amtp;
}

void GeoTiffInfo::SetOwner(TiffImporter* ITC)
{
	m_ITC = ITC;

	CFile cfGeotiff(m_sPathGeoDef.scVal(), CFile::modeRead);
	CArchive ca(&cfGeotiff, CArchive::load);
	m_ecDef.em = new ElementMap;    // m_ecDef will delete the element map
	m_ecDef.em->Serialize(ca);

	CFile cfDatum(m_sPathDatDef.scVal(), CFile::modeRead);
	CArchive ca2(&cfDatum, CArchive::load);
	m_ecDatum.em = new ElementMap;  // m_ecDatum will delete the element map
	m_ecDatum.em->Serialize(ca2);

	CFile cfEllipsoid(m_sPathEllDef.scVal(), CFile::modeRead);
	CArchive ca3(&cfEllipsoid, CArchive::load);
	m_ecEllipsoid.em = new ElementMap;  // m_ecEllipsoid will delete the element map
	m_ecEllipsoid.em->Serialize(ca3);
}

void GeoTiffInfo::ReadGeoTiffInfo()
{
	// Model tie points are already specified in the regular TIFF keys
	if (iModPixelScalesAdr != 0)
		m_ITC->GetPixelScales(iModPixelScalesAdr, m_mpsScales);

	if (iModTiePointsAdr != 0)
	{
		m_amtp = new ModelTiePoint[iNrTiePoints];
		m_ITC->GetTiePoints(iModTiePointsAdr, iNrTiePoints, m_amtp);
	}

	if (iGeoKeyDirAdr == 0)
		return;

	// Now read the GeoTiff keys
	m_iProjCSTypeGeoKey = 0;
	m_iGeographicCSGeoKey= 0;
	unsigned short iGeogPrimeMeridian = 0;  //// read from file, but not used
	double rGeogLinearUnitCode = 0; //// read from file, but not used
	unsigned short iGeogAngularUnitCode = 0; //// read from file, but not used

	GeoKeyDir gkd(m_ITC->fSwap);

	m_ITC->filTIFF.Seek(iGeoKeyDirAdr);
	gkd.ReadFrom(m_ITC->filTIFF);
	int iNrGeoKeys = gkd.iValue();
	bool fIlwisGenerated = false;
	for (int i = 0; i < iNrGeoKeys; i++) 
	{
		gkd.ReadFrom(m_ITC->filTIFF);

		switch (gkd.iKeyID()) 
		{
			case 1024 : m_iCoordSystType = gkd.iValue(); break;
			case 1025 : m_fCornersOfCorners = (gkd.iValue() == 1); break; // IPixelArea = 1 (center of corner) or 0 (corner of corner)
			case 1026 : m_sGTCitation = m_ITC->sAsciiParams(gkd); break;
			case 2048 : m_iGeographicCSGeoKey = gkd.iValue(); 
						if ((m_iGeographicCSGeoKey >= 4000) && (m_iGeographicCSGeoKey <= 4199))
							m_sGeogCSE = sCSEGeog(m_iGeographicCSGeoKey);
						else
							m_sGeogCS = sCSGeog(m_iGeographicCSGeoKey);
						ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;
			case 2049 : m_sGeogCitation = m_ITC->sAsciiParams(gkd);
						fIlwisGenerated = m_sGeogCitation.sSub(0,11) == "Projection=";
						break;
			case 2050 : ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;                
			case 2051:  iGeogPrimeMeridian = gkd.iValue(); break;
			case 2052:  rGeogLinearUnitCode = m_ITC->rDoubleParams(gkd); break;
			case 2054:  iGeogAngularUnitCode = gkd.iValue(); break;
			case 2056:  ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;                
			case 3072:  m_iProjCSTypeGeoKey = gkd.iValue();
						m_sProjCS = sCSProj(m_iProjCSTypeGeoKey);
						ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;
			case 3074:  ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;                           
			case 3075:  m_iProjCoordTransGeoKey = gkd.iValue();
						ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;
			case 4096:  ExtendDescription(gkd, m_sDescr, fIlwisGenerated);
						break;                            
		}
		
	}
}

bool GeoTiffInfo::fFindDatumInfo(String& sDatum)
{
	if (fCIStrEqual(m_sProjCS.sHead(" \t"), "PCS"))
		sDatum = m_sProjCS.sTail(" \t");
	else if (!fCIStrEqual(m_sGeogCS.sHead(" \t"), "GCSE"))
		sDatum = m_sGeogCS.sTail(" \t");
	else
		return false;

	String sDatEll = sReadProfileInfo(defDATUM, String("Datums"), sDatum);
	if (sDatEll.length() == 0)
		sDatum = String();

	return sDatum.length() > 0;
}

bool GeoTiffInfo::fFindEllipsoidInfo(String& sEllipsoid)
{
	if (!fCIStrEqual(m_sGeogCSE.sHead(" \t"), "GCSE"))
		return false;

	sEllipsoid = m_sGeogCSE.sTail(" \t");

	String sEll = sReadProfileInfo(defELLIPSOID, String("Ellipsoids"), sEllipsoid);
	if (sEll.length() == 0)
		sEllipsoid = String();

	return sEllipsoid.length() > 0;
}

// fFindUTMInfo tries to find out if a UTM coordsystem is meant
// The string parsed for UTM looks like:
//   PCS <datum name> UTM zone <zone number><Hemisphere>
// For example:
//   PCS Provisional South American 1956 UTM zone 19S
bool GeoTiffInfo::fFindUTMInfo(String& sDatum, long& iUTMZone, bool& fNorthHemis)
{
	bool fUTM = false;
	Array<String> as;
	Split(m_sProjCS, as);
	if (as.size() == 0)
		return fUTM;

	int i = 0;
	while (i < as.size() && !fCIStrEqual(as[i++], "UTM"));

	if (i < as.size() && fCIStrEqual(as[i++], "zone"))
	{
		sDatum = String();
		// reconstruct datum name
		for (int j = 1; j < i - 2; j++)
		{
			sDatum &= as[j];
			if (j < i - 3)
				sDatum &= " ";
		}
		// find Ellipsoid for the found datum
		String sDatEll = sReadProfileInfo(defDATUM, String("Datums"), sDatum);
		if (sDatEll.length() == 0)
			sDatum = String();

		String sZone = as[i];
		char c = toupper(sZone[sZone.length() - 1]);
		fNorthHemis = c == 'N';
		fUTM = fNorthHemis || (c == 'S');

		iUTMZone = 0;
		int iPos = 0;
		while (isdigit(sZone[iPos]))
			iUTMZone = iUTMZone * 10 + (sZone[iPos++] - 48);
	}

	return fUTM;
}

CoordSystem GeoTiffInfo::csy()
{
	CoordSystem csyLoc;
	FileName fnCsy = m_ITC->fnObjectName(".csy");
	String sDatum;
	String sEllipsoid;
	if (m_iCoordSystType == CSTypeGeographic ) 
	{
		csyLoc.SetPointer(new CoordSystemLatLon(fnCsy, 1));
		if (m_sGTCitation.length() > 0)
			csyLoc->sDescription = String("%S ; ", m_sGTCitation);
		csyLoc->sDescription &= m_sDescr;

		CoordSystemLatLon* csll = csyLoc->pcsLatLon();
		if (csll == 0)
			return csyLoc;  // should not happen; better use exception

		if ((m_iGeographicCSGeoKey >= 4000) && (m_iGeographicCSGeoKey <= 4199))
		{
			if (fFindEllipsoidInfo(sEllipsoid))
			{
				csll->ell = Ellipsoid(sEllipsoid);
			}
		}
		if ((m_iGeographicCSGeoKey >= 4200) && (m_iGeographicCSGeoKey <= 4999))
		{
			if (fFindDatumInfo(sDatum))
			{
				csll->datum = new MolodenskyDatum(sDatum, "");
				csll->ell = csll->datum->ell; 
			}
		}

	}
	else if (m_iCoordSystType == CSTypeProjected)
	{
		csyLoc.SetPointer(new CoordSystemProjection(fnCsy, 1));
		if (m_sGTCitation.length() > 0)
			csyLoc->sDescription = String("%S ; ", m_sGTCitation);
		else if (m_sGeogCitation.length() > 0)
			csyLoc->sDescription = String("%S ; ", m_sGeogCitation);
		csyLoc->sDescription &= m_sDescr;

		CoordSystemProjection* csp = csyLoc->pcsProjection();
		if (csp == 0)
			return csyLoc;  // should not happen; better use exception


		if (m_iProjCSTypeGeoKey >= 20000 && m_iProjCSTypeGeoKey <= 32760)
		{
			long iUTMZone;
			bool fNorthHemis;
			if (fFindUTMInfo(sDatum, iUTMZone, fNorthHemis))
			{
				csp->prj = Projection(String("UTM"));
				csp->prj->Param(pvNORTH, fNorthHemis ? 1L : 0L);
				csp->prj->Param(pvZONE, iUTMZone);
				if (sDatum.length() > 0)
				{
					String sArea = String();
					int iPos = m_sGeogCitation.iPos(String("Area="));
					if (iPos > 0)
					{
						String sTemp = m_sGeogCitation.sRight(m_sGeogCitation.length() - iPos - 5);
						sArea = sTemp.sHead(";");
					}
					csp->datum = new MolodenskyDatum(sDatum, sArea);
					csp->ell = csp->datum->ell; 
				}
			}
			else if (fFindDatumInfo(sDatum)) {
				csp->datum = new MolodenskyDatum(sDatum, "");
				csp->ell = csp->datum->ell; 
			}
			if (fFindEllipsoidInfo(sEllipsoid)) // this over-writes the sphere as ellipsoid 
				csp->ell = Ellipsoid(sEllipsoid); // in case of no Datum but still an ellipsoid
		}
		else  // user defined (that is unknown in GeoTiff itself)
		{
			// Get projection name (ILWIS terminology)
			String sProj = sCoordTransMethod(m_iProjCoordTransGeoKey);

			if (sProj.length() > 0)
			{
				csp->prj = Projection(sProj);

				// Retrieve projection parameters
				GeoKeyDir gkd(m_ITC->fSwap);

				// place file pointer at begin of GeoKeys again
				m_ITC->filTIFF.Seek(iGeoKeyDirAdr);
				gkd.ReadFrom(m_ITC->filTIFF);
				long iGKDvalue = gkd.iValue();
				for (int i = 0; i < iGKDvalue; i++) 
				{
					gkd.ReadFrom(m_ITC->filTIFF);
					ProjectionParamValue ppv = ppvParamType(gkd.iKeyID());
					if (ppv != pvNONE)
						csp->prj->Param(ppv, m_ITC->rDoubleParams(gkd));
				}
			}
			if (fFindDatumInfo(sDatum)) {
				csp->datum = new MolodenskyDatum(sDatum, "");
				csp->ell = csp->datum->ell; 
			}
			if (fFindEllipsoidInfo(sEllipsoid)) // this over-writes the sphere as ellipsoid 
				csp->ell = Ellipsoid(sEllipsoid); // in case of no Datum but still an ellipsoid
		}
	}
	return csyLoc;
}

GeoRef GeoTiffInfo::grf(CoordSystem& csy, RowCol rc)
{
	GeoRef gr(rc);
	FileName fnGeoRef = m_ITC->fnObjectName(".grf");
	if (iModTiePointsAdr != 0) 
	{
		if (iModPixelScalesAdr != 0 && iNrTiePoints == 1)
		{
			// given: pixelsize + 1 tiepoint
			Coord cMin, cMax;
			Coord cTie = Coord(m_amtp[0].rX, m_amtp[0].rY);
			RowCol rcTie = RowCol(m_amtp[0].rCol, m_amtp[0].rRow);
			int iPixelArea = m_fCornersOfCorners ? 0 : 1;
			cMin = Coord(cTie.x - rcTie.Col * m_mpsScales.rX,
						cTie.y - m_mpsScales.rY * (m_ITC->tii().iNrLines - rcTie.Row - iPixelArea));
			cMax = Coord(cTie.x + m_mpsScales.rX * (m_ITC->tii().iNrCols - rcTie.Col- iPixelArea),
						cTie.y - rcTie.Row * m_mpsScales.rY );

			csy->cb.cMin = cMin;
			csy->cb.cMax = cMax;
			if (m_iCoordSystType != CSTypeGeographic )
				csy->Updated();

			gr.SetPointer(new GeoRefCorners(fnGeoRef,
					csy, rc, m_fCornersOfCorners, cMin, cMax));
		}
		else if (iNrTiePoints == 2) 
		{
			// given: 2 tiepoints (pixelsize computed by Ilwis)
			// assumption: tiepoints lie in opposit corners !!!!
			// (Xscale and Yscale should not be different,
			// otherwise GeoRefSmpl has to be made,
			// and for this the affine transf matrix computed,
			// i.e. finding a11, a12, a21, a22, b1 and b2).
			Coord cMin, cMax;
			CoordBounds cbTieLimits;
			cbTieLimits += Coord(m_amtp[0].rX, m_amtp[0].rY);
			cbTieLimits += Coord(m_amtp[1].rX, m_amtp[1].rY);

			//this has to be extended for 2 tiepoints anywhere in image
			csy->sDescription = m_sGTCitation;
			csy->cb.cMin = cbTieLimits.cMin;
			csy->cb.cMax = cbTieLimits.cMax;
			if (m_iCoordSystType != CSTypeGeographic )
				csy->Updated();

			gr.SetPointer(new GeoRefCorners(fnGeoRef,
					csy, rc, m_fCornersOfCorners, cbTieLimits.cMin, cbTieLimits.cMax));
		}
		else if (iNrTiePoints > 2)
		{
			// given 3 or more tiepoints
			// georeftiepoint (affine transf) needed
			bool fSubPixelPrecision = false;
			for (int i = 0; i < iNrTiePoints; i++) 
			{
				if ((floor(m_amtp[i].rRow) != m_amtp[i].rRow) ||
				(floor(m_amtp[i].rCol) != m_amtp[i].rCol))
				{
					fSubPixelPrecision = true;
					break;
				}
			}
			GeoRefCTPplanar* gcp = new GeoRefCTPplanar(fnGeoRef, csy, rc, fSubPixelPrecision);
			gcp->fnBackgroundMap = m_ITC->fnObjectName(".mpr");

			gr.SetPointer(gcp);
			RowCol rcTie;
			Coord cRowCol;
			Coord cTie;
			CoordBounds cbTieLimits;
			for (int i = 0; i < iNrTiePoints; i++) 
			{
				cTie.x = m_amtp[i].rX;
				cTie.y = m_amtp[i].rY;
				if (fSubPixelPrecision)
				{
					cRowCol.x = m_amtp[i].rRow;
					cRowCol.y = m_amtp[i].rCol;
					gcp->AddRec(cRowCol, cTie);
				}
				else
				{
					rcTie.Row = m_amtp[i].rRow;
					rcTie.Col = m_amtp[i].rCol;
					gcp->AddRec(rcTie, cTie);
				}
				cbTieLimits += cTie;
			}
			gr->Updated();
			csy->cb.cMin = cbTieLimits.cMin;
			csy->cb.cMax = cbTieLimits.cMax;
			if (m_iCoordSystType != CSTypeGeographic )
				csy->Updated();
		}
	}
	return gr;
}

String GeoTiffInfo::sReadProfileInfo(const DefType dt, const String& sSection, const String& sKey)
{
	ElementMap* em;
	switch (dt)
	{
		case defGEOTIFF:
			em = m_ecDef.em;
			break;
		case defDATUM:
			em = m_ecDatum.em;
			break;
		case defELLIPSOID:
			em = m_ecEllipsoid.em;
			break;
	}
	return (*em)(sSection, sKey);
}

String GeoTiffInfo::sCSProj(int iCSProjID)
{
	String sKey = String("%i", iCSProjID);
	return sReadProfileInfo(defGEOTIFF, String("PCS"), sKey);
}

String GeoTiffInfo::sCSGeog(int iCSGeogID)
{
	String sKey = String("%i", iCSGeogID);
	return sReadProfileInfo(defGEOTIFF, String("GCS"), sKey);
}

String GeoTiffInfo::sCSEGeog(int iCSEGeogID)
{
	String sKey = String("%i", iCSEGeogID);
	return sReadProfileInfo(defGEOTIFF, String("GCSE"), sKey);
}

void GeoTiffInfo::ExtendDescription(GeoKeyDir& gkd, String& sDescr, bool fIlwisGenerated)
{
	String sSection;
	switch (gkd.iKeyID())
	{
		case 2048 : sSection = "GCSE";             // Geographic CS Type  Only Ellipsoid          Range: 4000 -  4099
					if (gkd.iValue() >= 4200)
						sSection = "GCS";          // Geographic CS Type, Datum + Prime Meridian  Range: 4200 -  4999
					break;
		case 2050 : sSection = "DatumE";           // Geodetic Datum ,    Only Ellipsoid          Range: 6000 -  6199
					if (gkd.iValue() >= 6200)
						sSection = "Datum";        // Geodetic Datum ,    no Prime Merid          Range: 6200 -  6999
					break;                
		case 2056:  sSection = "Ellipse"; break;   // Ellipsoid only                              Range: 7000 -  7999
		case 3072:  sSection = "PCS";     break;   // zone dependent prj's
		case 3074:  sSection = "Proj";    break;   // Projections (Special Definitions)           Range: 10000 - 19999
		case 3075:  sSection = "CT";      break;   // only 27 prj 's
		case 4096:  sSection = "VertCS";           // Vertical Coord Syst (incl ellips)           Range: 5000 -  5099
					if (gkd.iValue() >= 5100)
						sSection = "VertCS_Ortho"; // Vertical Coord Syst (orthometric)           Range: 5100 -  5199
					break;                            
	}
	// For Tiff file not generated by Ilwis 2.2: add it as description to provide
	// user with necessary details to create CoordSystem by hand
	if (sSection.length() > 0 && !fIlwisGenerated) 
	{
		String sCodeNr = String("%i", gkd.iValue());
		if (sDescr.length() > 0)
			sDescr &= "; ";
		sDescr &= sReadProfileInfo(defGEOTIFF, sSection, sCodeNr);
	}
}

void GeoTiffInfo::InitProjParam()
{
	m_mpProjParams[3078] = pvLAT1;   // Latitude of primary standard parallel
	m_mpProjParams[3079] = pvLAT2;   // Latitude of second standard parallel
	m_mpProjParams[3080] = pvLON0;   // Longitude of map-projection natural origin
	m_mpProjParams[3081] = pvLAT0;   // Latitude of map-projection natural origin
	m_mpProjParams[3082] = pvX0;     // Easting of map-projection natural origin
	m_mpProjParams[3083] = pvY0;     // Northing of map-projection natural origin
	m_mpProjParams[3088] = pvLON0;   // Longitude of center of projection
	m_mpProjParams[3089] = pvLAT0;   // Latitude of center of projection
	m_mpProjParams[3090] = pvX0;     // Easting of center of projection
	m_mpProjParams[3091] = pvY0;		 // Northing of center of projection
	m_mpProjParams[3092] = pvK0;		 // Scale at natural origin of projection
	m_mpProjParams[3093] = pvK0;     // Scale at center ratio
}			// In ILWIS center and nat origin of projection are not distinguished (yet)

void GeoTiffInfo::InitCoordTrans()
{
	m_mpCoordTransMethods[1]  = "Transverse Mercator";
	m_mpCoordTransMethods[7]  = "Mercator";
	m_mpCoordTransMethods[8]  = "Lambert Conformal Conic";
	m_mpCoordTransMethods[9]  = "Lambert Conformal Conic";
	m_mpCoordTransMethods[10] = "Lambert Azimuthal EqualArea";
	m_mpCoordTransMethods[11] = "Albers EqualArea Conic";
	m_mpCoordTransMethods[12] = "Azimuthal Equidistant";
	m_mpCoordTransMethods[13] = "Equidistant Conic";
	m_mpCoordTransMethods[14] = "StereoGraphic";
	m_mpCoordTransMethods[15] = "StereoGraphic";
	m_mpCoordTransMethods[16] = "StereoGraphic";
	m_mpCoordTransMethods[17] = "Plate Carree";
//	m_mpCoordTransMethods[18] = ""; // "Cassini Soldner";
	m_mpCoordTransMethods[19] = "Gnomonic";
	m_mpCoordTransMethods[20] = "Miller";
	m_mpCoordTransMethods[21] = "Orthographic";
	m_mpCoordTransMethods[22] = "PolyConic";
//	m_mpCoordTransMethods[23] = ""; // "Robinson";
	m_mpCoordTransMethods[24] = "Sinusoidal";
	m_mpCoordTransMethods[25] = "VanderGrinten";
}

ProjectionParamValue GeoTiffInfo::ppvParamType(int iKey)
{
	switch (iKey) 
	{
		case 3078:  // Latitude of primary standard parallel
		case 3079:  // Latitude of second standard parallel
		case 3080:  // Longitude of map-projection natural origin
		case 3081:  // Latitude of map-projection natural origin
		case 3082:  // Easting of map-projection natural origin
		case 3083:  // Northing of map-projection natural origin
		case 3088:  // Longitude of center of projection
		case 3089:  // Latitude of center of projection
		case 3090:  // Easting of center of projection
		case 3091:  // Northing of center of projection
		case 3092:  // Scale at natural origin ratio
		case 3093:  // Scale at center ratio
			return m_mpProjParams[iKey];
			break;
		default:
			return pvNONE;
			break;
	}
}

String GeoTiffInfo::sCoordTransMethod(int iMethod)
{
	switch (iMethod)
	{
		case 1:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
//		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
//		case 23:
		case 24:
		case 25:
			return m_mpCoordTransMethods[iMethod];
			break;
		default:
			return String();
			break;
	}
}

// Importer class
TiffImporter::TiffImporter(Tranquilizer& trnq, File& pfil, const FileName& fnObj)
	: 	tiffCODE_CLEAR(256), tiffCODE_END(257),
		trq(trnq), m_iMapLine(0),
		m_fnObject(fnObj),
		m_fMultipleImages(false),
		m_iNrImages(0),
		filTIFF(pfil)
{
	m_gti.SetOwner(this);
}

TiffImageInfo& TiffImporter::tii()
{
	return m_tii;
}

GeoTiffInfo& TiffImporter::gti()
{
	return m_gti;
}

void TiffImporter::ReadPackBitsLine(ByteBuf& bufLine)
{
	unsigned int iPos = 0;
	byte bNext;
	while ( iPos < m_iNrBytes ) 
	{
		bNext = bRead();
		if (bNext < 128)
		{
			for (int i = 0; i <= bNext; i++)
				if (iPos < m_iNrBytes)
					bufLine[iPos++] = bRead();
		}
		else if (bNext > 128)
		{
			byte b = bRead();
			for (int i = 0; i < 257 - bNext; i++ )
				if (iPos < m_iNrBytes)
					bufLine[iPos++] = b;
		}
	}
}

void TiffImporter::InitArray()
{
	m_CodeSize = 9;
	m_CodeMax = 257;
	m_MaxCode = 512;
	for (int i = 0; i < 256; i++ )
	{
		StrArrBef[i] = -1;
		StrArrCod[i] = i;
		StrArrLen[i] = 0;
	}
}

void TiffImporter::ReadLZWLine(ByteBuf& bufLine)
{
	byte b;

	for (int i = 0; i < m_iNrBytes; i++ )
	{
		while (m_iBlockNr > m_BlockLen)
		{
			int Code, C, D;
			m_iBit += m_CodeSize;
			while (m_iBit > 0)
			{
				b = bRead();
				m_iCurr = m_iCurr * 256 + b;
				m_iBit -= 8;
			}
			D = 1 << -m_iBit;
			Code = m_iCurr / D;
			m_iCurr %= D;
			if (Code == tiffCODE_CLEAR)
			{
				InitArray();
				m_CodeOld = tiffCODE_CLEAR;
				m_BlockLen = -1;
			}
			else if (Code == tiffCODE_END)
				return;
			else if ( Code <= m_CodeMax )
			{
				C = Code;
				while (C != -1)
				{
					bufBlock[StrArrLen[C]] = StrArrCod[C];
					C = StrArrBef[C];
				}
				m_BlockLen = StrArrLen[Code];
				if (m_CodeOld != tiffCODE_CLEAR)
				{
					m_CodeMax++;
					StrArrBef[m_CodeMax] = m_CodeOld;
					StrArrLen[m_CodeMax] = StrArrLen[m_CodeOld] + 1;
					C = Code;
					while (StrArrBef[C] != -1 )
						C = StrArrBef[C];
					StrArrCod[m_CodeMax] = StrArrCod[C];
				}
				m_CodeOld = Code;
			}
			else
			{
				m_CodeMax++;
				if ( Code != m_CodeMax )
					throw ErrorImportExport(SCVErrGifCode);
				StrArrBef[Code] = m_CodeOld;
				StrArrLen[Code] = StrArrLen[m_CodeOld] + 1;
				C = m_CodeOld;
				while (StrArrBef[C] != -1)
					C = StrArrBef[C];
				StrArrCod[Code] = StrArrCod[C];
				m_CodeOld = Code;
				C = Code;
				while (C != -1)
				{
					bufBlock[StrArrLen[C]] = StrArrCod[C];
					C = StrArrBef[C];
				}
				m_BlockLen = StrArrLen[Code];
			}
			if (m_CodeMax + 2 == m_MaxCode )
				if (m_CodeSize < 12 )
				{
					m_CodeSize++;
					m_MaxCode *= 2;
				}
			m_iBlockNr = 0;
		}
		bufLine[i] = bufBlock[m_iBlockNr];
		m_iBlockNr++;
	}
}

// Unpack bits and nibbles to bytes
void TiffImporter::UnPackLine(ByteBuf& bufLine) 
{
	ByteBuf bufOutLine(tii().iNrCols);

	byte bMask, iCur;
	int iNumShift, iTotShift;
	switch (tii().iBitsPerSample)
	{
		case 1: bMask = 0x01;
				iNumShift = 1;
				iTotShift = 8;
				break;
		case 4: bMask = 0x0f;
				iNumShift = 4;
				iTotShift = 2;
				break;
	}
	int iOutPos = 0;
	int iSh;
	for (int i = 0; i < m_iNrBytes; i++ ) 
	{
		iSh = iTotShift;
		while (iSh > 0 && iOutPos < tii().iNrCols)
		{
			iCur = (bufLine[i] >> (iSh - 1)) & bMask;
			iSh -= iNumShift;
			bufOutLine[iOutPos++] = iCur;
		}
	}

	for (int i = 0; i < tii().iNrCols; i++ )
		bufLine[i] = bufOutLine[i];
}

// dummy
bool TiffImporter::fReadHuffLine(ByteBuf& buf) 
{
	return true;
}

int TiffImporter::ReadStrip(Map& mp, long iLines) 
{
	int iBytesPerPixel;
	switch (tii().iBitsPerSample)
	{
		case 1:
		case 4: iBytesPerPixel = 1;
				break;
		default:
				iBytesPerPixel = tii().iBitsPerSample >> 3;
	}

	ByteBuf bufLine(tii().iNrCols * iBytesPerPixel);
	IntBuf  bufShort(tii().iNrCols * iBytesPerPixel);
	LongBuf bufColor(tii().iNrCols * iBytesPerPixel);

	m_iBlockNr = 0;
	m_BlockLen = -1;
	m_CodeSize = 9;
	m_iBit = 0;
	m_iCurr = 0;

	bool fColor = (0 != mp->dm()->pdcol());
	bool fValue = (0 != mp->dm()->pdvi());  // 16 bits
	for (long iRow = 0; iRow < tii().iRowsPerStrip && m_iMapLine < iLines; ++iRow) 
	{
		switch (tii().iCompression)
		{
			case     1 : filTIFF.Read(m_iNrBytes, bufLine.buf() ); break;
			case     5 : ReadLZWLine(bufLine); break;
			case (short)32773L : ReadPackBitsLine(bufLine);
		}
		if (tii().iBitsPerSample < 8)
			UnPackLine(bufLine);

		if (trq.fUpdate(m_iMapLine, iLines))
			return 1;

		if (fColor) 
		{
			for (int j = 0; j < m_iNrBytes / iBytesPerPixel; ++j )
				bufColor[j] = bufLine[iBytesPerPixel * j] + (((long)bufLine[iBytesPerPixel * j + 1]) << 8) + (((long)bufLine[iBytesPerPixel * j + 2]) << 16);
			mp->PutLineRaw(m_iMapLine++, bufColor);
		}
		else if (fValue)
		{
			// 16 bits grey (IKONOS imagery)
			for (int j = 0; j < m_iNrBytes / iBytesPerPixel; ++j )
			{
				long iVal;
				if (fSwap)
					iVal = ((long)bufLine[iBytesPerPixel * j] << 8) + bufLine[iBytesPerPixel * j + 1];
				else
					iVal = bufLine[iBytesPerPixel * j] + (((long)bufLine[iBytesPerPixel * j + 1]) << 8);

				if (tii().PhotogrInterp == 0)  // WhiteIsZero
					bufShort[j] = (m_bWhiteColor - iVal);
				else      // PhotogrInterp == 1 : BlackIsZero
					bufShort[j] = iVal;
			}
			mp->PutLineRaw(m_iMapLine++, bufShort);
		}
		else 
		{
			if (tii().PhotogrInterp == 0 || tii().PhotogrInterp == 1)
				if (m_bWhiteColor == 1 && tii().iBitsPerSample == 1)
					for (int iCnt = 0; iCnt < tii().iNrCols / iBytesPerPixel; ++iCnt )
						bufLine[iCnt] = m_bWhiteColor - bufLine[iCnt];
				else if (m_bWhiteColor == 0 && tii().iBitsPerSample > 1)  // invert black to white
					for (int iCnt = 0; iCnt < tii().iNrCols / iBytesPerPixel; ++iCnt )
						bufLine[iCnt] = m_bBlackColor - bufLine[iCnt];
			mp->PutLineRaw(m_iMapLine++, bufLine);
		}
	}
	return 0;
}

FileName TiffImporter::fnBaseObject(const String& sExt)
{
	FileName fnLoc = m_fnObject;
	fnLoc.sExt = sExt;
	return fnLoc;
}

FileName TiffImporter::fnObjectName(const String& sExt)
{
	FileName fnLoc = m_fnObject;
	fnLoc.sExt = sExt;
	if (m_fMultipleImages)
	{
		String sName = String("%S%ld", fnLoc.sFile, m_iNrImages);
		fnLoc.sFile = sName;
	}
	return FileName::fnUnique(fnLoc);
}

byte TiffImporter::bRead()
{
	byte b;
	filTIFF.Read(1, &b);
	return b;
}

short TiffImporter::shRead()
{
	unsigned short sh;
	filTIFF.Read(2, &sh);
	if (fSwap)
		Swap(sh);
	return sh;
}

long TiffImporter::iRead()
{
	unsigned long i;
	filTIFF.Read(4, &i);
	if (fSwap)
		SwapLong(i);
	return i;
}

double TiffImporter::rRead()
{
	double r;
	filTIFF.Read(8, &r);
	if (fSwap)
		SwapDouble(r);
	return r;
}

void TiffImporter::GetPixelScales(long iModPixelScalesAdr, ModelPixelScales& mpsScales)
{
	filTIFF.Seek(iModPixelScalesAdr);
	mpsScales.rX = rRead();
	mpsScales.rY = rRead();
	mpsScales.rZ = rRead();
}

void TiffImporter::GetTiePoints(long iModTiePointsAdr, long iNrTiePoints, ModelTiePoint* amtp)
{
	filTIFF.Seek(iModTiePointsAdr);
	for (int i = 0; i < iNrTiePoints; i++)
	{
		amtp[i].rCol = rRead();
		amtp[i].rRow = rRead();
		amtp[i].rH   = rRead();
		amtp[i].rX   = rRead();
		amtp[i].rY   = rRead();
		amtp[i].rZ   = rRead();
	}
}

String TiffImporter::sAsciiParams(GeoKeyDir& gkd)
{
	long iOldPos = filTIFF.iLoc(); // remember current position in file
	char* s = new char[gkd.iCount() + 1];
	filTIFF.Seek(tii().iGeoAsciiParamsAdr + gkd.iValue());
	filTIFF.Read(gkd.iCount(), s);
	filTIFF.Seek(iOldPos);
	for (int i = 0; i < gkd.iCount(); ++i)
	{
		switch (s[i])
		{
			case '\0':
			case '\r':
			case '\n':
				s[i] = ';';
				break;
		}
	}
	s[gkd.iCount()] = '\0';
	String str(s);
	delete [] s;
	return str;
}

double TiffImporter::rDoubleParams(GeoKeyDir& gkd)
{
	long iOldPos = filTIFF.iLoc(); // remember current position in file
	filTIFF.Seek(tii().iGeoDoubleParamsAdr + sizeof(double) * gkd.iValue());
	double r;
	filTIFF.Read(sizeof(double), &r);
	if ( fSwap)
		SwapDouble(r);
	filTIFF.Seek(iOldPos);
	return r;
}

bool TiffImporter::fReadPalette(int iNrOfColors, Array<Color>& aiColors)
{
	Array<unsigned short> ashPalette(3 * iNrOfColors);  // R + G + B all as shorts
	int iBytesToRead = 2 * 3 * iNrOfColors;
	if (iBytesToRead == filTIFF.Read(iBytesToRead, &ashPalette[0] ) )
	{
		aiColors.resize(iNrOfColors);
		ByteBuf cl(3);
		for (int i = 0; i < iNrOfColors; i++ )
		{
			int j = i;
			for (int k = 0; k < 3; k++)
			{
				if (fSwap) Swap(ashPalette[j]);
				cl[k] = ashPalette[j] >> 8;
				j += iNrOfColors;
			}
			aiColors[i] = Color(cl[0], cl[1], cl[2]) ;
		}
		return true;
	}

	return false;
}

Domain TiffImporter::dmDetermine()
{
	FileName fnDom = fnObjectName(".dom");
	if (tii().iBitsPerSample == 1)
		return Domain("bit");

	if (tii().iBitsPerSample == 16)
		return Domain(fnDom, 0L, 32766L);

	if	((tii().iBitsPerSample >= 24) || (tii().iSamplesPerPixel > 1))
		return Domain("Color");

	if (tii().Palette != 0 ) 
	{
		long iNrOfColors = 1 << tii().iBitsPerSample;
		iNrOfColors = iNrOfColors > 256 ? 256 : iNrOfColors;
		Domain dm = Domain(fnDom, iNrOfColors, dmtPICTURE);
		Representation rpr = dm->rpr();
		RepresentationClass* prprc = dynamic_cast<RepresentationClass*>( rpr.ptr() );

		Array<Color> acColors;
		filTIFF.Seek(tii().Palette);

		if (fReadPalette(iNrOfColors, acColors))
			for (int i = 0; i < iNrOfColors; i++ )
				prprc->PutColor(i, acColors[i]);

		else
		{
			dm->fErase = true;
			rpr->fErase = true;
			throw ErrorImportExport(SCVErrReading);
		}
		return dm;
	}
	else
		return Domain("Image");
}

long TiffImporter::iReadTiffIntro()
{
	String sErr = SCVErrNoTIFF;
	TIFF_HeaderStruct TIFF_Header(filTIFF);
	bool fOk = TIFF_Header.fIsTiff() && (TIFF_Header.iVersion() == 42);
	if (!fOk)
		throw ErrorImportExport( sErr);

	fSwap = (TIFF_Header.sID() == "MM");

	return TIFF_Header.iOffset(); // address of the first IFD
}

long TiffImporter::iReadTiffImageInfo()
{
	short NrFields = shRead();

	unsigned long Predictor = 1;       //  other value: decoding will not work
	long iNrCounts = 0;
	long iBpsCount = 0;
	long iPlanarConf = 1;

	TiffTagField ttfFromFile(fSwap);
	for (int i = 0; i < NrFields; i++) 
	{
		if (!ttfFromFile.ReadFrom(filTIFF) )
		  throw ErrorImportExport(SCVErrReading);

		switch (ttfFromFile.iTag())
		{
		  case 256: m_tii.iNrCols  = ttfFromFile.iOffset(); break;
		  case 257: m_tii.iNrLines = ttfFromFile.iOffset(); break;
		  case 258: iBpsCount = ttfFromFile.iCount();
					m_tii.iBitsPerSample = ttfFromFile.iOffset(); 
					break;
		  case 259: m_tii.iCompression = ttfFromFile.iOffset();  break;
		  case 262: m_tii.PhotogrInterp = ttfFromFile.iOffset(); break;
		  case 273: m_tii.iNrStrips = ttfFromFile.iCount(); 
					m_tii.iStripOffset = ttfFromFile.iOffset();
					break;
		  case 277: m_tii.iSamplesPerPixel = ttfFromFile.iOffset(); break;
		  case 278: m_tii.iRowsPerStrip = ttfFromFile.iOffset();    break;
		  case 279: iNrCounts = ttfFromFile.iCount();
					m_tii.iStripByteCounts = ttfFromFile.iOffset();
					break;
		  case 284: iPlanarConf = ttfFromFile.iOffset();                   break;
		  case 317: Predictor = ttfFromFile.iOffset();                     break;
		  case 320: m_tii.Palette = ttfFromFile.iOffset();               break;
		  case 33550L: m_gti.iModPixelScalesAdr = ttfFromFile.iOffset(); break;
		  case 33922L: m_gti.iNrTiePoints = ttfFromFile.iCount() / 6;
					  m_gti.iModTiePointsAdr = ttfFromFile.iOffset();
					  m_tii.fGeoTiff = true;
					break;
		  case 34735L: m_gti.iGeoKeyDirAdr = ttfFromFile.iOffset(); 
					tii().fGeoTiff = true;
					break;
		  case 34736L: m_tii.iGeoDoubleParamsAdr = ttfFromFile.iOffset(); break;
		  case 34737L: m_tii.iGeoAsciiParamsAdr = ttfFromFile.iOffset();  break;
		}
	}
	// if nothing went wrong we can now read the offset to the next IFD
	// this will be returned to the main loop
	long iNextDirOffset;
	filTIFF.Read(4, &iNextDirOffset);

	if (iBpsCount > 1)
	{
		filTIFF.Seek(tii().iBitsPerSample);
		tii().iBitsPerSample = 0;    // eigenlijk: bits per pixel
		for (short ibps = 0; ibps < iBpsCount; ibps++)
			tii().iBitsPerSample += shRead();
	}
	String sErr = SCVErrNoTIFF;
	bool fOk = true;
	if (tii().iBitsPerSample > 24)
	{
		fOk = false;
		sErr = SCVErrTooManyPlanes;
	}

	// PhotoMetricInterpretation:
	//    0 or 1: bilevel or greyscale images
	//    2:      RGB
	//    3:      Palette images
	//    5:      Separated (usually CMYK; optional for level 6)
	switch (m_tii.PhotogrInterp)
	{
		case 0 :   // WhiteIsZero
			m_bWhiteColor = 0;
			m_bBlackColor = (1 << m_tii.iBitsPerSample) - 1;
			break;
		case 1 :   // BlackIsZero
			m_bWhiteColor = (1 << m_tii.iBitsPerSample) - 1;
			m_bBlackColor = 0;
			break;
		case 2 :    // RGB: also supported
			break;
	}

	if (iPlanarConf == 2)    // 2=planar format, hardly ever used so no support now
	{
		fOk = false;
		sErr = SCVErrNoPlanarTIF;
	}
	else if ( m_tii.iNrCols < 1 || m_tii.iNrLines < 1 ) 
	{
		fOk = false;
		sErr = SCVErrWrongLineCol;
	}
	else if (Predictor != 1)    // No support for compression predictors yet
	{
		fOk = false;
		sErr = SCVErrNoHorDiffLZW;
	}
	else if (m_tii.iCompression != 1 && 
	         m_tii.iCompression != 5 && 
	         m_tii.iCompression != (short)32773L)
	{
		fOk = false;
		sErr = SCVErrUnsupCompres;
	}
	if (m_tii.iCompression == 5)
	{
		if (m_tii.iBitsPerSample < 8)
		{
			fOk = false;
			sErr = SCVErrUnsuppTIFLZW;
		}
		else
		{
			StrArrBef.Resize(4096);
			StrArrCod.Resize(4096);
			StrArrLen.Resize(4096);
			bufBlock.Size(5001);
			InitArray();
		}
	}
	if (!fOk)
		throw ErrorImportExport( sErr);

	m_iNrBytes = 1 + (m_tii.iNrCols * m_tii.iBitsPerSample - 1) / 8;
	
	return iNextDirOffset;
}

void TiffImporter::Import()
{
	m_iNrImages = 0;
	m_fMultipleImages = false;
	long iNextDirOffset = iReadTiffIntro();
	ObjectCollection ocTIFF;
	while (iNextDirOffset != 0)
	{
		filTIFF.Seek(iNextDirOffset);
		tii().Reset();   // prepare for a new tiff image
		iNextDirOffset = iReadTiffImageInfo();
		m_iNrImages++;
		m_fMultipleImages = m_fMultipleImages || iNextDirOffset != 0;

		Domain dm = dmDetermine();
		if (!dm->fSystemObject()) dm->fErase = true;

		CoordSystem csy;

		RowCol rc(tii().iNrLines, tii().iNrCols);
		GeoRef gr(rc);
		if (tii().fGeoTiff)
		{
			gti().ReadGeoTiffInfo();
			csy = gti().csy();
			gr  = gti().grf(csy, rc);
		}

		if (csy.fValid() && !csy->fSystemObject()) csy->fErase = true;
		if (gr.fValid()  && !gr->fSystemObject())  gr->fErase  = true;

		// Read the image from the TIFF file and create the ILWIS map
		Map mpOut(fnObjectName(".mpr"), gr, rc, dm);
		mpOut->fErase = true;

		m_iMapLine = 0;
		trq.SetText(SCVTextConverting);
		if (tii().iNrStrips == 1)  // skip
		{
			filTIFF.Seek(tii().iStripOffset);
			if (ReadStrip(mpOut, tii().iNrLines) )
				return;  // Stop button is activated
		}
		else
		{
			unsigned long iStripLine;
			for (int i = 0; i < tii().iNrStrips; i++)
			{
				filTIFF.Seek(tii().iStripOffset + i * 4);
				iStripLine = iRead();
				filTIFF.Seek(iStripLine);
				if (ReadStrip(mpOut, tii().iNrLines) )
					return;  // Stop button is activated
			}
		}
		trq.fUpdate(tii().iNrLines, tii().iNrLines);

		// Read was successful
		mpOut->fErase = false;
		dm->fErase    = false;
		if (csy.fValid()) csy->fErase = false;
		if (gr.fValid())  gr->fErase  = false;
		
		if (m_fMultipleImages && !ocTIFF.fValid())
		{
			ParmList pm;
			ocTIFF = ObjectCollection(fnBaseObject(".ioc"), "ObjectCollection", pm);
		}
		if (m_fMultipleImages && ocTIFF.fValid())
		{
			mpOut->Store();  // make sure the ODF is on disk, 
			                 // otherwise adding to the IOC is only partially succesful 
			ocTIFF->Add(mpOut);
			if (dm.fValid() && !dm->fSystemObject())
			{
				ocTIFF->Add(dm);
				if (dm->rpr().fValid())
					ocTIFF->Add(dm->rpr());
			}
			if (csy.fValid() && !csy->fSystemObject())
				ocTIFF->Add(csy);
			if (gr.fValid() && !gr->fSystemObject())
				ocTIFF->Add(gr);
		}
	}
}

void ImpExp::ImportTIF(File& filTIFF, const FileName& fnObject)
{
	trq.SetTitle(SCVTitleImportTIFF);
	trq.fUpdate(0);

	// fnObject is the base name for the ILWIS map
	// If the TIFF file contains more than one image fnObject
	// will also be the name of the object collection that will be
	// created in that case. Multiple images will result in map names
	// based on the template fnObject_nn.mpr where nn is a number starting at "1"
	// A similar story is valid for the service objects (domain, coodsys and georef)
	TiffImporter ITC(trq, filTIFF, fnObject);

	ITC.Import();
}





