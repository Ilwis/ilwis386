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
/* Impother.c
  Import alien rasters with header skip
  by Willem Nieuwenhuis June 7, 1996
	Last change:  JEL  18 Apr 97    5:16 pm
*/

/*
  Revision history:
  WN, 04/04/97: When the number of bands is larger than one, new names are created for 
                the Ilwis maps to have the names unique. If the new names exceeded 
                8 characters (the name part) the names were set incorrectly, resulting 
                in only one Ilwis map. Solved.
*/
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\CONV.H"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\File\objinfo.h"


#define BUFSIZE 60000

class ImpossibleError : public ErrorObject
{
public:
	ImpossibleError() : ErrorObject(WhatError(TR("Import Error"), 9999)) {}
};

/* 
  Reverse byte order to convert High-Endian (Motorola) to Low-Endian (Intel)
  (and also vice-versa, but that is not used here)
*/
void SwapLine(char* buf, long iBytesPerLine, int iBytesPerPixel)
{
	if (iBytesPerPixel == 2)
	{
		char b;
		for (long i = 0; i < iBytesPerLine; i++, i++)
		{
			b = buf[i];
			buf[i] = buf[i + 1];
			buf[i + 1] = b;
		}
	}
	else
	{
		char b0, b1;
		for (long i = 0; i < iBytesPerLine; i++, i++, i++, i++)
		{
			b0 = buf[i];
			b1 = buf[i + 1];
			buf[i] = buf[i + 3];
			buf[i + 1] = buf[i + 2];
			buf[i + 2] = b1;
			buf[i + 3] = b0;
		}
	}
}

