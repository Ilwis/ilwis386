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
/* tls/fspace.c
   Feature Space Implementation for ILWIS 2.0
   march 1995, by Jan-Willem Rensman 
   (c) Computer Department ITC  
	Last change:  JEL  16 Apr 97    1:03 pm
*/
  
#define FSPACE_C  
#include "Engine\SampleSet\fspace.h "
#include "Engine\Map\Raster\mapstore.h "
#include "Engine\Base\DataObjects\Tranq.h"
#include "Headers\Hs\DAT.hs"

FeatureSpace::FeatureSpace(const SampleSet& sms,
                           short     ib1, 
                           short     ib2)
{
  long i;  
  long l = MXBYTsqr;
  // in bFSPtr the actual feature space is stored.
  bFSptr = new byte[l];
  // in bNRPtr the number of identical pixel combinations is stored.
  bNRptr = new word[l];
  // in bFSPtr the actual feature space is stored.
  bSNRptr = new word[l];
/*
  // in bCLPtr the sample class is stored. If it is 0 and bFSPtr != 0,
  // there are more than one class mapped on this pixel, and no 
  // unambiguous restore can be done in case of a delete sample.
  bCLptr = new HUGE byte[l];
  // bFSBPtr is the backup for bFSPtr.
  bNRBptr = new HUGE byte[l]; 
  // bNRBPtr is the backup for bNRPtr.
  bFSBptr = new HUGE byte[l];
  // bCLBPtr is the backup for bCLPtr.
  bCLBptr = new HUGE byte[l];

  for (i = 0; i < MXBYTsqr; i++) { bFSptr[i] = 0; bNRptr[i] = 0; 
                                   bCLptr[i] = 0; bFSBptr[i] = 0; 
                                   bNRBptr[i] = 0; bCLBptr[i] = 0; }
*/                                   
  for (i = 0; i < MXBYTsqr; i++) { 
    bFSptr[i] = 0; bNRptr[i] = 0; 
    bSNRptr[i] = 0; 
  }
  sampleset = sms;
  _iBand1 = (short)(ib1-sms->mpl()->iLower());
  _iBand2 = (short)(ib2-sms->mpl()->iLower()); 
  map1 = sms->mpl()[_iBand1+sms->mpl()->iLower()];
  map2 = sms->mpl()[_iBand2+sms->mpl()->iLower()];
  _fNeedCompute = false;
/*  
  if (Compute() != errOK) {    if (bFSptr) delete [] bFSptr;    bFSptr = 0;
                               if (bNRptr) delete [] bNRptr;    bNRptr = 0;
                               if (bCLptr) delete [] bCLptr;    bCLptr = 0;
                               if (bFSBptr) delete [] bFSBptr;  bFSBptr = 0;
                               if (bNRBptr) delete [] bNRBptr;  bNRBptr = 0; 
                               if (bCLBptr) delete [] bCLBptr;  bCLBptr = 0; return; }
*/                               
  sampleset->RegisterFS(this);    
  dc = sampleset->dc();
}

void FeatureSpace::AddSample(const RowCol& rc, byte bClassVal)
{
  AddSample((byte)map1->iRaw(rc), (byte)map2->iRaw(rc), bClassVal);
}

void FeatureSpace::AddSample(byte bVal1, byte bVal2, byte bClassVal)
{
  if (bClassVal == 255) {
    AddSelect(bVal1, bVal2);
    return;
  }
  XY xy(bVal1, bVal2); 
  byte bBU = (byte)iRaw(xy);
  long i = (bVal1 + (MAXBYTE * bVal2));
  if (bNRptr[i] != 0xffff)
    bNRptr[i]++;
  if (bNRptr[i] == 1)
  {
    bFSptr[i] = bClassVal;
//    bCLptr[i] = bClassVal;
    if (bBU != iRaw(xy)) Changed(xy);
    return;
  }  
  byte bClassOrg = bFSptr[i];
  byte bClassNew = bClassVal;
/*  
  if (bClassNew == 255)
  { 
    if (bClassOrg == 255) return;
    bFSptr[i] = bClassNew;
    bCLptr[i] = 0;
    if (bBU != iRaw(xy)) Changed(xy);
  } 
  else {
    if (bClassOrg == 255)
      bClassOrg = 0;*/
    if ((sampleset->smplS()(bClassNew, iBand1(), CLASSSTD) *
         sampleset->smplS()(bClassNew, iBand2(), CLASSSTD)) <
        (sampleset->smplS()(bClassOrg, iBand1(), CLASSSTD) *
         sampleset->smplS()(bClassOrg, iBand2(), CLASSSTD))) 
            {
      bFSptr[i] = bClassNew;
//      bCLptr[i] = 0;
      if (bBU != iRaw(xy)) Changed(xy);
    }  
//  }
}   

