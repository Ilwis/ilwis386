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
#pragma warning( disable : 4786 )

#include "Engine\DataExchange\TableExternalFormat.h"
#include "Engine\DataExchange\TableDelimited.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"


//--[ MakeColumnIfno] ----------------------------------------------------------------------
MakeColumnInfo::MakeColumnInfo(vector<ClmInfo>& columnInfo) :
	vcl(columnInfo),
	iColumn(0)   
{}

void MakeColumnInfo::operator()(const FldInfo& v)
{
	ClmInfo clm;
	if ( v.fDomainValue )
	{
		clm.dvs = DomainValueRangeStruct(v.rMin, v.rMax, v.rStep);
		clm.dtDomainType = dmtVALUE;
		clm.fnDomain = FileName("value.dom");
	}
	else if ( v.fDomainBool )
	{
		clm.dtDomainType = dmtBOOL;
  		clm.fnDomain = FileName("bool.dom");
	}
	else if ( v.fDomainYesNo )
	{
		clm.dtDomainType = dmtBOOL;
		clm.fnDomain = FileName("YesNo.dom");
	}
	else if ( v.fDomainClass  || v.fDomainID )
	{
		FileName fnDomain(FileName::fnUnique(String("%s%d.dom", (v.fDomainClass ? "Classes" : "IDS"), iColumn)));
		clm.dtDomainType = v.fDomainClass ? dmtCLASS : dmtID;
		clm.fnDomain = fnDomain;
		clm.fCreate = true;
		clm.fExtend = true;
	}
	else
	{
		clm.dtDomainType = dmtSTRING;
		clm.fnDomain = FileName("String");
	}
	clm.sColumnName = String("Column%d", ++iColumn);
	vcl.push_back(clm);
}

//--[ FldInfo ]-----------------------------------------------------------------------------
FldInfo::FldInfo() :
	fDomainBool(true),
	fDomainValue(true),
	fDomainYesNo(true),
	fDomainClass(true),
	fDomainID(true),
	fDomainString(true),
	fKeyAllowed(true),
	rMin(rUNDEF),
	rMax(rUNDEF),
	rStep(rUNDEF)
{}

 //--[ SplitOnDelimeter ]----------------------------------------------------------------------
SplitOnDelimeter::SplitOnDelimeter(TableExternalFormat::InputFormat eDel, FieldValues &L, int iSz) : 
			ifFormat(eDel), fWithinQuotes(false), line(L), iSize(iSz)
{
	iWhere = 0;
	line.resize(0);
}			

void SplitOnDelimeter::operator()(char c)
{
	if ( c == '\"')
		fWithinQuotes = !fWithinQuotes;
	else 
	{
		if ( !fWithinQuotes && // anything between quotes will be accepted
			 ((ifFormat == TableExternalFormat::ifComma && c   == ',' )  ||  // encountering a delimeter
			  (ifFormat == TableExternalFormat::ifSpace && c   == ' ')   ||  // will trigger putting the word
			  (ifFormat == TableExternalFormat::ifFixed && c   == ' ')   ||  // in the list
			  (ifFormat == TableExternalFormat::ifSpace && c   == '\t')  ||  // 
			  (ifFormat == TableExternalFormat::ifIlwis14 && c == ' ')   ||  //
			  (ifFormat == TableExternalFormat::ifIlwis14 && c == '\t')))    //
		{
			if (( ifFormat == TableExternalFormat::ifSpace) ||
				( ifFormat == TableExternalFormat::ifFixed))
			{  
				if ( c == ' ' && sWord != "") // skip spaces between two fields
					line.push_back(sWord);
				else if ( c == '\t' )  // each separate TAB delimits two columns
					line.push_back(sWord);
			}
			else if ( ifFormat == TableExternalFormat::ifIlwis14)
			{  
				if ( (c == ' ' || c == '\t') && sWord != "") // skip spaces and tabs between two fields
					line.push_back(sWord);
			}
			else
				line.push_back(sWord);

			sWord = "";
		}
		else
			sWord += c;

	}
	if ( iWhere == iSize - 1) // ending the line will also put the word in the line
		line.push_back(sWord);

	++iWhere;
}
//--[ TableDelimited ]--------------------------------------------------------------------------

TableDelimited::TableDelimited(const FileName& fnObj, TablePtr& p)
: TableExternalFormat(fnObj, p)
{
}

