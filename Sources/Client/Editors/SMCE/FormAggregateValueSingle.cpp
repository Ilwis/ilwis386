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
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\SMCE\FormAggregateValue.h"
#include "Client\Editors\SMCE\FormAggregateValueSingle.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fldcol.h"
#include "Client\ilwis.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Scripting\CalculateAggregateValue.h"
#include "Headers\Hs\Appforms.hs"

FormAggregateValueSingle::FormAggregateValueSingle(CWnd *parent, const String& sCmd) :
	FormBaseDialog(parent, TR("Aggregate Values"), true, false, true)
{
	int iImg = IlwWinApp()->iImage("Calculationsingle");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

	fbs	|= fbsBUTTONSUNDER ;

	create();
}

void FormAggregateValueSingle::create()
{
	fmInput = new FieldMap(root, TR("&Raster Map"), &sOutMap);
	fldCol = new FieldColumnWithNone(root, TR("&Attribute column"), Table(), &sAttributeColumn, dmVALUE);	
	favf = new FieldAggregateValueFunc(root, TR("&Operation"), &sOperation);
	favf->SetCallBack((NotifyProc)(&FormAggregateValueSingle::HandleOperationChanges));
	fmInput->SetCallBack((NotifyProc)&FormAggregateValueSingle::HandleInputMapChanges);
	SetAdditionalBlock(root, favf, false);

	FormBaseDialog::create();
	
}

int FormAggregateValueSingle::HandleInputMapChanges(Event *)
{
	fmInput->StoreData();
	Domain dm;
	GeoRef grf;
	Map mp;	
	if ( sOutMap != "")
	{
		mp = Map(sOutMap);
		if ( mp.fValid() )
		{
			dm = mp->dm();	
			grf = mp->gr();
		}			
	}		
		
	Domain dmBoolean = Domain("bool");
	if ( dm.fValid() && dm->pdv())
		dm = Domain("value");

	if ( grf.fValid() )
	{
		ObjectLister *ol1 = new MapListerDomainAndGeoRef(grf->fnObj, grf->rcSize(), dm->fnObj);		
		fmMapWeight->SetObjLister(ol1);
		ObjectLister *ol2 = new MapListerDomainAndGeoRef(grf->fnObj, grf->rcSize(), dmBoolean->fnObj);
		fmMapBoolean->SetObjLister(ol2);
	}	
	/*else
	{
		ObjectExtensionLister *ol1 = new MapListerDomainType(dmVALUE, true);		
		ObjectExtensionLister *ol2 = new MapListerDomainType(dmVALUE, true);		
		fmMapWeight->SetObjLister(ol1);
		fmMapBoolean->SetObjLister(ol2);
    	fldCol->Hide();
	}*/
	sAttributeColumn = "";
	if ( mp.fValid())
	{
		if ( mp->fTblAtt())
		{
			Table tbl = mp->tblAtt();
			fldCol->FillWithColumns(tbl->fnObj);
			fldCol->Show();
			return 1;
		}
		else
			fldCol->Hide();
	}
	fldCol->FillWithColumns();
	return 1;	
		
}


int FormAggregateValueSingle::HandleOperationChanges(Event *)
{
	fmInput->StoreData();
	favf->StoreData();
	cbAdditional->StoreData();	
	frPower->Hide();
	rgBoolean->Hide();
	cbAdditional->Hide();
	fldCol->Hide();

	Domain dm;
	if ( sOutMap != "")
	{
		Map mp(sOutMap);
		if ( mp.fValid() )
		{
			dm = mp->dm();
	
		}			 
	}	
	if ( fCIStrEqual(sOperation,"connectivityindex") | fCIStrEqual(sOperation,"shapeindex"))
	{
		if ( dm.fValid() && !dm->pdsrt())
			
		fmInput->SetObjLister(new MapListerDomainType(dmCLASS | dmIDENT, true));

	}	
	if ( fCIStrEqual(sOperation, "connectivityindex") )
	{
		frPower->Show();
	}	
	else
	{
		fmInput->SetObjLister(new MapListerDomainType(dmVALUE, true));		
		cbAdditional->Show();		
	}		
	return 1;
}

int FormAggregateValueSingle::exec()
{
	AggregateValueOperationStruct avos;
	cbAdditional->StoreData();
	frPower->StoreData();
	rgBoolean->StoreData();
	fmMapWeight->StoreData();
	fmMapBoolean->StoreData();
	fsExpression->StoreData();
	fldCol->StoreData();
	avos.sOperation = sOperation;

	if ( fCIStrEqual(sOperation, "connectivityindex")) 	
		avos.rPower = rPower;

	if ( fAdditional )
	{
		if ( sMapName != "")
			avos.fnAdditional = FileName(sMapName);
	
		avos.sBooleanExpression = sBooleanExpression;	
	}	
	CalculateAggregateValue cav(avos);

	IlwWinApp()->Execute("?" + cav.sBuildExpression(FileName(sOutMap), sAttributeColumn));
	
	return 1;
}
