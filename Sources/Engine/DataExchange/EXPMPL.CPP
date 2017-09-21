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
  Export maplist 'scheduler'
  by Jelle Wind, may 1997
	Last change:  WN   15 Jun 98   11:48 am
*/

#include "Headers\toolspch.h"
#include "Engine\Base\Msm.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\SpatialReference\Gr.h"

void ImpExp::ExpMpl(const FileName& fnObject, const FileName& fnFile, const String& sExtFormat, const String& _sMethod ) {

	IlwisObject object = IlwisObject::obj(fnObject);
	if (!object->fCalculated())
		object->Calc();

	String sMethod = _sMethod.sTrimSpaces();

	if (fCIStrEqual(_sMethod, "gdal")) {
		GeoRef gr;
		FileName fnTempGrf;
		FileName fnTempMpl;
		bool fRestoreGrf = false;
		IlwisObject::iotIlwisObjectType otype = IlwisObject::iotObjectType(object->fnObj);
		if ( otype == IlwisObject::iotMAPLIST) {
			MapList mpl = MapList(object->fnObj);
			if (mpl.fValid()) {
				gr = mpl->gr();
				if (gr.fValid()) {
					if (!gr->pgc() && gr->fDependent() && gr->fNorthOriented() && gr->fLinear()) { // the gdal ILWIS driver can only handle plain GeoRefCorners
						FileName fnShort = gr->fnObj;
						fnShort.sFile = "georef"; // the gdal ILWIS driver can't handle quoted names for the georeference
						fnTempGrf = FileName::fnUnique(fnShort);
						fnTempMpl = FileName::fnUnique(object->fnObj);
						GeoRef grfTemp (fnTempGrf, String("GeoRefCorners(%li,%li,1,%g,%g,%g,%g)", gr->rcSize().Row, gr->rcSize().Col, gr->cb().cMin.x, gr->cb().cMin.y, gr->cb().cMax.x, gr->cb().cMax.y)); // cb() results by definition in a corners-of-corners GeoRef
						grfTemp->SetCoordSystem(gr->cs()); // actually the CoordinateSystem filename should also be "not quoted" for the gdal ILWIS driver (TODO)
						grfTemp->Store();
						MoveFileEx(object->fnObj.sFullPath().c_str(), fnTempMpl.sFullPath().c_str(), MOVEFILE_REPLACE_EXISTING);
						mpl->SetGeoRef(grfTemp);
						mpl->Store(); // the individual map filenames should also be "not quoted" for the gdal ILWIS driver (TODO)
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
			MoveFileEx(fnTempMpl.sFullPath().c_str(), object->fnObj.sFullPath().c_str(), MOVEFILE_REPLACE_EXISTING);
			GeoRef grfTemp(fnTempGrf);
			grfTemp->fErase = true;
		}
	}
	else if (fCIStrEqual(sExtFormat, "ErdasLAN"))
		ExportLAN(fnObject, FileName(fnFile, ".lan"));
	else
	{
		throw ErrorImportExport(TR("Unknown conversion format"));
	}
}


