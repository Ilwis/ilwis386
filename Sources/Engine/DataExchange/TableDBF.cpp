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

#include <time.h>
#include "Engine\DataExchange\TableDBF.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\CONV.hs"

FieldAlgorithm::FieldAlgorithm(File& ip) :
	inpFile(ip)
{}

//--[ SetColumnInfo ]-----------------------------------------------------------------
SetColumnInfo::SetColumnInfo(File& ip, bool fFull) :
	FieldAlgorithm(ip), m_fFull(fFull)
{}

void SetColumnInfo::operator()(ClmInfo &info)
{
	if ( info.dtDomainType == dmtUNKNOWN)
		return;

	info.iNrRecs++;
	char *buf = new char[info.iColumnWidth + 1];

	int iL = inpFile.iLoc();
	inpFile.Read(info.iColumnWidth, buf); 
	buf[info.iColumnWidth] = 0;
	String sValue = String(buf).sTrimSpaces();
	double rV = sValue.rVal();
	if ( info.dvs.dm()->pdv() && rV != rUNDEF )
	{
		double rStep = info.dvs.vr()->rStep();
		if ( rStep != 1.0 )
		{
			double rMin = info.dvs.rrMinMax().rLo();
			double rMax = info.dvs.rrMinMax().rHi();
			rMin = (( rMin < rV ) && rMin != -1e307) ? rMin : rV;
			rMax = (( rMax > rV ) && rMax != 1e307 ) ? rMax : rV;
			info.dvs = DomainValueRangeStruct(rMin, rMax, rStep);
		}
		else
		{
			int iMin = info.dvs.riMinMax().iLo();
			int iMax = info.dvs.riMinMax().iHi();
			iMin = (( iMin < rV ) && iMin != -1e9) ? iMin : rV;
			iMax = (( iMax > rV ) && iMax != 1e9 ) ? iMax : rV;
			info.dvs = DomainValueRangeStruct(iMin, iMax, (int)rStep);
		}
	}
	else
	{
/*		if ( sValue.size() > 20 )
			info.dtDomainType = dmtID;
		
		if ( info.Strings.size() > 60 )
			info.dtDomainType = dmtID;*/

		if (info.fKeyAllowed)
			if (m_fFull || info.Strings.size() <= 60)
			{
				if (sValue != sUNDEF && sValue.length() > 0)
					info.Strings.insert(sValue);
				info.fKeyAllowed &= (info.Strings.size() == info.iNrRecs);
			}

		if ( sValue.size() > 40 )
		{
			info.dtDomainType = dmtSTRING;
			info.fnDomain = "string";
		}
	}
	
	delete [] buf;
}
//--[ GetFieldStrings ]-----------------------------------------------------------------
GetFieldStrings::GetFieldStrings(File& ip) :
	FieldAlgorithm(ip)
{}

void GetFieldStrings::operator()(ClmInfo &info)
{
	if ( info.dtDomainType == dmtUNKNOWN)
		return;

	char *buf = new char[info.iColumnWidth + 1];

	int iL = inpFile.iLoc();
	inpFile.Read(info.iColumnWidth, buf); 
	buf[info.iColumnWidth] = 0;
	String sValue = String(buf).sTrimSpaces();
	double rV = sValue.rVal();
	if (rV != rUNDEF)
		return;
	else if (sValue != sUNDEF)
		info.Strings.insert(sValue);
	
	delete [] buf;
}

//--[ TableDBF ]----------------------------------------------------------------------
TableDBF::TableDBF(const FileName& fnObj, TablePtr& p)
	: TableExternalFormat(fnObj, p),
		m_buf(0)
{
	Header hdrDBF;
	File InputFile(fnData());
	if ( InputFile.fEof() )
		throw ErrorObject(String(SDATErrCouldNotFind_S.scVal(), fnData().sFullPath()));

	InputFile.Read(sizeof(hdrDBF ), &hdrDBF);

	int iNrFields = (hdrDBF.iHeaderSize - sizeof(Header) - 1) / sizeof(Descriptor);

	Descriptor descriptor;
	for (short iField = 0; iField < iNrFields; iField++)
	{
		InputFile.Read(sizeof(Descriptor), &descriptor);
		colInfo[iField].sColumnName = String(descriptor.cNameField);
		colInfo[iField].iColumnWidth = descriptor.bFieldSize;
		if ( colInfo[iField].edtType == ClmInfo::edtDBFMemo )
		{
			MemoVector v;
			v.sColumnName = colInfo[iField].sColumnName;
			memos.push_back(v);
		}
	}

	Load();

	for ( int iCol=0; iCol < iCols(); ++iCol)
			ac[iCol]->SetReadOnly(false);
}

