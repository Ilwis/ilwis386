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
/* $Log: /ILWIS 3.0/Import_Export/IMPGIS.cpp $
 * 
 * 8     7-08-01 12:43 Koolhoven
 * removed domain member from MapList, because it served no purpose
 * 
 * 7     14/03/01 16:01 Willem
 * LAN file can contain an invalid number of classes, when only values are
 * available. The check now also includes the availability of the TRL
 * file, before classes are assumed.
 * 
 * 6     24/03/00 11:45 Willem
 * Importing a LAN/GIS file with 10 bands or more now generates proper
 * filenames
 * 
 * 5     16/03/00 16:29 Willem
 * - Replaced the obsolete trq.Message() function with MessageBox()
 * function
 * 
 * 4     8/02/00 10:24 Willem
 * - Added "pragma pack" to align datastructures
 * - Solved string comparison problem
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:11p Martin
 * ported files to VS
// Revision 1.3  1998/09/17 09:12:27  Wim
// 22beta2
//
// Revision 1.2  1997/08/07 22:48:16  Willem
// Cahnged variable for fileseek from short into long, to prevent negative
// file offsets, causing crashes.
//
/*
  import .GIS format
  by Li Fei, Nov. 94
  modified by Li Fei, Oct 95
  ILWIS Department ITC
	Last change:  WN    7 Aug 97    7:47 pm
*/

