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
/* $Log: /ILWIS 3.0/Import_Export/EXPRAS.CPP $
 * 
 * 9     11/08/01 12:27p Martin
 * trimmed spaces of the method paramters to make comparing the strings
 * correct
 * 
 * 8     10-01-01 4:31p Martin
 * 
 * 7     14/11/00 10:23 Willem
 * Objects to be exported are now first calculated if needed
 * 
 * 6     24/08/00 10:54 Willem
 * Added export to Arc/Info ASCII grid (again)
 * 
 * 5     15/03/00 11:49 Willem
 * - String Comparisons are now case-insensitive
 * - A messaeg is now displayed for unsupported formats
 * 
 * 4     29/02/00 11:51 Willem
 * Added Atlas IDA raster export
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:10p Martin
 * ported files to VS
// Revision 1.4  1998/09/17 09:11:32  Wim
// 22beta2
//
// Revision 1.3  1998/06/10 13:00:10  Willem
// In case the export is called from the script and the user makes a mistake
// in the format name, and types a non-existing conversion format, ILWIS
// will now display 'unknown conversion format' instead of 'not yet implemented
//
// Revision 1.2  1997/09/26 21:48:58  Willem
// Else keyword was missing, causing the error message "not yet implemented" to show.
//
/*
  Export raster 'scheduler'
  by Willem Nieuwenhuis, oct 1996
	Last change:  WN   15 Jun 98   11:02 am
*/

#include "Headers\toolspch.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\Gr.h"

void ImpExp::ExpRas(const FileName& fnObject, const FileName& fnFile, const String& sExtFormat, const String& _sMethod )
{
	IlwisObject object = IlwisObject::obj(fnObject);
	if (!object->fCalculated())
		object->Calc();

	String sMethod = _sMethod.sTrimSpaces();
	
	if ( fCIStrEqual(sMethod, "GeoGateway"))
		ExportGeoGateway(fnObject, fnFile, sExtFormat);
	else if (fCIStrEqual(sExtFormat, "TIFF"))
		ExportGeoTiff(fnObject, FileName(fnFile,".tif"));
	else if (fCIStrEqual(sExtFormat, "ErdasGIS"))
		ExportGIS(fnObject, FileName(fnFile, ".gis"));
	else if (fCIStrEqual(sExtFormat, "ErdasLAN"))
		ExportLAN(fnObject, FileName(fnFile, ".lan"));
	else if (fCIStrEqual(sExtFormat, "BMP"))
		ExportBMP(fnObject, FileName(fnFile, ".bmp"));
	else if (fCIStrEqual(sExtFormat, "Idrisi"))
		ExportIDRISI(fnObject, FileName(fnFile, ".img"));
	else if (fCIStrEqual(sExtFormat, "IDA"))
		ExportIDA(fnObject, FileName(fnFile, ".img"));
	else if (fCIStrEqual(sExtFormat, "ASCII"))
		ExportASCII(fnObject, FileName(fnFile, ".asc"));
	else if (fCIStrEqual(sExtFormat, "ArcInfoNAS"))
		ExportNAS(fnObject, FileName(fnFile, ".asc"));
	else if (fCIStrEqual(_sMethod, "gdal")) {
		GeoRef gr;
		FileName fnTemp;
		bool fRestoreGrf = false;
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(object->fnObj);
		if ( otype == IlwisObject::iotRASMAP) {
			Map mp = Map(object->fnObj);
			if (mp.fValid()) {
				gr = mp->gr();
				if (gr.fValid()) {
					if (!gr->pgc() && gr->fDependent() && gr->fNorthOriented() && gr->fLinear()) { // the gdal ILWIS driver can only handle plain GeoRefCorners
						FileName fnShort = gr->fnObj;
						fnShort.sFile = "georef"; // the gdal ILWIS driver can't handle quoted names for the georeference
						fnTemp = FileName::fnUnique(fnShort);
						GeoRef grfTemp (fnTemp, String("GeoRefCorners(%li,%li,1,%g,%g,%g,%g)", gr->rcSize().Row, gr->rcSize().Col, gr->cb().cMin.x, gr->cb().cMin.y, gr->cb().cMax.x, gr->cb().cMax.y)); // cb() results by definition in a corners-of-corners GeoRef
						grfTemp->SetCoordSystem(gr->cs()); // actually the CoordinateSystem filename should also be "not quoted" for the gdal ILWIS driver (TODO)
						grfTemp->Store();
						mp->SetGeoRef(grfTemp);
						mp->Store();
						fRestoreGrf = true;
					}
				}
			}
		}

		String ilwDir = getEngine()->getContext()->sIlwDir();
		String gdal = ilwDir + "\\gdal_translate.exe";
		String sCmd("!%S -of %S \"%S\" \"%S\"", gdal, sExtFormat, fnObject.sFullPath(), fnFile.sFullPath());
		getEngine()->Execute(sCmd);
		if (fRestoreGrf) {
			Map mp = Map(object->fnObj);
			mp->SetGeoRef(gr);
			mp->Store();
			GeoRef grfTemp(fnTemp);
			grfTemp->fErase = true;
		}
	}
	else
	{
		throw ErrorImportExport(TR("Unknown conversion"));
	}
}


