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
#ifndef IMPDBF_H
#define IMPDBF_H

#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Table\tbl.h"

enum dtDomainType{dtValue, dtString, dtBool, dtDate, dtClass, dtId, dtNone,
dtTableClass, dtTableId, dtUnknown };
enum ccColumnClass { ccClass, ccId, ccValue, ccString };

class DBIVImport;
class DBInfo;

class DBInfo
{
	friend class DBIVImport;
	friend class DBColumnSelector;
private:
	struct Header              // header size = 32 bytes
	{
		byte  bValid;            // dBase control byte
		byte  bYear;             //
		byte  bMonth;            //
		byte  bDay;              // Date of last update
		long  iNrRecords;        // Number of records
		short iHeaderSize;       // Size of Header and Descriptors + 1
		short iRecordSize;       // Size of one record as described in all DBaseDescriptor
		byte  bReserved[20];     //   unused bytes
	};

public:
	DBInfo(const FileName& dbFile);

	String sName(short i);
	dtDomainType dtType(short i);
	void SetAltDomain(short i, dtDomainType type);
	byte bWidth(short i);
	byte bPrecision(short i);
	void ClearTableDomain();

	struct Descriptor           // descriptor size = 32 bytes
	{
		char cNameField[11];      // Name of Field
		char cType;               // Type of Field (char, numeric, date, etc)
		byte bReserved1[4];       //   unused bytes
		byte bFieldSize;          // Size of Field
		byte bDecimals;           // Number of decimals for numeric types
		byte bReserved2[14];      //   unused bytesDes

		//       Descriptor() {}
		//       ~Descriptor() {}
	};

protected:
	Array<Descriptor> fields;
	Array<dtDomainType> altDomain;

	long    iNrRecords;
	short   iHeaderSize;
	short   iRecordSize;
	short   iNrFields;
	short   iMaxFieldLength;

	String MakeField(char *rec, short iWidth, short& count);

};

class DBIVImport
{
	friend ImpExp;

public:
	DBIVImport(const FileName& in, const FileName& out, Tranquilizer&);

	bool fImport();
	void SetTableDomain(const Domain& dom);
	const Table& GetTable();

protected:
	DBInfo      Info;
	FileName    fnDBTable;
	FileName    fnIlwisTable;
	Domain      dmTable;
	Table       tbl;

	String MakeField(char *rec, short iWidth, short& count);
	ValueRange MakeValueRange(short iFld);
	void MakeTable();
	void MakeColumn(short iFld);
private:
	Tranquilizer& trq;
};

#endif


