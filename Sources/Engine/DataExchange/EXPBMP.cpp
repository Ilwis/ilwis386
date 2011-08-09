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
/*Log: /ILWIS 3.0/Import_Export/EXPBMP.cpp $
 * 
 * 2     17-06-99 2:09p Martin
 * ported files to VS
//Revision 1.4  1998/09/16 17:41:07  Wim
//22beta2
//
//Revision 1.3  1998/03/12 21:08:44  Willem
//In case of less then 256 colors the palette colours were determined incorrectly.
//
//Revision 1.2  1997/09/19 16:58:20  janh
//2 trq statements moved upward (just after try
//trq.message added to avoid export of Dom id maps to BMP
//
/*
  export ILWIS to .BMP format
  ILWIS Department ITC
	Last change:  WN    6 Feb 98    4:57 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\expbmp.h"
#include "Engine\DataExchange\Convloc.h"       // for the prototypes"
#include "Headers\Hs\CONV.hs"         // for the strings"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmident.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmpict.h"

/*
   fnObject   : the name of the ilwis map
   fnFile     : the name of the .BMP file
   The ExportBMP function has its prototype in the CONVLOC.H file
*/
void ImpExp::ExportBMP(const FileName& fnObject, const FileName& fnFile)
{
  try {
    (BMPExporter(fnObject, fnFile)).Export(trq);
  }
  catch (AbortedByUser& ) {  // just stop, no message
  }
  catch (ErrorObject& err) {
    err.Show();
  }
}

/* ----------------- BMPExporter ------------------- */
BMPExporter::BMPExporter(const FileName& _fnIn, const FileName& _fnOut) :
    fnIn(_fnIn),
    fnOut(_fnOut),
    outFile(NULL)
{
}

BMPExporter::~BMPExporter()
{
  if (outFile) 
    delete outFile;
}

void BMPExporter::Export(Tranquilizer& trq)
{
  trq.SetTitle(TR("Exporting to Windows BMP"));  // the title in the report window
  trq.SetText(TR("Processing..."));   // the text in the report window
  
  inMap = Map(fnIn);

  FileName fnOutPut = FileName::fnUnique(FileName(String("%S.bmp",fnOut.sFullPath(false))));
  outFile = new File(fnOutPut, facCRT);
  outFile->SetErase(true);  // remove the file in case of errors.

  // setup headers
  PrepareHeaders();

  // write data
  trq.SetText(TR("Processing..."));
  if (inMap->dm()->pdi())         // Image map
      ExportImagePixels(trq);
  else if (inMap->dm()->pdbool()) // Bool map
      ExportBoolPixels(trq);
  else if (inMap->dm()->pdbit())  // Bit map
      ExportBitPixels(trq);
  else if (inMap->dm()->pdv())    // other value maps
      ExportValuePixels(trq);
  else if (inMap->dm()->pdp())    // Picture map
      ExportPicturePixels(trq);
  else if (inMap->dm()->pdcol())  // Color map
      ExportColorPixels(trq);
  else if (inMap->dm()->pdc())    // Class map
      ExportSortPixels(trq);
  else if (inMap->dm()->pdid())   // ID map
      ExportIDPixels(trq);

  outFile->SetErase(false);  // no problems occurred: keep the BMP file
}

// Prepare information in header structures that does not
// depend on the type of image exported: equal for all
void BMPExporter::PrepareHeaders() {
  bfhFile.cImageFileType[0] = 'B';
  bfhFile.cImageFileType[1] = 'M';
  bfhFile.iReserved1        = 0;
  bfhFile.iReserved2        = 0;
  bfhFile.iDataOffset       = sizeof(bfhFile) + sizeof(bihInfo);

  bihInfo.iInfoHeaderSize = sizeof(bihInfo); // 40 bytes
  bihInfo.iImageWidth     = inMap->iCols();
  bihInfo.iImageHeight    = inMap->iLines();
  bihInfo.iPlaneCount     = 1;   // always 1
  bihInfo.iCompressionMethod    = NO_COMPRESSION;
  bihInfo.iHorizontalResolution = 0;
  bihInfo.iVerticalResolution   = 0;
  bihInfo.iNrPaletteColors      = 0;
  bihInfo.iNrSignificantColors  = 0;
}