void FeatureSpace::AddSelect(byte bVal1, byte bVal2)
{
  long i = (bVal1 + (MAXBYTE * bVal2));
  if (bSNRptr[i] != 0xffff)
    bSNRptr[i]++;
  if (bSNRptr[i] == 1) {
    XY xy(bVal1, bVal2); 
    Changed(xy);
  }  
}   
/*
void FeatureSpace::DelSelect(byte bVal1, byte bVal2)
{
  long i = (bVal1 + (MAXBYTE * bVal2));
  if (bSNRptr[i] != 0) 
    bSNRptr[i]--;
  if (bSNRptr[i] == 0) {
    XY xy(bVal1, bVal2); 
    Changed(xy);
  }  
}*/

void FeatureSpace::DelSelect()
{
  for (long i = 0; i < MXBYTsqr; i++)
    bSNRptr[i] = 0; 
/*  
  memset(bSNRptr, 0, 0x8000);
  memset(bSNRptr+0x4000, 0, 0x8000);
  memset(bSNRptr+0x8000, 0, 0x8000);
  memset(bSNRptr+0xC000, 0, 0x8000);*/
}

void FeatureSpace::DelSample(const RowCol& rc)
{
  DelSample((byte)map1->iRaw(rc), (byte)map2->iRaw(rc));
}

void FeatureSpace::DelSample(byte bVal1, byte bVal2)
{
  XY xy(bVal1, bVal2); 
  byte bBU = (byte)iRaw(xy);
  long i;
  i = (bVal1 + (MAXBYTE * bVal2));
  if (bSNRptr[i] != 0)
    bNRptr[i]--;
  if (bNRptr[i] != 0 /*&& bCLptr[i] == 0*/)
  {
    _fNeedCompute = true;
    return;
  }
  if (bNRptr[i] == 0) 
  {
    bFSptr[i] = 0;
    if (bBU != iRaw(xy)) Changed(xy);
  }  
}
  
int FeatureSpace::Compute()
{
  long i, r, c, iRows;
  byte bv;
  RowCol rc;
  Tranquilizer trq;
  trq.SetTitle(String(SDATTitleFeatureSpaceOf_SS.scVal(), map1->sName(false, sampleset->fnObj.sPath()), map2->sName(false, sampleset->fnObj.sPath())));
  trq.SetText(SDATTextCalculating);
  trq.Start();
  for (i = 0; i < MXBYTsqr; i++) { 
    bFSptr[i] = 0; bNRptr[i] = 0; 
    bSNRptr[i] = 0; 
  }
//  MapStore* ptr = sampleset->map()->pms();
  Map mp = sampleset->map();
  iRows = mp->iLines();
  long iCols = mp->iCols();
  ByteBuf bb(iCols);
  for (r = 0; r < iRows; r++) {
    if (trq.fUpdate(r, iRows)) 
      return 1; //errUsrABO
    mp->GetLineRaw(r, bb);
    for (c = 0; c < iCols; c++) {
      bv = bb[c];
      if (bv)
        AddSample(RowCol(r, c), bv);     
    }
  }
  trq.fUpdate(r, iRows);
  _fNeedCompute = false;
  return errOK;
}

long FeatureSpace::iRaw(XY& xy) const
{
  ByteBuf bb(1);
  GetLineRaw(xy.bY, bb, xy.bX, 1);
  return bb[0];
}

void FeatureSpace::GetLineRaw(long iLine, ByteBuf& bb, long iFrom, long iNum) const
{
  short iTo, iCol;
  if (iNum == 0)
    iTo = 256-(short)iFrom;
  else 
    iTo =(short)( iFrom + iNum);
  for (iCol = (short)iFrom; iCol < iTo; iCol++) {
    long i = iCol + (MAXBYTE * iLine);
    if (bSNRptr[i] != 0) {
      bb[iCol-iFrom] = 255;
      continue;
    }
    bb[iCol-iFrom] = bFSptr[i];
  }  
}
/*

void FeatureSpace::Backup()
{
  long i;
  for (i = 0; i < MXBYTsqr; i++) 
  {
    bFSBptr[i] = bFSptr[i];
    bNRBptr[i] = bNRptr[i];
    bCLBptr[i] = bCLptr[i];
  }
}

void FeatureSpace::Restore()
{
  long i;
  for (i = 0; i < MXBYTsqr; i++) 
  {
    bFSptr[i] = bFSBptr[i];
    bNRptr[i] = bNRBptr[i];
    bCLptr[i] = bCLBptr[i];
  }
  Changed(XY(0,0));
}
*/
FeatureSpace::~FeatureSpace()
{ 
  sampleset->UnregisterFS(this);
  ReleaseMem();
}

void FeatureSpace::ReleaseMem()
{ 
  if (bFSptr) delete [] bFSptr; 
  if (bNRptr) delete [] bNRptr; 
  if (bSNRptr) delete [] bSNRptr; 
/*  
  if (bCLptr) delete [] bCLptr; 
  if (bFSBptr) delete [] bFSBptr; 
  if (bNRBptr) delete [] bNRBptr; 
  if (bCLBptr) delete [] bCLBptr; */
}

/*
XY::XY(short bBX, short bBY)
{
  bX     = bBX;
  bY     = bBY;
}
*/




