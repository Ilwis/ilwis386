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
// $Log: /ILWIS 3.0/Calculator/Fncsmsc2.cpp $
 * 
 * 9     28-06-04 12:40 Hendrikse
 * corrected the equation double rStd = sqrt((n * rSum2 - sqr(rSum)) / n /
 * (n-1)); in the function ttestcol_so (for the Student t test statistic);
 * a division by n was forgotten. 
 * 
 * 8     11/07/01 2:05p Martin
 * function cnt also works on domain sort now
 * 
 * 7     30-10-01 18:33 Koolhoven
 * in stdevcol_so() correct for impossible sqrt of negative value
 * 
 * 6     11/28/00 12:47p Martin
 * prevent overflow of undef in sec_so
 * 
 * 5     8-05-00 18:18 Hendrikse
 * now for dms function on single angles (no lat or lon) the new 3
 * parameter version of sDegree is used:
 * LatLon::sDegree(buf[i],15,true);//fShowMinus == true so that minus sign
 * is preserved
 * 
 * 4     4/04/00 4:20p Wind
 * added function cnt for entire columns that counts all records that are
 * not undef
 * 
 * 3     4/04/00 1:55p Wind
 * added functions dms, sec, lat, lon, degree for conversion between
 * strings and degrees/seconds/lat/lon
 * 
 * 2     10/13/99 1:18p Wind
 * comment
*/
// Revision 1.3  1998/10/08 15:45:02  Wim
// Added name_so() and code_so() to convert the items in a domain sort
// to a string as name resp. code.
//
// Revision 1.2  1998-09-16 18:30:36+01  Wim
// 22beta2
//
// Revision 1.1  1997/09/01 17:32:33  Wim
// Initial revision
//

#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\DataObjects\Buf.h"
#include "Engine\Scripting\Fncsmisc.h"
#include "Engine\Table\Col.h"
#include "Engine\Domain\dmsort.h"

void avgcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rAvg = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rAvg += r;
  }
  if (n > 0)
    rAvg /= n;
  else
    rAvg = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rAvg;
  soRes->PutVal(bufRes);
}

void sumcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSum = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rSum += r;
  }
  if (n == 0)
    rSum = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rSum;
  soRes->PutVal(bufRes);
}

void cntcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  LongBuf bufRes;
  bufRes.Size(soRes->iSize());
  long n=0;
	if ( col->dm()->pdsrt() )
	{
		for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) 
		{
			long iRaw = col->iRaw(j);
			if ( iRaw == iUNDEF )
				continue;
			++n;

		}			
	}
	else
	{
	  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) 
		{
	    double r = col->rValue(j);
	    if (r == rUNDEF)
	      continue;
	    ++n;
	  }
	}
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = n;
  soRes->PutVal(bufRes);
}

void stdevcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSum = 0, rSum2 = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rSum += r;
    rSum2 += r*r;
  }
  double rStd;
  if (n > 1) 
  {
    double r2 = (n * rSum2 - sqr(rSum)) / n / (n-1);
    if (r2 > 0)
      rStd = sqrt(r2);
    else
      rStd = 0;
  }
  else
    rStd = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rStd;
  soRes->PutVal(bufRes);
}

void varcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSum = 0, rSum2 = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rSum += r;
    rSum2 += r*r;
  }
  double rVar;
  if (n > 1)
    rVar = (n * rSum2 - sqr(rSum)) / n / (n-1);
  else
    rVar = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rVar;
  soRes->PutVal(bufRes);
}

void stderrcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSum = 0, rSum2 = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rSum += r;
    rSum2 += r*r;
  }
  double rStdErr;
  if (n > 1)
    rStdErr = sqrt((n * rSum2 - sqr(rSum)) / (n-1) / n / n);
  else
    rStdErr = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rStdErr;
  soRes->PutVal(bufRes);
}

void ttestcol_so(StackObject* soRes, const StackObject* soMu, const StackObject* soCol)
{
  RealBuf bufMu;
  soMu->GetVal(bufMu);
  double rMu = bufMu[0];
  Column col = soCol->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSum = 0, rSum2 = 0;
  long n=0;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double r = col->rValue(j);
    if (r == rUNDEF)
      continue;
    ++n;
    rSum += r;
    rSum2 += r*r;
  }
  double rT;
  if (n > 1) {
    double rAvg = rSum / n;
    double rStd = sqrt((n * rSum2 - sqr(rSum)) / n / (n-1));
    rT = (rAvg - rMu) / (rStd / sqrt((double)n));
  }
  else
    rT = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rT;
  soRes->PutVal(bufRes);
}