/*
  Revision history:
  WN 04-07-97: Changed a String parameter to char* in read function. This
               worked for some reason although a cast from String to char*,
               and next a cast for char* to void* was needed.
  WN 25-07-97: TRL file is now only scanned for GIS files; exception handling
               introduced to handle missing TRL file
               Replaced use of domain Int with proper ValueRange for 2-byte maps
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

void ImpExp::ImportGISLAN(File& FileIn, const FileName& fnObject)
{
#pragma pack(1)
	struct 
	{
		char caHDWORD[6];
		short iPack, iNBands;
		char cUnused1[6];
		union 
		{
			float rfl;
			long lg;
		} unRCols, unRows, unRx, unRy;
		char caUnused2[56];
		short iMapTyp, iNclass;
		char caUnused3[14];
		short iAutyp;
		float rAcre, rXmap, rYmap, rXcell, rYcell;
	} erdheader;
#pragma pack()

	if (128 != FileIn.Read(128, &erdheader))
		throw ErrorImportExport(TR("File Reading Error"));

	short iver = 0;
	if (!strncmp(erdheader.caHDWORD,"HEADER", 6))
		iver = 1;
	else if (!strncmp(erdheader.caHDWORD,"HEAD", 4))
		iver = 2;
	if (!iver)
		throw ErrorImportExport(TR("Not an Erdas LAN or GIS File"));
	
	trq.SetTitle(TR("Importing from Erdas"));
	long iLine = erdheader.unRows.lg;
	long iCol = erdheader.unRCols.lg;
	if (iver == 1)
	{
		iLine = erdheader.unRows.rfl + 0.5;
		iCol = erdheader.unRCols.rfl + 0.5;
	}
	RowCol rc(iLine, iCol);
	GeoRef gr;
	if (erdheader.iMapTyp == 0 || erdheader.rXcell == 0 || erdheader.rYcell == 0 )
		gr = GeoRef(rc);
	else
	{
		Coord cMin, cMax;
		cMin = Coord(erdheader.rXmap, erdheader.rYmap - erdheader.rYcell * (iLine - 1));
		cMax = Coord(erdheader.rXmap + erdheader.rXcell * (iCol - 1), erdheader.rYmap);
		gr.SetPointer(new GeoRefCorners(FileName(fnObject, ".grf", true),
			CoordSystem(), rc, false,   // center of corners
			cMin, cMax));
	}
	Domain dm;
	ValueRange vr;
	FileName fnTRL(FileIn.sName(), ".TRL", true);
	bool fUsesClasses = erdheader.iNclass > 0 && File::fExist(fnTRL);

	if (erdheader.iPack == 2)
	{
		dm = Domain("value");
		vr = ValueRange(-SHRT_MAX + 1, SHRT_MAX);
	}
	else
	{
		dm = Domain("Image");
		bool fIsLAN = fCIStrEqual(FileName(FileIn.sName()).sExt, ".LAN");
		if (!fIsLAN && File::fExist(fnTRL))
		{
			try
			{
				File fil(fnTRL);
				bool fHasDomain = false;
				fil.Seek(2048);
				if (!fil.fEof())
				{
					char str[32];           // revised 04-07-97
					char *str2;
					Array<String> sa;
					short i;
					for (i = 0; i < erdheader.iNclass; i++)
					{
						if (32 != fil.Read(32, str))     // read GIS variable: can be any descriptive string
							break;
						str2 = strtok(str, "~");
						if (str2 == 0)
							sa &= String("Undefined");
						else
							sa &= str2;
					}
					if (i > 1)
					{
						dm = Domain(fnObject, i);
						fHasDomain = true;
						for (short j = 1; j <= i; j++)    // DomainSort starts at 1!
							dm->pdc()->SetVal(j, sa[j - 1]);
					}
				}
				fil.Seek(128);
				if (!fil.fEof())
				{
					long iNrCl = erdheader.iNclass;
					if (iNrCl == 0)
						iNrCl = 256;
					
					int iOff = 1;  // it is a DomainClass
					if (!fHasDomain)
					{
						dm = Domain(fnObject, iNrCl, dmtPICTURE);
						iOff = 0;
					}
					
					Representation rp(FileName(fnObject, ".RPR"), dm);
					RepresentationClass *prc = dynamic_cast<RepresentationClass*>( rp.ptr() );
					ISTRUE(fINotEqual, prc, (RepresentationClass *)0);
					
					ByteBuf cGRB(3 * 256);
					fil.Read(3 * 256, cGRB.buf());
					
					for (short i = 0; i < iNrCl; i++)
						prc->PutColor(i + iOff, Color(cGRB[i + 256], cGRB[i], cGRB[i + 512]));
					dm->SetRepresentation(rp);
				}
			}
			catch (FileErrorObject&) { // just skip .TRL if not found
			}
		}
	}
	DomainValueRangeStruct dvs(dm, vr);
	short iBands = erdheader.iNBands;
	Array<Map> amp(iBands);
	if (iBands > 1)
	{
		FileName fnMap(fnObject, ".mpr", true);
		String sMapFile = fnObject.sFile;
		for (short j = 0; j < amp.iSize(); ++j)
		{
			fnMap.sFile = sMapFile;
			fnMap.sFile &= String("%d", j + 1);
			amp[j] = Map(FileName::fnUnique(fnMap), gr, gr->rcSize(), dvs);
			amp[j]->fErase = true;
		}
	}  
	else
	{
		amp[0] = Map(fnObject, gr, gr->rcSize(), dvs);
		amp[0]->fErase = true;
	} 
	trq.SetText(TR("Converting..."));
	if (erdheader.iPack == 2 || (erdheader.iPack == 1 && erdheader.iNclass == 256))
	{
		IntBuf ibuf(iCol);
		for (long i = 0; i < iLine; i++)
		{
			for (short j = 0; j < iBands; ++j)
			{
				if (trq.fUpdate(i, iLine))
					return;

				if (2 * iCol != FileIn.Read(2 * iCol, ibuf.buf()))
					if (i == 0)
						throw ErrorImportExport(TR("File Reading Error"));
					else
					{
						gr = GeoRef(RowCol( i, iCol ) );
						amp[j]->SetGeoRef(gr);
						getEngine()->Message(TR("Solving File size mismatch in header.").c_str(),
							TR("Importing from Erdas").c_str(), 
							MB_OK | MB_ICONEXCLAMATION);
						break;
					}
					if (fUsesClasses)
						for (long k = 0; k < iCol; k++)
							if (ibuf[k] != SHRT_MAX)
								ibuf[k]++;    // assume raw value of zero is NOT undefined value, only for class/ID
							else
								ibuf[k] = 0;  // symmetric with exportGIS MAXINT will become zero
					amp[j]->PutLineRaw(i, ibuf);
			}
		}  
	}
	else
	{
		ByteBuf bbuf(iCol);
		if (erdheader.iPack != 1)
		{
			for (long i = 0; i < iLine; i++)
			{
				for (short j = 0; j < iBands; ++j)
				{
					if (trq.fUpdate(i, iLine))
						return;

					if (iCol != FileIn.Read(iCol, bbuf.buf()))
						if (i == 0)
							throw ErrorImportExport(TR("File Reading Error"));
						else
						{
							gr = GeoRef(RowCol(i, iCol));
							amp[j]->SetGeoRef(gr);
							getEngine()->Message(TR("Solving File size mismatch in header.").c_str(),
								TR("Importing from Erdas").c_str(), 
								MB_OK | MB_ICONEXCLAMATION);
							break;
						}
						if (fUsesClasses)
							for (long k = 0; k < iCol; k++)
								bbuf[k]++;   // assume raw value of zero is NOT undefined value, only for class/ID
						amp[j]->PutLineRaw(i, bbuf);
				}  
			}
		}
		else  // nibble packed values
		{
			if (iCol % 2 == 0)    // for maps with an even number of pixels per row
			{
				for (long i = 0; i < iLine; i++)
				{
					for (short j = 0; j < iBands; ++j)
					{
						if (trq.fUpdate(i, iLine))
							return;

						if (iCol / 2 != FileIn.Read(iCol / 2, bbuf.buf()))
							if (i == 0)
								throw ErrorImportExport(TR("File Reading Error"));
							else
							{
								gr = GeoRef(RowCol(i, iCol));
								amp[j]->SetGeoRef(gr);
								getEngine()->Message(TR("Solving File size mismatch in header.").c_str(),
									TR("Importing from Erdas").c_str(), 
									MB_OK | MB_ICONEXCLAMATION);
								break;
							}
							for (long k = iCol / 2 - 1; k >= 0; k--)
							{
								bbuf[2 * k + 1] = bbuf[k] >> 4;
								bbuf[2 * k] = bbuf[k] & 0x0f;
							}
							if (fUsesClasses)
								for (long k2 = 0; k2 < iCol; k2++)
									bbuf[k2]++;   // assume raw value of zero is NOT undefined value, only class/ID
							amp[j]->PutLineRaw(i, bbuf);
					}
				}
			}  
			else   // odd number of pixel per row
			{
				short l = 0;
				for (long i = 0; i < iLine; i++)
				{
					for (short j = 0; j < iBands; ++j)
					{
						ULONGLONG loc = 128 + (j + 1) * (l * iCol) / 2;
						FileIn.Seek(loc);
						if (trq.fUpdate(i, iLine))
							return;

						if (iCol / 2 + 1 != FileIn.Read(iCol / 2 + 1, bbuf.buf()))
							if (i == 0)
								throw ErrorImportExport(TR("File Reading Error"));
							else
							{
								gr = GeoRef(RowCol(i, iCol));
								amp[j]->SetGeoRef(gr);
								getEngine()->Message(TR("Solving File size mismatch in header.").c_str(),
									TR("Importing from Erdas").c_str(), 
									MB_OK | MB_ICONEXCLAMATION);
								break;
							}
							if (l % 2 == 0)
							{
								bbuf[iCol - 1] = bbuf[iCol / 2] & 0x0f;
								for (long k = iCol / 2 - 1; k >= 0; k--)
								{
									bbuf[2 * k + 1] = bbuf[k] >> 4;
									bbuf[2 * k] = bbuf[k] & 0x0f;
								}
							}
							else
							{
								for (long k = iCol / 2; k >= 1; k--)
								{
									bbuf[2 * k] = bbuf[k] >> 4;
									bbuf[2 * k - 1] = bbuf[k] & 0x0f;
								}
								bbuf[0] = bbuf[0] >> 4;
							}
							l++;
							if (fUsesClasses)
								for (long k2 = 0; k2 < iCol; k2++)
									bbuf[k2]++;   // assume raw value of zero is NOT undefined value, only class/ID
							amp[j]->PutLineRaw(i, bbuf);
					}
				}  
			}
		}
	}
	trq.fUpdate(iLine, iLine);
	for (short j = 0; j < amp.iSize(); ++j)
		amp[j]->fErase = false;

	if (iBands > 1)
	{
		Array<FileName> afnMap(iBands);
		for (short j = 0; j < iBands; ++j)
			afnMap[j] = amp[j]->fnObj;
		MapList mpl(FileName(fnObject, ".mpl", true), afnMap);
	}
}


