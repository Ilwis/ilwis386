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
#ifndef TABLEEXTERNALFORMAT_H
#define TABLEEXTERNALFORMAT_H

#include "Engine\Table\tblstore.h"
#include <set>
#include <vector>

typedef vector<String> FieldValues;

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP ClmInfo
{
public:
	enum ExternalDomainType{edtUnknown, edtDBFMemo, edtDBFDate};

	ClmInfo();
	DomainValueRangeStruct dvs;
	String                 sColumnName;
	DomainType             dtDomainType;
	ExternalDomainType     edtType;
	FileName               fnDomain;
	int                    iNrRecs;
	bool                   fCreate;
	bool                   fExtend;
	bool                   fKeyAllowed;
	bool                   fKey;
	String                 sUndefValue;
	int                    iColumnWidth;
	set<String>            Strings;
	vector<String>         vcStrings;

	bool operator==(const ClmInfo& ci);
};


class TableExternalFormat : public TableStore
{
	public:
		enum InputFormat{ifComma,ifSpace,ifFixed, ifDBF, ifIlwis14, ifDBFADO, ifUnknown};
		enum DataUsage{duIlwis, duUseAs, duNative, duUnknown};

		TableExternalFormat(const FileName& fnObj, TablePtr& p);
		TableExternalFormat(const FileName& fnIn,
                   const FileName& fnObj, TablePtr& p,
                   const FileName& fnDat,
                   int iSkip, InputFormat ifFormat,
                   vector<ClmInfo> &colinfo);
		virtual void   Store();
		static TableExternalFormat::InputFormat ifGetFormat(const String& s);

  protected:
 		void            PutRow(FieldValues &line, int iKeyColumn, long iRecNr);
		virtual void    ProcessHeader(File &InpFile) {}
		virtual void    ProcessHeader(CStdioFile &InpFile) {}
		virtual bool    SplitLine(File& sIf, FieldValues& result, long& iRec) { return false;};
		virtual bool    SplitLine(CStdioFile& sIf, FieldValues& result, long& iRec) { return false;};
		virtual void    AddToTableDomain(FieldValues &line, int iKeyColumn);
		virtual void    Load();
		virtual void    PutExternalField(FieldValues &line, int &iMemoCol, long iRec, int iField) {}
		virtual long    iNewRecord(long iRec);

		int             iGetKeyColumnIndex();

		vector<ClmInfo> colInfo;
		Domain          dmString;
		InputFormat     ifFormat;
		int             iSkipLines;
		int				iTotalRec;
		String          sKeyColumn;
		FileName        FileIn;

		DomainSort      *m_pdsTable; // set in case a key column is used
};

#endif