void BMPExporter::WriteHeadersAndPalette()
{
  long iBytes;
  switch (bihInfo.iBitsPerPixel) {
    case  1: iBytes = (inMap->iCols() + 7) / 8; break;
    case  8: iBytes = inMap->iCols(); break;
    case 24: iBytes = inMap->iCols() * 3; break;
  }
  long iBytesPerLine = (((iBytes - 1) / 4) + 1) * 4;

  bihInfo.iBitmapSize = iBytesPerLine * inMap->iLines();

  // adjust the image start to include the palette size
  if (bihInfo.iBitsPerPixel < 24)
    bfhFile.iDataOffset += sizeof(RGBType) * (1 << bihInfo.iBitsPerPixel);

  // the entire BMP file size
  bfhFile.iBMPFileSize = bfhFile.iDataOffset + bihInfo.iBitmapSize;

  outFile->Write(sizeof(bfhFile), &bfhFile);
  outFile->Write(sizeof(bihInfo), &bihInfo);

  // write palette
  if (bihInfo.iBitsPerPixel < 24)
    outFile->Write(sizeof(RGBType) * (1 << bihInfo.iBitsPerPixel), &rgbPalette);
}

// Create a palette from the representation. If this is not available
// setup a gray palette. The palette will always have 256 entries,
// (the pixels are stored as bytes) although only the first relevant
// will contain meaningful values
// This function is called in case values can be stored in bytes
void BMPExporter::MakePalette(Representation& rpr)
{
  RepresentationGradual* prg = rpr->prg();
  RepresentationValue* prv   = rpr->prv();
  
  long iC;
  RangeReal rrStretch;
  if (rpr.fValid()) {
    rgbPalette[0].bRed   = 0;   // UNDEF.red
    rgbPalette[0].bGreen = 0;   // UNDEF.green
    rgbPalette[0].bBlue  = 0;   // UNDEF.blue
    if (prg != NULL) {
      rrStretch = rrDetermineValueRange(inMap);
      for (iC = 1; iC < 256; iC++) {
        Color clr = rpr->clr(iC - 1, rrStretch);
        rgbPalette[iC].bRed   = clr.red();
        rgbPalette[iC].bGreen = clr.green();
        rgbPalette[iC].bBlue  = clr.blue();
      }
    }
    else if (prv != NULL) {
      for (iC = 1; iC < 256; iC++) {
        Color clr = rpr->clr(iC - 1);
        rgbPalette[iC].bRed   = clr.red();
        rgbPalette[iC].bGreen = clr.green();
        rgbPalette[iC].bBlue  = clr.blue();
      }
    }
    else {  // Classes
      rgbPalette[0].bRed   = 0;   // UNDEF.red
      rgbPalette[0].bGreen = 0;   // UNDEF.green
      rgbPalette[0].bBlue  = 0;   // UNDEF.blue
      for (iC = 1; iC < bihInfo.iNrPaletteColors; iC++) {
        Color clr = rpr->clrRaw(iC);
        rgbPalette[iC].bRed   = clr.red();
        rgbPalette[iC].bGreen = clr.green();
        rgbPalette[iC].bBlue  = clr.blue();
      }
    }
  }
  else  // create a gray palette
    for (iC = 0; iC < 256; iC++) {
      rgbPalette[iC].bRed   = iC;
      rgbPalette[iC].bGreen = iC;
      rgbPalette[iC].bBlue  = iC;
    }
}

