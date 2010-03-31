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
#include "Engine\Scripting\InstAggregateValue.h"
#include "Engine\Scripting\Instrucs.h"
#include "Engine\Map\Polygon\POL.H"
//#include "Applications\Polygon\POLNUMB.H"
#include "Engine\Table\Rec.h"

InstAggregateValue::InstAggregateValue(Instructions* instrucs, const Map& mpInp, const String& sAttribC, const String& sOper, const Map& mpA) :
	InstBase(instrucs),
	mpInput(mpInp),
	mpAdditional(mpA),
	sOperation(sOper),
	rPower(rUNDEF),
	sAttribColumn(sAttribC)
{
	if (mpAdditional.fValid())
		m_iNrParams = 3;
	else
		m_iNrParams = 2;
}

InstAggregateValue::InstAggregateValue(Instructions* instrucs, const Map& mpInp, const String& sAttribC, const String& sOper, double power) :
	InstBase(instrucs),
	sOperation(sOper),
	mpInput(mpInp),
	mpAdditional(Map()),
	rPower(power),
	sAttribColumn(sAttribC)
{
		m_iNrParams = 3; // in this constructor, the caller (CodeGenerator::AddInstAggregateValue) has confirmed that "power" is on the stack
}

bool InstAggregateValue::fValidOperation(const String& sOper)
{
	if (fCIStrEqual(sOper, "average") ||
		fCIStrEqual(sOper, "sum") ||
		fCIStrEqual(sOper, "maximum") ||
		fCIStrEqual(sOper, "minimum") ||
		fCIStrEqual(sOper, "shapeindex") ||
		fCIStrEqual(sOper, "connectivityindex") )
		return true;
	return false;

}

bool InstAggregateValue::fMatchDomain(const String& sOper, const Domain& dm)
{
	if (fCIStrEqual(sOper, "average") ||
		fCIStrEqual(sOper, "sum") ||
		fCIStrEqual(sOper, "maximum") ||
		fCIStrEqual(sOper, "minimum"))
	{
		if ( dm->pdv() )
			return true;
	}
	if ( fCIStrEqual(sOper, "shapeindex") ||
		fCIStrEqual(sOper, "connectivityindex") )	
	{
		if ( dm->pdsrt() )
			return true;
	}
	return false;
	
}

long InstAggregateValue::dmtValidDomainTypes(const String& sOper)
{
	if (fCIStrEqual(sOper, "average") ||
		fCIStrEqual(sOper, "sum") ||
		fCIStrEqual(sOper, "maximum") ||
		fCIStrEqual(sOper, "minimum"))
	{
		return 0x04 | 0x100; //note: these values come from objlist.h which i will not include here
	}
	if ( fCIStrEqual(sOper, "shapeindex") ||
		fCIStrEqual(sOper, "connectivityindex") )	
	{
		return 0x01 | 0x02;
	}
	return 0;	
}

void InstAggregateValue::Exec()
{
	mpStart = mpInput;
	FileName fnSuitable(FileName::fnUnique(mpStart->fnObj));
	if ( sAttribColumn != "")
		mpStart = mpCalcAttributeMap();
		

	String sExpr = sMakeExpression(fnSuitable);
	BaseMap mpSuitable = mpCalculateSuitableMap(sExpr);

	double rRes;
	if ( fCIStrEqual(sOperation, "connectivityindex") )
		rRes = rCalculateResult(mpSuitable->tblAtt(), mpSuitable);
	else
	{
		Table tblHistogram= tblCalculateHistogram(mpSuitable);
		rRes = rCalculateResult(tblHistogram, mpSuitable);		
	}
	// We came here as a result of a 2 or 3 parameter call of AggregateValue(...)
	// The fist parameter (the map's filename) is already popped from stkCalc
	// Therefore, at this point stkCalc still contains the one 1 or 2 remaining parameters
	// The first of the two parametes is the operation (a StackObject of type sotStringVal)
	// The second parameter is optional, and can either be the power, or an additional map
	// In both cases, the second parameter is a StackObject of type sotLongVal
	// If available, the second parameter is on-top, thus first to be popped.
	// The member variable m_iNrParams is supposed to contain the correct total number of parameters (2 or 3)

	if (!inst->stkCalc.fEmpty())
	{
		StackObject* so = inst->stkCalc.pop();
		delete so;
	}
	if ((m_iNrParams == 3) && !inst->stkCalc.fEmpty())
	{
		StackObject* so = inst->stkCalc.pop();
		delete so;
	}

	StackObjectType sot = sotRealVal;
	DomainValueRangeStruct dvs(-1.0e300, 1.0e300, 0.00001);
	StackObject* soRes = new StackObject(sot, dvs, inst->env.iBufSize);
  for (long i=0; i < inst->env.iBufSize; i++)
    (*soRes->rBuf)[i] = rRes;

	inst->stkCalc.push(soRes);
}