TableDBF::TableDBF(
    const FileName& _FileIn,
    const FileName& fnObj, TablePtr& p,
    const FileName& fnDat,
    vector<ClmInfo> &colInfo) :
  TableExternalFormat(_FileIn, fnObj, p, fnDat, iUNDEF, ifDBF, colInfo),
	  m_buf(0)
{
	for(vector<ClmInfo>::iterator cur = colInfo.begin(); cur != colInfo.end(); ++cur)
	{
		if ( (*cur).edtType == ClmInfo::edtDBFMemo )
		{
			MemoVector v;
			v.sColumnName = (*cur).sColumnName;
			memos.push_back(v);
		}
	}

	Load();
}

TableDBF::~TableDBF()
{
	if (m_buf != 0)
		delete [] m_buf;
}

// Scan the DBF table:
// fnDBF is the file name
// columnInfo: container for column information (type, name etc)
// fFullScan: check entire file; by default only the beginning of the file is scanned
// fUseColInfo: do not parse the header again, but instead use the columnInfo to scan the
//              values in the records.
void TableDBF::Scan(const FileName &fnDBF, vector<ClmInfo>& columnInfo, bool fFullScan, bool fUseColInfo)
{
	Header hdrDBF;
	File InputFile(fnDBF);
	if ( InputFile.fEof() )
		throw ErrorObject(String(SDATErrCouldNotFind_S.scVal(), fnDBF.sFullPath()));

	InputFile.Seek(0);
	InputFile.Read(sizeof(hdrDBF ), &hdrDBF);

	long iNrFields = (hdrDBF.iHeaderSize - sizeof(Header) - 1) / sizeof(Descriptor);
	long iNrRecords = hdrDBF.iNrRecords;
	long iRecordSize = hdrDBF.iRecordSize;
	long iHeaderSize = hdrDBF.iHeaderSize;
	long iCalcSize = iHeaderSize + iNrRecords * iRecordSize;
	// Check the file size 
	if (iCalcSize <= 0)
		throw ErrorObject(SCVImpDBFheaderincorrect);
	// Check the file size against what is expected:
	if (iCalcSize > InputFile.iSize())
		throw ErrorObject(SCVImpDBFFileTooSmall);
	// Check the record size, must be smaller than 32768
	if (iRecordSize > 32767)
		throw ErrorObject(SCVImpRecordsTooLarge);
	// The number of records and also the number of fields should be larger than 0
	if (iNrFields <= 0)
		throw ErrorObject(SCVImpFieldCountIncorrect);
	if (iNrRecords <= 0)
		throw ErrorObject(SCVImpRecordCountIncorrect);

	if (!fUseColInfo)
	{
		Descriptor descriptor;
		columnInfo.resize(iNrFields);
		for (short iField = 0; iField < iNrFields; iField++)
		{
			int iL = InputFile.iLoc();
			InputFile.Read(sizeof(Descriptor), &descriptor);
			columnInfo[iField].iColumnWidth = descriptor.bFieldSize;

			char cType = descriptor.cType;
			descriptor.cType = '\0';
			columnInfo[iField].sColumnName = String(descriptor.cNameField).sTrimSpaces();
			switch (cType)
			{
				case 'L':
					columnInfo[iField].dtDomainType = dmtBOOL;
					columnInfo[iField].fnDomain = "bool";
					break;
				case 'F' :
				case 'N' :
					{
						int iNumPos = descriptor.bFieldSize;
						if (descriptor.bDecimals > 0)
							iNumPos -= descriptor.bDecimals + 1;  // + 1: for the "." character
						double rDec = pow(10.0, -descriptor.bDecimals);
						double rMant = pow(10.0, iNumPos) - rDec;
						columnInfo[iField].dvs = DomainValueRangeStruct(-rMant, rMant, rDec );
						columnInfo[iField].dtDomainType = dmtVALUE;
						columnInfo[iField].fnDomain = "value";
					}
					break;
				case 'D':
					columnInfo[iField].edtType = ClmInfo::edtDBFDate;
					columnInfo[iField].dvs = DomainValueRangeStruct(-1e9, 1e9, 1);
					columnInfo[iField].dtDomainType = dmtVALUE;
					columnInfo[iField].fnDomain = "value";
					break;
				case 'C':
					columnInfo[iField].dtDomainType = dmtSTRING;
					columnInfo[iField].fnDomain = "String.dom";
					columnInfo[iField].fCreate = false;
					columnInfo[iField].fExtend = false;
					break;
				default:
					columnInfo[iField].dtDomainType = dmtUNKNOWN;
					columnInfo[iField].edtType = ClmInfo::edtDBFMemo;
			}
			columnInfo[iField].fnDomain.sExt = ".dom";
		}
	}

	InputFile.Seek(iHeaderSize);
	int iField = 0;
	for (; iField < iNrFields; iField++)
		columnInfo[iField].iNrRecs = iNrRecords;

	if (!fFullScan)
		return;

	if (fUseColInfo)  // collect strings for classes/id's
	{
		GetFieldStrings inf(InputFile);

		while (!InputFile.fEof())
			if (InputFile.cReadChar() == ' ') // delete records are skipped
				for_each(columnInfo.begin(), columnInfo.end(), inf);
	}
	else
	{
		// Initialize to zero; needed for SetColumnInfo
		for (iField = 0; iField < iNrFields; iField++)
			columnInfo[iField].iNrRecs = 0;

		SetColumnInfo inf (InputFile, fFullScan);
		while (!InputFile.fEof())
			if (InputFile.cReadChar() == ' ') // delete records are skipped
				for_each(columnInfo.begin(), columnInfo.end(), inf);
	}


	for (iField = 0; iField < iNrFields; iField++)
	{
		ClmInfo& ci = columnInfo[iField];
		ci.iNrRecs = iNrRecords;
		ci.vcStrings.resize(ci.Strings.size());
		copy(ci.Strings.begin(), ci.Strings.end(), ci.vcStrings.begin());
	}
}

