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
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#pragma warning( disable : 4715 )

#include <set>
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\File\COPIER.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ApplicationMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\PointMapIlwis20.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Base\File\BaseCopier.h"
#include "Engine\Base\File\ObjectCopier.h"
#include "Headers\Hs\CONV.hs"
#include "Headers\Hs\point.hs"


PointMapIlwis20::PointMapIlwis20()
{
}

PointMapIlwis20::PointMapIlwis20(const FileName& fnForgn, ForeignFormat::mtMapType _mtType)
: ForeignFormat(fnForgn.sFullPath(), _mtType)
{
}

PointMapIlwis20::~PointMapIlwis20()
{
}

void PointMapIlwis20::LoadTable(TablePtr* tbl)
{
}

// Nothing needs to be changed in the new ODF
void PointMapIlwis20::WriteODF(IlwisObject obj)
{
}

// This function relies on the fact that the pointmap details
// remain in the ODF, when the table is changed
void PointMapIlwis20::Store(IlwisObject obj)
{
	if (File::fExist(fnGetForeignFile())) 
	{
		getEngine()->Message(SDATWarnFileAlreadyExists.scVal(),
                              SPNTTitleExport20Points.scVal(),
                              MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	PointMapPtr *pntmap = dynamic_cast<PointMapPtr*>(obj.pointer());

	Domain dmMap = pntmap->dm();
	bool fInternalDom = dmMap->fnObj == pntmap->fnObj;

	FileName fnForeignAtt;
	if (pntmap->fTblAtt())
	{
		FileName fnAtt(pntmap->sTblAtt());
		if (fInternalDom)
		{
			fnForeignAtt = FileName::fnUniqueShort(FileName(fnGetForeignFile(), ".tbt"));
			ObjectCopier::CopyFiles(fnAtt, fnForeignAtt, true); 
		}
		else
			fnForeignAtt = fnAtt;

		ObjectCopier::CopyFiles(pntmap->fnObj, fnGetForeignFile(), true); 
	}
	else
		ObjectCopier::CopyFiles(pntmap->fnObj, fnGetForeignFile(), true); 
	// Create a new domain if UniqueID, because this is specific to version 3 and up
	// Also change domain if it is internal
	if (dmMap->pdUniqueID() || fInternalDom)
	{
		ObjectInfo::WriteElement("BaseMap", "Domain", fnGetForeignFile(), fnGetForeignFile());         // rewrite domain
		ObjectInfo::WriteElement("BaseMap", "DomainInfo", fnGetForeignFile(), (char*)0);      // remove domaininfo
		if (dmMap->pdUniqueID())
		{
			ObjectInfo::WriteElement("Domain", "Type", fnGetForeignFile(), "DomainIdentifier");   // ...
			ObjectInfo::WriteElement("Domain", "Class", fnGetForeignFile(), "Domain Identifier"); // ...
		}

		ObjectInfo::ReadElement("BaseMap", "Domain", fnGetForeignFile(), dmMap);  // reread the domain

		// adjust the domain of the new attribute table
		if (pntmap->fTblAtt())
		{
			ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnGetForeignFile(), fnForeignAtt);  // rewrite attribute table
			Table tblAtt(fnForeignAtt);
			tblAtt->SetDomain(dmMap);

			for (long j = 0; j < tblAtt->iCols(); j++)
			{
				// Check columns of attribute table for map domain
				Column colLoc = tblAtt->col(j);
				if (colLoc->dm()->fnObj == pntmap->fnObj)
					colLoc->SetDomainValueRangeStruct(dmMap);

				// Check columns for DomainUniqueID
				if (colLoc->dm()->dmt() == dmtUNIQUEID)
				{
					FileName fnColDom = FileName::fnUniqueShort(FileName(colLoc->sName(), ".dom"));
					Domain dmCol(fnColDom, tblAtt->iRecs(), dmtID);
					colLoc->SetDomainValueRangeStruct(dmCol);
				}
			}
			tblAtt->Updated();
		}
		ObjectInfo::WriteElement("TableView", (char*)0, fnForeignAtt, (char*)0); // remove clutter
	}

	Tranquilizer trq(SPNTTitleExport20Points);
	trq.Start();
	trq.SetText(SPNTTextStoringPoints);

	{
		Table tblPoint(fnGetForeignFile());   // Open as table
		tblPoint->LoadData();
		tblPoint->fErase = true;

		// Check the version of the pointmap: if it is already version 2
		// we don't have to translate coordinates
		String sVersion;
		ObjectInfo::ReadElement("Ilwis", "Version", pntmap->fnObj, sVersion);
		if (sVersion.sHead(".").iVal() > 2)
		{
			Domain dmReal("value");
			ValueRange vr(-1e8,1e8,0.001);
			Column colX = tblPoint->colNew("X", dmReal, vr);
			Column colY = tblPoint->colNew("Y", dmReal, vr);
			colX->SetOwnedByTable(true);
			colX->SetDescription("X-coord");
			colY->SetOwnedByTable(true);
			colY->SetDescription("Y-coord");
			tblPoint->Store();  // make sure the new columns are stored

			Column colCoords = tblPoint->col("Coordinate");

			for (long i = tblPoint->iOffset(); i < tblPoint->iOffset() + tblPoint->iRecs(); i++)
			{
				if (trq.fUpdate(1, tblPoint->iOffset() + tblPoint->iRecs()))
					return;
				Coord crd = colCoords->cValue(i);
				colX->PutVal(i, crd.x);
				colY->PutVal(i, crd.y);
			}
			tblPoint->RemoveCol(colCoords);
		}

		for (long j = 0; j < tblPoint->iCols(); j++)
		{
			// Check columns on map domain in case of internal map domain
			Column colLoc = tblPoint->col(j);
			if (colLoc->dm()->fnObj == pntmap->fnObj)
				colLoc->SetDomainValueRangeStruct(dmMap);

			// Check columns for DomainUniqueID
			if (colLoc->dm()->dmt() == dmtUNIQUEID)
			{
				FileName fnColDom = FileName::fnUniqueShort(FileName(colLoc->sName(), ".dom"));
				Domain dmCol(fnColDom, tblPoint->iRecs(), dmtID);
				colLoc->SetDomainValueRangeStruct(dmCol);
			}
		}
		
		tblPoint->fErase = false;
	} // Force a store and close of the table

	ObjectInfo::WriteElement("Ilwis", "Version", fnGetForeignFile(), "2.2");  // this should be a 2.2 PointMap
	ObjectInfo::WriteElement("TableView", (char*)0, fnGetForeignFile(), (char*)0);   // remove any TableView section

	trq.Stop();
}

CoordSystem PointMapIlwis20::GetCoordSystem()
{
	return CoordSystem();
}
