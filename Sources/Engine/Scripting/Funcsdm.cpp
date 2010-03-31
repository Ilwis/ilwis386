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
/* $Log: /ILWIS 3.0/Calculator/Funcsdm.cpp $
 * 
 * 15    7-01-05 13:31 Retsios
 * [bug=6566]
 * ifnotundef with 2 parameters should ignore the domain of the first
 * parameter.
 * 
 * 14    10-05-04 17:17 Retsios
 * [bug=6521] ifundef/ifnotundef now allow "undefined" as one of the
 * parameters in case they are called with 3 parameters.
 * Also some code cleanup / readability improvement.
 * 
 * 13    2/18/02 3:00p Martin
 * typo in the check routines for AddDomains (and substract). I assume the
 * the check was meant for Hi and Low of a value range and not only for
 * the low (in which case the second part of the check is quite strange)
 * 
 * 12    11/07/01 2:05p Martin
 * function cnt also works on domain sort now
 * 
 * 11    11/05/01 11:32a Martin
 * added function for checking "value domains" this will also accept color
 * domains
 * 
 * 10    22-02-01 3:48p Martin
 * ifnotundef used the same function to check if valid params were given
 * as ifundef. The functions are not symetric if they have two params so
 * the function of ifundef was appropriate. ifnotundef has now its own
 * function
 * 
 * 9     25-07-00 3:29p Martin
 * bit domain changed to bool when choosing a domain
 * 
 * 8     22-11-99 12:44 Wind
 * buf with ifundef, ifnotundef and 2 params (bug 939)
 * 
 * 7     10/13/99 5:00p Wind
 * debugged domain checking for IfUndef
 * debugged string compatible domains, to not allow values
 * 
 * 6     9/15/99 9:31a Wind
 * see previous
 * 
 * 5     9/15/99 9:17a Wind
 * change for 2.23 was copied wrongly
 * 
 * 4     15-09-99 8:33a Martin
 * added 2.23 stuff
 * 
 * 3     8/24/99 9:43a Wind
 * function BoolDomain() did not take operators with 1 operand (like
 * 'not')
 * 
 * 2     3/12/99 3:05p Martin
 * Added support for case insensitive
// Revision 1.7  1998/10/08 15:29:52  Wim
// Added fSortDomains()
//
// Revision 1.6  1998-09-16 18:30:36+01  Wim
// 22beta2
//
// Revision 1.5  1997/09/08 12:10:22  Wim
// Allow in fIffCheckDomains and fIfUndefCheckDomains() also
// color domains
//
// Revision 1.4  1997-08-26 16:44:04+02  Wim
// Added MinHalfPiToHalfPiDomain(), MinPiToPiDomain, ZeroToPiDomain()
//
// Revision 1.3  1997-08-06 10:28:15+02  Wim
// LongDomain() changed to LongPosDomain()
// it makes sure that the resulting dvs has a ValueRangeInt which only accepts positive values
//
// Revision 1.2  1997-07-25 16:38:02+02  Wim
// Changed default value range for RealDomain
// and domain name for Min1To1Domain
//
/* funcsdm.c
	Last change:  WK    8 Oct 98    4:28 pm
*/

#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\ARRAY.H"
#include "Engine\Scripting\INSTRUC.H"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Scripting\Parscnst.h"
#include "Engine\Scripting\FUNCS.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Base\DataObjects\Buf.h"


bool fDontCare(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
	return true;
}

bool fValueCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
	{
    if (!(acv[i]->dvs.fValues() || acv[i]->dvs.dm()->dmt() == dmtCOLOR) ) 
		{
      iWrongParm = i;
      return false;
    }
	}		
  return true;
}

bool fValueDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
	{
    if (!acv[i]->dvs.fValues()) 
		{
      iWrongParm = i;
      return false;
    }
	}		
  return true;
}

bool fCoordDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
    if ((acv[i]->vt != vtCOORD) && (0 == acv[i]->dvs.dm()->pdcrd())) {
      iWrongParm = i;
      return false;
    }
  return true;
}

bool fColorDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
    if ((acv[i]->vt != vtCOLOR) && (0 == acv[i]->dvs.dm()->pdcol()) && (0 == acv[i]->dvs.dm()->pdp())) {
      iWrongParm = i;
      return false;
    }
  return true;
}

