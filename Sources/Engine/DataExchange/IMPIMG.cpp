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
  import .IMG format
  by Li Fei, Oct. 94
  modified by Li Fei, Sep 95
  ILWIS Department ITC
	Last change:  JEL  28 Apr 97    4:03 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\Convloc.h"

void ImpExp::ImportIMG(File& FileIn, const FileName& fnObject)
{
#pragma pack(1)
	struct {
		unsigned char reserved1[22];
		unsigned char image_type;
		unsigned char projection;
		unsigned char reserved[6];
		short height;
		short width;
		unsigned char reserved3[4];
		unsigned char title[80];
		unsigned char reserved4[394];
	} Hd;
#pragma pack()
	
	trq.SetTitle(TR("Importing from Atlas Image"));
	trq.fUpdate(0);
	if ( (sizeof Hd) != FileIn.Read(sizeof Hd , &Hd) )
		throw ErrorImportExport(TR("Solving File size mismatch in header."));
	
	int iMapSubType = (int)(Hd.image_type);
	bool fOK = false;
	switch (iMapSubType)
	{
		case 0:   case 100: case 200:
		case 1:   case 101:
		case 6:   case 106:
		case 10:  case 110:
		case 11:  case 111:
		case 12:  case 112:
		case 13:  case 113:
			fOK = true;
			break;
		default:
			fOK = false;
	}
	if (!fOK)
	{
		String sErr = String("%S (%i)", TR("Invalid image type found"), iMapSubType);
		throw ErrorImportExport(sErr);
	}
	
	long iLines = Hd.height;
	long iCols = Hd.width;
	if (iLines <= 0 || iCols <= 0)
	{
		String sErr = String("%S (%ld x %ld)", TR("Incorrect number of lines or columns"), iLines, iCols);
		throw ErrorImportExport(sErr);
	}
	if ((iLines * iCols + 512) != FileIn.iSize())
	{
		String sErr(TR("Size of %S does not match number of lines and columns").c_str(), FileIn.sName());
		throw ErrorImportExport(sErr);
	}

	ByteBuf bbuf(Hd.width);
	
	RowCol rc(iLines, iCols);
	GeoRef gr(rc);
	Domain dm("Image");
	Map mp(fnObject, gr, rc, dm);

	int iCnt = 0;
	while (iCnt < 80 && Hd.title[iCnt] >= 0x20)
		iCnt++;
	Hd.title[iCnt] = 0;
	String sDesc = String(Hd.title).sTrimSpaces();
	if (sDesc.length() > 0)
		mp->SetDescription(sDesc);

	mp->fErase = true;
	trq.SetText(TR("Converting..."));
	for (long i = 0; i < iLines; i++) {
		if (trq.fUpdate(i, iLines))
			return;
		
		if ( iCols != FileIn.Read( iCols, bbuf.buf() ) )
			throw ErrorImportExport(TR("File Reading Error"));
		
		mp->PutLineRaw(i,bbuf);
	}
	trq.fUpdate(iLines, iLines);
	mp->fErase = false;  // success, keep output
}


