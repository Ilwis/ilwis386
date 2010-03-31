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
  export .IMG format
  by Willem Nieuwenhuis, februari 2000
  ILWIS Department ITC
*/
#ifndef EXPORT_IDAIMAGE_H
#define EXPORT_IDAIMAGE_H

#pragma pack(1)
struct IDA_Header 
{
	unsigned char reserved1[22];
	unsigned char image_type;
	unsigned char projection;
	unsigned char reserved[6];
	short height;
	short width;
	unsigned char reserved3[4];
	unsigned char title[80];
	unsigned char reserved4[394];
};
#pragma pack()

class IDAExporter
{
    public:
        IDAExporter(const FileName& _fnIn, const FileName& _fnOut);
        ~IDAExporter();
        virtual void Export(Tranquilizer&);

    protected:
        FileName        fnIn;
        FileName        fnOut;
        Map             inMap;
        File            *outFile;
        IDA_Header      idaHeader;
  
        void WriteHeader();

        void ExportImagePixels(Tranquilizer&);   // Images
        void ExportBoolPixels(Tranquilizer&);    // Bool maps
        void ExportValuePixels(Tranquilizer&);   // other value maps
        void ExportSortPixels(Tranquilizer&);    // Class maps
        void ExportIDPixels(Tranquilizer&);      // ID maps
        void ExportBitPixels(Tranquilizer&);     // bit maps

        void WriteAsRaw(Tranquilizer&);
        void WriteAsByteValues(Tranquilizer&);
};

#endif
