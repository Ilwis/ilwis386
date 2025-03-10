/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\RECSTRUC.H"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignFormat.h"

void ImpExp::ExpTbl(const FileName& fnObject, const FileName& fnFile, const String& sExtFormat)
{
	IlwisObject object = IlwisObject::obj(fnObject);
	if (!object->fCalculated())
		object->Calc();
	
	if (fCIStrEqual(sExtFormat, "dBase"))
		ExportDBF(fnObject, FileName(fnFile,".dbf"));
	else if (fCIStrEqual(sExtFormat, "dBaseSDF"))
		ExportSDF(fnObject, FileName(fnFile,".sdf"));
	else if (fCIStrEqual(sExtFormat, "Delimited"))
		ExportDelim(fnObject, FileName(fnFile,".txt"));
	else if (fCIStrEqual(sExtFormat, "Ilwis2"))
	{
		ForeignFormat *ff = NULL; //ForeignFormat::Create(fnFile, object, sExtFormat);
		if (ff != 0)
			ff->Store(object);
	}
	else 
	{
		throw ErrorImportExport(TR("Unknown conversion format"));
	}
}