bool fSortDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
    if ((acv[i]->dvs.dm()->pdsrt() == 0)) {
      iWrongParm = i;
      return false;
    }
  return true;
}

bool fStringCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  for (unsigned int i=iStartIndex; i< acv.iSize(); i++)
    if (acv[i]->dvs.fValues()) {
      iWrongParm = i;
      return false;
    }
    else if ((acv[i]->dvs.dm()->pdsrt() == 0) && (acv[i]->dvs.dm()->pds() == 0)) {
      iWrongParm = i;
      return false;
    }
  return true;
/*  if (acv[iStartIndex]->dvs.dm()->pds()) { // string
    for (int i=1+iStartIndex; i< acv.iSize(); i++)
      if ((acv[i]->dvs.dm()->pdsrt() == 0) && (acv[i]->dvs.dm()->pds() == 0)) {
        iWrongParm = i;
        return false;
      }
    return true;
  }
  if (acv[iStartIndex]->dvs.dm()->pdsrt()) { // class or id
    for (int i=1+iStartIndex; i< acv.iSize(); i++)
      if ((acv[i]->dvs.dm()->pds() == 0) && (acv[i]->dvs.dm() != acv[iStartIndex]->dvs.dm())) {
        iWrongParm = i;
        return false;
      }
    return true;
  }
  return false;*/
}

bool fStringCompatibleDomainsNoID(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  if (acv[iStartIndex]->dvs.dm()->pds()) { // string
    for (unsigned int i=1+iStartIndex; i< acv.iSize(); i++)
      if ((acv[i]->dvs.dm()->pdc() == 0) && (acv[i]->dvs.dm()->pds() == 0)) {
        iWrongParm = i;
        return false;
      }
    return true;
  }
  if (acv[iStartIndex]->dvs.dm()->pdc()) { // class
    for (int i=1+iStartIndex; i< acv.iSize(); i++)
      if ((acv[i]->dvs.dm()->pds() == 0) && (acv[i]->dvs.dm() != acv[iStartIndex]->dvs.dm())) {
        iWrongParm = i;
        return false;
      }
    return true;
  }
  return false;
}

bool fCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  if (acv[iStartIndex]->dvs.fValues()) // all should be value domains
    return fValueDomains(acv, iStartIndex, iWrongParm);
  return fStringCompatibleDomains(acv, iStartIndex, iWrongParm);
}

bool fIffCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  if (acv.iSize() != 3) {
    iWrongParm = min(3, acv.iSize());
    return false;
  }
  if (!acv[0]->dvs.fValues()) {
    iWrongParm = 0;
    return false;
  }
  if (acv[1]->dvs.dm()->pdnone() && acv[2]->dvs.dm()->pdnone()) { // both undef
    iWrongParm = 1;
    return false;
  }
  if (0 != acv[1]->dvs.dm()->pdnone())
    return true;
  if (0 != acv[2]->dvs.dm()->pdnone())
    return true;
  if (0 != acv[1]->dvs.dm()->pdcol())
    return fColorDomains(acv, 1, iWrongParm);
  return fCompatibleDomains(acv, 1, iWrongParm);
/* bool f = fCompatibleDomains(acv, 1, iWrongParm);
  if (f) // check first parm
    f = acv[0]->dvs.fValues();
  if (!f)
    iWrongParm = 0;
  return f;*/
}

bool fIfUndefCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  // ifundef can have 2 or 3 params; in case of two the first one is also put as third
  int iParmStart = 1;
  if (acv.iSize() == 2) 
    iParmStart = 0;
	else
	{
		// 3 params
		if (acv[1]->dvs.dm()->pdnone() && acv[2]->dvs.dm()->pdnone()) { // both undef
			iWrongParm = 1;
			return false;
		}
		if (0 != acv[1]->dvs.dm()->pdnone())
			return true;
		if (0 != acv[2]->dvs.dm()->pdnone())
			return true;
	}
  if (0 != acv[1]->dvs.dm()->pdcol())
    return fColorDomains(acv, iParmStart, iWrongParm);
  else
    return fCompatibleDomains(acv, iParmStart, iWrongParm);
}

