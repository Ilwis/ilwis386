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
#ifndef ILWTBLDEL_H
#define ILWTBLDEL_H

#include "Engine\DataExchange\TableExternalFormat.h"
#include <set>
#include <vector>

//! class FldInfo 
class FldInfo
{
public:
	FldInfo();

	bool fDomainValue;
	bool fDomainYesNo;
	bool fDomainBool;
	bool fDomainID;
	bool fDomainClass;
	bool fDomainString;
	double rMin;
	double rMax;
	double rStep;
	double rLastValue;

	bool fKeyAllowed;

	set<String> Strings;
};

//! class MakeColumnInfo
class MakeColumnInfo
{
public:
	MakeColumnInfo(vector<ClmInfo> &columnInfo);

	vector<ClmInfo>& vcl;
	int iColumn;

  void operator()(const FldInfo& v);
};

//! class TableDelimited
class TableDelimited: public TableExternalFormat
{
public:
	TableDelimited(const FileName& fnObj, TablePtr& p);
	TableDelimited(const FileName& fnIn,
					const FileName& fnObj, TablePtr& p,
					const FileName& fnDat,
					int iSkip, TableExternalFormat::InputFormat iff, 
					vector<ClmInfo> &colinfo);

	TableDelimited::~TableDelimited() {};
	static _export void Scan(FileName fnObj, int &iSkip, TableExternalFormat::InputFormat& eDelim, vector<ClmInfo>& columnInfo, bool fFullScan = false, bool fUseColInfo = false);
	static _export void GetFormatType(FileName fnObj, TableExternalFormat::InputFormat& eDel, int& iSkip, int &iCols, bool fHintOnly = false);
	virtual void Store();

protected:
	virtual bool SplitLine(CStdioFile& InputFile, FieldValues& result, long& iRec);
	virtual void ProcessHeader(CStdioFile& InputFile);
	
	static void ScanLine(FieldValues& sFields, vector<FldInfo> &scanInfo, bool fHintOnly, int iRecs);
	static void ScanStrings(FieldValues& sFields, vector<FldInfo> &scanInfo, int iRecs);
	static void ParseLine(const String& sLine, FieldValues& result, TableExternalFormat::InputFormat eDel);
	void ParseLine(const String& sLine, FieldValues& result);
	static void ScanForStructure(const String& sLine, int& iNumberOfSpaceFields, int& iNumberOfCommaFields );

private:
};

//! class TableAsciiDelimited
class TableAsciiDelimited : public TableDelimited
{
public:
	TableAsciiDelimited(const FileName& fnObj, TablePtr& p);
	TableAsciiDelimited(const FileName& fnIn,
					const FileName& fnObj, TablePtr& p,
					const FileName& fnDat,
					int iSkip, TableExternalFormat::InputFormat iff, 
					vector<ClmInfo> &colinfo);

	TableAsciiDelimited::~TableAsciiDelimited() {};

};

//! class SplitOnDelimeter
class SplitOnDelimeter
{
public:
	SplitOnDelimeter(TableExternalFormat::InputFormat eDel, FieldValues &L, int iSz);
	void operator()(char c);

protected:
private:
	bool fWithinQuotes;
	FieldValues &line;
	string sWord;
	TableExternalFormat::InputFormat ifFormat;
	int iSize;
	int iWhere;
};

#endif // ILWTBLDEL_H