TableDelimited::TableDelimited(
		const FileName& _FileIn,
		const FileName& fnObj, TablePtr& p,
		const FileName& fnDat,
		int iSkip, TableExternalFormat::InputFormat eDel,
		vector<ClmInfo> &clInfo)
	: TableExternalFormat(_FileIn, fnObj, p, fnDat, iSkip, eDel, clInfo)
{
}

void TableDelimited::ProcessHeader(CStdioFile& InputFile)
{
	CString sDummy;
	int iSkip = iSkipLines;
	for ( iSkip = 0; iSkip< iSkipLines; ++iSkip)
		InputFile.ReadString(sDummy);
}

bool TableDelimited::SplitLine(CStdioFile& InputFile, FieldValues& result, long& iRec)
{
	CString sLine;
	InputFile.ReadString(sLine);
	if (sLine.GetLength() == 0) return false;

	if ( ifFormat == TableExternalFormat::ifComma || ifFormat == TableExternalFormat::ifSpace )
		ParseLine(String(sLine), result, ifFormat);
	else
		ParseLine(String(sLine), result);

	return true;
}

String ExpandTabs(const String& sLine)
{
	String sResult;
	int iStart = 0;
	int iPos = sLine.find("\t");
	while (iPos != string::npos)
	{
		sResult &= sLine.substr(iStart, iPos - iStart);
		int iSpaces = 8 - sResult.length() % 8;
		sResult &= String("%*s", iSpaces, "");
		
		iStart = iPos + 1;
		iPos = sLine.find("\t", iPos + 1);
	}
	sResult &= sLine.substr(iStart, sLine.length() - iStart);

	return sResult;
}

// ParseLine() reads substrings from a string. The start position and the
// number of characters to read are determined by colInfo.
// If a line contains TAB characters these are expanded first to 8 positions
void TableDelimited::ParseLine(const String& sLineIn, FieldValues& result)
{
	String sLine;
	if (sLineIn.find("\t"))
		sLine = ExpandTabs(sLineIn);
	else
		sLine = sLineIn;
	
	int iBegin=0;
	int iEnd=0;
	for(int i=0; i < colInfo.size(); ++i)
	{
		iEnd = iBegin + colInfo[i].iColumnWidth;
		if ((i == colInfo.size() - 1 && colInfo[i].iColumnWidth == 0)  || // autosize last column
		    (iEnd > sLine.length()))   // or iEnd passes end of line
			iEnd = sLine.length();
		result.push_back(sLine.substr(iBegin, iEnd - iBegin));
		iBegin = iEnd;
	}
}
	
void TableDelimited::ParseLine(const String& sLine, FieldValues& result, TableExternalFormat::InputFormat eDel)
{
	SplitOnDelimeter splitter(eDel, result, sLine.size());
	for_each(sLine.begin(), sLine.end(), splitter);
}

void TableDelimited::ScanForStructure(const String& sLine, int& iNumberOfSpaceFields, int& iNumberOfCommaFields )
{
	bool fSpaceState = false;
	iNumberOfSpaceFields = iNumberOfCommaFields = 0;
	if (sLine.length() == 0)
		return;

	for( int i=0; i < sLine.size(); ++i)
	{
		if ( sLine[i] == '\t' || sLine[i] == ' ')
		{
			if ( !fSpaceState)
				++iNumberOfSpaceFields;
			fSpaceState = sLine[i] == ' ' ? true : false;
		}
		else if (sLine[i] == ',')
		{
			++iNumberOfCommaFields;
			fSpaceState = false;
		}
		else 
			fSpaceState = false;
	}

}

