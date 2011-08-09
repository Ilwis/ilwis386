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
#include "Engine\Domain\dm.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\CONV.hs"

//--[ ClmInfo ]-----------------------------------------------------------------------------
ClmInfo::ClmInfo() :
	sColumnName(sUNDEF),
	dtDomainType(dmtID),
	iNrRecs(0),
	fCreate(false),
	fExtend(false),
	fKeyAllowed(true),
	fKey(false),
	edtType(edtUnknown),
	iColumnWidth(iUNDEF)
{}

bool ClmInfo::operator==(const ClmInfo& clmInf)
{
	return fCIStrEqual(sColumnName , clmInf.sColumnName);
}

//--[ TableExternalFormat ] -----------------------------------------------------------------
TableExternalFormat::TableExternalFormat(const FileName& fnObj, TablePtr& p) :
	TableStore(fnObj, p),
	FileIn(fnData())
{
	int iFormat = iUNDEF;
	String sUndefs, sDomainTypes, sExternalDomains;
	int iCol;
	ObjectInfo::ReadElement(ptr.sSection("TableExternalFormat").c_str(), "SkipLines", ptr.fnObj, iSkipLines);
	ObjectInfo::ReadElement(ptr.sSection("TableExternalFormat").c_str(), "Delimiter", ptr.fnObj, iFormat);
	ObjectInfo::ReadElement(ptr.sSection("TableExternalFormat").c_str(), "KeyColumn", ptr.fnObj, sKeyColumn);
	ObjectInfo::ReadElement(ptr.sSection("TableExternalFormat").c_str(), "NrOfColumns", ptr.fnObj, iCol);
	ObjectInfo::ReadElement(ptr.sSection("TableExternalFormat").c_str(), "UndefValues", ptr.fnObj, sUndefs);

	colInfo.resize(iCol);
	for (int c = 0; c < iCol; ++c) 
	{
		colInfo[c].sUndefValue = sUndefs.sHead(",");
		sUndefs = sUndefs.sTail(",");
	}

	ifFormat = (InputFormat)iFormat;

	if ( fnData().sExt != ".tb#" )
	{
		for ( int iCol=0; iCol < iCols(); ++iCol)
			ac[iCol]->SetReadOnly();
	}
}

TableExternalFormat::TableExternalFormat( const FileName& _FileIn, const FileName& fnObj, TablePtr& p,
																					const FileName& fnDat, int iSkip, InputFormat _ifFormat, 
																					vector<ClmInfo> &clInfo) :
		TableStore(fnObj, p, fnDat),
		iSkipLines(iSkip),
		ifFormat(_ifFormat),
		FileIn(_FileIn),
		colInfo(clInfo)
{
	iTotalRec = colInfo[0].iNrRecs;
	for (int i = 0; i < colInfo.size(); ++i)  
	{
		Domain dm;
		ClmInfo cli = colInfo[i];
		FileName fnNewDomain	;	
		bool fIsSystem = ObjectInfo::fSystemObject(cli.fnDomain);
		if ( fIsSystem)
			fnNewDomain = cli.fnDomain;
		else			
			fnNewDomain = FileName::fnUnique( FileName(cli.fnDomain,".dom"));

		switch (colInfo[i].dtDomainType)  
		{
			case dmtVALUE:
				{  
					if ( cli.fCreate) // new value domain
					{
						RangeReal vr = cli.dvs.vr()->rrMinMax();
						dm = Domain(fnNewDomain, vr.rLo(), vr.rHi(), cli.dvs.vr()->rStep());
					}
					else
					{
						if ( fCIStrEqual(cli.fnDomain.sFile , "value") ) // default value domain with range
						{
							RangeReal vr = cli.dvs.vr()->rrMinMax();
							dm = Domain("value", vr.rLo(), vr.rHi(), cli.dvs.vr()->rStep());
						}
						else
							dm = Domain(cli.fnDomain);
					}
					colNew(colInfo[i].sColumnName, dm, colInfo[i].dvs.vr());
				}
				break;
			case dmtCLASS:
			case dmtID:
				{
					if ( cli.fCreate )
					{
						dm = Domain(fnNewDomain, 0 /*iTotalRec*/, colInfo[i].dtDomainType);
					}
					else
						dm = Domain(cli.fnDomain);

					dm->pdsrt()->dsType = DomainSort::dsMANUAL;

					if (cli.fKey)
					{
						if (cli.fKeyAllowed)
						{
							ptr.SetDomain(dm);
							sKeyColumn = cli.sColumnName;
						}
						else
							throw ErrorObject(TR("Key column is not allowed to have duplicate values"));
					}
					else
						colNew(cli.sColumnName, dm);

					dm->SetWidth(colInfo[i].iColumnWidth);
				}
				break;
			case dmtBOOL:
				if ( fCIStrEqual(cli.fnDomain.sFile, "YesNo"))
					colNew(cli.sColumnName, Domain("YesNo"));
				else
					colNew(cli.sColumnName, Domain("bool"));

				break;
			case dmtSTRING:
				{
					Column col = colNew(cli.sColumnName, Domain("string"));
					col->dm()->SetWidth(colInfo[i].iColumnWidth);
				}
				break;
			default:
				// do nothing, columns of types not supported by Ilwis (e.g. memos)
				break;
    }
  }
}

