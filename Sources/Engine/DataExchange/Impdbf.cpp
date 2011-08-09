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
/* $Log: /ILWIS 3.0/Import_Export/Impdbf.cpp $
 * 
 * 9     16-01-03 14:11 Willem
 * - Added: code to remove dots from a fieldname in the DBF file. A dot
 * will cause problems as the table class will think the fieldname then
 * contains the name of the table also, which is not the case
 * 
 * 8     9/18/00 9:39a Martin
 * the load state of the imported table is set to true.
 * 
 * 7     29/02/00 11:32 Willem
 * MessageBox is now shown using as parent the mainWindow
 * 
 * 6     22/02/00 14:42 Willem
 * - pow() function arguments are now correctly placed (they were
 * switched)
 * - Tranquilizer is not stopped before importing anymore
 * - DrawItem now lets parent draw first
 * 
 * 5     9/10/99 1:20p Wind
 * adaption to changes in FileName constructors
 * 
 * 3     17-06-99 3:59p Martin
 * // -> /*
 * 
 * 2     17-06-99 2:11p Martin
 * ported files to VS
// Revision 1.8  1998/09/18 17:05:05  Wim
// helptopic number for dBase import form
//
// Revision 1.7  1998-09-17 10:13:12+01  Wim
// 22beta2
//
// Revision 1.5  1997/11/20 11:33:31  martin
// Last remnant of unchecked adding of elements to domain sort removed. An uncaught exception caused a GPF.
//
// Revision 1.4  1997/09/23 11:27:54  janh
// In DBIVImport::fImport I added if (0!= tbl->dm()->pdnone()
// before tbl->iRecNew to avoid doubling the Nr of Records in _iRec
//
// Revision 1.3  1997/08/05 20:49:53  Willem
// Added comments to clearify the selection of the VAlueRange for numeric values
//
// Revision 1.2  1997/07/28 15:53:28  Willem
// Initialized for RCS usage
//
/*
  impdbf.c import dBase III/IV files
  by Willem Nieuwenhuis, July 96
  ILWIS Department ITC
  Last change:  WK   18 Sep 98    6:04 pm
*/

/*
  Revision history:
  WN: 14/04/97: Fixed a problem that caused NULL objects instead of invalid object to be
                appended to an Array<ValueRange>, resulting in not completely imported tables.
  MS: 10/07/97: Rewritten: because of the changes in dependencies TableDBF could not be
                derived from TableStore anymore. Also added user interaction to allow changing
                the default domain for the Table and the Columns
  WK: 22/07/97: Made the user interface workable.
*/

#define IMPDBF_C
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
//#include "Client\Base\events.h"
#include "Engine\DataExchange\Impdbf.h"
#include "Headers\Hs\CONV.hs"
#include "Headers\Htp\Ilwismen.htp"

#define SZ_NA_COL 20
#define TY_COL SZ_NA_COL + 8

void ThrowIncompatibleDomainError(const FileName& fn)
{
    throw ErrorObject(WhatError(TR("Only domain Class or ID allowed"), 1), fn);
}


static Array<DBInfo::Descriptor> dummyDescriptor;

//----------------------------------------------------------------------------------------

void ImpExp::ImportDBF(const FileName& fnDBF, const FileName& fnObject)
{
 /*   DBIVImport dbi(fnDBF, fnObject, trq);
    DBChoiceForm frm(win, &dbi.Info);
    if ( frm.fOkClicked() ) {
      trq.SetTitle(TR("Importing dBase III/IV table"));
      dbi.fImport();
    }*/
}

DBInfo::DBInfo(const FileName& fnDbFile) :
            iMaxFieldLength(0),
            iNrRecords(0),
            iNrFields(0),
            iHeaderSize(0),
            iRecordSize(0)
{
        File file(fnDbFile);
        Header hdrDBF;
        file.Seek(0);
        file.Read(sizeof(Header), &hdrDBF);
        iNrFields = (hdrDBF.iHeaderSize - sizeof(Header) - 1) / sizeof(Descriptor);
        iNrRecords = hdrDBF.iNrRecords;
        iRecordSize = hdrDBF.iRecordSize;
        iHeaderSize = hdrDBF.iHeaderSize;

        fields.Resize(iNrFields);
        altDomain.Resize(iNrFields);
        for (short iField = 0; iField < iNrFields; iField++)
        {
           file.Read(sizeof(Descriptor), &(fields[iField]));
           iMaxFieldLength = max(iMaxFieldLength, bWidth(iField));
           if ( sName(iField) != "NAME" )
                altDomain[iField] = dtUnknown;
           else
                altDomain[iField] = dtTableClass;
        }
}