// try to determine the type of the ascii table
// if fHintOnly is true then only check the first 10 lines, otherwise the entire file
// fHintOnly is useful when used from the inport form.
void TableDelimited::GetFormatType(FileName fnObj, TableExternalFormat::InputFormat& eDel, int& iSkip, int &iCols, bool fHintOnly)
{
	if (File::fIsBinary(fnObj))
		throw ErrorObject(String("%S is not a text file", fnObj.sFullPath()));

	File InputFile(fnObj);
	int iNumberOfCommaFields = 0;
	int iNumberOfSpaceFields = 0;
	int iSpaceHeader = 0;
	int iCommaHeader = 0;
	int iLine = 0;
	bool fFixed = false;

	while( !InputFile.fEof() && !(fHintOnly && iLine > 10))
	{
		String sLine;
		InputFile.ReadLnAscii(sLine);
		sLine = sLine.sTrimSpaces();
		int iNrCF, iNrSF;
		ScanForStructure(sLine, iNrSF, iNrCF);
		if (iNrSF > 0)
		{
			if (iNumberOfSpaceFields == 0)
				iNumberOfSpaceFields = iNrSF + 1;
			else if (iNrSF > iNumberOfSpaceFields)
				iNumberOfSpaceFields = iNrSF + 1;
		}
		if (iNrCF > 0)
		{
			if (iNumberOfCommaFields == 0)
				iNumberOfCommaFields = iNrCF + 1;
			else if (iNrCF < iNumberOfCommaFields)
			{
				iNumberOfCommaFields = iNrCF + 1;
				iCommaHeader = iLine;
			}
		}
		++iLine;
	}
	if ( eDel == ifUnknown )
	{
		if (iNumberOfCommaFields != 0)
			eDel = TableExternalFormat::ifComma;
		else if (fFixed)
			eDel = TableExternalFormat::ifFixed;
		else if ( iNumberOfSpaceFields != 0 && iNumberOfCommaFields == 0)
			eDel = TableExternalFormat::ifSpace;
		else if ( iNumberOfSpaceFields != 0 && iNumberOfCommaFields != 0)
			eDel = iSpaceHeader < iCommaHeader ? TableExternalFormat::ifSpace : TableExternalFormat::ifComma;
		else if ( iNumberOfSpaceFields == 0 && iNumberOfCommaFields == 0) {
			iNumberOfCommaFields = 1;
			eDel = TableExternalFormat::ifComma;
		} else
			eDel = TableExternalFormat::ifUnknown;
	}
	switch (eDel)
	{
		case TableExternalFormat::ifComma:
			iCols = iNumberOfCommaFields;
			break;
		case TableExternalFormat::ifSpace:
		case TableExternalFormat::ifFixed:
			iCols = iNumberOfSpaceFields;
			break;
		default:
			iCols = 0;
			break;
	}
//	iSkip = min(iSpaceHeader, iCommaHeader);
}

// Scan the delimited table:
// fnObj is the delimited table file name
// iSkipLines: the number of lines to skip
// eDel: the type of the input table (Comma, Space or Fixed)
// columnInfo: container for column information (type, name etc)
// fFullScan: check entire file; by default only the beginning of the file is scanned
// fUseColInfo: do not parse the column information again, but instead use the columnInfo to scan the
//              values in the records.
void TableDelimited::Scan(FileName fnObj, int &iSkipLines, TableExternalFormat::InputFormat& eDel, 
		vector<ClmInfo>& columnInfo, bool fFullScan, bool fUseColInfo)
{
	int iNrCols;
	bool fHintOnly = !fFullScan;
	TableDelimited::GetFormatType(fnObj, eDel, iSkipLines, iNrCols, fHintOnly);
	int iSkip = iSkipLines;

	File InputFile(fnObj);
	vector<FldInfo> scanInfo;
	int iRecs = 0;
	int iLine = 0;
	FieldValues header;
	while( !InputFile.fEof() && !(fHintOnly && iLine > 10))
	{
		String sLine;
		InputFile.ReadLnAscii(sLine);
		iLine++;
		if ( iSkip > 0 )
		{
			if (iLine == 1) {
				ParseLine(sLine, header, eDel);
			}
			iSkip--;
			continue;
		}
		FieldValues line;
		ParseLine(sLine, line, eDel);
		iRecs++;
		if (fUseColInfo)
			ScanStrings(line, scanInfo,iRecs);  // collect strings for class/id domains
		else
			ScanLine(line, scanInfo, fHintOnly, iRecs); // collect column info from table

	}
	// read last line and include it in the scan
	const long iMaxLineSize = 8196;
	if (fHintOnly && InputFile.iSize() > iMaxLineSize) {
		InputFile.Seek(-iMaxLineSize, true);
		String sLine;
		++iRecs;
		while(!InputFile.fEof()) {
			InputFile.ReadLnAscii(sLine); // find the last line and keep it in sLine
		}
		FieldValues line;
		ParseLine(sLine, line, eDel);
		if (fUseColInfo)
			ScanStrings(line, scanInfo,iRecs);  // collect strings for class/id domains
		else
			ScanLine(line, scanInfo, fHintOnly, iRecs); // collect column info from table
	}
	if (!fUseColInfo)
	{
		columnInfo.clear();
		MakeColumnInfo inf(columnInfo);
		for_each(scanInfo.begin(), scanInfo.end(), inf);
	}

	for (int i = 0; i < columnInfo.size(); i++)
	{
		ClmInfo& ci = columnInfo[i];
		ci.iNrRecs = iRecs;
		if (iSkipLines > 0)
			ci.sColumnName = header[i].sTrimSpaces();
		if (fUseColInfo && i < scanInfo.size())
			ci.Strings = scanInfo[i].Strings;
		ci.vcStrings.resize(ci.Strings.size());
		copy(ci.Strings.begin(), ci.Strings.end(), ci.vcStrings.begin());
	}
}

