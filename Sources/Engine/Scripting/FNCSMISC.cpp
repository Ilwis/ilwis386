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
// $Log: /ILWIS 3.0/Calculator/FNCSMISC.cpp $
 * 
 * 8     11/07/01 3:39p Martin
 * set some guards on the string functions (no right with negative
 * numbers)
 * 
 * 7     11/07/01 3:28p Martin
 * other string functions disregard the code now also and only check the
 * name
 * 
 * 6     8-03-01 2:49p Martin
 * substr construction in the sub function could lead to GPF if indexes
 * were used outside the size of the string
 * 
 * 5     11/28/00 8:57a Martin
 * sValueByRaw  (of sorts) must have changed. It now returns code plus
 * name. I strip of the code for the left functions
 * 
 * 4     10-11-99 10:39a Martin
 * In function now is case insensitive
 * 
 * 3     10/13/99 4:20p Wind
 * comments
 * 
 * 2     10/12/99 1:10p Wind
 * solved bug 939; ifundef2_so and ifnotundef2_so now check on type of 2nd
 * parm in stead of 1st
*/
// Revision 1.5  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.4  1997/09/17 08:24:54  Wim
// iff_so: Allow non value data to be processed correctly
// found with testing color maps
//
// Revision 1.3  1997-09-01 19:32:13+02  Wim
// Split off last half to fncsmsc2.c
//
// Revision 1.2  1997-08-20 15:57:48+02  Wim
// changed iff() back to situation before 15 july.
// iff(a,b,c):
// When a is true returns b
// when a is false returns c
// when a is ? returns ?
//
/*
	Last change:  WK   17 Sep 97   10:08 am
*/
//#include "Engine\Base\DataObjects\ARRAY.H"
//#include "Engine\Base\DataObjects\strng.h"
#include "Engine\Scripting\CALCSTCK.H"
#include "Engine\Scripting\Calcvar.h"
#include "Engine\Base\DataObjects\Buf.h"
#include "Engine\Scripting\Fncsmisc.h"
#include "Engine\Table\Col.h"

void isundef_so(StackObject* soRes, const StackObject* so)
{
  if (so->sot == sotRealVal) {
    RealBuf buf;
    LongBuf bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = buf[i] == rUNDEF;
    soRes->PutVal(bufRes);
  }
  else if (so->sot == sotCoordVal) {
    CoordBuf buf;
    LongBuf bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = buf[i].fUndef();
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf, bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = buf[i] == iUNDEF;
    soRes->PutVal(bufRes);
  }
}

void isundefraw_so(StackObject* soRes, const StackObject* so)
{
  if (so->sot == sotLongRaw) {
    LongBuf buf, bufRes;
    so->GetRaw(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = buf[i] == iUNDEF;
    soRes->PutVal(bufRes);
  }
  else { // strings
    StringBuf buf;
    LongBuf bufRes;
    so->GetVal(buf);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = buf[i] == sUNDEF;
    soRes->PutVal(bufRes);
  }
}

void time2string_so(StackObject* soRes, const StackObject* so)
{
	RealBuf buf;
	StringBuf bufRes;
	so->GetVal(buf);
	bufRes.Size(soRes->iSize());
	for (short i=0; i<soRes->iSize(); i++) {
		if (buf[i] == rUNDEF || buf[i] == iUNDEF || buf[i] == shUNDEF){
			bufRes[i] = "?";
		}
		else{
			ILWIS::Time t(buf[i]);
			bufRes[i] = t.toString();
		}
	}
	soRes->PutVal(bufRes);
}

void time_so(StackObject* soRes, const StackObject* so)
{
	StringBuf buf;
	RealBuf bufRes;
	so->GetVal(buf);
	bufRes.Size(soRes->iSize());
	for (short i=0; i<soRes->iSize(); i++) {
		if (buf[i] == sUNDEF){
			bufRes[i] = 0;
		}
		else{
			bufRes[i] = (double)ILWIS::Time(buf[i]);
			//soRes->dvs.SetDomain(Domain("d:\\Data\\ILWIS\\at2.dom"));
		}
	}
	soRes->PutVal(bufRes);
}

void duration_so(StackObject* soRes, const StackObject* so)
{
	StringBuf buf;
	RealBuf bufRes;
	so->GetVal(buf);
	bufRes.Size(soRes->iSize());
	for (short i=0; i<soRes->iSize(); i++) {
		if (buf[i] == sUNDEF){
			bufRes[i] = 0;
		}
		else{
			bufRes[i] = (double)ILWIS::Duration(buf[i]);
			//soRes->dvs.SetDomain(Domain("d:\\Data\\ILWIS\\at2.dom"));
		}
	}
	soRes->PutVal(bufRes);
}

void iff_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  bool fUseValues = so1->dvs.fValues();
  if (fUseValues && (so1->sot == sotRealVal || so2->sot == sotRealVal)) {
    RealBuf buf1, buf2, bufRes;
    LongBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if (buf0[i] == iUNDEF)
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = (buf0[i] != 0) ? buf1[i] : buf2[i];
    }
    soRes->PutVal(bufRes);
  }
  else if (fUseValues && (so1->sot == sotLongVal || so2->sot == sotLongVal)) {
    LongBuf buf1, buf2, bufRes;
    LongBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if (buf0[i] == iUNDEF)
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = (buf0[i] != 0) ? buf1[i] : buf2[i];
    }
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotStringVal || so2->sot == sotStringVal) {
    StringBuf buf1, buf2, bufRes;
    LongBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if (buf0[i] == iUNDEF)
        bufRes[i] = sUNDEF;
      else
        bufRes[i] = (buf0[i] != 0) ? buf1[i] : buf2[i];
    }
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf1, buf2, bufRes;
    LongBuf buf0;
    so0->GetVal(buf0);
