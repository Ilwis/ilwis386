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
  import .GIF format
  by Li Fei, Nov. 94
  modified by Li Fei, Nov 95
  ILWIS Department ITC
	Last change:  WN   24 Jun 97    7:41 pm
*/
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Representation\Rprclass.h"

#pragma pack(1)
struct stbl
{
  short         iBef;
  unsigned char ucCod;
  short         Len;
};

struct ScreenStruct
{
  char ucSig[3];           // Signature, should be: GIF
  char ucVer[3];           // Version, can only be: 87a or 89a
  short iNrCols, iNrLines;   // 
  unsigned char ucM;       // Color Information
  unsigned char ucBkgrd;   // Background colour index
  unsigned char ucNul;     // Pixel Aspect ratio
};

struct ImageStruct
{
  short iWleft, iWtop, iWcols, iWlines;
  unsigned char uc;
};

#pragma pack()

stbl undef( const stbl& ) { 
  return stbl();
}

class param {
public:
  int iCol;
  byte ucCdInit, ucCdSz, ucaPkBlk[261], PackedBlockLen;
  int BlockNr1, BlockLen1, LastLen1, iBit, iInd, iCdMax, iCdOd, iMaxCd, Mask, Code, iCdCl;
  param() : BlockLen1(-1) {}
};

void InitArray1(Array<stbl>&, param&);

void ReadWriteLine( File& FileIn, Array<stbl>&, ByteBuf&, Array<byte>&, param&);

void InitArray1(Array<stbl>& stblarT, param& aParam) {
  aParam.ucCdSz = aParam.ucCdInit+1;
  aParam.Mask = 0xffff >> (16 - aParam.ucCdSz);
  aParam.iCdMax = aParam.iCdCl + 1;
  aParam.iMaxCd = 2 * aParam.iCdCl;
  for (int i = 0; i < aParam.iCdCl; i++) {
    stblarT[i].iBef = -1;
    stblarT[i].ucCod = i;
    stblarT[i].Len = 0;
  }    
}

void ReadWriteLine( File& FileIn, Array<stbl>& stblarT, ByteBuf& bbuf, Array<byte>& Block1, param& aParam) {
  for(int i = 0; i < aParam.iCol; i++) {
    while (aParam.BlockNr1 > aParam.BlockLen1 ) {
      long I4;
      unsigned int W2;
      int C = 0;
      if (aParam.iInd >= aParam.LastLen1 - 2) {
        if (aParam.LastLen1 >= 2) {
          aParam.ucaPkBlk[0] = aParam.ucaPkBlk[aParam.LastLen1 - 2];
          aParam.ucaPkBlk[1] = aParam.ucaPkBlk[aParam.LastLen1 - 1];
        }
        aParam.iInd += 2 - aParam.LastLen1;
        FileIn.Read(1, &aParam.PackedBlockLen);
        if (aParam.PackedBlockLen == 0)
          throw ErrorImportExport(SCVErrGifCode);
        if (aParam.PackedBlockLen != FileIn.Read(aParam.PackedBlockLen, &aParam.ucaPkBlk[2]))
          throw ErrorImportExport(SCVErrReading);
        aParam.LastLen1 = aParam.PackedBlockLen + 2;
      }
      I4 = ((long)aParam.ucaPkBlk[aParam.iInd + 2] << 16) +
           ((long)aParam.ucaPkBlk[aParam.iInd + 1] << 8) + 
                  aParam.ucaPkBlk[aParam.iInd];
      if (aParam.iBit > 0)
        I4 >>= aParam.iBit;
      W2 = I4;
      aParam.Code = W2 & aParam.Mask;
      aParam.iBit += aParam.ucCdSz;
      while (aParam.iBit > 7) {
        aParam.iInd++;
        aParam.iBit -= 8;
      }
      if (aParam.Code == aParam.iCdCl) {
        InitArray1(stblarT, aParam);
        aParam.iCdOd = aParam.iCdCl;
        aParam.BlockLen1 = -1;
      }
      else if (aParam.Code == aParam.iCdCl + 1)
        aParam.BlockLen1 =- 1;
      else if (aParam.Code <= aParam.iCdMax) {
        C = aParam.Code;
        while (C != -1) {
          Block1[stblarT[C].Len] = stblarT[C].ucCod;
          C = stblarT[C].iBef;
        }
        aParam.BlockLen1 = stblarT[aParam.Code].Len;
        if (aParam.iCdOd != aParam.iCdCl) {
          aParam.iCdMax++;
          stblarT[aParam.iCdMax].iBef = aParam.iCdOd;
          stblarT[aParam.iCdMax].Len = stblarT[aParam.iCdOd].Len + 1;
          C = aParam.Code;
          while (stblarT[C].iBef != -1)
            C = stblarT[C].iBef;
          stblarT[aParam.iCdMax].ucCod = stblarT[C].ucCod;
        }
        aParam.iCdOd = aParam.Code;
      }
      else {
        aParam.iCdMax++;
        if ( aParam.Code != aParam.iCdMax )
          throw ErrorImportExport(SCVErrGifCode);
        stblarT[aParam.Code].iBef = aParam.iCdOd;
        stblarT[aParam.Code].Len = stblarT[aParam.iCdOd].Len + 1;
        C = aParam.iCdOd;
        while (stblarT[C].iBef != -1)
          C = stblarT[C].iBef;
        stblarT[aParam.Code].ucCod = stblarT[C].ucCod;
        aParam.iCdOd = aParam.Code;
        C = aParam.Code;
        while (C != -1) {
          Block1[stblarT[C].Len] = stblarT[C].ucCod;
          C = stblarT[C].iBef;
        }
        aParam.BlockLen1 = stblarT[aParam.Code].Len;
      }
      if ( aParam.iCdMax + 1 == aParam.iMaxCd)
        if (aParam.ucCdSz < 12) {
          aParam.ucCdSz++;
          aParam.Mask = 0xFFFFL >> (16 - aParam.ucCdSz);
          aParam.iMaxCd *= 2;
        }
      aParam.BlockNr1 = 0;
    }
    bbuf[i] = Block1[aParam.BlockNr1];
    aParam.BlockNr1++;
  }
}

