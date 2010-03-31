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
#ifndef EXPORTGISLAN_H
#define EXPORTGISLAN_H

#define ATEND true

#pragma pack(1)
struct ErdasHeader
{
	char cHDWORD [6];
	short iIPACK;
	short iNBANDS;
	char cDUM1[6];
	long iCOLS;
	long iROWS;
	long iXSTART;
	long iYSTART;
	char cDUM2[56];
	short iMAPTYPE;
	short iNCLASS;
	char cDUM3[14];
	short iIAUTYP;
	float rACRE;
	float rXLeftUpper;
	float rYLeftUpper;
	float rXSize;
	float rYSize;
};
#pragma pack()

enum GISLANType { tpErr = 0,
               tpRawByte, tpRawInt,      // for Images and Pictures
               tpValByte, tpValInt,      // other value maps
               tpClassByte, tpClassInt}; // class maps

class GISExporter 
{
public:
    GISExporter(const FileName& fnFile);
    virtual void BuildHeader();
    void WriteHeader();
    void WriteTrlFile();
    short DetOutSize();
    virtual ~GISExporter();

    short iIOcase;                // Determined in DetOutSize
    Map mp;
    GeoRef gr;
    StoreType st;
    Domain dm;
    long iLines, iCols;
    long iNrClass;
    void SetErase(bool fErase);

    ByteBuf bBuf;
    IntBuf  iBuf;
    LongBuf lBuf;

    File* filGISLAN;

protected:
	ErdasHeader sHeader;

    FileName fnFile;
};

class LANExporter : public GISExporter
{
public:
    LANExporter(const FileName& fnFile);
    virtual ~LANExporter();
    virtual void BuildHeader();

    MapList mpl;
};

#endif
