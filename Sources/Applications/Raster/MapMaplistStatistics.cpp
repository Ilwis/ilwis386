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
/* $Log: /ILWIS 3.0/RasterApplication/MapMaplistStatistics.cpp $
 * 
 * 6     22-09-06 14:48 Retsios
 * For functions sum, min and max, use the stepsize of the first band
 * (instead of stepsize 0.1 that corresponds to the default value domain).
 * For function cnt, use stepsize 1.0.
 * 
 * 5     12-05-06 15:19 Retsios
 * Added sum and cnt to the maplist statistics functions
 * 
 * 4     9-06-05 13:39 Retsios
 * - Stddev: use abs value in formula to prevent sqrt of negative values
 * (which result in UNDEF stddev). The negative values are only due to
 * machine calc inaccurracy.
 * - Increase the range and precision of the temporary maps to double.
 * - Set the domainvaluerange of the output map when this is known (after
 * several calculations in fFreezing).
 * - Tranquilizer now covers the outer loop.
 * 
 * 3     21-04-05 16:43 Willem
 * - for std function now correct buffers are read (failed to read second
 * buffer)
 * - for min, max function initialization uses UNDEF values to calculate
 * proper result
 * - removed support for class/id domain
 * - output domain is now calculated based on the input value range and
 * the operation: min,max: output domain is identical to input domain.
 * avg,std output domain uses the same valuerange as input but uses true
 * values (step = 0). med uses the same valuerange as input but adds one
 * decimal (in case bandcount is even this becomes important) 
 * 
 * 2     21-04-05 13:01 Willem
 * Added new application: MapMaplistStatistics
 * 
 * 1     19-04-05 12:25 Willem
 * Create
 *
 * MapMaplistStatistics
   Copyright Ilwis System Development ITC
   April 2005, Willem Nieuwenhuis
*/
#include "Applications\Raster\MapMaplistStatistics.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Table\TBLHIST.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Engine\Base\Algorithm\Qsort.h"

IlwisObjectPtr * createMapMaplistStatistics(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapMaplistStatistics::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapMaplistStatistics(fn, (MapPtr &)ptr);
}

const char* MapMaplistStatistics::sSyntax()
{
    return "MapMaplistStatistics(maplist,avg|sum|cnt|med|std|min|max)\n"
              "MapMaplistStatistics(maplist,avg|sum|cnt|med|std|min|max,startband,endband)";
}

String MapMaplistStatistics::sExpression() const
{
    return String("MapMaplistStatistics(%S, %S, %li, %li)",
        mpl->sNameQuoted(true, fnObj.sPath()),
        m_StatFunction,
        m_iStartBand, m_iEndBand);
}

MapMaplistStatistics* MapMaplistStatistics::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
    String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
    if (!fCIStrEqual(sFunc, "MapMaplistStatistics"))
        AppNameError(fn, sFunc);

    Array<String> as;
    int  iParms = IlwisObjectPtr::iParseParm(sExpr, as);
    // First check the correct parameter count
    if ((iParms != 2) && (iParms != 4))
        ExpressionError(sExpr, sSyntax());

    // Now check for a valid stat function
    string sStatFunction = as[1];
    if (!fCIStrEqual(sStatFunction, "avg") &&
        !fCIStrEqual(sStatFunction, "sum") &&
        !fCIStrEqual(sStatFunction, "cnt") &&
        !fCIStrEqual(sStatFunction, "med") &&
        !fCIStrEqual(sStatFunction, "std") &&
        !fCIStrEqual(sStatFunction, "min") &&
        !fCIStrEqual(sStatFunction, "max"))
        ExpressionError(sExpr, sSyntax());

    // Check for proper domain
    // dmtValue, dmtImage: all functions
    // dmtClass, dmtID, dmtUniqueID: avg, med functions
    // other domain types not allowed
    MapList mpl(as[0], fn.sPath());
    if (!mpl.fValid())
        return 0;

    Map firstBand = mpl->map(mpl->iLower());
    DomainType dmt = firstBand->dm()->dmt();
    if (dmt != dmtVALUE && dmt != dmtIMAGE)
        throw ErrorObject(String("Statistics function not applicable for this domain (%S)", mpl->map(0)->dm()->fnObj.sFileExt()));

    // Check band range available and validity
    int iStartBand = mpl->iLower();
    int iEndBand = mpl->iUpper();
    if (iParms == 4)
    {
        int iVal = as[2].iVal();
        if (iVal != iUNDEF)
        {
            iStartBand = (iVal >= mpl->iLower() && iVal <= mpl->iUpper()) ? iVal : iStartBand;
            iVal = as[3].iVal();
            if (iVal != iUNDEF)
                iEndBand = (iVal >= mpl->iLower() && iVal <= mpl->iUpper()) ? iVal : iEndBand;
        }
        if (iVal == iUNDEF)
            ExpressionError(sExpr, sSyntax());
    }

    // Now we can create the map
    return new MapMaplistStatistics(fn, p, mpl, sStatFunction, iStartBand, iEndBand);
}

