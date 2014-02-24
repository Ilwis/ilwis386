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
  Export ILWIS Table to dBase .dbf format
  by Li Fei, Feb. 96
  modified Mar. 96 by Li Fei
  ILWIS Department ITC
	Last change:  WN    6 Jan 97    2:33 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\DataExchange\DBF.H"

void ImpExp::ExportDBF(const FileName& fnObject, const FileName& fnFile)
{
	trq.SetTitle(TR("Export table to dBase DBF"));
	trq.SetText(TR("Loading Table. Please wait..."));
	trq.fUpdate(0);  // needed to display the text
	trq.SetDelayShow(false);
	Table tbl = Table(fnObject);

	TableToDBF DBF(tbl);

	File fileOut(fnFile,facCRT);
	fileOut.SetErase(true);
	
	fileOut.Write(sizeof(DBF.Header), &DBF.Header);

	trq.SetText(TR("Write Column Info"));
	int iCol = 0;
	while (DBF.fFillDescriptor())
	{
		fileOut.Write(sizeof(DBF.Descriptor), &DBF.Descriptor);
		trq.fUpdate(iCol, tbl->iCols());
		iCol++;
	}

	fileOut.Write(1, &DBF.bEndDescriptor);

	ULONGLONG iPos=fileOut.iLoc();
	fileOut.Seek(0);
	fileOut.Write( sizeof(DBF.Header), &DBF.Header);
	fileOut.Seek(iPos);

	for (short i=0; i<DBF.Header.iszRecord; i++)
		DBF.carRecord[i] = ' ';

	trq.SetText(TR("Processing..."));
	for (long iRec = DBF.iMinRec; iRec <= DBF.iMaxRec; iRec++ )
	{
		DBF.FillRecord(iRec);
		fileOut.Write(DBF.Header.iszRecord, &DBF.carRecord[0]);
		if (trq.fUpdate(iRec, DBF.iMaxRec))
			return;
	}
	fileOut.Write(1, &DBF.bEndFile);
	fileOut.SetErase(false);
}




