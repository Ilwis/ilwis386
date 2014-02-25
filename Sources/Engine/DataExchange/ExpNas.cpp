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
/*
  export ILWIS to Arc/Info NAS format
  By Willem Nieuwenhuis, august 2000
  ILWIS Department ITC
*/

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include <string.h>
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"

class NASExporter
{
public:
	NASExporter(const FileName& fnObject, const FileName& fnFile, Tranquilizer& trq);
	~NASExporter();
	
	void Export();
	
protected:
	enum iconvtype {ictINT, ictBYTERAW, ictINTRAW, ictFLOAT, ictNONE };
	
	iconvtype ictDetermineDomainType(const Map& mp);
	String DoubleToStr(const double& rVal);
	String LongToStr(long iVal);
	String IntToStr(short iVal);

	void ReadLine(const Map& mp, long iRow, iconvtype ict);
	String GetBufVal(long iCol, iconvtype ict);
	void InitBuf(long iCols, iconvtype ict);

private:
	void DoubleConvert(const double& rVal, int iPrecision, char *pcOut);
	
	String m_sUndef;
	long   m_iDblPrecision;
	
	File* m_filASC;
	FileName m_fnObject;
	FileName m_fnFile;
	
	ByteBuf bbBuf;
	IntBuf  isbBuf;
	LongBuf ibBuf;
	RealBuf rbBuf;

	Tranquilizer& m_trq;
};



NASExporter::NASExporter(const FileName& fnObject, const FileName& fnFile, Tranquilizer& trq)
	: m_trq(trq)
{
	m_fnObject = fnObject;
	m_fnFile = fnFile;

	m_filASC = new File(fnFile, facCRT);
	m_filASC->SetErase(true);
}


NASExporter::~NASExporter()
{
	if (m_filASC) delete m_filASC;
}


NASExporter::iconvtype NASExporter::ictDetermineDomainType(const Map& mp)
{
	iconvtype ictConv = ictNONE;

	DomainSort *pds = mp->dm()->pdsrt();
	DomainValue *pdv = mp->dm()->pdv();

	double rspUNDEF = 0;
	if (mp->dm()->pdi())
	{
		rspUNDEF = -1;
		ictConv = ictINTRAW;
	}
	else if (mp->dm()->pdbit())
	{
		rspUNDEF = -1;
		ictConv = ictINT;
	}
	else if (mp->dm()->pdbool())
	{
		rspUNDEF = 0;
		ictConv = ictBYTERAW;
	}
	else if (pds)
	{
		long iNrItems = pds->iSize();
		if (iNrItems < 255L)
		{
			rspUNDEF = 0;
			ictConv = ictBYTERAW;
		}
		else if (iNrItems < 32767L)
		{
			rspUNDEF = -32767;
			ictConv = ictINTRAW;
		}
		else
			ictConv = ictNONE;
	}
	else if (pdv)
	{
		RangeReal rrMap = mp->rrMinMax(BaseMapPtr::mmmCALCULATE);  // force the minmax calculation
		double rStep = mp->vr()->rStep();

		if (abs(rStep - 1.0) < 1e-6)
		{
			if (rrMap.rLo() >= 0.0 && rrMap.rHi() <= 254.0)
				ictConv = ictINT;
			else if (rrMap.rLo() >= -32766.0 && rrMap.rHi()<= 32767.0)
				ictConv = ictINT;
			else
				ictConv = ictNONE;

	        rspUNDEF = -32767.0;
		}
		else
		{
			if (abs(rStep) < 1e-18)
				rStep = (rStep < 0) ? -1e-18 : 1e-18;
			double rTmp = max(abs(rrMap.rLo()), abs(rrMap.rHi()));
			m_iDblPrecision = 9;
			if (rTmp > 1e-11 && rTmp < 1e12)
			{
				long iDec = (long)abs(floor(log10(rStep)));
				long iBeforeDec = (long)floor(log10(rTmp)) + 1;

				m_iDblPrecision = iDec + iBeforeDec;
				rspUNDEF = rrMap.rHi() + 10 * rStep;
				ictConv = ictFLOAT;
			}
			else
				ictConv = ictNONE;
		}
	}
	// Now convert the undefined number (rspUNDEF) to a string
	if (ictConv == ictFLOAT)
		m_sUndef = DoubleToStr(rspUNDEF);
	else
		m_sUndef = String("%i", (short)rspUNDEF);

	return ictConv;
}