MapMaplistStatistics::MapMaplistStatistics(const FileName& fn, MapPtr& p)
: MapFromMapList(fn, p)
{
    String s;
    ReadElement("MapMaplistStatistics", "Function", s);
    if (!fCIStrEqual(s, "avg") &&
        !fCIStrEqual(s, "sum") &&
        !fCIStrEqual(s, "cnt") &&
        !fCIStrEqual(s, "med") &&
        !fCIStrEqual(s, "std") &&
        !fCIStrEqual(s, "min") &&
        !fCIStrEqual(s, "max"))
            m_StatFunction = "avg";
    else
        m_StatFunction = s;

    int i;
    ReadElement("MapMaplistStatistics", "StartBand", i);
    if (i != iUNDEF)
        m_iStartBand = i;
    else
        m_iStartBand = mpl->iLower();

    ReadElement("MapMaplistStatistics", "EndBand", i);
    if (i != iUNDEF)
        m_iEndBand = i;
    else
        m_iEndBand = mpl->iUpper();

    fNeedFreeze = true;
    sFreezeTitle = "MapMaplistStatistics";
}

MapMaplistStatistics::MapMaplistStatistics(const FileName fn, MapPtr& p, const MapList& mpl,
    const string sStatFunction, const int iStartBand, const int iEndBand)
: MapFromMapList(fn, p, mpl, Domain("Image")),   // temp domain, see below
    m_StatFunction(sStatFunction), m_iStartBand(iStartBand), m_iEndBand(iEndBand)
{
		Map firstBand = mpl->map(mpl->iLower());
		Domain dm = firstBand->dm();
		ValueRange vr = firstBand->vr();
		DomainValueRangeStruct dvs(dm);
		if (fCIStrEqual(sStatFunction, "avg") ||
				fCIStrEqual(sStatFunction, "std"))
		{
				dm = Domain("value");
				vr = ValueRange(vr->rrMinMax(), 0.0);
				dvs = DomainValueRangeStruct(dm, vr);
		}
		if (fCIStrEqual(sStatFunction, "med"))
		{
				dm = Domain("value");
				vr = ValueRange(vr->rrMinMax(), 0.1);
				dvs = DomainValueRangeStruct(dm, vr);
		}
		SetDomainValueRangeStruct(dvs);

		fNeedFreeze = true;
		sFreezeTitle = "MapMaplistStatistics";

    if (!fnObj.fValid())
        objtime = objdep.tmNewest();
}

void MapMaplistStatistics::Store()
{
    MapFromMapList::Store();
    WriteElement("MapFromMapList", "Type", "MapMaplistStatistics");
    WriteElement("MapMaplistStatistics", "Function", m_StatFunction);
    WriteElement("MapMaplistStatistics", "StartBand", (long) m_iStartBand);
    WriteElement("MapMaplistStatistics", "EndBand", (long) m_iEndBand);
}