Map InstAggregateValue::mpCalcAttributeMap()
{
	String sExpr("%S.%S", mpStart->fnObj.sRelativeQuoted(false), sAttribColumn);
	Map mpAttrib(FileName::fnUnique(mpStart->fnObj), sExpr); 
	mpAttrib->Calc();
	mpAttrib->fErase = true;
	return mpAttrib;
}

// calculates an expression needed to create a intermediate map that can be
// used to retrieve values for the retrieval of the AggregateValue number
String InstAggregateValue::sMakeExpression(const FileName& fnSuitable)
{
	String sExpr;
	if ( mpAdditional.fValid())
	{
		if ( mpAdditional->dm()->pdbool() ) // boolean masl
		{
			sExpr = String("iff(%S, %S, %c)", mpAdditional->fnObj.sRelativeQuoted(), mpStart->fnObj.sRelativeQuoted(), '?');
		}
		else if (mpAdditional->dm()->pdv()) //weight map
		{
			sExpr = String("%S * %S", mpStart->fnObj.sRelativeQuoted() , mpAdditional->fnObj.sRelativeQuoted());
		}
	}
	else if ( fCIStrEqual(sOperation,"shapeindex") )
	{
		sExpr = String("PolygonMapFromRas(%S)", mpStart->fnObj.sRelativeQuoted());
	}
	else if (fCIStrEqual(sOperation,"connectivityindex"))
	{
		sExpr = String("MapAreaNumbering(%S, 8)", mpStart->fnObj.sRelativeQuoted());
	}
	return sExpr;
}

// all (until now) AggregateValue expressions need data from the histogram
// to calculate the final result
Table InstAggregateValue::tblCalculateHistogram(BaseMap& mpSuitable)
{
	String sExpr;
	if (fCIStrEqual("shapeindex", sOperation))
		sExpr = String("TableHistogramPol(%S)", mpSuitable->fnObj.sRelativeQuoted());
	else
		sExpr = String("TableHistogram(%S)", mpSuitable->fnObj.sRelativeQuoted());

	String sName("%S.his", mpSuitable->fnObj.sRelative(false));
    Table tblHistogram;
	if ( Table::pGet(FileName(sName)) == 0 ) // already open?
		tblHistogram = Table(FileName(sName), sExpr);
	else
		tblHistogram.SetPointer(Table::pGet(FileName(sName)));
	if (!tblHistogram->fUpToDate())
	{
		tblHistogram->Calc();
		tblHistogram->Store();
	}
	return tblHistogram;

}

void InstAggregateValue::CalcAverageMinAndMax(BaseMap& mpSuitable, const Table& tbl, double& rAverage, double& rSum, double& rMax, double& rMin)
{
	bool fImage = mpSuitable->dm()->dmt() == dmtIMAGE;
	RealBuf bufValues;		
	if ( fImage )
	{
		bufValues.Size(256);
		for(int i=0; i <= 255; ++i)			
			bufValues[i] = i;
	}
	else
	{
		bufValues.Size(tbl->iRecs());
		tbl->col("value")->GetBufVal(bufValues, tbl->iOffset());
	}
	LongBuf bufNpix;
	bufNpix.Size(bufValues.iSize());
	tbl->col("npix")->GetBufVal(bufNpix, tbl->iOffset());
	double rTotalPix = tbl->col("npix")->rSum();
	double rTotal = 0.0;
	rMin=rUNDEF;
	rMax = rUNDEF;
	for(int i= 0; i < bufValues.iSize(); ++i)
	{
		if ( bufValues[i] != rUNDEF && bufNpix[i] > 0) // if no pixels have the value or it the value is undef it will be ignored
		{
			rTotal += bufValues[i] * bufNpix[i];
			if (rMin == rUNDEF || bufValues[i] < rMin )
				rMin = bufValues[i];
			if (rMax == rUNDEF || bufValues[i] > rMax )
				rMax = bufValues[i];
		}		
	}
	if (rTotalPix > 0)
	{
		rAverage = rTotal / rTotalPix;
		rSum = rTotal;
	}
}
// based on an expression a intermediate map is calculated
// this map will be used to calculate the histogram
BaseMap InstAggregateValue::mpCalculateSuitableMap(const String& sExpr)
{
	BaseMap mpSuitable = mpStart;
	if ( sExpr != "" )
	{
		if ( fCIStrEqual(sOperation,"shapeindex")) // shapeindex uses a polygon map
		{			
			PolygonMap mpTemp = PolygonMap(FileName(FileName::fnUnique(mpInput->fnObj), ".mpa"), sExpr);
			mpTemp->fErase = true;
			mpTemp->Store();
			mpTemp->Calc();
			String sPolNumberExpr("PolygonMapNumbering(%S)", mpTemp->fnObj.sFullPathQuoted());
			mpSuitable = PolygonMap(FileName(FileName::fnUnique(mpInput->fnObj), ".mpa"), sPolNumberExpr);
			FileName fn(FileName(mpTemp->fnObj, ".tbt"));
			Table tbl(fn); // table will be made by numbering and must be removed
			tbl->fErase = true;

		}			
		else
			mpSuitable = Map(FileName::fnUnique(mpInput->fnObj), sExpr);

		mpSuitable->fErase = true;
		mpSuitable->Store();
		mpSuitable->Calc();
		if ( !mpSuitable->fCalculated())
			throw ErrorObject("Haha could not do it!!!");
	
	}
	return mpSuitable;
}

