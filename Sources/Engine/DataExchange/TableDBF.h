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
#include "Engine\DataExchange\TableExternalFormat.h"
#include <vector>

//! class MemoVector
struct MemoVector
{
	String       sColumnName;
	vector<int>  iValues;
};

typedef vector< MemoVector > MemoData;

class FieldAlgorithm
{
public:
	FieldAlgorithm(File& ip);

	virtual void operator()(ClmInfo &info) = 0;

protected:
	File  &inpFile;
};

class GetFieldStrings : public FieldAlgorithm
{
public:
	GetFieldStrings(File& ip);

	virtual void operator()(ClmInfo &info);
};

class SetColumnInfo : public FieldAlgorithm
{
public:
	SetColumnInfo(File& ip, bool fFull = false);

	virtual void operator()(ClmInfo &info);

protected:
	bool  m_fFull;
};

class TableDBF: public TableExternalFormat
{
public:
	TableDBF(const FileName& fnObj, TablePtr& p);
	TableDBF(const FileName& fnIn,
				const FileName& fnObj, TablePtr& p,
				const FileName& fnDat,
				vector<ClmInfo> &colinfo);

	~TableDBF();

	virtual void     Store();
	static void _export Scan(const FileName &fnDBF, vector<ClmInfo>& columnInfo, bool fFullScan = false, bool fUseColInfo = false);
	static bool _export fIsDBF(const FileName& fn);		

protected:
	virtual bool     SplitLine(File &InputFile, FieldValues& line, int& iRec);
	virtual void     ProcessHeader(File &InpFile);
	virtual void     StoreAsDBF();
	void             WriteField(File* Out, String sV, int iWidth,  const Domain& dm);
	void             WriteDescriptorField(File* Out, String sName, const Domain& dm, const ClmInfo::ExternalDomainType edtTpe );
	virtual void     PutExternalField(FieldValues &line, int &iMemoCol, int iRec, int iField);
	void             AddToTableDomain(FieldValues &line, int iKeyColumn);
	long             iNewRecord(long iRec);


#pragma pack(1)

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
	
	struct Descriptor           // descriptor size = 32 bytes
	{
		char cNameField[11];      // Name of Field
		char cType;               // Type of Field (char, numeric, date, etc)
		byte bReserved1[4];       //   unused bytes
		byte bFieldSize;          // Size of Field
		byte bDecimals;           // Number of decimals for numeric types
		byte bReserved2[14];      //   unused bytesDes
	};
#pragma pack()
	
private:
	int        iNrFields;
	int        iRecordSize;
	int        iHeaderSize;
	MemoData   memos;
	char       *m_buf;
};

/*! \class MemoVector
* Class used to hold information about the columns of a DBF file that contain memo fields.
*/
/*! \var  vector<int>  MemoVector::iValues Holds the index values of the memo column
*/
/*! \var  String MemoVector::sColumnName The name of the memo column.
*/
/*! \class TableDBF 
*   Can use a dbf file as input table and access the data in their native format or transform it to Ilwis 
*   format.
*/
/* !fn TableDBF::TableDBF(const FileName& fnObj, TablePtr& p)
*       Used to load defined tables from disk
* \param fnObj Ilwis table to be openend
*/