void DBInfo::SetAltDomain(short i, dtDomainType type)
{
    if ( i < 0 ) return ;
    if ( type == dtTableClass || type == dtTableId )
    {
        for (short iFld = 0; iFld < iNrFields; iFld++)
            if ( altDomain[iFld]==dtTableClass || altDomain[iFld]==dtTableId )
                altDomain[iFld]=dtString;
    }
    altDomain[i]=type;
}

String DBInfo::sName(short i)
{
    if ( i > iNrFields ) return "";
    char sName[12];
    memcpy(sName, fields[i].cNameField, 11);
    sName[11]='\0';
    String s(sName);
	// Remove dot's if present (although not allowed!) to prevent problems when creating the column lateron
	String::iterator itLast = remove(s.begin(), s.end(), '.');  // remove the dot(s)
	s.erase(itLast, s.end());  // adjust the string length

    return s.sTrimSpaces();
}

dtDomainType DBInfo::dtType(short i)
{
    dtDomainType t = altDomain[i];
    if ( t != dtUnknown ) return t;
    else
    {
        switch ( fields[i].cType )
        {
            case 'L':
                return dtBool;
            case 'F' :
            case 'N' :
                return dtValue;
            case 'C' :
                return dtString;
            case 'D' :
                return dtDate;
        }
    }
    return dtUnknown;
}

byte DBInfo::bWidth(short i)
{
    return fields[i].bFieldSize;
}

byte DBInfo::bPrecision(short i)
{
    return fields[i].bDecimals;
}

void DBInfo::ClearTableDomain()
{
    for(int i=0; i<iNrFields; ++i)
    {
        dtDomainType type=dtType(i);
        if ( type == dtTableClass || type == dtTableId )
        {
            altDomain[i]=dtUnknown;
        }
    }
}    

DBIVImport::DBIVImport(const FileName& in, const FileName& out, Tranquilizer& _trq) :
            Info(in),
            fnDBTable(in),
            fnIlwisTable(out),
            trq(_trq)

{}

void DBIVImport::SetTableDomain(const Domain& dom)
{
    if ( dom->pdsrt())
    {
       Info.ClearTableDomain(); 
       dmTable=dom;
    }       
    else
       ThrowIncompatibleDomainError(fnIlwisTable); 
}

bool DBIVImport::fImport()
{
    File file(fnDBTable);
    char *RecBuf = new char[ Info.iRecordSize + 1];
    MakeTable();
    if (0 != tbl->dm()->pdnone())
      tbl->iRecNew(Info.iNrRecords);
    bool fFirstRecord = true;
    file.Seek(Info.iHeaderSize);
    trq.SetText(TR("Reading records"));
    for ( long iRec=1; iRec <= Info.iNrRecords; ++iRec)
    {
        short count=0;
        file.Read(Info.iRecordSize, RecBuf);
        if ( trq.fUpdate(iRec, Info.iNrRecords )) return false;
        if ( '*' == *RecBuf ) continue;
        for (short iFld = 0; iFld < Info.iNrFields; iFld++)
        {
            dtDomainType type=Info.dtType(iFld);
            if ( type == dtUnknown ) continue;
            String sField = MakeField(RecBuf, Info.bWidth(iFld), count);
            if ( type != dtTableClass && type != dtTableId )
            {
                if ( fFirstRecord ) MakeColumn(iFld);
                String s=Info.sName(iFld);
                Column col=tbl->col(s);
                if ( Info.dtType(iFld)==dtBool )
                    col->PutVal( iRec, strchr("yYtT", sField[0])!=NULL ? "T" : "F");
                else if ( type == dtString || type==dtDate || type==dtValue )
                    col->PutVal( iRec, sField );
                else if ( type == dtClass || type == dtId )
                {
                    if (col->dm()->pdsrt()->iRaw(sField)==iUNDEF) //only add elements that do not exist
                        col->dm()->pdsrt()->iAdd(sField);
                    col->PutVal(iRec, sField);

                }
                else
                        col->PutVal( iRec, iUNDEF);
            }
            else
            {

                long v = dmTable->pdsrt()->iRaw(sField);
                if ( v != iUNDEF )                        // value exists in the domain, do not add
                {
                    int n = getEngine()->Message(TR("Domain element exists already. Continue ?").c_str(), TR("Domain error").c_str(), MB_YESNO | MB_ICONHAND);
                    if ( n ==IDNO )
                    {
                        delete [] RecBuf;
                        return false;
                    }

                }
                else
                   dmTable->pdsrt()->SetVal(iRec, sField);
            }
        }
        fFirstRecord=false;
    }
    delete [] RecBuf;
    if ( tbl.fValid() )
	{
		tbl->Loaded(true); // the table is loaded externally so it already loaded
		tbl->Store();		
	}		
    else return false;

    return true;
}