// retrieve the aggregated value
double InstAggregateValue::rCalculateResult(Table& tbl, BaseMap& mpSuitable)
{
	if ( !tbl.fValid())
		return rUNDEF;
	
	double rAverage = rUNDEF, rSum = rUNDEF, rMax = rUNDEF, rMin = rUNDEF	;
	if ( !( fCIStrEqual("shapeindex", sOperation) || fCIStrEqual("connectivityindex", sOperation)))
		CalcAverageMinAndMax(mpSuitable, tbl,  rAverage, rSum, rMax, rMin);
	
	if ( fCIStrEqual("average", sOperation))
	{
		if ( mpStart->dm()->pdbool() && tbl->recLast().sValue(0).fVal()) // special case for bool, average of bool is fraction of all trues
			return tbl->recLast().sValue(2).rVal() / 100.0;
		return rAverage;
	}
	else if ( fCIStrEqual("sum", sOperation) ) 
	{
		if ( mpStart->dm()->pdbool() && tbl->recLast().sValue(0).fVal()) // special case for bool, sum is total number of trues
			return tbl->recLast().sValue(1).rVal();
		return rSum;
	}
	else if ( fCIStrEqual("minimum", sOperation) ) 
	{
		return rMin;
	}
	else if ( fCIStrEqual("maximum", sOperation) ) 
	{
		return rMax;
	}
	else if (fCIStrEqual("shapeindex", sOperation) )
	{

		return rShapeIndex(tbl);
		
	}
	else if (fCIStrEqual("connectivityindex", sOperation) )
	{
		return rConnectivityIndex(tbl);
	}

	return rUNDEF;
}

double InstAggregateValue::rShapeIndex(Table& tblHistogram)
{
	double rShapeIndexValue = 0.0;
	DomainValueRangeStruct dvs = tblHistogram->col("area")->dvrs();
	DomainValueRangeStruct dvs2(dvs.rrMinMax().rLo(), dvs.rrMinMax().rHi(), 0.0);
	tblHistogram->col("area")->SetDomainValueRangeStruct(dvs2);
	for(int i=1; i <= tblHistogram->iRecs(); ++i)
	{
		Ilwis::Record rec = tblHistogram->rec(i);
		rShapeIndexValue += rCalcShapeIndex(rec.sValue(1,8).rVal(), rec.sValue(2,8).rVal());
	}
	return rShapeIndexValue;
}

double InstAggregateValue::rCalcShapeIndex(double rPerimeter, double rArea)
{
	if ( rArea == 0.0)
		return 0.0;
	return rPerimeter / sqrt(rArea);
}

double InstAggregateValue::rConnectivityIndex(Table& tblAtt)
{
	double rConnectivityIndexValue = 0.0;
	DomainValueRangeStruct dvs = tblAtt->col("area")->dvrs();
	DomainValueRangeStruct dvs2(dvs.rrMinMax().rLo(), dvs.rrMinMax().rHi(), 0.0);
	tblAtt->col("area")->SetDomainValueRangeStruct(dvs2);

	double rAreaSum = 0.0;
	double rAreaSumPower = 0.0;

	for(int i=1; i <= tblAtt->iRecs(); ++i)
	{
		Ilwis::Record rec = tblAtt->rec(i);
		double rArea = rec.sValue(2,8).rVal();
		rAreaSumPower += pow(rArea, rPower);
		rAreaSum += rArea;
	};

	if ( rAreaSum == 0.0 )
		return rUNDEF;

	double rResult = (pow(rAreaSumPower, 1.0/rPower) * 100 / rAreaSum) / tblAtt->iRecs();

	if (rResult < 1.7e+308)
		return rResult;
	else
		return rUNDEF;
}