void chisquarecol_so(StackObject* soRes, const StackObject* soObs, const StackObject* soExp)
{
  Column colObs = soObs->colGet();
  Column colExp = soExp->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rChi2 = 0;
  bool fFound = false;
  for (long j = colObs->iOffset(); j < colObs->iOffset()+colObs->iRecs(); j++) {
    double rObs = colObs->rValue(j);
    double rExp = colExp->rValue(j);
    if ((rObs == rUNDEF) || (rExp == rUNDEF))
      continue;
    if (rExp < 1e-10) {
      rChi2 = rUNDEF;
      break;
    }
    fFound = true;
    double rTmp = rObs - rExp;
    rChi2 += rTmp * rTmp / rExp;
  }
  if (!fFound)
    rChi2 = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rChi2;
  soRes->PutVal(bufRes);
}

void covarcol_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  Column col0 = so0->colGet();
  Column col1 = so1->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSumX = 0, rSumY = 0, rSumXY = 0;
  long n = 0;
  for (long j=col0->iOffset(); j < col0->iOffset()+col0->iRecs(); j++) {
    double r0 = col0->rValue(j);
    double r1 = col1->rValue(j);
    if ((r0 == rUNDEF) || (r1 == rUNDEF))
      continue;
    ++n;
    rSumX += r0;
    rSumY += r1;
    rSumXY += r0*r1;
  }
  double rVarXY;
  if (n > 1)
    rVarXY = (n * rSumXY - rSumX * rSumY) / n / (n - 1);
  else
    rVarXY = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rVarXY;
  soRes->PutVal(bufRes);
}

void corrcol_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  Column col0 = so0->colGet();
  Column col1 = so1->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rSumX = 0, rSumY = 0, rSumX2 =0, rSumY2 = 0, rSumXY = 0;
  long n = 0;
  for (long j=col0->iOffset(); j < col0->iOffset()+col0->iRecs(); j++) {
    double r0 = col0->rValue(j);
    double r1 = col1->rValue(j);
    if ((r0 == rUNDEF) || (r1 == rUNDEF))
      continue;
    ++n;
    rSumX += r0;
    rSumX2 += r0*r0;
    rSumY += r1;
    rSumY2 += r1*r1;
    rSumXY += r0*r1;
  }
  double rCorr;
  double rVarX, rVarY, rVarXY;
  if (n > 1) {
    rVarXY = (n * rSumXY - rSumX * rSumY) / n / (n - 1);
    rVarX  = (n * rSumX2 - rSumX * rSumX) / n / (n - 1);
    rVarY  = (n * rSumY2 - rSumY * rSumY) / n / (n - 1);
    if ((rVarX > 0) && (rVarY > 0))
      rCorr = rVarXY / sqrt(rVarX * rVarY);
    else
      rCorr = rUNDEF;
  }
  else
    rCorr = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rCorr;
  soRes->PutVal(bufRes);
}

void mincol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rMin = DBL_MAX;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double rVal = col->rValue(j);
    if (rVal == rUNDEF)
      continue;
    if (rVal < rMin)
      rMin = rVal;
  }
  if (rMin == DBL_MAX)
    rMin = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rMin;
  soRes->PutVal(bufRes);
}
void maxcol_so(StackObject* soRes, const StackObject* so)
{
  Column col = so->colGet();
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  double rMax = -DBL_MAX;
  for (long j=col->iOffset(); j < col->iOffset()+col->iRecs(); j++) {
    double rVal = col->rValue(j);
    if (rVal == rUNDEF)
      continue;
    if (rVal > rMax)
      rMax = rVal;
  }
  if (rMax == -DBL_MAX)
    rMax = rUNDEF;
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = rMax;
  soRes->PutVal(bufRes);
}

void ndvi_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal) {
    RealBuf buf0, buf1;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (int i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = (buf1[i]-buf0[i]) / (buf0[i] + buf1[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (int i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = ((double)(buf1[i]-buf0[i])) / (buf0[i] + buf1[i]);
    soRes->PutVal(bufRes);
  }
}

void name_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  LongBuf buf;
  so->GetRaw(buf);
  DomainSort* ds = so->dvs.dm()->pdsrt();
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = ds->sNameByRaw(buf[i],0);
  soRes->PutVal(bufRes);
}

void code_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  LongBuf buf;
  so->GetRaw(buf);
  DomainSort* ds = so->dvs.dm()->pdsrt();
  for (int i=0; i<soRes->iSize(); i++)
    bufRes[i] = ds->sCodeByRaw(buf[i],0);
  soRes->PutVal(bufRes);
}

void dms_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  RealBuf buf;
  so->GetVal(buf);
  for (int i=0; i<soRes->iSize(); i++) 
    bufRes[i] = LatLon::sDegree(buf[i],15,true);//fShowMinus == true
  soRes->PutVal(bufRes);
}

void lat_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  RealBuf buf;
  so->GetVal(buf);
  for (int i=0; i<soRes->iSize(); i++) 
    bufRes[i] = LatLon::sLat(buf[i]);
  soRes->PutVal(bufRes);
}

