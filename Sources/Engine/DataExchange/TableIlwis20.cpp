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
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\TableIlwis20.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Engine\Base\File\ObjectCopier.h"
#include "Headers\Hs\CONV.hs"
#include "Headers\Hs\Table.hs"


TableIlwis20::TableIlwis20()
{
}

TableIlwis20::TableIlwis20(const FileName& fnForgn, ForeignFormat::mtMapType _mtType)
: ForeignFormat(fnForgn.sFullPath(), _mtType)
{
}

TableIlwis20::~TableIlwis20()
{
}

void TableIlwis20::LoadTable(TablePtr* tbl)
{
}

// Nothing needs to be changed in the new ODF
void TableIlwis20::WriteODF(IlwisObject obj)
{
}

// This function relies on the fact that the pointmap details
// remain in the ODF, when the table is changed
void TableIlwis20::Store(IlwisObject obj)
{
	if (File::fExist(fnGetForeignFile())) 
	{
		getEngine()->Message(TR("File already exists, choose another name").c_str(),
                              TR("Export table to ILWIS 2").c_str(),
                              MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	TablePtr *tbl = dynamic_cast<TablePtr*>(obj.pointer());
	
	Domain dmTable = tbl->dm();
	bool fUsesInternalDom = !fCIStrEqual(dmTable->fnObj.sExt, ".dom");

	// Ilwis 3 and Ilwis 2 tables uses identical storage layout
	// therfore a copy is sufficient
	ObjectCopier::CopyFiles(tbl->fnObj, fnGetForeignFile(), true);			

	// Create a new domain if UniqueID, because this is specific to version 3 and up
	// Also change domain if it is internal in another object
	if (dmTable->pdUniqueID() || fUsesInternalDom)
	{
		FileName fnTableDom = FileName::fnUniqueShort(FileName(fnGetForeignFile(), ".dom"));
		dmTable = Domain(fnTableDom, tbl->iRecs(), dmtID, dmTable->pdsrt()->sGetPrefix());
		ObjectInfo::WriteElement("Table", "Domain", fnGetForeignFile(), dmTable);         // rewrite domain
		ObjectInfo::WriteElement("Table", "DomainInfo", fnGetForeignFile(), (char*)0);      // remove domaininfo
	}


	// Check columns for UniqueID, and also remove columns with types unknown in version 2
	bool fCoordColFound = false;
	for (long j = tbl->iCols() - 1; j >= 0; j--)  // walk backwards to handle deletions properly
	{
		// Check columns for DomainUniqueID
		Column colLoc = tbl->col(j);
		fCoordColFound = fCoordColFound || colLoc->dm()->dmt() == dmtCOORD;
		if (colLoc->dm()->dmt() == dmtUNIQUEID)
		{
			String sPrefix = colLoc->dm()->pdsrt()->sGetPrefix();
			FileName fnColDom = FileName::fnUniqueShort(FileName(colLoc->sName(), ".dom"));
			Domain dmCol(fnColDom, tbl->iRecs(), dmtID, sPrefix);
			colLoc->SetDomainValueRangeStruct(dmCol);
		}
		if (colLoc->dm()->dmt() == dmtCOORDBUF)
			tbl->RemoveCol(colLoc);
	}
	ObjectInfo::WriteElement("TableView", (char*)0, fnGetForeignFile(), (char*)0); // remove clutter

	Tranquilizer trq(TR("Export table to ILWIS 2"));
	trq.Start();
	trq.SetText(TR("Writing records"));

	if (fCoordColFound)
	{
		String sVersion;
		ObjectInfo::ReadElement("Ilwis", "Version", tbl->fnObj, sVersion);
		if (sVersion.sHead(".").iVal() > 2)
		{
			tbl->LoadData();  // make sure the table is loaded now

			long k;
			Domain dmReal("value");
			ValueRange vr(-1e8,1e8,0.001);
			// First convert coordinate columns
			for (k = 0; k < tbl->iCols(); k++)
			{
				Column colLoc = tbl->col(k);
				if (colLoc->dm()->dmt() == dmtCOORD)
				{
					String sColName = String("%S-X", colLoc->sName());
					Column colX = tbl->colNew(sColName, dmReal, vr);
					colX->SetOwnedByTable(true);
					colX->SetDescription(String("X-coord of column %S", sColName));

					sColName = String("%S-Y", colLoc->sName());
					Column colY = tbl->colNew(sColName, dmReal, vr);
					colY->SetOwnedByTable(true);
					colY->SetDescription(String("Y-coord of column %S", sColName));
					tbl->Store();  // make sure the new columns are stored

					for (long i = tbl->iOffset(); i < tbl->iOffset() + tbl->iRecs(); i++)
					{
						if (trq.fUpdate(1, tbl->iOffset() + tbl->iRecs()))
							return;
						Coord crd = colLoc->cValue(i);
						colX->PutVal(i, crd.x);
						colY->PutVal(i, crd.y);
					}
				}
			}

			// Then remove the converted Coordinate columns, starting from the end
			for (k = tbl->iCols() - 1; k >= 0; k--)
			{
				Column colLoc = tbl->col(k);
				if (colLoc->dm()->dmt() == dmtCOORD)
					tbl->RemoveCol(colLoc);
			}
		}
	} // Force a store and close of the table

	ObjectInfo::WriteElement("Ilwis", "Version", fnGetForeignFile(), "2.2");  // this should be a 2.2 Table
	ObjectInfo::WriteElement("TableView", (char*)0, fnGetForeignFile(), (char*)0);   // remove any TableView section

	trq.Stop();
}
