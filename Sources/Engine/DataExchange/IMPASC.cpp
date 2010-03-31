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
  import ILWIS .ASC format
  by Li Fei, Nov. 94
  modified by Li Fei, Oct 95
  ILWIS Department ITC
	Last change:  WN   24 Jun 97    7:26 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\strtod.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\Convloc.h"

void ImpExp::ImportASC(File& FileIn, const FileName& fnObject)
{
	String sLine;
	FileIn.ReadLnAscii(sLine);
	FileIn.ReadLnAscii(sLine);
	
	int iFr = atoi(strtok(sLine.sVal(), " \t\v"));
	int iNr = atoi(strtok(0, " \t\v"));
	long iCol = iNr - iFr + 1;
	iFr = atoi(strtok(0, " \t\v"));
	iNr = atoi(strtok(0, " \t\v"));
	long iLine = iNr - iFr + 1;
	int iMpTp = atoi(strtok(0, " \t\v"));
	if (iLine < 1 || iCol < 1 || iMpTp < 0 || iMpTp > 2)
		throw ErrorImportExport(SCVErrNotSupported);

	int iScale = atoi(strtok(0, " \t\v"));
	double rTemp = pow(10.0, iScale);
	RowCol rc(iLine, iCol);
	GeoRef gr(rc);
	DomainValueRangeStruct dvs;
	if (iMpTp == 0)
		dvs = DomainValueRangeStruct(Domain("bit"));
	else if (iMpTp == 1)
		dvs = DomainValueRangeStruct(Domain("Image"));
	else if (iScale == 0)
		dvs = DomainValueRangeStruct(-32766L, 32767L);
	else
		dvs = DomainValueRangeStruct(-32766L * rTemp, 32767L * rTemp, rTemp );
	Map mp(fnObject, gr, rc, dvs);
	mp->fErase = true;
	IntBuf ibufOut(iCol);

	int i = 0, j = 0;
	char *p;
	rTemp = 1 / pow(10.0, iScale);
	trq.SetTitle(SCVTitleImportIlwisASC);
	trq.SetText(SCVTextConverting);
	while (!(FileIn.fEof()) && j < iLine) {
		if (trq.fUpdate(j, iLine))
			return;

		FileIn.ReadLnAscii(sLine);
		int k = 0;
		p = &(sLine[0]);
		while (p && j<iLine) {
			if (k == 0) {
				p = strtok(sLine.sVal(), " \t\v");
				k = 1;
			}
			if (iScale == 0)
				ibufOut[i++] = atoi(p);
			else
				ibufOut[i++] = atofILW(p) * rTemp;
			p = strtok(0, " \t\v");
			if (i == iCol) {
				mp->PutLineRaw(j++, ibufOut);
				i = 0;
			}
		}
	}
	trq.fUpdate(iLine, iLine);
	mp->fErase = false;
}