bool TableDBF::fIsDBF(const FileName& fn)
{
	Header hdrDBF;
	File file(fn);
	file.Read(sizeof(hdrDBF ), &hdrDBF);
	if ( hdrDBF.bValid == 0x03 || hdrDBF.bValid == 0x04 || hdrDBF.bValid == 0x05 )
	{
		if ( hdrDBF.bReserved[18] == 0 && hdrDBF.bReserved[19] == 0 ) 
		{
			int iNrFields = (hdrDBF.iHeaderSize - sizeof(Header) - 1) / sizeof(Descriptor);	
			int iEnd = sizeof(hdrDBF) + iNrFields * sizeof(Descriptor);
			file.Seek(iEnd);
			char buf[1];
			file.Read(1, buf);
			if ( *buf == 0x0D )
				return true;
		}			
	}		
	return false;	
}


void TableDBF::ProcessHeader(File &InputFile)
{
	Header hdrDBF;
	InputFile.Read(sizeof(hdrDBF ), &hdrDBF);
	iTotalRec = hdrDBF.iNrRecords;

	Descriptor descriptor;
	int iNrFields = (hdrDBF.iHeaderSize - sizeof(Header) - 1) / sizeof(Descriptor);
	if (colInfo.size() == 0)
	{
		colInfo.resize(iNrFields);
		int iIlWisColumn = 0;

		for (short iField = 0; iField < iNrFields; iField++)
		{
			int iL = InputFile.iLoc();
			InputFile.Read(sizeof(Descriptor), &descriptor);
			colInfo[iField].iColumnWidth = descriptor.bFieldSize;
			colInfo[iField].iNrRecs = iTotalRec;
			char cType = descriptor.cType;
			descriptor.cType = '\0';
			colInfo[iField].sColumnName = String(descriptor.cNameField).sTrimSpaces();

			if ( sKeyColumn == colInfo[iField].sColumnName)
			{
				colInfo[iField].dtDomainType = ptr.dm()->dmt();
				continue;
			}

			switch (cType )
			{
				case 'L':
					colInfo[iField].dtDomainType = dmtBOOL;
					++iIlWisColumn;
					break;
				case 'F' :
					colInfo[iField].dtDomainType = dmtVALUE;
					++iIlWisColumn;
					break;
				case 'D':
					colInfo[iField].edtType = ClmInfo::edtDBFDate; // fall through dates are implemented as number
				case 'N' :
					colInfo[iField].dtDomainType = dmtVALUE;
					++iIlWisColumn;
					break;
				case 'C': // must rely on previous assessment, if changes are made outside ilwis, trouble here.
					colInfo[iField].dtDomainType = ac[iIlWisColumn++]->dm()->dmt();
					break;
				case 'M':
					colInfo[iField].edtType = ClmInfo::edtDBFMemo;
			}
		}
	}

	InputFile.Seek(hdrDBF.iHeaderSize);
}