TableExternalFormat::InputFormat TableExternalFormat::ifGetFormat(const String& s)
{
	if ( fCIStrEqual(s , "Comma" ))		return ifComma;
	if ( fCIStrEqual(s , "Space" ))		return ifSpace;
	if ( fCIStrEqual(s , "Fixed" ))		return ifFixed;
	if ( fCIStrEqual(s , "DBF" ))		return ifDBF;
	if ( fCIStrEqual(s , "Ilwis14" ))	return ifIlwis14;
	return ifUnknown;
}

void TableExternalFormat::Load()
{
	File InputFile(FileIn);

	if ( InputFile.fEof() )
		throw ErrorObject(String(TR("Could not find %S").c_str(), fnData().sFullPath()));

	int iKeyColumn = iGetKeyColumnIndex();
	m_pdsTable = 0;
	if ( iKeyColumn != iUNDEF)
	{
		m_pdsTable = ptr.dm()->pdsrt();
		ptr.SetDomain(ptr.dm());
	}		

	int iFileSz = InputFile.iSize();
	Tranquilizer trq;
	if (ifFormat == ifDBF)
		trq.SetTitle(TR("Importing dBase III/IV table"));
	else
		trq.SetTitle(TR("Import ASCII Table"));
	trq.SetText(TR("Reading File"));

	iTotalRec = colInfo[0].iNrRecs;
	ProcessHeader(InputFile);
//	if (iTotalRec > 0)
//		iRecNew(iTotalRec);  // when possible create records in one go

	int iRec = 0;
	while( !InputFile.fEof() )
	{

		if ( trq.fUpdate(InputFile.iLoc(), iFileSz) )
			return;

		vector<String> line;
		if ( SplitLine(InputFile, line, iRec))
		{
			// if there is a key column add its value to the table domain
			if ( iKeyColumn != iUNDEF)
				AddToTableDomain(line, iKeyColumn);
			else
				iRec = iNewRecord(iRec);

			PutRow(line, iKeyColumn, iRec );
		}
	}

	// Make sure that all new DomainSort's are sorted
	// Both of the table domain (in case of a key column) and
	// of all columns with DomainSort
	for (int iCol = 0; iCol < colInfo.size(); iCol++)
	{
		ClmInfo ci = colInfo[iCol];
		if (ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID)
		{
			DomainSort *pds = 0;
			if (iKeyColumn != iUNDEF && iKeyColumn == iCol)
				pds = ptr.dm()->pdsrt();
			else
			{
				// Adjust Table-Column index in case a possible Key Column is located before
				// the current column. The key column is not listed in the Table-Columns array
				long iCurCol = iCol;
				if (iKeyColumn >= 0 && iKeyColumn < iCol)
					iCurCol--;
				
				pds = col(iCurCol)->dm()->pdsrt();
			}

			if (pds != 0)
			{
				pds->dsType = DomainSort::dsALPHANUM;
				pds->SortAlphaNumeric();
			}
		}
	}
}

int TableExternalFormat::iNewRecord(int iRec)
{
	if (iRec >= iRecs() && ptr.dm()->pdnone() != NULL)
		return iRecNew();
	else
		return iRec + 1;
}

int TableExternalFormat::iGetKeyColumnIndex()
{
	for(int iIndex=0; iIndex < colInfo.size(); ++iIndex)
	{
		String sCol = colInfo[iIndex].sColumnName;
		if ( fCIStrEqual(colInfo[iIndex].sColumnName, sKeyColumn))
			return iIndex;
	}

	return iUNDEF;
}