// number are formatted correctly in the range -1e-10 upto 1e+10
// number are formatted as:
//  smmmmmmmmm.nnnnnnnnn
// with:
//  s   sign ('-', ' ')
//  m   digit
//  n   digit after dot
// iPrecision is the number of digits behind the decimal point ('.')
void NASExporter::DoubleConvert(const double& rVal, int iPrecision, char *pcOut)
{
	int iDecPos;
	int iSign;

	char *pcRes = pcOut;
	char *p = _ecvt(rVal, iPrecision + 1, &iDecPos, &iSign);

	if (abs(iDecPos) >= iPrecision)
	{
		// the number cannot be displayed
		strcpy(pcRes, "-9999.9999");  // undef
		return;
	}

	*pcRes++ = (iSign == 0) ? ' ' : '-';
	if (iDecPos <= 0)
	{
		*pcRes++ = '0';
		*pcRes++ = '.'; // cDecimalPoint(); so not language independent!
		--iPrecision;   // the zero also counts as a significant digit here

		while (iDecPos < 0)
		{
			*pcRes++ = '0';  // 0.00000....
			iDecPos++;
		}

		while (iPrecision-- > 0)
			*pcRes++ = *p++;  // add iPrecision digits after '.'
	}
	else
	{
		while (iDecPos-- > 0)
		{
			*pcRes++ = *p++;  // add digits before '.'
			iPrecision--;
		}

		*pcRes++ = '.'; // cDecimalPoint(); so not language independent!

		while (iPrecision-- > 0)
			*pcRes++ = *p++;  // add rest of digits after '.'
	}

	*pcRes = '\0';
}

String NASExporter::LongToStr(long iVal)
{
	if (iVal == iUNDEF)
		return m_sUndef;
	else
		return String("%i", iVal);
}

String NASExporter::IntToStr(short iVal)
{
	if (iVal == shUNDEF)
		return m_sUndef;
	else
		return String("%i", iVal);
}

String NASExporter::DoubleToStr(const double& rVal)
{
	float flNum = floatConv(rVal);
	if (flNum == flUNDEF)
		return m_sUndef;

	char pc[30];

	DoubleConvert(rVal, m_iDblPrecision, pc);

	return String(pc);
}

String NASExporter::GetBufVal(long iCol, iconvtype ict)
{
	String sNumVal;
	switch (ict)
	{
		case NASExporter::ictINT :  // 4 bytes per pixel
			sNumVal = String("%S ", LongToStr(ibBuf[iCol]));
			break;
		case NASExporter::ictINTRAW :  // 2 bytes per pixel
			sNumVal = String("%S ", IntToStr(isbBuf[iCol]));
			break;
		case NASExporter::ictBYTERAW :  // 1 byte per pixel
			sNumVal = String("%i ", bbBuf[iCol]);
			break;
		case NASExporter::ictFLOAT :  // 4 bytes per pixel
			sNumVal = String("%S ", DoubleToStr(rbBuf[iCol]));
			break;
	}
	return sNumVal;
}

void NASExporter::InitBuf(long iCols, iconvtype ict)
{
	switch (ict)
	{
		case NASExporter::ictINT :
			ibBuf.Size(iCols);  // 4 bytes per pixel
			break;
		case NASExporter::ictINTRAW :
			isbBuf.Size(iCols);  // 2 bytes per pixel
			break;
		case NASExporter::ictBYTERAW :
			bbBuf.Size(iCols);  // 1 byte per pixel
			break;
		case NASExporter::ictFLOAT :
			rbBuf.Size(iCols);  // 4 bytes per pixel
			break;
	}
}