bool fIfNotUndefCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  // ifundef can have 2 or 3 params; in case of two the first one is also put as third
	// Bas Retsios: 23 dec 2004. I don't know if above text is applicable at calculation, but in order to support the
	// syntax mentioned in the help, the domain of the first parameter does not have to be compatible with the 2nd.
  int iParmStart = 1;
  if (acv.iSize() != 2) 
	{
		// 3 params
		if (acv[1]->dvs.dm()->pdnone() && acv[2]->dvs.dm()->pdnone()) { // both undef
			iWrongParm = 1;
			return false;
		}
		if (0 != acv[1]->dvs.dm()->pdnone())
			return true;
		if (0 != acv[2]->dvs.dm()->pdnone())
			return true;
	}
  if (0 != acv[1]->dvs.dm()->pdcol())
    return fColorDomains(acv, iParmStart, iWrongParm);
  else
	{
		if (acv.iSize() == 2)
		{
			 if (acv[0]->dvs.dm()->pdcol() || acv[0]->dvs.dm()->pdbit() || acv[0]->dvs.dm()->pdp())
			 {
				 iWrongParm = 0;
				 return false;
			 }
			 else
				 return fCompatibleDomains(acv, iParmStart, iWrongParm);
		}			
		else
			return fCompatibleDomains(acv, iParmStart, iWrongParm);
	}		
}

bool fString1Values(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm)
{
  if (acv.iSize() <= iStartIndex)
    return true;
  if ((acv[0]->dvs.dm()->pds() == 0) && (acv[0]->dvs.dm()->pdsrt() == 0)) {
    iWrongParm = 0;
    return false;
  }
  return fValueDomains(acv,1, iWrongParm);
}

void SameDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv)
{
  dvs = acv[0]->dvs;
  if (dvs.fValues()) {
    if (dvs.fRealValues())
      sot = sotRealVal;
    else
      sot = sotLongVal;
  }
  else if (dvs.fRawAvailable())
    sot = sotLongRaw;
  else
    sot = sotStringVal;
}

static double min2(double r1, double r2)
{
  return r1 < r2 ? r1 : r2;
}

static double max2(double r1, double r2)
{
  return r1 > r2 ? r1 : r2;
}

void AddDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName())) {
    if (acv[0]->dvs.fRawIsValue() || acv[0]->dvs.dm()->pdi())
      sDomName = "value";
    else
      sDomName = acv[0]->dvs.dm()->sName();
  }
  else
    sDomName = "value";
  ValueRange vr;
  RangeReal rr0, rr1;
  rr0 = acv[0]->dvs.rrMinMax();
  rr1 = acv[1]->dvs.rrMinMax();
  double rStep = 1;
  rStep = min2(rStep, acv[0]->dvs.rStep());
  rStep = min2(rStep, acv[1]->dvs.rStep());
  vr = ValueRange(rr0.rLo()+rr1.rLo(),
                  rr0.rHi()+rr1.rHi(),rStep);
  if ((vr->riMinMax().iLo() >= 0) && (vr->riMinMax().iHi() <= 255)) 
	{
    // if one of operands has image domain and other is constant
    if (acv[0]->dvs.dm()->pdi()) {
      if (0 != dynamic_cast<CalcVarConstInt*>(acv[1].ptr)) {
        dvs = DomainValueRangeStruct(Domain("image"));
        sot = sotLongVal;
        return;
      }  
    }
    if (acv[1]->dvs.dm()->pdi()) {
      if (0 != dynamic_cast<CalcVarConstInt*>(acv[0].ptr)) {
        dvs = DomainValueRangeStruct(Domain("image"));
        sot = sotLongVal;
        return;
      }  
    }
  }  
  dvs = DomainValueRangeStruct(Domain(sDomName), vr);
  sot = dvs.fRealValues() ? sotRealVal : sotLongVal;
}

void SubtractDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                            FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName())) {
    if (acv[0]->dvs.fRawIsValue() || acv[0]->dvs.dm()->pdi())
      sDomName = "value";
    else
      sDomName = acv[0]->dvs.dm()->sName();
  }
  else
    sDomName = "value";
  ValueRange vr;
  RangeReal rr0, rr1;
  rr0 = acv[0]->dvs.rrMinMax();
  rr1 = acv[1]->dvs.rrMinMax();
  double rStep = 1;
  rStep = min2(rStep, acv[0]->dvs.rStep());
  rStep = min2(rStep, acv[1]->dvs.rStep());
  vr = ValueRange(rr0.rLo()-rr1.rHi(),
                  rr0.rHi()-rr1.rLo(), rStep);
  if ((vr->riMinMax().iLo() >= 0) && (vr->riMinMax().iHi() <= 255)) {
    // if one of operands has image domain and other is constant
    if (acv[0]->dvs.dm()->pdi()) {
      if (0 != dynamic_cast<CalcVarConstInt*>(acv[1].ptr)) {
        dvs = DomainValueRangeStruct(Domain("image"));
        sot = sotLongVal;
        return;
      }  
    }
    if (acv[1]->dvs.dm()->pdi()) {
      if (0 != dynamic_cast<CalcVarConstInt*>(acv[0].ptr)) {
        dvs = DomainValueRangeStruct(Domain("image"));
        sot = sotLongVal;
        return;
      }  
    }
  } 
  dvs = DomainValueRangeStruct(Domain(sDomName), vr);
  sot = dvs.fRealValues() ? sotRealVal : sotLongVal;
}

  
void TimesDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName = "value";
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName())) {
    if (acv[0]->dvs.fRawIsValue())
      if (0 == acv[0]->dvs.dm()->pdi())
        sDomName = acv[0]->dvs.dm()->sName();
  }    
  ValueRange vr;
  
  RangeReal rr0, rr1;
  rr0 = acv[0]->dvs.rrMinMax();
  rr1 = acv[1]->dvs.rrMinMax();
  double rStep = 1;
  rStep *= acv[0]->dvs.rStep();
  rStep *= acv[1]->dvs.rStep();
  double rLo = 1e100;
  double rHi = -1e100;
  try {
    double r;

    //checks so that no double overflows will occur.
    if (rr0.rLo() < -1e100) rr0.rLo() = -1e100;
    if (rr1.rLo() < -1e100) rr1.rLo() = -1e100;
    if (rr0.rHi() >  1e100) rr0.rHi() =  1e100;
    if (rr1.rHi() >  1e100) rr1.rHi() =  1e100;

    r = rr0.rLo() * rr1.rLo();
    if (r < rLo) rLo = r; if (r > rHi) rHi = r;
    r = rr0.rLo() * rr1.rHi();
    if (r < rLo) rLo = r; if (r > rHi) rHi = r;
    r = rr0.rHi() * rr1.rHi();
    if (r < rLo) rLo = r; if (r > rHi) rHi = r;
    r = rr0.rHi() * rr1.rLo();
    if (r < rLo) rLo = r; if (r > rHi) rHi = r;
    if (rr0.fContains(0) || rr1.fContains(0))
      if (0 < rLo) rLo = 0; if (0 > rHi) rHi = 0;
  }
  catch (const ErrorFloatingPoint&) {
    rLo = -1e100;
    rHi = 1e100;
    rStep = 0;
  }
  if (rStep == 0)    
    rStep = 0.0001;
    
  if (acv[0]->dvs.fRealValues() || acv[1]->dvs.fRealValues()) 
    vr = ValueRange(rLo, rHi, rStep);
  else if ((rLo < -LONG_MAX) || (rHi > LONG_MAX))
    vr = ValueRange(rLo, rHi, 1);
  else
    vr = ValueRange(longConv(rLo), longConv(rHi));
    
  dvs = DomainValueRangeStruct(Domain(sDomName), vr);
  sot = dvs.fRealValues() ? sotRealVal : sotLongVal;
}

void DivideDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName())) {
    if (acv[0]->dvs.dm()->pdvr())
      sDomName = acv[0]->dvs.dm()->sName();
    else
      sDomName = "value";
  }
  else
    sDomName = "value";
  ValueRange vr;
  RangeReal rr0 = acv[0]->dvs.rrMinMax();
  RangeReal rr1 = acv[1]->dvs.rrMinMax();
  double rStep1 = acv[1]->dvs.rStep();
  if (rr1.rLo() == 0)
    rr1.rLo() = rStep1;
  if (rr1.rHi() == 0)
    rr1.rLo() = -rStep1;
  double rLo, rHi;
  double rStep = rUNDEF;
  if (rr1.rLo() == rr1.rHi()) {
    if (rr1.rLo() != 0) {
      try {
        rLo = rr0.rLo() / rr1.rLo();
      }
      catch (const ErrorFloatingPoint&) {
        rLo = -1e100;
        rStep = 0;
      }
      try {
        rHi = rr0.rHi() / rr1.rHi();
      }
      catch (const ErrorFloatingPoint&) {
        rHi = 1e100;
        rStep = 0;
      }
      vr = ValueRange(rLo, rHi, 0.01);
    }
    else
      vr = ValueRange(Domain("value"));
  }
  else if (rr1.fContains(0)) {
    if (rStep1 != 1)
      vr = ValueRange(Domain("value"));
    else {
      rLo = min2(-rr0.rHi(),rr0.rLo());
      rHi = max2(-rr0.rLo(),rr0.rHi());
    }
  }
  else {
    try {
      if (rr0.rHi() >= 0)
        rHi = rr0.rHi() / rr1.rLo();
      else
        rHi = rr0.rHi() / rr1.rHi();
    }
    catch (const ErrorFloatingPoint&) {
      rHi = 1e100;
      rStep = 0;
    }
    try {
      if (rr0.rLo() >= 0)
        rLo = rr0.rLo() / rr1.rHi();
      else
        rLo = rr0.rLo() / rr1.rLo();
    }
    catch (const ErrorFloatingPoint&) {
      rLo = -1e100;
      rStep = 0;
    }
  }
  if (!vr.fValid()) {
    if (rLo > rHi) {
      double r = rLo; rLo = rHi; rHi = r;
    }
    if (rStep == rUNDEF) {
      rStep = 1;
      rStep *= acv[0]->dvs.rStep();
      rStep *= acv[1]->dvs.rStep();
      if (rStep == 1)
        rStep = 0.01;
      else if (rStep == 0)
        rStep = 0.0001;
    }
    vr = ValueRange(rLo, rHi, rStep);
  }
  dvs = DomainValueRangeStruct(Domain(sDomName), vr);
  sot = sotRealVal;
}

void DivDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName()))
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  ValueRange vr;
  RangeInt ri0 = acv[0]->dvs.riMinMax();
  RangeInt ri1 = acv[1]->dvs.riMinMax();
  if (ri1.iHi() < 0) {
    long i = ri1.iHi();
    ri1.iHi() = abs(ri1.iLo());
    ri1.iLo() = abs(i);
  }
  else if (ri1.iLo()< 0)
    ri1.iLo() = 1;
  if (ri1.iLo() == 0)
    ri1.iLo() = 1;
  if (ri1.iHi() == 0)
    ri1.iLo() = -1;
  long iLo, iHi;
  if (ri1.iLo() == ri1.iHi()) {
    if (ri1.iLo() != 0) {
      iLo = ri0.iLo() / ri1.iLo();
      iHi = ri0.iHi() / ri1.iHi();
      vr = ValueRange(iLo, iHi);
    }
    else
      vr = ValueRange(-LONG_MAX+1, LONG_MAX);
  }
  else if (ri1.fContains(0)) {
    iLo = min2(-ri0.iHi(),ri0.iLo());
    iHi = max2(-ri0.iLo(),ri0.iHi());
  }
  else {
    if (ri0.iHi() >= 0)
      iHi = ri0.iHi() / ri1.iLo();
    else
      iHi = ri0.iHi() / ri1.iHi();
    if (ri0.iLo() >= 0)
      iLo = ri0.iLo() / ri1.iHi();
    else
      iLo = ri0.iLo() / ri1.iLo();
  }
  if (!vr.fValid()) {
    if (iLo >iHi) {
      long i = iLo; iLo = iHi; iHi = i;
    }
    vr = ValueRange(iLo, iHi);
  }
  dvs = DomainValueRangeStruct(Domain(sDomName), vr);
  sot = sotLongVal;
}

void ModDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName;
  if (fCIStrEqual(acv[0]->dvs.dm()->sName() , acv[1]->dvs.dm()->sName()))
    sDomName = acv[0]->dvs.dm()->sName();
  else
    sDomName = "value";
  RangeInt ri = acv[1]->dvs.riMinMax();
  dvs = DomainValueRangeStruct(Domain(sDomName), ValueRange(0, abs(ri.iHi())-1));
  sot = sotLongVal;
}