void TableExternalFormat::AddToTableDomain(FieldValues &line, int iKeyColumn)
{
	String sVal = line[iKeyColumn];
	Domain dm = ptr.dm();

	if ( dm->iRaw(sVal) == iUNDEF )
	{
		iRecNew();
		m_pdsTable->iAdd(sVal);
	}
}

void TableExternalFormat::PutRow(FieldValues &line, int iKeyColumn, long iRecNr)
{
	if ( iKeyColumn != iUNDEF )
			iRecNr = ptr.dm()->iRaw(line[iKeyColumn]);

	int iCol = 0;

	for(int iField = 0, iColumn = 0; iField < line.size(); ++iField)
	{
		if ( iColumn >= ac.iSize() ) // specified more columns than are actually present
			continue;

		if ( iKeyColumn == iField) // key columns are the table domain, no separate column will be made
			continue;

		string sValue = line[iField];
		long iR = m_pdsTable ? m_pdsTable->iKey(iRecNr) : iRecNr;

		ClmInfo cl = colInfo[iField]; // debug
		switch (colInfo[iField].dtDomainType)  
		{
			case dmtID:	
			case dmtCLASS:
				{
					if (sValue == colInfo[iField].sUndefValue )
						ac[iColumn]->PutVal(iR, sUNDEF );
					else
					{
						const Domain& dm = ac[iColumn]->dm();
						if (dm->iRaw(sValue) == iUNDEF  && colInfo[iField].fExtend)
							dm->pdsrt()->iAdd(sValue);

						ac[iColumn]->PutVal(iR, sValue);
					}
				}
				break;
			case dmtVALUE:
				if (sValue == colInfo[iField].sUndefValue )
					ac[iColumn]->PutVal(iR,rUNDEF);
				else  
					ac[iColumn]->PutVal(iR, sValue);
				break;
			case dmtBOOL:
			{
				long iValue;
				String s = colInfo[iField].fnDomain.sFile;
				if ( fCIStrEqual(colInfo[iField].fnDomain.sFile , "Bool"))
					iValue = (fCIStrEqual(sValue , "T" )  || fCIStrEqual(sValue , "True")) ? 1 : 0;
				else
					iValue = (fCIStrEqual(sValue , "Y" )  || fCIStrEqual(sValue , "Yes")) ? 1 : 0;

				if (sValue == colInfo[iColumn].sUndefValue )
					ac[iColumn]->PutVal(iR,iUNDEF);
				else  
					ac[iColumn]->PutVal(iR, iValue);
			}
			case dmtSTRING:
				if (sValue == colInfo[iColumn].sUndefValue )
					ac[iColumn]->PutVal(iR,sUNDEF);
				else  
					ac[iColumn]->PutVal(iR, sValue);
				break;
			case dmtUNKNOWN: // some formats may implement this function
				PutExternalField( line, iCol, iR, iField);
				break;
		}
		if ( colInfo[iField].dtDomainType != dmtUNKNOWN) // only 'real' columns
			++iColumn;
	}
}

void TableExternalFormat::Store()
{
	if (ptr.fnObj.sFile.length() == 0)  // empty file name
		return;
	
	TableStore::Store();
	// write undef info per column
	ObjectInfo::WriteElement(ptr.sSection("TableExternalFormat").c_str(),"SkipLines", ptr.fnObj, (long)iSkipLines);
	ObjectInfo::WriteElement(ptr.sSection("TableExternalFormat").c_str(),"Delimiter", ptr.fnObj, (long)ifFormat);
	ObjectInfo::WriteElement(ptr.sSection("TableExternalFormat").c_str(),"KeyColumn", ptr.fnObj, sKeyColumn);
	ObjectInfo::WriteElement(ptr.sSection("TableExternalFormat").c_str(),"NrOfColumns", ptr.fnObj, (long)colInfo.size());
	String sUndefs, sDomainTypes, sExternalDomains;
	int c = 0;
	for (; c < colInfo.size() - 1; ++c) 
	{
		sUndefs += String("%S,", colInfo[c].sUndefValue);
	}
	sUndefs += String("%S", colInfo[c].sUndefValue);
	
	ObjectInfo::WriteElement(ptr.sSection("TableExternalFormat").c_str(), "UndefValues", ptr.fnObj, sUndefs);
}