// Create a palette for ID maps with less than 256 items
// The colors used are the same as displayed on the screen:
// the colors are extracted from basedrwr.c, from the function
//    zColor BaseDrawer::clrPrimary(int iNr)
void BMPExporter::MakeIDPalette()
{
  Color col;

  rgbPalette[0].bRed   = 0;   // UNDEF.red
  rgbPalette[0].bGreen = 0;   // UNDEF.green
  rgbPalette[0].bBlue  = 0;   // UNDEF.blue
  
  for (long i = 1; i <= bihInfo.iNrPaletteColors; i++) {
    switch (i % 31) {
      case  0: col = Color(255,  0,  0); break;
      case  1: col = Color(255,255,  0); break;
      case  2: col = Color(  0,  0,255); break;
      case  3: col = Color(255,  0,255); break;
      case  4: col = Color(  0,255,255); break;
      case  5: col = Color(  0,255,  0); break;
      case  6: col = Color(128,128,128); break;
      case  7: col = Color(224,224,224); break;
      case  8: col = Color(128,  0,  0); break;
      case  9: col = Color(128,128,  0); break;
      case 10: col = Color(  0,  0,128); break;
      case 11: col = Color(128,  0,128); break;
      case 12: col = Color(  0,128,128); break;
      case 13: col = Color(  0,128,  0); break;
      case 14: col = Color(255,128,  0); break;
      case 15: col = Color(191,  0,  0); break;
      case 16: col = Color(191,191,  0); break;
      case 17: col = Color(  0,  0,191); break;
      case 18: col = Color(191,  0,191); break;
      case 19: col = Color(  0,191,191); break;
      case 20: col = Color(  0,191,  0); break;
      case 21: col = Color(191,191,191); break;
      case 22: col = Color(192,220,192); break;
      case 23: col = Color( 63,  0,  0); break;
      case 24: col = Color( 63, 63,  0); break;
      case 25: col = Color(  0,  0, 63); break;
      case 26: col = Color( 63,  0, 63); break;
      case 27: col = Color(  0, 63, 63); break;
      case 28: col = Color(  0, 63,  0); break;
      case 29: col = Color( 63, 63, 63); break;
      case 30: col = Color(127, 63,  0); break;
    }
    rgbPalette[i].bRed   = col.red();
    rgbPalette[i].bGreen = col.green();
    rgbPalette[i].bBlue  = col.blue();
  }
}

void BMPExporter::ExportImagePixels(Tranquilizer& trq)
{
    bihInfo.iBitsPerPixel = 8;
    bihInfo.iNrPaletteColors = 256;

    // Fill palette
    Representation rpr = inMap->dm()->rpr();
    MakePalette(rpr);
    
    WriteHeadersAndPalette();
    
    // transfer the image data
    WriteAsRaw(trq);
}    

void BMPExporter::ExportBoolPixels(Tranquilizer& trq)
{
    bihInfo.iBitsPerPixel = 8;
    bihInfo.iNrPaletteColors = 3;

    // Fill palette
    rgbPalette[0].bRed   = 0;   // UNDEF.red
    rgbPalette[0].bGreen = 0;   // UNDEF.green
    rgbPalette[0].bBlue  = 0;   // UNDEF.blue

    rgbPalette[1].bRed   = 255; // false.red
    rgbPalette[1].bGreen = 0;   // false.green
    rgbPalette[1].bBlue  = 0;   // false.blue

    rgbPalette[2].bRed   = 0;   // true.red
    rgbPalette[2].bGreen = 255; // true.green
    rgbPalette[2].bBlue  = 0;   // true.blue

    WriteHeadersAndPalette();

    // transfer the image data
    WriteAsRaw(trq);
}    

void BMPExporter::ExportValuePixels(Tranquilizer& trq)
{
    Domain dm = inMap->dm();
    DomainValue* pdv = dm->pdv();
    if (pdv == NULL)
      return;    // impossible to come here

    Representation rpr = dm->rpr();
    double rStep = pdv->rStep();
    RangeReal rrMinMax = pdv->rrMinMax();

    bool fPalPossible = false;
    if (rStep == 1) {
      fPalPossible = (rrMinMax.rLo() >= 0 && rrMinMax.rHi() <= 255);
      if (fPalPossible) {
        bihInfo.iBitsPerPixel = 8;
        bihInfo.iNrPaletteColors = 256;

        MakePalette(rpr);
        WriteHeadersAndPalette();

        WriteAsByteValues(trq);
        return;
      }
    }
    if (!fPalPossible) {
      bihInfo.iBitsPerPixel = 24;

      WriteHeadersAndPalette();

      WriteAsColorValues(trq);
    }

    return;
}