void ImpExp::ImportOTH(const FileName& fnFile, const FileName& fnObject,
                       long iHeaderSize, long iNrBands, long iNrCols,
                       ImportRasterPixelStructure irps, int iBytesPerPixel, 
                       ImportRasterFileStructure irfs,
                       bool fUseAs, bool fByteSwap, bool fCreateMPR, const String& sDescr)
{
	bool fErase = false;
	Array<FileName> objects;
	try
	{
		trq.SetTitle(TR("Import Raster"));
		bool fCreateMapList = iNrBands > 1;
		if (iNrBands == 0)
			iNrBands = 1;
		if (iNrBands == 1)
			fCreateMPR = true;

		DomainValueRangeStruct dvs;
		if (iBytesPerPixel == 1)
			fByteSwap = false;
		switch (irps)
		{
			case irpsBIT:
				switch (iBytesPerPixel)
				{
					case 1:
						dvs.SetDomain(Domain("bool"));
						break;
					default:
						throw ImpossibleError();
				}
				break;
			case irpsBYTE:
				switch (iBytesPerPixel)
				{
					case 1:
						dvs.SetDomain(Domain("image"));
						break;
					case 2:
						dvs = ValueRange(-32766, 32767);
						break;
					case 4:
						dvs = ValueRange(-LONG_MAX+2, LONG_MAX-1);
						break;
					default:
						throw ImpossibleError();
				}
				break;
			case irpsFLOAT:
				switch (iBytesPerPixel)
				{
					case 4:
						dvs = ValueRange(-1e38, 1e38, 0);
						break;
					case 8:
						dvs = ValueRange(-1e300, 1e300, 0);
						break;
					default:
						throw ImpossibleError();
				}
				break;
			default:
				throw ImpossibleError();
		}
		objects.push_back(dvs.dm()->fnObj);

		File fileAlien(fnFile);
		long iNrLines = (fileAlien.iSize() - iHeaderSize) / (iNrBands * iNrCols);
		iNrLines /= iBytesPerPixel;
		RowCol rc = RowCol(iNrLines, iNrCols);
		GeoRef gr(rc);
		objects.push_back(gr->fnObj);

		String sImport = String("imported from %S", fnFile.sFullName());
		String sDescription;
		if (fCreateMapList && !fCreateMPR)
		{
			MapList mpl(fnObject, dvs, gr, gr->rcSize(), iNrBands, "band");
			mpl->SetUseAs(fUseAs);
			if (sDescr == "")
				sDescription = String("Maplist %S %S", mpl->fnObj.sFile, sImport);
			else
				sDescription = sDescr;
			mpl->SetDescription(sDescription);
			mpl->Updated();

			objects.push_back(mpl->fnObj);
		}

		Array<FileName> afnObj(iNrBands);
		int iDigits = ceil(log10((double)iNrBands)+0.0001);
		for (long i = 0;  i < iNrBands; ++i) 
		{
			afnObj[i] = fnObject;
			if (iNrBands > 1)
			{
				String sNr(String("%%0%ii", iDigits).c_str(), 1 + i);
				String& sFile = afnObj[i].sFile;
				sFile &= sNr;
			}
			FileName fnObj, fnData; // fnObj: ODF file for info of this band; fnData: data file for this band
			if (fCreateMPR)
			{
				afnObj[i].sExt = ".mpr";
				Map map(afnObj[i], gr, rc, dvs, mfLine, iUNDEF, fUseAs);
				if (sDescr == "")
					sDescription = String("Map %S %S", map->fnObj.sFile, sImport);
				else
					sDescription = sDescr;
				map->SetDescription(sDescription);
				fnObj = map->fnObj;
				if (!fUseAs)
				{
					fnData = fnObj;
					fnData.sExt = ".mp#";
				}
				else 
					fnData = fnFile;

				objects.push_back(map->fnObj);
			} // map is created and closed; needed for the following copy
			else
			{
				fnObj = fnObject;
				fnObj.sExt = ".mpl";
				fnObj.sSectionPostFix = String(":%li", i + 1);
				fnData = fnFile;
				fUseAs = true;
			}

			long iBytesPerLine = iNrCols*iBytesPerPixel;
			if (irps == irpsBIT) {
				iBytesPerLine /= 8;
				if (0 != iNrCols % 8)
					iBytesPerLine++;
			}
			if (irfs == irfsPIXELINTERLEAVED)
				iBytesPerLine *= iNrBands;

			if (!fUseAs)  // copy relevant data and convert data to .mp#
			{
				char* buf = new char[iBytesPerLine];
				trq.SetText(String("%S %S --> %S", TR("Copy"), fnFile.sFullName(), fnObj.sFullName()));
				trq.Start();
				File filOld(fnFile, facRO);
				File filNew(fnData, facCRT);
				switch (irfs)
				{
					case irfsBANDINTERLEAVED: 
					{
						for (long j=0; j < iNrLines; j++)
						{
							if (fErase = trq.fUpdate(j, iNrLines)) 
								break;
							filOld.Seek(iHeaderSize + j * (iNrBands*iBytesPerLine) + i * iBytesPerLine);
							filOld.Read(iBytesPerLine, buf);
							if (fByteSwap)
								SwapLine(buf, iBytesPerLine, iBytesPerPixel);
							filNew.Write(iBytesPerLine, buf);
						}
						trq.fUpdate(iNrLines, iNrLines);
						break;
					}
					case irfsBANDSEQUENTIAL: 
					{ // start at offset for map i in the input file
						filOld.Seek(iHeaderSize+i*(iNrLines*iBytesPerLine));
						for (long j=0; j < iNrLines; j++)
						{
							if (fErase = trq.fUpdate(j, iNrLines)) 
								break;
							filOld.Read(iBytesPerLine, buf);
							if (fByteSwap)
								SwapLine(buf, iBytesPerLine, iBytesPerPixel);
							filNew.Write(iBytesPerLine, buf);
						}
						trq.fUpdate(iNrLines, iNrLines);
						break;
					}
					case irfsPIXELINTERLEAVED: 
					{ // start at offset for map i in the input file
						for (long j=0; j < iNrLines; j++)
						{
							if (fErase = trq.fUpdate(j, iNrLines)) 
								break;
							for (long k=0; k < iNrCols; k++)
							{
								filOld.Seek(iHeaderSize+i*iBytesPerPixel+j*iBytesPerLine+k*iNrBands*iBytesPerPixel);
								filOld.Read(iBytesPerPixel, buf);
								if (fByteSwap)
									SwapLine(buf, iBytesPerPixel, iBytesPerPixel);
								filNew.Write(iBytesPerPixel, buf);
							}
						}
						trq.fUpdate(iNrLines, iNrLines);
						break;
					}
				}  // end switch irfs
				delete [] buf;
			}
			else  // Use as: use fnFile for data storage, adjust offset and row length
			{
				// set data file info
				ObjectInfo::WriteElement("MapStore", "Data", fnObj, fnData);
				ObjectInfo::WriteElement("MapStore", "UseAs", fnObj, fUseAs);
				ObjectInfo::WriteElement("MapStore", "Structure", fnObj, "Line");
				switch (irfs)
				{
					case irfsBANDINTERLEAVED:
					{
						ObjectInfo::WriteElement("MapStore", "StartOffset", fnObj, String("%li", iHeaderSize+i*iNrCols*iBytesPerPixel));
						ObjectInfo::WriteElement("MapStore", "RowLength", fnObj, String("%li", iNrCols*iNrBands));
						ObjectInfo::WriteElement("MapStore", "PixelInterLeaved", fnObj, bool(0));
						break;
					}
					case irfsBANDSEQUENTIAL:
					{
						ObjectInfo::WriteElement("MapStore", "StartOffset", fnObj, String("%li", iHeaderSize+i*iNrCols*iNrLines*iBytesPerPixel));
						ObjectInfo::WriteElement("MapStore", "RowLength", fnObj, String("%li", iNrCols));
						ObjectInfo::WriteElement("MapStore", "PixelInterLeaved", fnObj, bool(0));
						break;
					}
					case irfsPIXELINTERLEAVED:
					{
						ObjectInfo::WriteElement("MapStore", "StartOffset", fnObj, String("%li", iHeaderSize+i*iBytesPerPixel));
						ObjectInfo::WriteElement("MapStore", "RowLength", fnObj, String("%li", iNrCols*iNrBands));
						ObjectInfo::WriteElement("MapStore", "PixelInterLeaved", fnObj, bool(1));
						break;
					}
				}
				ObjectInfo::WriteElement("MapStore", "SwapBytes", fnObj, fByteSwap);
			}
			// StoreType on disk is probably wrong
			// because we did all copying (!fUseAs) or interpretation (fUseAs) ourselves
			// we have to correct this based on the input parameters
			String sStoreType;
			switch (irps)
			{
				case irpsBIT:
					sStoreType = "Bit";
					break;
				case irpsBYTE:
					switch (iBytesPerPixel)
					{
						case 1 : sStoreType = "Byte"; break;
						case 2 : sStoreType = "Int"; break;
						case 4 : sStoreType = "Long"; break;
						default:
							throw ImpossibleError();
					}
					break;
				case irpsFLOAT:
					switch (iBytesPerPixel)
					{
						case 4: sStoreType = "Float"; break;
						case 8: sStoreType = "Real"; break;
						default:
							throw ImpossibleError();
					}
					break;
				default:
					throw ImpossibleError();
			}
			ObjectInfo::WriteElement("MapStore", "Type", fnObj, sStoreType);
		}
		
		trq.SetText(String("%S %S", TR("Creating maplist"), fnObject.sFullName()));
		if (fCreateMapList && fCreateMPR)
		{
			MapList mpl(fnObject, afnObj);
			mpl->SetUseAs(fUseAs);
			if (sDescr == "")
				sDescription = String("Maplist %S %S", mpl->fnObj.sFile, sImport);
			else
				sDescription = sDescr;
			mpl->SetDescription(sDescription);
			mpl->Updated();
		}

		if (fErase)
		{
			// Import was interrupted; remove all intermediate stuff
			for (int i = 0; i < objects.size(); ++i)
			{
				FileName fn = objects[i];
				if (fn.fValid())
				{
					IlwisObject obj = IlwisObject::obj(fn);  // open the already created objects
					obj->fErase = true;           // make them disappear
				}
			}
		}
	}
	catch (const ErrorObject& )
	{
		// Some error occured, makes sure intermediate stuff is deleted
		for (long i = 0; i < objects.size(); ++i)
		{
			FileName fn = objects[i];
			if (fn.fValid())
			{
				IlwisObject obj = IlwisObject::obj(fn);  // open the already created objects
				obj->fErase = true;           // make them disappear
			}
		}

		throw; // rethrow
	}
	if (fErase)
		throw ErrorUserAbort();
}