void lon_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  RealBuf buf;
  so->GetVal(buf);
  for (int i=0; i<soRes->iSize(); i++) 
    bufRes[i] = LatLon::sLon(buf[i]);
  soRes->PutVal(bufRes);
}

void degree_so(StackObject* soRes, const StackObject* so)
{
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  StringBuf buf;
  so->GetVal(buf);
  for (int i=0; i<soRes->iSize(); i++) 
    bufRes[i] = LatLon::rDegree(buf[i]);
  soRes->PutVal(bufRes);
}

void sec_so(StackObject* soRes, const StackObject* so)
{
  RealBuf bufRes;
  bufRes.Size(soRes->iSize());
  StringBuf buf;
  so->GetVal(buf);
  for (int i=0; i<soRes->iSize(); i++)
	{
		double rV = LatLon::rDegree(buf[i]);
		if ( rV != rUNDEF )			
			bufRes[i] = rV * 3600;
		else
			bufRes[i] = rV;			
			
	}		
  soRes->PutVal(bufRes);
}

void string_so(StackObject* soRes, const StackObject* so)
{
  StringBuf bufRes;
  bufRes.Size(soRes->iSize());
  if (so->sot == sotCoordVal) {
    CoordBuf buf;
    so->GetVal(buf);
    for (int i=0; i<soRes->iSize(); i++)
      bufRes[i] = so->dvs.sValue(buf[i]);
  }
  else {
    RealBuf buf;
    so->GetVal(buf);
    for (int i=0; i<soRes->iSize(); i++)
      if (buf[i] == rUNDEF)
        bufRes[i] = sUNDEF;
      else
        bufRes[i] = so->dvs.sValue(buf[i], 0);
  }  
  soRes->PutVal(bufRes);
}

void value_so(StackObject* soRes, const StackObject* so)
{
  StringBuf buf;
  RealBuf bufRes;
  so->GetVal(buf);
  bufRes.Size(soRes->iSize());
  for (int i=0; i<soRes->iSize(); i++)
    if (buf[i] == sUNDEF)
      bufRes[i] = rUNDEF;
    else
      bufRes[i] = buf[i].rVal();
  soRes->PutVal(bufRes);
}

void dist_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  CoordBuf buf0, buf1;
  RealBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (int i=0; i<soRes->iSize(); i++) 
    if (buf0[i].fUndef() ||buf1[i].fUndef())
      bufRes[i] = rUNDEF;
    else
      bufRes[i] = rDist(buf0[i],buf1[i]);
  soRes->PutVal(bufRes);
}

void dist2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  CoordBuf buf0, buf1;
  RealBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (int i=0; i<soRes->iSize(); i++)
    if (buf0[i].fUndef() ||buf1[i].fUndef())
      bufRes[i] = rUNDEF;
    else
      bufRes[i] = rDist2(buf0[i],buf1[i]);
  soRes->PutVal(bufRes);
}

void crdx_so(StackObject* soRes, const StackObject* so0)
{
  CoordBuf buf0;
  RealBuf bufRes;
  so0->GetVal(buf0);
  bufRes.Size(soRes->iSize());
  for (int i=0; i<soRes->iSize(); i++) 
    if (buf0[i].fUndef())
      bufRes[i] = rUNDEF;
    else
      bufRes[i] = buf0[i].x;
  soRes->PutVal(bufRes);
}

void crdy_so(StackObject* soRes, const StackObject* so0)
{
  CoordBuf buf0;
  RealBuf bufRes;
  so0->GetVal(buf0);
  bufRes.Size(soRes->iSize());
  for (int i=0; i<soRes->iSize(); i++) 
    if (buf0[i].fUndef())
      bufRes[i] = rUNDEF;
    else
      bufRes[i] = buf0[i].y;
  soRes->PutVal(bufRes);
}

void clrred_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).red();
  soRes->PutVal(buf);
}

void clrgreen_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).green();
  soRes->PutVal(buf);
}

void clrblue_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).blue();
  soRes->PutVal(buf);
}

void clryellow_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).yellow();
  soRes->PutVal(buf);
}
void clrmagenta_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).magenta();
  soRes->PutVal(buf);
}
void clrcyan_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).cyan();
  soRes->PutVal(buf);
}
void clrgrey_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).grey();
  soRes->PutVal(buf);
}

void clrhue_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).hue();
  soRes->PutVal(buf);
}
void clrsat_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).sat();
  soRes->PutVal(buf);
}
void clrintens_so(StackObject* soRes, const StackObject* so0)
{
  LongBuf buf;
  so0->GetRaw(buf);
  for (int i=0; i<soRes->iSize(); i++)
    buf[i] = Color(buf[i]).intens();
  soRes->PutVal(buf);
}