MapMaplistStatistics::~MapMaplistStatistics()
{
}

// accumulation functions
double inline addval(double sum, double value) { return sum + value; }
double inline addsqrval(double sum, double value) { return sum + value * value; }
double inline minval(double val1, double val2) { return val1 == rUNDEF ? val2 : (val1 < val2 ? val1 : val2); }
double inline maxval(double val1, double val2) { return val1 == rUNDEF ? val2 : (val1 < val2 ? val2 : val1); }
double inline cntval(double cnt, double value) { return value == rUNDEF ? cnt : (cnt + 1.0); }
// finishing up functions
double inline divideN(double sum, double N, double dummy) { return sum / N; }
double inline divideNmin1(double sum, double N, double dummy) { return sum / (N - 1); }
double inline stddevend(double sum, double sum2, double N) { return sqrt(fabs(sum2*N - sum*sum) /(N* (N - 1))); }
double inline noCalc(double val1, double val2, double val3) { return val1; }
//Sort service functions
bool CompareDoubleLess(long pos1, long pos2, void *buffer) {
    RealBuf* buf = (RealBuf*) buffer;
    return (*buf)[pos1] < (*buf)[pos2];
}
void SwapDoubles(long pos1, long pos2, void *buffer) {
    RealBuf* buf = (RealBuf*) buffer;
    double temp = (*buf)[pos2];
    (*buf)[pos2] = (*buf)[pos1];
    (*buf)[pos1] = temp;
}
//buffer functions with single result
double inline median(RealBuf& buf) {
    long size = buf.iSize();
    QuickSort(0, size - 1, CompareDoubleLess, SwapDoubles, &buf);
    if (size % 2 == 1)
        return buf[size / 2];
    else
        return (buf[size / 2] + buf[(size / 2) +1]) / 2;
}
// add the typedefs for the generic functions
typedef double (*CalcFunction2)(double v1, double v2);
typedef double (*CalcFunction3)(double v1, double v2, double v3);
typedef double (*BufCalcFunction)(RealBuf& buf);

// The generic calculation functions
CalcFunction2   pCalcFunction;
CalcFunction2   pCalcFunction2;
CalcFunction3   pEndCalcFunction;
BufCalcFunction pBufCalcFunction;

