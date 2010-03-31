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
  dbase .dbf format
  by Li Fei, March 96
  ILWIS Department ITC
	Last change:  WN   14 Mar 97   11:22 am
*/

/* Revision history:
   22/11/96 : Start version
   14/03/97 : Add boolean, to indicate if spaces in dBase records should be converted
              into '_'. ArcView does not accept spaces there. This conversion is only 
              performed for ArcView in case of non-value domains. Trailing spaces are 
              not affected.
              Example: "Canal doble" would become "Canal_doble"
*/

#ifndef DBASE_DEF
#define DBASE_DEF

#define CHANGE_INTERNAL_SPACES true

// maximum number of positions for numeric values
#define DBASE_MAXNUMERIC    19

class DomainToDBF;

class dBaseDBF {
private:
  struct File_Header {
     byte bValid;
     byte bDate[3];
     long iNrRecords;
     short iszHeader;
     short iszRecord;
     byte bReserved[20];
     File_Header();
  };
  struct DBF_Descriptor {
    char cNameField[11];
    char cField;
    byte bReserved1[4];
    byte bszField;
    byte bDecimal;
    byte bReserved2[14];
    DBF_Descriptor();
  };
protected:
  long iCols;
  ByteBuf bbufWidth;
  ByteBuf bbufDecimal;
  void SetDmDescriptor();
  void SetDmRecord(long iRec);
  short iRecIndex, iColIndex;
  bool fHasDm, fHasTbl;
  bool fConvertInternalSpaces;      // ArcView does not accept spaces in DBF records
  DomainValueRangeStruct dvs;
  Table  tbl;
  void iWidth(const DomainValueRangeStruct& dvrs, 
                      Column&,
                      bool fForDomain);
  void MinMaxRecs(long, long);
public:
  dBaseDBF();
  long iMinRec, iMaxRec;
  File_Header Header;
  DBF_Descriptor Descriptor;
  Array<char> carRecord;
  byte bEndDescriptor, bEndFile;
  void AllocRecord();
  void FillRealRecord(double);
  void FillRecord(long);
  bool fFillDescriptor();
private:
  void ConvertSpacesForArcView(short, const String&, Array<char>&);
  short iStrDmWidth();
  short iStrColWidth();
  Column colCur;
};

class DomainToDBF : public dBaseDBF {
public:
  DomainToDBF(const DomainValueRangeStruct&, long iRecs, bool fCnvForArcView = false);
};

class TableToDBF : public dBaseDBF {
public:
  TableToDBF(const Table&, long iRecs = 0, bool fCnvForArcView = false);
};

#endif