void BMPExporter::ExportPicturePixels(Tranquilizer& trq)
{
    DomainPicture* pdp = inMap->dm()->pdp();
    bihInfo.iBitsPerPixel = 8;
    bihInfo.iNrPaletteColors = pdp->iColors();

    // Fill palette
    Representation rpr = pdp->rpr();
    MakePalette(rpr);
    WriteHeadersAndPalette();

    // transfer the image data
    WriteAsRaw(trq);
}    

void BMPExporter::ExportColorPixels(Tranquilizer& trq)
{
    bihInfo.iBitsPerPixel = 24;

    WriteHeadersAndPalette();

    WriteAsColor(trq);
}

// export class map
void BMPExporter::ExportSortPixels(Tranquilizer& trq)
{
    DomainSort* pds = inMap->dm()->pdsrt();
    if (pds->iNettoSize() > 255) {
      TooManyClassesForExport(outFile->sName());
    }
    else {
      bihInfo.iBitsPerPixel = 8;
      bihInfo.iNrPaletteColors = pds->iNettoSize() + 1; // the extra 1 == UNDEF
      
      Representation rpr=pds->rpr();
      MakePalette(rpr);
      WriteHeadersAndPalette();

      WriteAsRaw(trq);
    }
}    

// export ID map
void BMPExporter::ExportIDPixels(Tranquilizer& trq)
{
    DomainIdentifier* pdi = inMap->dm()->pdid();
    if (pdi->iNettoSize() > 255) {
      bihInfo.iBitsPerPixel = 24;

      WriteHeadersAndPalette();

      WriteAsColor(trq);   // ID's are interpreted as a color
    }
    else {
      bihInfo.iBitsPerPixel = 8;
      bihInfo.iNrPaletteColors = pdi->iNettoSize();
      
      MakeIDPalette();
      WriteHeadersAndPalette();
      
      WriteAsRaw(trq);
    }
}    

void BMPExporter::ExportBitPixels(Tranquilizer& trq)
{
    bihInfo.iBitsPerPixel = 1;
    bihInfo.iNrPaletteColors = 2;

    // Fill palette
    rgbPalette[0].bRed   = 255;
    rgbPalette[0].bGreen = 255;
    rgbPalette[0].bBlue  = 255;
    
    rgbPalette[1].bRed   = 0;
    rgbPalette[1].bGreen = 0;
    rgbPalette[1].bBlue  = 0;

    WriteHeadersAndPalette();

    ByteBuf buf(inMap->iCols());
    byte b[4] = {0, 0, 0, 0};

    // transfer the image data
    for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
    {
        inMap->GetLineRaw(inMap->iLines() - iRow - 1, buf);
        if ( trq.fUpdate(iRow, inMap->iLines())) 
          return;

// Pack bits into bytes using the default TIFF FillOrder (==1)
// meaning: lower column numbers are stored in higher bits:
// for example: column 0 will be stored in the highest bit
//              column 1 in second highest bit, etc
        byte iMask = 0x80;
        byte bBits = 0;
        long iBitCol = 0; // position in the buffer for the packed bits
                          // the buffer is reused to store the packed bits
        for (long iCol = 0; iCol < inMap->iCols(); iCol++) {
          if (buf[iCol] != 0) bBits |= iMask;
          iMask /= 2;
          if (iMask == 0) {
            buf[iBitCol] = bBits;
            iMask = 0x80;
            bBits = 0;
            iBitCol++;
          }
        }
        if (iMask != 0x80)
          buf[iBitCol] = bBits;  // do not forget the last bits
        else
          iBitCol--;             // adjust for correct number of bits
        outFile->Write(iBitCol + 1, buf.buf());

        // adjust to 4 byte boundary
        long j = (iBitCol + 1) % 4;
        if (j > 0)
          outFile->Write(4 - j, b);
    }
}    