bool MapMaplistStatistics::fFreezing()
{
    RowCol rc = rcSize();
    long iCols = rc.Col;
    long iLines = rc.Row;

    trq.SetText("Calculating maplist statistics"); //SMAPTextCalcCompHist);
    trq.SetTitle(sFreezeTitle);
    trq.SetHelpTopic(htpFreeze);

    // Make sure we have the correct domain and function
    Map firstBand = mpl->map(mpl->iLower());
    DomainType dmt = firstBand->dm()->dmt();
    if (dmt != dmtVALUE && dmt != dmtIMAGE)
        throw ErrorObject(String("Statistics function not applicable for this domain (%S)", firstBand->dm()->fnObj.sFileExt()));

    // Create a temp map to accumulate all values
    Map map = Map(FileName::fnUnique("mmls.mpr"), gr(), rcSize(), Domain("value", -1e308, 1e308, 0.0));
    map->fErase = true;

    if (trq.fUpdate(0, iLines)) 
        return false;

    bool ret;
    // Setup the calc functions
    // and start the calculation
    trq.fText("Initializing buffers");
    if (fCIStrEqual(m_StatFunction, "avg")) {
        pCalcFunction = &addval;
        pEndCalcFunction = &divideN;
        clear(map, 0.0);
        ret = CalculateSingleBuffer(map);
    } else if (fCIStrEqual(m_StatFunction, "sum")) {
        pCalcFunction = &addval;
        pEndCalcFunction = &noCalc;
        clear(map, 0.0);
        ret = CalculateSingleBuffer(map);
    } else if (fCIStrEqual(m_StatFunction, "cnt")) {
        pCalcFunction = &cntval;
        pEndCalcFunction = &noCalc;
        clear(map, 0.0);
        ret = CalculateSingleBuffer(map);
    } else if (fCIStrEqual(m_StatFunction, "med")) {
        pBufCalcFunction = &median;
        ret = CalculatePixelBuffer(map);
    } else if (fCIStrEqual(m_StatFunction, "std")) {
        pCalcFunction = &addval;
        pCalcFunction2 = &addsqrval;
        pEndCalcFunction = &stddevend;
        clear(map, 0.0);
        ret = CalculateDoubleBuffer(map);
    } else if (fCIStrEqual(m_StatFunction, "min")) {
        pCalcFunction = &minval;
        pEndCalcFunction = &noCalc;
        clear(map, rUNDEF);
        ret = CalculateSingleBuffer(map);
    } else {  // max
        pCalcFunction = &maxval;
        pEndCalcFunction = &noCalc;
        clear(map, rUNDEF);
        ret = CalculateSingleBuffer(map);
    }
    // Check if tranquilizer indicates user break
    if (!ret)
        return ret;

    SetDVRS(map->rrMinMax(), firstBand->vr()->rStep());

		CreateMapStore();

		// The output map has the same domain as the input, so
    // now we have to convert the output values back to the
    // range of the input domain
    // finish up by calculating the results from the accumulated values
    RealBuf buf(iCols);
    for (int row = 0; row < iLines; ++row) {
        map->GetLineVal(row, buf);
        pms->PutLineVal(row, buf);  // takes care of the conversion
    }
    // Succeeded: keep the final result
    ptr.fErase = false;

    return true;
}

// init functions: clear temp map
void MapMaplistStatistics::clear(Map& aMap, double value)
{
    // Initialize the temp output in "map" by clearing all pixels
    RealBuf buf(aMap->iCols());
    for (int col = 0; col < aMap->iCols(); ++col)
        buf[col] = value;
    trq.SetText("Initializing");
    for (int row = 0; row < aMap->iLines(); ++row)
    {
        aMap->PutLineVal(row, buf);
        trq.fUpdate(row, aMap->iLines());
    }
}

// move through all bands one by one for all pixels, while needing one temp output
// (avg, min, max)
// the temp file  is already initialized
bool MapMaplistStatistics::CalculateSingleBuffer(Map& aMap)
{
    long iCols = aMap->iCols();
    long iLines = aMap->iLines();

    trq.SetText("Aggregate statistical values");
    RealBuf buf(aMap->iCols());
    RealBuf bufInput(iCols);
    long iNrBands = mpl->iSize();
    // Accumulate all values in a temp map
    for (int iBand = m_iStartBand; iBand <= m_iEndBand; ++iBand)
    {
        for (int row = 0; row < iLines; ++row) {
            aMap->GetLineVal(row, buf);
            mpl->map(iBand)->GetLineVal(row, bufInput);
            for (int col = 0; col < iCols; ++col)
                buf[col] = (*pCalcFunction)(buf[col], bufInput[col]);
            aMap->PutLineVal(row, buf);
        }
        if (trq.fUpdate(iBand - m_iStartBand, m_iEndBand - m_iStartBand)) 
            return false;
    }
		trq.SetText("Final statistical calculation");
    // finish up by calculating the results from the accumulated values
    for (int row = 0; row < iLines; ++row) {
        aMap->GetLineVal(row, buf);
        for (int col = 0; col < iCols; ++col)
            buf[col] = (*pEndCalcFunction)(buf[col], iNrBands, iNrBands);
        aMap->PutLineVal(row, buf);
        if (trq.fUpdate(row, iLines)) 
            return false;
    }
		return true;
}