void ImpExp::ImportGIF(File& FileIn, const FileName& fnObject)
{
  Array<stbl> stblarT(4096);

  Array<byte> Block1(5001);
  param aParam;

  trq.SetTitle(SCVTitleImportGIF);
  trq.fUpdate(0);

	ScreenStruct screen;
  FileIn.Read(sizeof screen, &screen);
  if (strncmp(screen.ucSig, "GIF", 3))
    throw ErrorImportExport(SCVErrNoGIF);

  if (screen.ucNul)
    throw ErrorImportExport(SCVErrNotSupported);

  byte bCol[3], b;
  Domain dm("Image");
  if (screen.ucM & 0x80) {          // Global Color Table
    trq.SetText(SCVTextGifGetGlobalLUT);
    int iNrCol = 1 << ((screen.ucM & 0x07) + 1);
    dm = Domain(fnObject, iNrCol, dmtPICTURE);
    Representation rp(FileName(fnObject, ".RPR"), dm);
    RepresentationClass* prc = dynamic_cast<RepresentationClass*>(rp.ptr());
    for ( int i = 0; i < iNrCol; i++) {
      FileIn.Read(3, bCol);
      prc->PutColor(i, Color( bCol[0], bCol[1], bCol[2]));
    }
    dm->SetRepresentation(rp);
  }

  // Skip extension blocks
  do {
    FileIn.Read(1, &b);
    if (b == '!') {
      trq.SetText(SCVTextGifSkipExt);
      byte count, buf[256];
      FileIn.Read(1, &count);
      do {
        FileIn.Read(1, &count);
        if (count)
          FileIn.Read(count, buf);
      }
      while (count == 0);
    }
  }
  
  while (b != ',' && !FileIn.fEof());    // Found an image descriptor
  if (FileIn.fEof())
    throw ErrorImportExport(SCVErrGifNoImage);

	ImageStruct image;
  if ((sizeof image) != FileIn.Read(sizeof(image), &image) )
    throw ErrorImportExport(SCVErrReading);

  aParam.iCol = image.iWcols;
  long iLine = image.iWlines;

  ByteBuf bbuf(aParam.iCol);

  int iNrp = (image.uc & 0x07) + 1;
  if (image.uc & 0x80) {             // Local Color Table
    trq.SetText(SCVTextGifGetLocalLUT);
    int iNrCol = 1 << iNrp;
    dm = Domain(fnObject, iNrCol, dmtPICTURE);
    Representation rp(FileName(fnObject, ".RPR"), dm);
    RepresentationClass* prc = dynamic_cast<RepresentationClass*>(rp.ptr());
    for (int i = 0; i < iNrCol; i++) {
      if (3 != FileIn.Read(3, bCol))
        throw ErrorImportExport(SCVErrReading);
      prc->PutColor(i, Color(bCol[0], bCol[1], bCol[2]));
    }
    dm->SetRepresentation(rp);
  }

  RowCol rc((long)iLine, (long)aParam.iCol);
  GeoRef grOut = GeoRef(rc);
  Map mpOut(fnObject, grOut, rc, dm);
  mpOut->fErase = true;
  FileIn.Read(1, &aParam.ucCdInit);
  aParam.iCdCl = 1 << aParam.ucCdInit;
  aParam.BlockNr1 = 0;
  aParam.iInd = 0;
  aParam.iBit = 0;
  aParam.BlockLen1 = -1;
  aParam.PackedBlockLen = 0;
  aParam.LastLen1 = 0;
  InitArray1(stblarT, aParam);
  String sErr;
  trq.SetText(SCVTextConverting);

  if (image.uc & 0x40) {    // Interlaced
    long iCnt = 0;
    long i = 0;
    while (i < iLine) {
      ReadWriteLine(FileIn, stblarT, bbuf, Block1, aParam);
      if (trq.fUpdate(iCnt, iLine))
        return;
      mpOut->PutLineRaw(i, bbuf);
      i += 8;
      iCnt++;
    }
    i = 4;
    while (i < iLine) {
      ReadWriteLine(FileIn, stblarT, bbuf, Block1, aParam);
      if (trq.fUpdate(iCnt, iLine))
        return;
      mpOut->PutLineRaw(i, bbuf);
      i += 8;
      iCnt++;
    }
    i = 2;
    while (i < iLine) {
      ReadWriteLine(FileIn, stblarT, bbuf, Block1, aParam);
      if (trq.fUpdate(iCnt, iLine))
        return;
      mpOut->PutLineRaw(i, bbuf);
      i += 4;
      iCnt++;
    }
    i = 1;
    while (i < iLine) {
      ReadWriteLine(FileIn, stblarT, bbuf, Block1, aParam);
      if (trq.fUpdate(iCnt, iLine))
        return;
      mpOut->PutLineRaw(i, bbuf);
      i += 2;
      iCnt++;
    }
  }
  else 
    for (int i = 0; i < iLine; i++) {
      ReadWriteLine(FileIn, stblarT, bbuf, Block1, aParam);
      if (trq.fUpdate(i, iLine))
        return;
      mpOut->PutLineRaw(i, bbuf);
    }
  trq.fUpdate(iLine, iLine);
  mpOut->fErase = false;
}


