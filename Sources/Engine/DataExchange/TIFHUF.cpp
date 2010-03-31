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
/* tifhuf.c
  by Willem Nieuwenhuis 21 feb 1996
  Import TIFF bilevel Modified Huffman compression (CCITT-1)
	Last change:  WN   10 Sep 96    1:27 pm
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\DataExchange\proj.h"
#include "Engine\DataExchange\imptif.h"

/* The following tables are described in the document TIFHUF.DOC (Word 6 format)
*/
int WhiteNoZero[]    = { -2, -14, -4, -8, 3, -6, 128, 8, -10, 4, 9, -12, 16, 17, -16,
                         -22, 5, -18, -20, 64, 14, 15, 6, 7 };
int WhiteOneZero[]   = { -2, -40, -4, -20, 11, -6, -8, -12, 27, -10, 59, 60, -14, 18,
                         -16, -18, 1472, 1536, 1600, 1728, -22, -32, -24, -28, 24, -26,
                         49, 50, -30, 25, 51, 52, -34, 192, -36, -38, 55, 56, 57, 58,
                         -42, 2, -44, -54, 1664, -46, -48, -50, 448, 512, -52, 640, 704,
                         768, -56, -68, -58, -62, 576, -60, 832, 896, -64, -66, 960, 1024,
                         1088, 1152, -70, 256, -72, -74, 1216, 1280, 1344, 1408 };
int WhiteTwoZero[]   = { -2, -22, -4, -10, 12, -6, -8, 26, 53, 54, -12, -18, -14, -16, 39,
                         40, 41, 42, -20, 21, 43, 44, -24, 10, -26, -30, 28, -28, 61,
                         62, -32, -34, 63, 0, 320, 384 };
int WhiteThreeZero[] = { -2, -14, -4, -8, 20, -6, 33, 34, -10, -12, 35, 36, 37, 38, -16,
                         1, 19, -18, 31, 32 };
int WhiteFourZero[]  = { -2, 13, 23, -4, 47, 48 };
int WhiteFiveZero[]  = { -2, 22, 45, 46 };
int WhiteSixZero[]   = { 29, 30 };

int BlackNoZero[]    = { 3, 2 };
int BlackOneZero[]   = { 1, 4 };
int BlackTwoZero[]   = { 6, 5 };
int BlackThreeZero[] = { -2, 7, 9, 8 };
int BlackFourZero[]  = { -2, -4, 10, 11, -6, 12, -8, -22, 15, -10, -12, -18, -14, -16, 128,
                         192, 26, 27, -20, 19, 28, 29, -24, -36, -26, -30, 20, -28, 34, 35,
                         -32, -34, 36, 37, 38, 39, -38, 0, 21, -40, 42, 43};
int BlackFiveZero[]  = { -2, -26, 13, -4, -6, -18, -8, -12, 23, -10, 50, 51, -14, -16, 44,
                         45, 46, 47, -20, 16, -22, -24, 57, 58, 61, 256, -28, 14, -30, -38,
                         17, -32, -34, -36, 48, 49, 62, 63, -40, -46, -42, -44, 30, 31,
                         32, 33, -48, 22, 40, 41 };
int BlackSixZero[]   = { -2, -34, -4, -16, 18, -6, -8, -12, 52, -10, 640, 704, -14, 55, 768,
                         832, -18, -28, -20, -24, 56, -22, 1280, 1344, -26, 59, 1408, 1472,
                         -30, 24, 60, -32, 1536, 1600, -36, -52, -38, -44, 25, -40, -42,
                         320, 1664, 1728, -46, -48, 384, 448, -50, 53, 512, 576, -54, 64,
                         -56, -60, 54, -58, 896, 960, -62, -64, 1024, 1088, 1152, 1216 };
int SevenZero[]      = { -2, -14, -4, -8, 1792, -6, 1984, 2048, -10, -12, 2112, 2176, 2240,
                         2304, -16, -18, 1856, 1920, -20, -22, 2368, 2432, 2496, 2560 };

int *PrefixTable[] = { WhiteNoZero, WhiteOneZero, WhiteTwoZero, WhiteThreeZero,
                       WhiteFourZero, WhiteFiveZero, WhiteSixZero, SevenZero,
                       BlackNoZero, BlackOneZero, BlackTwoZero, BlackThreeZero,
                       BlackFourZero, BlackFiveZero, BlackSixZero, SevenZero };

byte TiffImporter::bitNext() {
  bMask >> 1;
  if (bMask == 0) {
    bMask = 128;
    filTIFF->Read(1, &bTiffVal);           // let File take care of buffering
//    iBufPos++;
  }
//  if (0 == (bufHufLine[iBufPos] & bMask))
  if (0 == bTiffVal & bMask)
    return 0;
  else
    return 1;
}


bool TiffImporter::fReadHuffLine(ByteBuf& buf) {
  bool fWhiteStream = true;
  long iTotLength = 0;
  short iPrefix = -1;
  int *aiStreams;
  bMask = 1;
  iBufPos = 0;
  while (iTotLength < iNrCols) {
    while (bitNext() == 0)
      iPrefix++;
    if (iPrefix < 0)
      Throw ErrorObject("Tiff file corrupt");
    if (fWhiteStream)
      aiStreams = PrefixTable[iPrefix];
    else
      aiStreams = PrefixTable[iPrefix + 8];
    short iStInd = 0, iStreamLength = 0;
    do {
      iStreamLength = aiStreams[iStInd + bitNext()];
      if (iStreamLength < 0)
        iStInd = -iStreamLength;
    }
    while (iStreamLength < 0);
    iTotLength += iStreamLength;
    if (fWhiteStreams)
      bColor = bWhiteColor;
    else
      bColor = bBlackColor;
    for (short i = iBufPos; i < iBufPos + iStreamLength; i++)
      bb[i] = bColor;
    if (iStreamLength < 64)    // terminating code
      fWhiteStream = !fWhiteStream;
  }
  return (itotStream == iNrCols);
}


