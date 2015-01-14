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

#include "Engine\DataExchange\TableDelimited.h"
#include "Engine\DataExchange\TableIlwis14.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\CONV.hs"

class GetDomains
{
public:
	GetDomains(vector<ClmInfo> &clinfo, int& iSkipLines, int &iSpecFields) 
		: colInfo(clinfo), iOffset(iSkipLines), m_iSpecFields(iSpecFields) 
	{}
	void operator()(const String& sField)
	{
		ClmInfo ci;
		if ( sField[0] == '#' )
		{
			iOffset = String("%S", sField.substr(1)).iVal();
			return;
		}
		int iLastChar = sField.size() - 1;
		ci.dtDomainType = dmtVALUE;
		switch ( sField[ iLastChar ] )
		{
			case '#':
				m_iSpecFields++;
				ci.dvs = DomainValueRangeStruct(0, 255);
				ci.fnDomain = FileName("value");
				break;
			case '%':
				m_iSpecFields++;
				ci.dvs = DomainValueRangeStruct(-32766, 32767);
				ci.fnDomain =FileName("value");
				break;
			case '!':
				m_iSpecFields++;
				ci.dvs = DomainValueRangeStruct(-LONG_MAX + 1,LONG_MAX - 1);
				ci.fnDomain = FileName("value");
				break;
			case '&':
				m_iSpecFields++;
				ci.dvs = DomainValueRangeStruct(-1e38,1e38,0);
				ci.fnDomain = FileName("value");
				break;
			case '$':
				m_iSpecFields++;
				if ( fCIStrEqual(sField , "Name$") )
				{
					ci.dtDomainType = dmtID;
					ci.fKey = true;
					ci.fCreate = true;
					ci.fnDomain = FileName::fnUnique("Name.dom");
				}
				else
					ci.dtDomainType = dmtSTRING;
		}
		ci.sColumnName = sField.substr(0, sField.size() - 1);
		colInfo.push_back(ci);
	}

private: 	
	vector<ClmInfo> &colInfo;
	int &iOffset;
	int &m_iSpecFields;
};

//---[ TableIlwis14 ]---------------------------------------------------------------------------------

void TableIlwis14::ReplaceUnderScores(String& sField)
{
	replace(sField.begin(), sField.end(), '_', ' ');
	sField = sField.sTrimSpaces();
}

TableIlwis14::TableIlwis14(const FileName& fnObj, TablePtr& p) :
	TableDelimited(fnObj, p)
{
	Load();
}

TableIlwis14::TableIlwis14(const FileName& fnIn, const FileName& fnObj, TablePtr& p, const FileName& fnDat, int iSkip, vector<ClmInfo> &colinfo) :
	TableDelimited(fnIn, fnObj, p, fnDat, iSkip, TableExternalFormat::ifSpace, colinfo) 
{
	Load();
}

// Scan the ilwis 1.4 table:
// fnIn is the Ilwis 1.4 table file name
// iSkipLines: the number of lines to skip (should be 1 for ilwis 1.4)
// iSpecFields: Ilwis 1.4 columns with type specifiers (#=byte, !=int, $=string, &=real)
// colInfo: container for column information (type, name etc)
// fFullScan: check entire file; by default only the beginning of the file is scanned
// fUseColInfo: do not parse the column information again, but instead use the columnInfo to scan the
//              values in the records.
void TableIlwis14::Scan(const FileName& fnIn, int& iSkipLines, int& iSpecFields, vector<ClmInfo> &columnInfo, bool fFullScan, bool fUseColInfo)
{
	if (File::fIsBinary(fnIn))
		throw ErrorObject(String(TR("%S is not a text file").c_str(), fnIn.sFullPath()));

	File InputFile(fnIn);
	String sHeader;
	InputFile.ReadLnAscii(sHeader);
	sHeader = sHeader.sTrimSpaces();
	FieldValues record;
	ParseLine(sHeader, record, TableExternalFormat::ifIlwis14);
	if (!fUseColInfo)
	{
		GetDomains gd(columnInfo, iSkipLines, iSpecFields);
		for_each(record.begin(), record.end(), gd);

		return;
	}

	// Scan values for domain class/id
	int iRecs = 0;
	int iLine = 1;
	bool fHintOnly = !fFullScan;
	while (!InputFile.fEof() && !(fHintOnly && iLine > 10))
	{
		String sLine;
		InputFile.ReadLnAscii(sLine);
		iLine++;

		FieldValues line;
		ParseLine(sLine, line, TableExternalFormat::ifIlwis14);
		iRecs++;
		ScanStrings(line, columnInfo, iRecs);  // collect strings for class/id domains
	}
}

void TableIlwis14::ScanStrings(FieldValues& sFields, vector<ClmInfo> &scanInfo, int iRecs)
{
	if (scanInfo.size() == 0 )
		return; // should only be called for fully parsed structure

	int iField=0;
	for(FieldValues::iterator cur = sFields.begin(); cur != sFields.end(); ++cur)
	{
		String sField = *cur;
		double rV = sField.rVal();
		ClmInfo& ci = scanInfo[iField];
		if (ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID)
		{
			if (sField != sUNDEF)
				ci.Strings.insert(sField);
			ci.fKeyAllowed &= (ci.Strings.size() == iRecs);
		}
		++iField;
	}
	for (iField = 0; iField < sFields.size(); iField++)
	{
		ClmInfo& ci = scanInfo[iField];
		ci.vcStrings.resize(ci.Strings.size());
		copy(ci.Strings.begin(), ci.Strings.end(), ci.vcStrings.begin());
	}
}

bool TableIlwis14::SplitLine(CStdioFile& InputFile, FieldValues& result, long& iRec)
{
	bool fReturn = TableDelimited::SplitLine(InputFile, result, iRec);
	for_each(result.begin(), result.end(), ReplaceUnderScores);

	return fReturn;
}

void TableIlwis14::Store()
{
	TableDelimited::Store();
}

