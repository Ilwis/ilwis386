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
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\Dat2.h"
#include "Engine\Base\File\File.h"
#include "Engine\Base\File\FILENAME.H"

// Compression method
#define NO_COMPRESSION    0
#define RLE_8_BITS        1
#define RLE_4_BITS        2

#pragma pack(1)
struct BMPFileHeader {
  char     cImageFileType[2];
  long     iBMPFileSize;         // size of the entire BMP file
  short    iReserved1;
  short    iReserved2;
  long     iDataOffset;
};

struct BMPInfoHeader {
  long     iInfoHeaderSize;       // size of this structure
  long     iImageWidth;
  long     iImageHeight;
  short    iPlaneCount;           // BMP supports only 1
  short    iBitsPerPixel;         // 1, 4, 8 or 24 bits
  long     iCompressionMethod;    // 0 (no compression), 1 or 2
  long     iBitmapSize;           // size of the image
  long     iHorizontalResolution; // pixels per meter, not geographic
  long     iVerticalResolution;   // pixels per meter, not geographic
  long     iNrPaletteColors;      // count colors in palette
  long     iNrSignificantColors;  // colors sorted by frequency
};

struct RGBType {
  RGBType() { bReserved = 0; }
  byte bBlue;
  byte bGreen;
  byte bRed;
  byte bReserved;
};

struct Pixel3Byte {
  byte bPixel[3];
};

struct BMPExportInfo
{
  short iBitsPerSample;   // can be 1(bit), 8(byte), or 24(colors)
  short iSamplesPerPixel;
  short iNrPalColors;             // valid colors in the LUT
  unsigned short aiColorMap[256 * 3]; // Color LUT for 8 bit
};
#pragma pack()

class BMPExporter
{
    public:
        BMPExporter(const FileName& _fnIn, const FileName& _fnOut);
        ~BMPExporter();
        virtual void Export(Tranquilizer&);

    protected:
        FileName        fnIn;
        FileName        fnOut;
        Map             inMap;
        File            *outFile;
        BMPFileHeader   bfhFile;
        BMPInfoHeader   bihInfo;
        RGBType         rgbPalette[256];
  
        void PrepareHeaders();
        void WriteHeadersAndPalette();

        void MakePalette(Representation&);
        void MakeIDPalette();
        
        void ExportImagePixels(Tranquilizer&);   // Images
        void ExportBoolPixels(Tranquilizer&);    // Bool maps
        void ExportValuePixels(Tranquilizer&);   // other value maps
        void ExportPicturePixels(Tranquilizer&); // Pictures
        void ExportColorPixels(Tranquilizer&);   // Color maps
        void ExportSortPixels(Tranquilizer&);    // Class maps
        void ExportIDPixels(Tranquilizer&);      // ID maps
        void ExportBitPixels(Tranquilizer&);     // bit maps

        void WriteAsRaw(Tranquilizer&);
        void WriteAsByteValues(Tranquilizer&);
        void WriteAsColor(Tranquilizer&);
        void WriteAsColorValues(Tranquilizer&);

        void WriteColor(const Color& color);
        
        RangeReal rrDetermineValueRange(Map&);
};


