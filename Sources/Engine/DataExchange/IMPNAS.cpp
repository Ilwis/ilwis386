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
  import workstation and PC .NAS format
  by Li Fei, Oct. 94
  modified by Li Fei, Oct 95
  ILWIS Department ITC
	Last change:  WN   13 Mar 98    2:50 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\strtod.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Map\Raster\Map.h"

void ImpExp::ImportNAS(File& FileIn, const FileName& fnObject)
{
	trq.SetTitle(TR("Importing from Arc/Info uncompressed ASCII"));
	trq.fUpdate(0);

	bool fPc = true;
	String sLine;
	double nodata = rUNDEF;
	double xc, yc, rSize;
	long iCol;
	long iLine;
	bool fCornerOfCorner;
	
	FileIn.ReadLnAscii(sLine);
	String s1 = strtok(sLine.sVal(), " \t\v");
	if (_stricmp(s1.sVal(), "NAS") == 0)
	{
		FileIn.ReadLnAscii(sLine);
		long iFr = atoi(strtok(sLine.sVal(), " \t\v"));
		long iNr = atoi(strtok(0, " \t\v"));
		iLine = iNr - iFr + 1;
		iFr = atoi(strtok(0, " \t\v"));
		iNr = atoi(strtok(0, " \t\v"));
		iCol = iNr - iFr + 1;
	}
	else if (_stricmp(s1.sVal(), "NCOLS") == 0)
	{
		fPc = false;
		iCol = atoi(strtok(0, " \t\v"));
		FileIn.ReadLnAscii(sLine);
		strtok(sLine.sVal(), " \t\v");
		iLine = atoi(strtok(0, " \t\v"));
		FileIn.ReadLnAscii(sLine);
		String ss = strtok(sLine.sVal(), " \t\v");
		xc = atofILW(strtok(0, " \t\v"));
		FileIn.ReadLnAscii(sLine);
		strtok(sLine.sVal(), " \t\v");
		yc = atofILW(strtok(0, " \t\v"));
		FileIn.ReadLnAscii(sLine);
		strtok(sLine.sVal(), " \t\v");
		rSize = atofILW(strtok(0, " \t\v"));
		FileIn.ReadLnAscii(sLine);
		String s1 = sLine;
		if (_stricmp(strtok(s1.sVal(), " \t\v"), "NODATA_VALUE") == 0)
		{
			nodata = atof(strtok(0, " \t\v"));
			FileIn.ReadLnAscii(sLine);
		}
		fCornerOfCorner = fCIStrEqual(ss, "XLLCORNER");
		if (!fCornerOfCorner)
		{
			xc -= rSize / 2;
			yc -= rSize / 2;
		}
	}
	else
		throw ErrorImportExport(TR("File format not supported"));

	if (iLine < 1 || iCol < 1)
		throw ErrorImportExport(TR("Incorrect number of lines or columns"));
	
	RowCol rc((long)iLine, (long)iCol);  
	FileName fnGrf = FileName::fnUnique(FileName(fnObject, ".grf", true));
	GeoRef gr;
	if (fPc)
		gr.SetPointer(new GeoRefNone(fnGrf, rc));
	else
	{
		// (xc, yc) is lower left coordinate
		Coord cMin, cMax;
		cMin = Coord(xc, yc);
		cMax = Coord(xc + rSize * iCol, yc + rSize * iLine);
		gr.SetPointer(new GeoRefCorners(fnGrf, CoordSystem(), 
							rc, fCornerOfCorner,
                            cMin, cMax));
	}
	gr->Updated();
	gr->fErase = true;

	// Assume floating point numbers 
	Domain dm("value");
	ValueRange vr = ValueRange(-FLT_MAX, FLT_MAX, 2.0 * FLT_MIN);

	DomainValueRangeStruct dvs(dm, vr);
	Map mp(fnObject, gr, rc, dvs);
	mp->fErase = true;

	long i = 0, j = 0;
	char *p;
	trq.SetText(TR("Converting..."));

	bool fFirst = true;
	bool fHasUndef = nodata != rUNDEF;
	if (vr->rStep() < 1)
	{
		while ( !(FileIn.fEof()) && j<iLine )
		{
			RealBuf rbufOut(iCol);

			if (trq.fUpdate(j, iLine))
				return;

			if (fFirst)
				fFirst = false;
			else
				FileIn.ReadLnAscii(sLine);
			long k = 0;
			p = &(sLine[0]);
			while ( p && j<iLine )
			{
				if (k == 0)
				{
					p = strtok(sLine.sVal(), " \t\v");
					k = 1;
				}
				double rTemp = atof(p);
				if (fHasUndef && rTemp == nodata)
					rbufOut[i++] = rUNDEF;
				else
					rbufOut[i++] = rTemp;
				p = strtok(0, " \t\v");
				if (i == iCol)
				{
					mp->PutLineVal(j++, rbufOut);
					i = 0;
				}
			}
		}
	}

	trq.fUpdate(iLine, iLine);
	mp->fErase = false;
	gr->fErase = false;
}