void TableDelimited::ScanLine(FieldValues& sFields, vector<FldInfo> &scanInfo, bool fHintOnly, int iRecs)
{
	if (scanInfo.size() <= sFields.size() )
		scanInfo.resize(sFields.size());
	int iField=0;
	for(FieldValues::iterator cur = sFields.begin(); cur != sFields.end(); ++cur)
	{
		String sField = *cur;
		if (sField.length() == 0 || sField == sUNDEF)
			continue; // ignore this in the "Domain" decision
		double rV = sField.rVal();
		FldInfo& fi = scanInfo[iField];
		if ( rV != rUNDEF)
		{
			fi.rMax = rMAX(rV, fi.rMax);
			fi.rMin = rMIN(rV, fi.rMin);
			String sFrac = sField.sTail(".").sTrimSpaces();
			if ( rV - int(rV) == 0.0 ) sFrac = ""; // maybe its all zero's
			int iSz = -sFrac.size();
			double rS = rMIN(fi.rStep, pow(10.0, iSz));
			fi.rStep = rMIN(fi.rStep, rS);
		}
		else
		{
			fi.fDomainValue = false;
			if (!fCIStrEqual(sField , "T") &&
				!fCIStrEqual(sField , "F") &&
				!fCIStrEqual(sField , "True") &&
				!fCIStrEqual(sField , "False") )
			{
				fi.fDomainBool = false;
			}
			if (!fCIStrEqual(sField , "Y") &&
   				!fCIStrEqual(sField , "N") &&
	  			!fCIStrEqual(sField , "Yes") &&
		  		!fCIStrEqual(sField , "No") )
			{
				fi.fDomainYesNo = false;
			}
			fi.fDomainClass = fi.fDomainID = false;
/*			if ( sField.size() > 20 )
				fi.fDomainClass = false;
			if ( fi.Strings.size() > 60 )
				fi.fDomainClass = false;*/

			if (fi.fKeyAllowed)
				if (!fHintOnly || fi.Strings.size() <= 60)
				{
					if (sField != sUNDEF)
						fi.Strings.insert(sField);
					fi.fKeyAllowed &= (fi.Strings.size() == iRecs);
				}
				
			if (sField.size() > 40 )
			{
				fi.fDomainValue = false;
				fi.fDomainYesNo = false;
				fi.fDomainBool = false;
				fi.fDomainID = false;
				fi.fDomainClass = false;
				fi.fDomainString = true;
			}
		}
		++iField;
	}
}

void TableDelimited::ScanStrings(FieldValues& sFields, vector<FldInfo> &scanInfo, int iRecs)
{
	if (scanInfo.size() == 0 )
		return; // should only be called for fully parsed structure

	int iField=0;
	for(FieldValues::iterator cur = sFields.begin(); cur != sFields.end(); ++cur)
	{
		String sField = *cur;
		double rV = sField.rVal();
		FldInfo& fi = scanInfo[iField];
		if (fi.fDomainClass || fi.fDomainID)
		{
			if (sField != sUNDEF)
				fi.Strings.insert(sField);
			fi.fKeyAllowed &= (fi.Strings.size() == iRecs);
		}
		++iField;
	}
}

void TableDelimited::Store()
{
	TableExternalFormat::Store();
	if ( fnData().sExt != ".tb#" ) 
		ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(),"Type", ptr.fnObj, "TableDelimited");
	else
		ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(),"Type", ptr.fnObj, "TableBinary");
}


//--[ TableAsciiDelimited ]----------------------------------------------------------------------
TableAsciiDelimited::TableAsciiDelimited(const FileName& fnObj, TablePtr& p)
	: TableDelimited(fnObj, p)
{
	Load();
}

TableAsciiDelimited::TableAsciiDelimited(const FileName& fnIn, const FileName& fnObj, TablePtr& p,
		const FileName& fnDat, int iSkip, TableExternalFormat::InputFormat iff, 
		vector<ClmInfo> &colinfo)
	: TableDelimited(fnIn, fnObj, p, fnDat, iSkip, iff, colinfo)
{
	Load();
}
