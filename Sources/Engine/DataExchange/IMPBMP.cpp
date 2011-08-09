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
  import .BMP format
  by Li Fei, Oct. 94
  modified by Li Fei, Oct 95
  ILWIS Department ITC
	Last change:  JEL  28 Apr 97    4:02 pm
	Jelle added support for 24-bit:  WN    3 Jan 97    5:06 pm
*/
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

void ImpExp::ImportBMP(File& FileIn, const FileName& fnObject)
{
#pragma pack(2)
  struct 
	{
    short FileHd;
    long bfSize;
    short bfReserved1,bfReserved2;
    long bfOffbits;
  } FileHd;   // BITMAPFILEHEADER structure

  struct 
	{
    long biSize,biWidth,biHeight;
    short biPlanes,biBitCount;
    long biCom,biSizeIm,biX,biY,biClru,biClri;
  } InfoHd;  // BITMAPINFOHEADER structure

	struct 
	{
		unsigned char ucBl,ucGr,ucRd,ucRv;
	} RGB;
#pragma pack()

  if ( (sizeof FileHd) != FileIn.Read(sizeof FileHd, &FileHd) )
    throw ErrorImportExport(TR("File Reading Error"));

  if (FileHd.FileHd == 0)
    throw ErrorImportExport(TR("Bitmap versions 1.x and 2.x are not supported"));

  else if ( FileHd.FileHd != 0x4d42 )  // 'MB'
    throw ErrorImportExport(TR("Not a Microsoft Windows Bitmap"));

  if ( (sizeof InfoHd) != FileIn.Read(sizeof InfoHd, &InfoHd) )
    throw ErrorImportExport(TR("File Reading Error"));

  if (InfoHd.biCom != BI_RGB) // only uncompressed allowed
    throw ErrorImportExport(TR("Compressed Bitmap is not supported"));

  trq.SetTitle(TR("Importing from Windows Bitmap"));
  int iNrRGB,iNrByte;
  Domain dm;
  switch (InfoHd.biBitCount) {
    case 1: iNrRGB=2;dm="bit";iNrByte=((InfoHd.biWidth+31)/32)*4;break;
    case 4: iNrRGB=16;dm=Domain(fnObject,iNrRGB,dmtPICTURE);
            iNrByte=((InfoHd.biWidth+7)/8)*4;break;
    case 8: iNrRGB=256;dm=Domain(fnObject,iNrRGB,dmtPICTURE);
            iNrByte=((InfoHd.biWidth+3)/4)*4;break;
    case 16:iNrRGB=0;dm=Domain("color");
            iNrByte=((InfoHd.biWidth*2+2)/4)*4;break;
    case 24:iNrRGB=0;dm=Domain("color");
            iNrByte=((InfoHd.biWidth*3+3)/4)*4;break;
    default:throw ErrorImportExport(TR("Only 1, 4, 8, 16 or 24 bit format are supported"));
  }
  long iLine=abs(InfoHd.biHeight);
	bool fUpDirection = InfoHd.biHeight > 0; // true: bottom to top; false: top to bottom
  if ( iNrRGB==16 || iNrRGB==256 ) 
	{
    Representation rp( FileName(fnObject,".RPR"), dm);
    RepresentationClass* prc=dynamic_cast<RepresentationClass*>( rp.ptr() );
    if (InfoHd.biClru)
      iNrRGB=InfoHd.biClru;
    for ( int i=0;i<iNrRGB;i++) {
      if ( 4 != FileIn.Read(4,&RGB) )
        throw ErrorImportExport(TR("File Reading Error"));
      prc->PutColor(i, Color( RGB.ucRd, RGB.ucGr, RGB.ucBl ) );
    }
    dm->SetRepresentation(rp);
  }

  RowCol rc(iLine, InfoHd.biWidth);
  GeoRef grOut(rc);
  Map mpOut(fnObject, grOut, rc, dm);
  mpOut->fErase=true;  // Erase map when an error occurs

  FileIn.Seek(FileHd.bfOffbits);
  ByteBuf bbuf(3 * (InfoHd.biWidth + 31));
  LongBuf colorbuf(InfoHd.biWidth);

  trq.SetText(TR("Converting..."));
  for (int i=0;i<iLine;i++) {
    if ( trq.fUpdate( i, iLine ) )
      return;
    if ( iNrByte != FileIn.Read(iNrByte,bbuf.buf() ) )
      if ( i==0 )
        throw ErrorImportExport(TR("File Reading Error"));
      else {
        grOut=GeoRef( RowCol( (long)i, (long)InfoHd.biWidth ) );
        mpOut->SetGeoRef( grOut );
			  getEngine()->Message(TR("Solving File size mismatch in header.").c_str(),
                                 TR("Importing from Windows Bitmap").c_str(), 
                                 MB_OK | MB_ICONEXCLAMATION);
        break;
      }
    if ( InfoHd.biBitCount==4 ) 
      for (int j=InfoHd.biWidth-1;j>=0;j--) 
        if (j%2)
          bbuf[j]=0x0f & bbuf[j/2];
        else
          bbuf[j]=bbuf[j/2]>>4;
    else if ( InfoHd.biBitCount==1 ) 
      for (int j=InfoHd.biWidth-1;j>=0;j--) {
        int iTemp=7-j%8;
        bbuf[j]=( (1<<iTemp) & bbuf[j/8] )>>iTemp;
      }
    else if ( InfoHd.biBitCount == 16 ) {
      for (int j = 0; j < InfoHd.biWidth; ++j)
			{
				unsigned long uCol = bbuf[j * 2] + ((unsigned long)bbuf[j * 2 + 1] << 8);
				colorbuf[j] = ((uCol << 19) & 0xf80000) | ((uCol << 6) & 0xf800) | ((uCol >> 7) & 0xf8);
			}
      mpOut->PutLineRaw(fUpDirection ? iLine - i - 1 : i,colorbuf);
      continue;
			}
    else if ( InfoHd.biBitCount == 24 ) {
      for (int j = 0; j < InfoHd.biWidth; ++j)
        colorbuf[j] = (long)bbuf[j * 3 + 2]+ (((long)bbuf[j*3+1]) << 8) + (((long)bbuf[j * 3]) << 16);
      mpOut->PutLineRaw(fUpDirection ? iLine - i - 1 : i,colorbuf);
      continue;
    }
    mpOut->PutLineRaw(fUpDirection ? iLine - i - 1 : i,bbuf);
  }
  trq.fUpdate(iLine, iLine);
  mpOut->fErase=false;  // Import successful, keep the imported map
}