/*  bool fRaw = so1->sot == sotLongRaw || so2->sot == sotLongRaw;
    if (fRaw) {
      if ((so1->dvs.dm()->pdnone()) || (so2->dvs.dm()->pdnone()))
        fRaw = true;
      else
        fRaw = (so1->dvs == so2->dvs) && (so1->dvs == soRes->dvs);
    } 
    if (fRaw) {*/
      so1->GetRaw(buf1);
      so2->GetRaw(buf2);
/*  }
    else {
      so1->GetVal(buf1);
      so2->GetVal(buf2);
    }*/
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++) {
      if (buf0[i] == iUNDEF)
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = (buf0[i] != 0) ? buf1[i] : buf2[i];
    }
//  if (fRaw)
      soRes->PutRaw(bufRes);
//  else
//    soRes->PutVal(bufRes);
  }
}

void LocateUndefs(const StackObject* so0, Buf<bool>& bufIsUndef)
{
  if (so0->sot == sotLongRaw) 
  {
    LongBuf bufL;
    so0->GetRaw(bufL);
    for (short i=0; i<so0->iSize(); i++)
      bufIsUndef[i] = bufL[i] == iUNDEF;
  }  
  else if (so0->sot == sotLongVal) 
  {
    LongBuf bufL;
    so0->GetVal(bufL);
    for (short i=0; i<so0->iSize(); i++)
      bufIsUndef[i] = bufL[i] == iUNDEF;
  }  
  else if (so0->sot == sotRealVal) 
  {
    RealBuf bufR;
    so0->GetVal(bufR);
    for (short i=0; i<so0->iSize(); i++)
      bufIsUndef[i] = bufR[i] == rUNDEF;
  }  
  else if (so0->sot == sotStringVal) 
  {
    StringBuf bufS;
    so0->GetVal(bufS);
    for (short i=0; i<so0->iSize(); i++)
      bufIsUndef[i] = bufS[i] == sUNDEF;
  }
  else if (so0->sot == sotCoordVal) 
  {
    CoordBuf bufC;
    so0->GetVal(bufC);
    for (short i=0; i<so0->iSize(); i++)
      bufIsUndef[i] = bufC[i].fUndef();
  }
}  

