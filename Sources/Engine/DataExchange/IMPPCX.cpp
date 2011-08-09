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
  import .PCX format
  by Li Fei, Aug. 94
  modified by Li Fei, Dec. 95
  ILWIS Department ITC
	Last change:  WN   18 Jul 97    2:16 pm
	Jelle added support for 24-bit:  WN    3 Jan 97    5:04 pm
*/
#include "Headers\toolspch.h"               // for the tranquillizer
#include "Engine\SpatialReference\Gr.h"             // for georef functions"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Representation\Rprclass.h"

void ImpExp::ImportPCX(File& FileIn, const FileName& fnObject)
{
#pragma pack(1)
  struct 
	{
    char          Manuf;
    unsigned char Version, Encoding, BitsPerPixel;
    short         Xmin, Ymin, Xmax, Ymax, Hres, Vres;
    unsigned char Palette[48], Reserved, Planes;
    short         BytesPerLine, PaletteType;
    unsigned char Filler[58];
  } PCXHeader;
#pragma pack()

  if ( (sizeof PCXHeader) != FileIn.Read(sizeof PCXHeader, &PCXHeader) )
    throw ErrorImportExport(TR("File Reading Error"));

  if ( PCXHeader.Manuf!=10 || PCXHeader.Encoding!=1 )
    throw ErrorImportExport(TR("Not a PCX file"));

  long iCol = PCXHeader.Xmax - PCXHeader.Xmin + 1;
  long iLine = PCXHeader.Ymax - PCXHeader.Ymin + 1;
  long iScan = PCXHeader.Planes * PCXHeader.BytesPerLine;
  ByteBuf bbufIn(iScan);

  long iBitBlock = 8 / PCXHeader.BitsPerPixel;
  long iColUnPak = iBitBlock * PCXHeader.BytesPerLine;
  ByteBuf bbufOut(iColUnPak);
  LongBuf colorOut(iColUnPak);

  long Loc=FileIn.iLoc();
  byte bTemp1, bTemp2;

	Domain dm("Image");
  if (PCXHeader.Planes == 3) 
	{
    dm = Domain("color");
  }
  else 
	{
    if ( PCXHeader.Version==5 && PCXHeader.BitsPerPixel==8 ) 
		{
      FileIn.Seek(769, true);
      FileIn.Read(1, &bTemp1);
      if ( bTemp1==12 ) {
        byte col[3];
        dm = Domain(fnObject, 256, dmtPICTURE);
        Representation rp( FileName(fnObject,".RPR"), dm);
        RepresentationClass* prc = dynamic_cast<RepresentationClass*>( rp.ptr() );
        for ( long i=0;i<256;i++) 
				{
          FileIn.Read(1,&col[0]);
          FileIn.Read(1,&col[1]);
          FileIn.Read(1,&col[2]);
          prc->PutColor(i, Color( col[0],col[1],col[2] ) );
        }
        dm->SetRepresentation(rp);
      }
    }
  }
  RowCol rc(iLine, iCol);
  GeoRef gr(rc);
  Map mp(fnObject, gr, rc, dm);
  mp->fErase = true;

	long iBitDepth = PCXHeader.BitsPerPixel * PCXHeader.Planes;
  long iConst=(1<<PCXHeader.BitsPerPixel)-1;

  FileIn.Seek(Loc,false);
  trq.SetTitle(TR("Importing from Painbrush-PCX"));
  trq.SetText(TR("Converting..."));

  if (PCXHeader.Planes != 3) 
	{
    for ( long i=0; i<iLine; i++ ) 
		{
      if (trq.fUpdate(i, iLine))
        return;

      long iScanCnt=0;
      while ( iScanCnt<iScan ) 
			{
        FileIn.Read(1, &bTemp1);
        if ( (bTemp1 & 0xc0)==0xc0 ) 
				{
          bTemp1&=0x3f;
          FileIn.Read(1, &bTemp2);
          for (long iDup=1; iDup<=bTemp1; iDup++)
            bbufIn[iScanCnt++]=bTemp2;
        }
        else
          bbufIn[iScanCnt++]=bTemp1;
      }
      for ( long izo=0; izo<iColUnPak; izo++ )
        bbufOut[izo]=0;

      for ( long iNrPlan=0; iNrPlan<PCXHeader.Planes; iNrPlan++ ) 
			{
        long iPlan=iNrPlan*PCXHeader.BytesPerLine;
        long iShiftBack=iNrPlan*PCXHeader.BitsPerPixel;
        for ( long iNrByPL=0; iNrByPL<PCXHeader.BytesPerLine; iNrByPL++ )
          for ( long iBit=0; iBit<iBitBlock; iBit++ )
            bbufOut[iBitBlock*iNrByPL+iBitBlock-1-iBit]+=( (bbufIn[iPlan+iNrByPL]
                        >> iBit*PCXHeader.BitsPerPixel) & iConst ) << iShiftBack;
      }
			if (iBitDepth == 1)
				for (long iCol = 0; iCol < bbufOut.iSize(); iCol++)
					if (bbufOut[iCol] == 1)
						bbufOut[iCol] = 255;
      mp->PutLineRaw(i,bbufOut);
    }
  }
  else {  
    long j, k;
    for ( long i=0; i<iLine; i++ ) 
		{
      if (trq.fUpdate(i, iLine))
        return;

      for (k=0; k < colorOut.iSize(); ++k) 
        colorOut[k] = 0;
      for (j=0; j < 3  ; ++j) 
			{
        long iScanCnt=0;
        while ( iScanCnt<iScan/3 ) 
				{
          FileIn.Read(1, &bTemp1);
          if ( (bTemp1 & 0xc0)==0xc0 ) 
					{
            bTemp1&=0x3f;
            FileIn.Read(1, &bTemp2);
            for (long iDup=1; iDup<=bTemp1; iDup++)
              bbufIn[iScanCnt++]=bTemp2;
          }
          else
            bbufIn[iScanCnt++]=bTemp1;
        }
        for ( long izo=0; izo<iColUnPak; izo++ )
          bbufOut[izo]=0;

        long iPlan=0;
        long iShiftBack=0;
        for ( long iNrByPL=0; iNrByPL<PCXHeader.BytesPerLine; iNrByPL++ )  
          for ( long iBit=0; iBit<iBitBlock; iBit++ )
            bbufOut[iBitBlock*iNrByPL+iBitBlock-1-iBit]+=( (bbufIn[iPlan+iNrByPL]
                        >> iBit*PCXHeader.BitsPerPixel) & iConst ) << iShiftBack;
        if (j == 0)
          for (k=0; k < colorOut.iSize(); ++k) 
            colorOut[k] = bbufOut[k];
        else if (j == 1)
          for (k=0; k < colorOut.iSize(); ++k) 
            colorOut[k] += (bbufOut[k] << 8);
        else 
          for (k=0; k < colorOut.iSize(); ++k) 
            colorOut[k] += (bbufOut[k] << 16);
      }
      mp->PutLineRaw(i,colorOut);
    }  
  }  
  trq.fUpdate(iLine, iLine);
  mp->fErase=false;
}


