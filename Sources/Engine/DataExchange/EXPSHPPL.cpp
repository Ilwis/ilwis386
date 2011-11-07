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

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\seg.h"
#include "Engine\Map\Polygon\pol.H"
#include "Engine\DataExchange\SHPFILE.H"
#include "Engine\DataExchange\DBF.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

void ImpExp::ExpPolToSHP(const FileName& fnObject, const FileName& fnFile) {
	trq.SetTitle(TR("Exporting Polygons to Arcview shapefile"));
	trq.SetText(TR("Processing..."));
	const PolygonMap polmap = PolygonMap(fnObject);
	/*
	// the following lines don't give the correct result (E4!), because the number of
	// deleted is NOT always stored in the .MPA file!!
	long iNrDel = polmap->iPolDeleted();
	long iNrPols = polmap->iFeatures();
	iNrPols = (iNrDel == iUNDEF) ? iNrPols : iNrPols - iNrDel;
	*/
	long iNrPols = polmap->iFeatures();
	if (iNrPols == 0) {
		String sMsg = String(TR("%S Map is empty, nothing has been exported").c_str(), String("ILWIS::Polygon"));
		getEngine()->Message(sMsg.c_str(),
			TR("Exporting Polygons to Arcview shapefile").c_str(), 
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	long iNrDel = polmap->iPolDeleted();
	iNrPols = (iNrDel == iUNDEF) ? iNrPols : iNrPols - iNrDel;
	if (iNrPols == 0) {
		String sMsg = String(TR("%S Map is empty, nothing has been exported").c_str(), String("ILWIS::Polygon"));
		getEngine()->Message(sMsg.c_str(),
			TR("Exporting Polygons to Arcview shapefile").c_str(), 
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	int i = 0;
	iNrPols = 0;
	ILWIS::Polygon *pol = (ILWIS::Polygon *) polmap->polFirst();
	while (pol && pol->fValid()) {
		if (pol->rArea() > 0)
			iNrPols++;
		pol = (ILWIS::Polygon *)polmap->getFeature(++i);
	}
	FileName fnSHP(fnFile, ".shp");
	FileName fnSHX(fnFile, ".shx");
	FileName fnDBF(fnFile, ".dbf");
	File fileSHP(fnSHP, facCRT);
	File fileSHX(fnSHX, facCRT);
	File fileDBF(fnDBF, facCRT);
	Shapefile_Pol shpPol(polmap, fileSHP);
	fileSHP.Write(sizeof(shpPol.FileHeader), &(shpPol.FileHeader));
	fileSHX.Write(sizeof(shpPol.FileHeader), &(shpPol.FileHeader));
	bool fTable = polmap->fTblAtt();
	dBaseDBF DBF;
	DomainValueRangeStruct dvs = polmap->dvrs();
	if (fTable)
		DBF = TableToDBF(polmap->tblAtt(), iNrPols, CHANGE_INTERNAL_SPACES);
	else
		DBF = DomainToDBF(dvs, iNrPols, CHANGE_INTERNAL_SPACES);
	fileDBF.Write(sizeof(DBF.Header), &DBF.Header);
	if (fTable)
		while (DBF.fFillDescriptor())
			fileDBF.Write(sizeof(DBF.Descriptor), &DBF.Descriptor);
	else
		fileDBF.Write(sizeof(DBF.Descriptor), &DBF.Descriptor);
	fileDBF.Write(1, &DBF.bEndDescriptor);
	long iPos = fileDBF.iLoc();
	fileDBF.Seek(0);
	fileDBF.Write(sizeof(DBF.Header), &DBF.Header);
	fileDBF.Seek(iPos);
	long iPolCnt = 0;
	try {
		int i = 0;
		pol = CPOLYGON(polmap->getFeature(i));
		while (pol&& pol->fValid()) {
			if (pol->rArea() > 0) {
				shpPol.Update(pol);
				fileSHX.Write(sizeof(shpPol.IndexRecord), &(shpPol.IndexRecord));
				if (dvs.fUseReals() && dvs.fValues())
					DBF.FillRealRecord(pol->rValue());
				else
					DBF.FillRecord(pol->iValue());
				fileDBF.Write(DBF.Header.iszRecord, &DBF.carRecord[0]);
				if (trq.fUpdate(iPolCnt++, iNrPols))
					return;
			}
			pol = CPOLYGON(polmap->getFeature(++i));
		}
		fileDBF.Write(1, &DBF.bEndFile);
		fileSHP.Seek(0);
		fileSHX.Seek(0);
		shpPol.SetFileHeader();
		fileSHP.Write(sizeof(shpPol.FileHeader), &(shpPol.FileHeader));
		shpPol.SetIndexFileHeader();
		fileSHX.Write(sizeof(shpPol.FileHeader), &(shpPol.FileHeader));
		trq.fUpdate(iNrPols, iNrPols);

		// if map contains a coordinate system export this to a .prj file
		if (polmap->cs().fValid()) {
			FileName fnPRJ(fnFile, ".prj");
			ExportEsriPrj(polmap->cs()->fnObj, fnPRJ);
		}
	}
	catch (ErrorObject& err) {
		//  catch (ErrorTooMuchPoints& err) {
		fileSHP.SetErase(true);
		fileSHX.SetErase(true);
		fileDBF.SetErase(true);
		err.Show();
	}
}