void ifundef3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  Buf<bool> bufIsUndef(so0->iSize());
  LocateUndefs(so0, bufIsUndef) ;
  
  if ((so1->sot == sotStringVal) || (so2->sot == sotStringVal)) {
    StringBuf  buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
        bufRes[i] = bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else if ((so1->sot == sotRealVal) || (so2->sot == sotRealVal)) {
    RealBuf buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else if ((so1->sot == sotLongVal) || (so2->sot == sotLongVal)) {
    LongBuf buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else
  {
    LongBuf buf1, buf2, bufRes;
    so1->GetRaw(buf1);
    so2->GetRaw(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutRaw(bufRes);
  }
}

void ifnotundef3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  Buf<bool> bufIsUndef(so0->iSize());
  LocateUndefs(so0, bufIsUndef); 
  
  if ((so1->sot == sotStringVal) || (so2->sot == sotStringVal)) {
    StringBuf  buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
        bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else if ((so1->sot == sotRealVal) || (so2->sot == sotRealVal)) {
    RealBuf buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else if ((so1->sot == sotLongVal) || (so2->sot == sotLongVal)) {
    LongBuf buf1, buf2, bufRes;
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf1, buf2, bufRes;
    so1->GetRaw(buf1);
    so2->GetRaw(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf2[i];
    soRes->PutRaw(bufRes);
  }
}

void ifundef2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  Buf<bool> bufIsUndef(so0->iSize());
  LocateUndefs(so0, bufIsUndef);
  
  if (so1->sot == sotRealVal) {
    RealBuf buf1, bufRes;
    RealBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotStringVal) {
    StringBuf buf1, bufRes;
    StringBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotCoordVal) {
    CoordBuf buf1, bufRes;
    CoordBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if ( (so1->sot == sotLongVal) )
  {
    LongBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes); 
  }  
  else {
    LongBuf buf1, bufRes;
    LongBuf buf0;
    so0->GetRaw(buf0);
    so1->GetRaw(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutRaw(bufRes);
  }
}

void ifnotundef2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  Buf<bool> bufIsUndef(so0->iSize());
  LocateUndefs(so0, bufIsUndef);
  
  if (so1->sot == sotRealVal) {
    RealBuf buf1, bufRes;
    RealBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotStringVal) {
    StringBuf buf1, bufRes;
    StringBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotCoordVal) {
    CoordBuf buf1, bufRes;
    CoordBuf buf0;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes);
  }
  else if (so1->sot == sotLongVal)
  {
    LongBuf buf0, buf1, bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutVal(bufRes); 
  }  
  else {
    LongBuf buf1, bufRes;
    LongBuf buf0;
    so0->GetRaw(buf0);
    so1->GetRaw(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      bufRes[i] = !bufIsUndef[i] ? buf1[i] : buf0[i];
    soRes->PutRaw(bufRes);
  }
}

void length_so(StackObject* soRes, const StackObject* so)
{
  StringBuf buf;
  LongBuf bufRes;
  so->GetVal(buf);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if (buf[i] == sUNDEF)
      bufRes[i] = iUNDEF;
    else
		{
				//String sName = buf[i].find(':') == -1 ? buf[i] : buf[i].substr(buf[i].find(':') + 2);
				bufRes[i] = buf[i].size();
		}			
  soRes->PutVal(bufRes);
}

void substr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  StringBuf buf0;
  LongBuf buf1, buf2;
  StringBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  so2->GetVal(buf2);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if (buf0[i] == sUNDEF || buf1[0] < 0 )
      bufRes[i] = sUNDEF;
    else
		{
			int iLen = min(buf0[i].size() - buf1[i] + 1 , buf2[i]);
			if ( buf1[i] - 1 > buf0[i].size() )
				bufRes[i]	= sUNDEF;
			else
			{
				//String sName = buf0[i].find(':') == -1 ? buf0[i]: buf0[i].substr(buf0[i].find(':') + 2);
			  bufRes[i] = buf0[i].sSub(buf1[i]-1, iLen);
			}			
		}			
  soRes->PutVal(bufRes);
}

void leftstr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  StringBuf buf0;
  LongBuf buf1;
  StringBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if (buf0[i] == sUNDEF || buf1[0] < 0 )
      bufRes[i] = sUNDEF;
    else
		{
			//String sName = buf0[i].find(':') == -1 ? buf0[i] : buf0[i].substr(buf0[i].find(':') + 2);
      bufRes[i] = buf0[i].sLeft(buf1[i]);
		}			
  soRes->PutVal(bufRes);
}

void rightstr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  StringBuf buf0;
  LongBuf buf1;
  StringBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if (buf1[i] < 0 || buf0[0] == sUNDEF)
      bufRes[i] = sUNDEF;
    else
		{
			String sName = buf0[i].find(':') == -1 ? buf0[i] : buf0[i].substr(buf0[i].find(':') + 2);
      bufRes[i] = sName.sRight(buf1[i]);
		}					
	  soRes->PutVal(bufRes);
}

void instr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  StringBuf buf0, buf1;
  LongBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if ((buf0[i] == sUNDEF) || buf1[i] == sUNDEF)
      bufRes[i] = iUNDEF;
    else
		{
			String sName = buf0[i].find(':') == -1 ? buf0[i] : buf0[i].substr(buf0[i].find(':') + 2);
			bufRes[i] = sName.toUpper().find(buf1[i].toUpper()) != -1;
		}			
      //bufRes[i] = strstr(buf0[i].c_str(), buf1[i].c_str()) != 0;
  soRes->PutVal(bufRes);
}