void NASExporter::ReadLine(const Map& mp, long iRow, iconvtype ict)
{
	switch (ict)
	{
		case NASExporter::ictINT :  // 4 bytes per pixel
			mp->GetLineVal(iRow, ibBuf);
			break;
		case NASExporter::ictINTRAW :  // 2 bytes per pixel
			mp->GetLineRaw(iRow, isbBuf);
			break;
		case NASExporter::ictBYTERAW :  // 1 byte per pixel
			mp->GetLineRaw(iRow, bbBuf);
			break;
		case NASExporter::ictFLOAT :  // 4 bytes per pixel
			mp->GetLineVal(iRow, rbBuf);
			break;
	}
}

void NASExporter::Export()
{
	Map mp(m_fnObject);
	if (!mp.fValid())
		return;
	
	iconvtype ictConv = ictDetermineDomainType(mp);

	if (ictConv == ictNONE)
		throw ErrorImportExport(String(TR("Values will not fit in %S file").c_str(), m_fnFile.sFile));

	long iCols = mp->iCols();
	long iLines = mp->iLines();

	m_filASC->WriteLnAscii(String("ncols %li", iCols));
	m_filASC->WriteLnAscii(String("nrows %li", iLines));

	if (mp->gr()->fGeoRefNone())
	{
		m_filASC->WriteLnAscii(String("xllcorner 0"));
		m_filASC->WriteLnAscii(String("yllcorner 0"));
		m_filASC->WriteLnAscii(String("cellsize 1"));
	}
	else
	{
		Coord crd;
		mp->gr()->RowCol2Coord(iLines, 0, crd);
		GeoRefCorners* grc = mp->gr()->pgc();
		CoordSystemLatLon *pcsll = 0;
		if (grc)
			pcsll = grc->cs()->pcsLatLon();
		String sX, sY;
		if (pcsll != 0) // LatLon coords
		{
			sX = String("%.4lf", crd.x);  // LatLon need four decimals to be able to
			sY = String("%.4lf", crd.y);  // handle minutes/seconds (resolution 0.3 second)
		}
		else
		{
			sX = String("%li", (long)crd.x);
			sY = String("%li", (long)crd.y);
		}
		if (grc && grc->fCornersOfCorners)
		{
			m_filASC->WriteLnAscii(String("xllcorner %S", sX));
			m_filASC->WriteLnAscii(String("yllcorner %S", sY));
		}
		else
		{
			m_filASC->WriteLnAscii(String("xllcenter %S", sX));
			m_filASC->WriteLnAscii(String("yllcenter %S", sY));
		}
		double rPixSz = mp->gr()->rPixSize();
		if (rPixSz < 1)
		{
			int iExp = abs(log(rPixSz) / log(10.0)) + 3;  // Use three significant digits
			m_filASC->WriteLnAscii(String("cellsize %.*f", iExp, rPixSz));
		}
		else
			m_filASC->WriteLnAscii(String("cellsize %.2f", rPixSz));
	}

	m_filASC->WriteLnAscii(String("nodata_value %S", m_sUndef));

	long iCol, iRow;
	String sNumVal;

	InitBuf(iCols, ictConv);
	for (iRow = 0; iRow < iLines ; iRow++)
	{
		if (m_trq.fUpdate(iRow, iLines))
			return;

		ReadLine(mp, iRow, ictConv);
		for (iCol = 0; iCol < iCols ; iCol++)
		{
			String sNumVal = GetBufVal(iCol, ictConv);
			m_filASC->Write(sNumVal.length(), sNumVal.c_str());
		}
		m_filASC->WriteLnAscii(String());
	}

	m_filASC->SetErase(false);  // succeeded
}


/*
   fnObject   : the name of the ilwis map
   fnFile     : the name of the .ASCII file
   The ExportNAS function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportNAS(const FileName& fnObject, const FileName& fnFile)
{
	try
	{
		trq.SetTitle(TR("Exporting to Arc/Info ASCII"));   // the title in the report window
		trq.SetText(TR("Processing..."));      // the text in the report window

		NASExporter NASex(fnObject, fnFile, trq);

		NASex.Export();
		trq.Stop();
	}
	catch (ErrorObject& err)
	{
		trq.Stop();
		err.Show();
	}
}