String DBIVImport::MakeField(char *rec, short iWidth, short& count)
{
    char *fld=new char[Info.iRecordSize + 1 ];
    memcpy(fld,rec+count+1, iWidth);
    fld[iWidth]='\0';
    String s(fld);
    count+=iWidth;
    delete [] fld;
    return s.sTrimSpaces();
}

void DBIVImport::MakeColumn(short iFld)
{
   ValueRange vr;
   String dom;
   dtDomainType dtType = Info.dtType(iFld);
   if ( dtType == dtTableClass || dtType == dtTableId ) return;
   switch ( dtType )
   {
    case dtBool:
        dom="bool";
        break;
    case dtString :
        dom="string";
        break;
    case dtValue :
        dom="value";
        vr = MakeValueRange(iFld);
        break;
    case dtDate :
        dom="value";
        vr = ValueRange(0, 29990000L);
        break;
    case dtId:
        {
        FileName fn(Info.sName(iFld).c_str(), ".dom");
        dom = FileName::fnUnique(fn).sFullName();
        Domain ColDomain = Domain(dom, 0, dmtID); //
        ColDomain->pdsrt()->dsType=DomainSort::dsMANUAL;
        DomainValueRangeStruct dvs(ColDomain, vr);
        Column col(tbl, Info.sName(iFld), dvs);
        return;
        }
    case dtClass:
        {
        FileName fn(Info.sName(iFld).c_str(), ".dom");
        dom = FileName::fnUnique(fn).sFullName();
        Domain ColDomain = Domain(dom, 0, dmtCLASS); //
        ColDomain->pdsrt()->dsType=DomainSort::dsMANUAL;
        DomainValueRangeStruct dvs(ColDomain, vr);
        Column col(tbl, Info.sName(iFld), dvs);
        return;
        }
    }
    FileName fnDom(dom, ".dom", true);
    Domain dm(fnDom);
    DomainValueRangeStruct dvs(dm, vr);
    Column col(tbl, Info.sName(iFld), dvs);

}

/*
  If the number of decimals is zero:
  1. The width of the number < 10, that is the value will be < 10e9:
     The number can be represented by an integer value
  2. The width >= 10, that is the number can be > 10e9:
     The number can exceed the integer ranges, select default valuerange
  If the number of decimals > 0:
     The step size is calculated directly from the number of decimals
*/
ValueRange DBIVImport::MakeValueRange(short iFld)
{

    if ( Info.bPrecision(iFld)!=0 )
    {
        double rStep = pow(10.0, -Info.bPrecision(iFld));
        short iBD = Info.bWidth(iFld) - Info.bPrecision(iFld);
        double rMinMax =  pow(10.0, iBD) - 1;
        return ValueRange(-rMinMax, rMinMax, rStep);
    }
    short iBD = Info.bWidth(iFld);
    if (iBD < 10)
    {
        long iMinMax = (long)pow(10.0, iBD) - 1;
        return ValueRange(-iMinMax, iMinMax);
    }

    return ValueRange(Domain("value"));
}

void DBIVImport::MakeTable()
{
    FileName fnTable  = FileName(fnIlwisTable, ".tbt", true);
    FileName fnTblDat = FileName(fnIlwisTable, ".tb#", true);
    if ( !dmTable.fValid())
        dmTable=Domain("none");
    for (short iFld = 0; iFld < Info.iNrFields; iFld++)
    {
        if ( Info.dtType(iFld) == dtTableClass || Info.dtType(iFld) == dtTableId )
        {
            String sName(Info.sName(iFld) );
            if ( sName.toUpper() == "NAME" )
                dmTable = Domain(FileName::fnUnique(FileName(fnIlwisTable, ".dom")),Info.iNrRecords, Info.dtType(iFld) == dtTableClass ? dmtCLASS : dmtID);
            else
               dmTable = Domain(FileName::fnUnique(FileName(sName.sLeft(8).c_str(), ".dom")),Info.iNrRecords, Info.dtType(iFld) == dtTableClass ? dmtCLASS : dmtID);
            break;
        }
    }
    tbl.SetPointer(new TablePtr(fnTable, fnTblDat, dmTable, String()));
    String sD = TR("Table ");
    sD &= fnTable.sFile;
    sD &= fnTable.sExt;
    tbl->sDescription = sD;
}

const Table& DBIVImport::GetTable()
{
    return tbl;
}    