bool TableDBF::SplitLine(File &InputFile, FieldValues& line, int& iRec)
{
	if ( InputFile.cReadChar() != ' ') // delete records are skipped
		return false;

	if (m_buf == 0)
	{
		int iBufSize = 0;
		for ( int iColumn = 0; iColumn < colInfo.size(); ++iColumn)
			iBufSize = max(iBufSize, colInfo[ iColumn ].iColumnWidth + 1);
		m_buf = new char[iBufSize];
	}

	for ( int iColumn = 0; iColumn < colInfo.size(); ++iColumn)
	{
		ClmInfo cli = colInfo[ iColumn ];
		if ( cli.dtDomainType == dmtUNKNOWN && cli.edtType != ClmInfo::edtDBFMemo )
			continue;
	
		InputFile.Read(cli.iColumnWidth, m_buf);
		m_buf[ cli.iColumnWidth ] = 0;
		String sValue = String(m_buf).sTrimSpaces();
		line.push_back(sValue);
	}
	
	return true;
}

void TableDBF::AddToTableDomain(FieldValues &line, int iKeyColumn)
{
	TableExternalFormat::AddToTableDomain(line, iKeyColumn);

	for(vector<MemoVector>::iterator cur = memos.begin(); cur != memos.end(); ++cur)
		(*cur).iValues.push_back(iUNDEF);
}

int TableDBF::iNewRecord(int iRec)
{
	for(vector<MemoVector>::iterator cur = memos.begin(); cur != memos.end(); ++cur)
		(*cur).iValues.push_back(iUNDEF);

	return TableExternalFormat::iNewRecord(iRec);
}

void TableDBF::PutExternalField(FieldValues &line, int &iMemoCol, int iRec, int iField)
{
	memos[iMemoCol].iValues[iRec - 1 ] = line[iField].iVal();
//	int iC = memos[iMemoCol][iRec - 1];
	++iMemoCol;
}

void TableDBF::Store()
{
	TableExternalFormat::Store();
	if ( fnData().sExt != ".tb#" ) 
	{
		ObjectInfo::WriteElement(ptr.sSection("TableStore").scVal(),"Type", ptr.fnObj, "TableDBF");
		StoreAsDBF();
	}
	else
		ObjectInfo::WriteElement(ptr.sSection("TableStore").scVal(),"Type", ptr.fnObj, "TableBinary");
}

