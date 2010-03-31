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
#include "Engine\Map\Raster\Map.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\Scripting\CalculateAggregateValue.h"


CalculateAggregateValue::CalculateAggregateValue(AggregateValueOperationStruct op) :
	avos(op)
{
	if ( avos.sBooleanExpression != "" )
	{
		mpAdditional = Map(FileName::fnUnique("avBooleanMask"), avos.sBooleanExpression);
		if ( mpAdditional.fValid() )
		{
			mpAdditional->fErase = true;
			mpAdditional->Calc();
		}
		else
			throw ErrorObject();
	}
	else if ( avos.fnAdditional != FileName() )
	{
		mpAdditional = Map(avos.fnAdditional);
	}
}

String CalculateAggregateValue::sBuildExpression(const FileName& fnInput, const String& sAttCol)	
{
	String sExpr = String("AggregateValue(%S",fnInput.sRelativeQuoted(false));
	if ( sAttCol != "")
		sExpr &= String(".%S", sAttCol);
			
	sExpr &= String(",\"%S\"", avos.sOperation);

	if ( fCIStrEqual(avos.sOperation, "average")  || 
		 fCIStrEqual(avos.sOperation, "sum") ||
		 fCIStrEqual(avos.sOperation, "maximum") ||
 		 fCIStrEqual(avos.sOperation, "minimum" ))
	{
		if ( mpAdditional.fValid() )
		{
			sExpr &= String(",%S", mpAdditional->fnObj.sRelativeQuoted());
	
		}
	}
	if ( avos.rPower != rUNDEF && fCIStrEqual(avos.sOperation, "connectivityindex" ))
	{	
			sExpr &= String(", %g", avos.rPower);
	}
	sExpr &= ")";	

	TRACE("%s\n", sExpr.scVal());
	return sExpr;
}

String CalculateAggregateValue::sCalculateAggregateValue(const FileName& fnInput, const String& sAttCol)
{	

	return Calculator::sSimpleCalc(sBuildExpression(fnInput, sAttCol));
}