// move through all bands one by one for all pixels, while needing two temp output
// maps (stddev)
// the first temp file  is already initialized
bool MapMaplistStatistics::CalculateDoubleBuffer(Map& aMap)
{
    long iCols = aMap->iCols();
    long iLines = aMap->iLines();
    // Create a second temp map to accumulate all values
    Map map2 = Map(FileName::fnUnique("mmlsd2.mpr"), gr(), rcSize(), Domain("value", -1e308, 1e308, 0.0));
    map2->fErase = true;

    clear(map2, 0.0);  // also clear second temp map

    RealBuf buf(iCols);
    RealBuf buf2(iCols);

    trq.SetText("Aggregate statistical values");
    RealBuf bufInput(iCols);
    long iNrBands = mpl->iSize();
    // Accumulate all values in the temp maps
    for (int iBand = m_iStartBand; iBand <= m_iEndBand; ++iBand)
    {
        for (int row = 0; row < iLines; ++row) {
            aMap->GetLineVal(row, buf);
            map2->GetLineVal(row, buf2);
            mpl->map(iBand)->GetLineVal(row, bufInput);
            for (int col = 0; col < iCols; ++col) {
                buf[col] = (*pCalcFunction)(buf[col], bufInput[col]);
                buf2[col] = (*pCalcFunction2)(buf2[col], bufInput[col]);
            }
            aMap->PutLineVal(row, buf);
            map2->PutLineVal(row, buf2);
        }
        if (trq.fUpdate(iBand - m_iStartBand, m_iEndBand - m_iStartBand)) 
            return false;
    }
    trq.SetText("Final statistical calculation");
    // finish up by calculating the results from the accumulated values
    for (int row = 0; row < iLines; ++row) {
        aMap->GetLineVal(row, buf);
        map2->GetLineVal(row, buf2);
        for (int col = 0; col < iCols; ++col)
            buf[col] = (*pEndCalcFunction)(buf[col], buf2[col], iNrBands);
        aMap->PutLineVal(row, buf);
        if (trq.fUpdate(row, iLines)) 
            return false;
    }
		return true;
}
// move through all bands one pixel at a time with one  temp output
// the temp file  does not require initialization
bool MapMaplistStatistics::CalculatePixelBuffer(Map& aMap)
{
    trq.SetText("Calculate pixel statistics");
    long iCols = aMap->iCols();
    long iLines = aMap->iLines();
    RealBuf buf(mpl->iSize());
    // Accumulate all values in the temp map
    for (int row = 0; row < iLines; ++row) {
        for (int col = 0; col < iCols; ++col) {
            // collect all the band values for the pixel at row, col
            for (int iBand = m_iStartBand; iBand <= m_iEndBand; ++iBand)
                buf[iBand - m_iStartBand] = mpl->map(iBand)->rValue(RowCol(row, col) );

            double res = (*pBufCalcFunction)(buf);

            aMap->PutVal(RowCol(row, col), res);
        }
        if (trq.fUpdate(row, iLines))
            return false;
    }
    return true;
}

void MapMaplistStatistics::SetDVRS(RangeReal & rrMinMax, double _rStep)
{
		// First set domain none, to force removal of current domain (possibly with the wrong storetype)
		DomainValueRangeStruct dvrsNone("none");
		SetDomainValueRangeStruct(dvrsNone);
		
		// Calculate the correct stepsize
		// default: use the same stepsize as the first band (functions affected: sum, min, max)
		double rStep = _rStep;
		// for the following functions a different stepsize is required
		if (fCIStrEqual(m_StatFunction, "avg") ||	fCIStrEqual(m_StatFunction, "std"))
				rStep = 0.0;
		else if (fCIStrEqual(m_StatFunction, "med"))
				rStep = 0.1;
		else if (fCIStrEqual(m_StatFunction, "cnt"))
				rStep = 1.0;

		// Then set the domain again, but now with the correct storetype
		Domain dm ("value");
		ValueRange vr (rrMinMax, rStep);
		DomainValueRangeStruct dvs = DomainValueRangeStruct(dm, vr);
		SetDomainValueRangeStruct(dvs);
}