void TableDBF::StoreAsDBF()
{
	Header hdrDBF;
	memset(&hdrDBF, 0, sizeof(Header));

	File *Out = new File(fnData(), facCRT);

	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime );
	CTime tim(osBinaryTime);

	hdrDBF.bValid = memos.size() == 0 ? 0x03: 0x8B;
	hdrDBF.bYear = (tim.GetYear() - 1900) % 100;
 	hdrDBF.bMonth = tim.GetMonth();
	hdrDBF.bDay = tim.GetDay();
	hdrDBF.iNrRecords = ptr.iRecs();

	Out->Write(sizeof(Header), & hdrDBF);
	int iRecSize=0;

	if ( sKeyColumn != "" )
	{
		WriteDescriptorField(Out, sKeyColumn, ptr.dm(), ClmInfo::edtUnknown);
		iRecSize += ptr.dm()->iWidth();
	}
	
	for(int iCol=0; iCol<iCols(); ++iCol)
	{
		if ( ac[iCol]->fErase ) continue;

		WriteDescriptorField(Out, ac[iCol]->sName(), ac[iCol]->dm(), ClmInfo::edtUnknown); 
		iRecSize += ac[iCol]->dm()->pdbool() ? 1 : ac[iCol]->dm()->iWidth();
  }

	for ( int iMemo=0; iMemo < memos.size(); ++iMemo )
	{
		WriteDescriptorField(Out, memos[iMemo].sColumnName, Domain("value", 0, 1e9), ClmInfo::edtDBFMemo);
		iRecSize += 10;
	}

	long iHere = Out->iLoc();
	Out->Seek(0);
	hdrDBF.iHeaderSize = iHere + 1;
	hdrDBF.iRecordSize = iRecSize + 1;
	Out->Write(sizeof(Header), &hdrDBF);
	Out->Seek(iHere);

	char cSeperator = 0xD;
	Out->Write(1, &cSeperator);

	for(int iRec = 1; iRec <= iRecs(); ++iRec )
	{
			cSeperator = ' ';
			Out->Write(1, &cSeperator); // delete marker

			if ( sKeyColumn != "")
				WriteField(Out, ptr.dm()->pdsrt()->sValue(iRec), ptr.dm()->iWidth(), ptr.dm());

			for(int iCol=0; iCol<iCols(); ++iCol)
			{
				if ( ac[iCol]->fErase ) continue;
				WriteField(Out, ac[iCol]->sValue(iRec), ac[iCol]->dm()->iWidth(), ac[iCol]->dm());
			}
			for ( int iMemo=0; iMemo < memos.size(); ++iMemo )
			{
				int iV = memos[iMemo].iValues[iRec - 1];
				String sV =  iV != iUNDEF ? String("%d", iV) : "";
				WriteField(Out, sV, 10, Domain("value", 0, 1e9)); 
			}
	}

	delete Out;
}

void TableDBF::WriteField(File* Out, String sVa, int iWidth, const Domain& dm)
{
	  String sV = sVa.sTrimSpaces();

		char *buf = new char[iWidth];

		string s(iWidth, ' ');

		if ( dm->pdbool() )
		{
			s = dm->iRaw(sVa) == 2 ? "T" : "F";
			iWidth=1;
		}
		else if ( dm->pdv() )
		{
			if ( sV == "?") sV = "0";
			int iPos = max(0, iWidth - sV.size());
			s.replace( iPos, iWidth - iPos, sV);
		}
		else
		{
			s.replace(0, min(iWidth, sV.size()), sV.substr(0, min(iWidth, sV.size())));
		}
		memcpy(buf, s.c_str(), s.size());
		Out->Write(iWidth, buf);
		delete [] buf;
}

void TableDBF::WriteDescriptorField(File* Out, String sName, const Domain& dm, const ClmInfo::ExternalDomainType edtTpe )
{
		Descriptor desc;
		memset(&desc, 0, sizeof(Descriptor));
		char buf[12];
		memset(buf, 0, 12);
    strcpy(buf, sName.sLeft(11).scVal());
		memcpy(desc.cNameField, buf, 11);

		desc.bFieldSize = dm->iWidth();
	
		DomainType dmt = dm->dmt();
		switch(dmt)
		{
			case dmtCLASS:
			case dmtID:
			case dmtSTRING:
			case dmtCOLOR:
				desc.cType = 'C';
				break;
			case dmtBOOL:
				desc.cType = 'L';
				desc.bFieldSize = 1;
				break;
			case dmtVALUE:
				{ 
					if ( edtTpe == ClmInfo::edtDBFMemo )
					{						
						desc.cType = 'M';
						desc.bFieldSize = 10;
					}
					else
					{
						DomainValue *pdv = dm->pdv();
						double rS = pdv->rStep();
						desc.cType = pdv->rStep() == 1.0 ? 'N' : 'F';
						if ( edtTpe == ClmInfo::edtDBFDate )
						{							
							desc.cType = 'D';
							desc.bFieldSize = 8;
						}
						else
							desc.bDecimals = pdv->rStep() != 1.0 ? pdv->pdvr()->iDec() : 0;
					}	
				}
				break;
			case dmtUNKNOWN:
				break;

		}
		Out->Write(sizeof(Descriptor), &desc);

}