void BMPExporter::WriteAsRaw(Tranquilizer& trq)
{
  ByteBuf buf(inMap->iCols());
  byte b[4] = {0, 0, 0, 0};
  
  for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
  {
    inMap->GetLineRaw(inMap->iLines() - iRow - 1, buf);
    if ( trq.fUpdate(iRow, inMap->iLines())) 
      return;
    outFile->Write(inMap->iCols(), buf.buf());

    // adjust to 4 byte boundary
    long j = inMap->iCols() % 4;
    if (j > 0)
      outFile->Write(4 - j, b);
  }
}

void BMPExporter::WriteAsByteValues(Tranquilizer& trq)
{
  LongBuf bufIn(inMap->iCols());
  ByteBuf bufOut(inMap->iCols());
  byte b[4] = {0, 0, 0, 0};
    
  for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
  {
    inMap->GetLineVal(inMap->iLines() - iRow - 1, bufIn);
    if ( trq.fUpdate(iRow, inMap->iLines())) 
      return;
    for (long iCol = 0; iCol < inMap->iCols(); iCol++) {
      long iVal = bufIn[iCol];
      if (iVal == iUNDEF)
        bufOut[iCol] = 0;
      else
        bufOut[iCol] = bufIn[iCol];   // values do fit, their range lies between 0 and 255
    }

    outFile->Write(inMap->iCols(), bufOut.buf());

    // adjust to 4 byte boundary
    long j = inMap->iCols() % 4;
    if (j > 0)
      outFile->Write(4 - j, b);
  }
}

void BMPExporter::WriteAsColor(Tranquilizer& trq)
{
  LongBuf iBuf(inMap->iCols());
  byte b[4] = {0, 0, 0, 0};
  
  for (long iRow = 0; iRow < inMap->iLines(); ++iRow)
  {
    inMap->GetLineRaw(inMap->iLines() - iRow - 1, iBuf);
    if ( trq.fUpdate(iRow, inMap->iLines())) 
      return;
    for (long iCol = 0; iCol < inMap->iCols(); ++iCol)
      WriteColor(iBuf[iCol]);

    // adjust to 4 byte boundary
    long j = (3 * inMap->iCols()) % 4;
    if (j > 0)
      outFile->Write(4 - j, b);
  }
}

void BMPExporter::WriteAsColorValues(Tranquilizer& trq)
{
  Representation rpr = inMap->dm()->rpr();
  RepresentationValue* prv = rpr->prv();
  RangeReal rr = rrDetermineValueRange(inMap);

  RealBuf rBuf(inMap->iCols());
  byte b[4] = {0, 0, 0, 0};
  Color color;
  
  for (long iRow = 0; iRow < inMap->iLines(); ++iRow) {
    inMap->GetLineVal(inMap->iLines() - iRow - 1, rBuf);
    if ( trq.fUpdate(iRow, inMap->iLines()))
      return;
    for (long iCol = 0; iCol < inMap->iCols(); ++iCol) {
      double rVal = rBuf[iCol];
      if (rVal == rUNDEF)
        color = Color(0, 0, 0); // black
      else if (0 == prv)
        color = rpr->clr(rBuf[iCol],rr);
      else
        color = rpr->clr(rBuf[iCol]);
      WriteColor(color);
    }

    // adjust to 4 byte boundary
    long j = (3 * inMap->iCols()) % 4;
    if (j > 0)
      outFile->Write(4 - j, b);
  }
}

void BMPExporter::WriteColor(const Color& color)
{
  Pixel3Byte rgb;
  rgb.bPixel[0] = color.blue();
  rgb.bPixel[1] = color.green();
  rgb.bPixel[2] = color.red();
  outFile->Write(sizeof(rgb), &rgb);
}

RangeReal BMPExporter::rrDetermineValueRange(Map& ValMap) 
{
  RangeReal rrMinMax = ValMap->rrPerc1(true);
  if ( !rrMinMax.fValid()) 
    return ValMap->dm()->pdv()->rrMinMax();

  return rrMinMax;
}