void strpos_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  StringBuf buf0, buf1;
  LongBuf bufRes;
  so0->GetVal(buf0);
  so1->GetVal(buf1);
  bufRes.Size(soRes->iSize());
  for (short i=0; i<soRes->iSize(); i++)
    if ((buf0[i] == sUNDEF) ||(buf1[i] == sUNDEF))
      bufRes[i] = iUNDEF;
    else 
		{
			int iPos = buf0[i].iPos(buf1[i]);
      //p = strstr(buf0[i].c_str(), buf1[i].c_str());
      if (iPos == iUNDEF )
        bufRes[i] = 0;
      else
			{
				String sName = buf0[i].find(':') == -1 ? buf0[i] : buf0[i].substr(buf0[i].find(':') + 2);	
				bufRes[i] = sName.iPos(buf1[0]) + 1;
			}				
    }
  soRes->PutVal(bufRes);
}

void inrange_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  LongBuf bufRes;
  if (so0->sot == sotRealVal || so1->sot == sotRealVal || so2->sot == sotRealVal) {
    RealBuf buf0, buf1, buf2;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF) || (buf2[i] == rUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = (buf0[i] >= buf1[i]) && (buf0[i] <= buf2[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1, buf2;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF) || (buf2[i] == iUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = (buf0[i] >= buf1[i]) && (buf0[i] <= buf2[i]);
    soRes->PutVal(bufRes);
  }
}

void min2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal) {
    RealBuf buf0, buf1;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = min(buf0[i],buf1[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1;
    LongBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = min(buf0[i],buf1[i]);
    soRes->PutVal(bufRes);
  }
}

void max2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal) {
    RealBuf buf0, buf1;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = max(buf0[i],buf1[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1;
    LongBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = max(buf0[i],buf1[i]);
    soRes->PutVal(bufRes);
  }
}

void min3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal || so2->sot == sotRealVal) {
    RealBuf buf0, buf1, buf2;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF) || (buf2[i] == rUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = min(min(buf0[i],buf1[i]),buf2[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1, buf2;
    LongBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF) || (buf2[i] == iUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = min(min(buf0[i],buf1[i]),buf2[i]);
    soRes->PutVal(bufRes);
  }
}

void max3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2)
{
  if (so0->sot == sotRealVal || so1->sot == sotRealVal || so2->sot == sotRealVal) {
    RealBuf buf0, buf1, buf2;
    RealBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == rUNDEF) ||(buf1[i] == rUNDEF) || (buf2[i] == rUNDEF))
        bufRes[i] = rUNDEF;
      else
        bufRes[i] = max(max(buf0[i],buf1[i]),buf2[i]);
    soRes->PutVal(bufRes);
  }
  else {
    LongBuf buf0, buf1, buf2;
    LongBuf bufRes;
    so0->GetVal(buf0);
    so1->GetVal(buf1);
    so2->GetVal(buf2);
    bufRes.Size(soRes->iSize());
    for (short i=0; i<soRes->iSize(); i++)
      if ((buf0[i] == iUNDEF) ||(buf1[i] == iUNDEF) || (buf2[i] == iUNDEF))
        bufRes[i] = iUNDEF;
      else
        bufRes[i] = max(max(buf0[i],buf1[i]),buf2[i]);
    soRes->PutVal(bufRes);
  }
}