void Min1To1Domain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv)
{
  String sDomName = "min1to1";
/*
  if (acv.iSize() > 0) {
    if (acv[0]->dvs.dm()->pdvr())
      sDomName = acv[0]->dvs.dm()->sName();
    else
      sDomName = "value";
  }
  else
    sDomName = "value";
*/
  dvs = DomainValueRangeStruct(Domain(sDomName), ValueRange(-1, 1, 0));
  sot = sotRealVal;
}

void Min1To1IntDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
  sot = sotLongVal;
  dvs = DomainValueRangeStruct(Domain("min1to1"), ValueRange(-1, 1));
}

void MinHalfPiToHalfPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv)
{
  dvs = DomainValueRangeStruct(ValueRange(-M_PI_2, M_PI_2, 0));
  sot = sotRealVal;
}

void MinPiToPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv)
{
  dvs = DomainValueRangeStruct(ValueRange(-M_PI, M_PI, 0));
  sot = sotRealVal;
}

void ZeroToPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv)
{
  dvs = DomainValueRangeStruct(ValueRange(0, M_PI, 0));
  sot = sotRealVal;
}

void RealOrLongDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                 FuncMath, const Array<CalcVariable>& acv)
{
  if (acv[0]->dvs.fRealValues() || acv[0]->dvs.fRealValues()) {
//    dvs = DomainValueRangeStruct(Domain("value"));
    dvs = DomainValueRangeStruct(Domain("value"), ValueRange(-1e300,1e300,0));
    sot = sotRealVal;
  }
  else {
    dvs = DomainValueRangeStruct(Domain("value"), ValueRange(-LONG_MAX+1, LONG_MAX-1));
    sot = sotLongVal;
  }
}

void RealDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
//  dvs = DomainValueRangeStruct(Domain("value"));
  dvs = DomainValueRangeStruct(Domain("value"), ValueRange(-1e300,1e300,0));
  sot = sotRealVal;
}

void LongPosDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
  dvs = DomainValueRangeStruct(Domain("value"), ValueRange(0, LONG_MAX-1));
  sot = sotLongVal;
}

void IntDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
  dvs = DomainValueRangeStruct(Domain("value"), ValueRange(-32766, 32767));
  sot = sotLongVal;
}

void ImageDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
  dvs = DomainValueRangeStruct(Domain("image"), ValueRange());
  sot = sotLongVal;
}

void BoolDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv)
{
  //if ((acv.iSize() > 1) /* for 'not' */ && acv[0]->dvs.fRawIsValue() && acv[1]->dvs.fRawIsValue()) // no undefs
  //  dvs = DomainValueRangeStruct(Domain("bit"));
  //else
    dvs = DomainValueRangeStruct(Domain("bool"));
  sot = sotLongVal;
}

void BoolDomainFromRaws(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv)
{
  if (acv[0]->dvs.dm()->pds()) {
    CalcVariable cv = acv[0];
    cv->SetDomainValueRangeStruct(acv[1]->dvs);
    CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cv.ptr);
    if (cvs)
      if (cvs->ics)
        cvs->ics->cv->SetDomainValueRangeStruct(acv[1]->dvs);
  }
  else if (acv[1]->dvs.dm()->pds()) {
    CalcVariable cv = acv[1];
    cv->SetDomainValueRangeStruct(acv[0]->dvs);
    CalcVarConstString* cvs = dynamic_cast<CalcVarConstString*>(cv.ptr);
    if (cvs)
      if (cvs->ics)
        cvs->ics->cv->SetDomainValueRangeStruct(acv[0]->dvs);
  }
  dvs = DomainValueRangeStruct(Domain("bool"));
  sot = sotLongVal;
}

void BoolDomainFromStrings(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv)
{
//  CalcVariable cv = acv[0];
//  cv->sot = sotStringVal;
 // cv = acv[1];
 // cv->sot = sotStringVal;
  dvs = DomainValueRangeStruct(Domain("bool"));
  sot = sotLongVal;
}

void BitDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&)
{
  dvs = DomainValueRangeStruct(Domain("bool"));
  sot = sotLongVal;
}




