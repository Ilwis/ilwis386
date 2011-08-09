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
//#include <ole2.h>

#pragma warning( disable : 4786 )

#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\constant.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\OLEDBProvider.h"

#include "DataExchange\ADOTable.h"
//#include "temp\msado15.tlh"


String Com_Error(VARTYPE vt, _com_error &e, _ConnectionPtr pConnection, FileName fnDB)
{
	String sErr;
	switch (vt)
	{
		case VT_BSTR:
			{
				_bstr_t bstrSource(e.Source());
				_bstr_t bstrDescription(e.Description());
				
				sErr = String("Code = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
					e.Error(), e.ErrorMessage(), (LPCSTR) bstrSource, (LPCSTR) bstrDescription);
				throw ErrorObject(sErr);
			}						
			break;
		case VT_DISPATCH:
			{
				ErrorPtr    pErr  = NULL;
				if( (pConnection->Errors->Count) > 0)
				{
					long nCount = pConnection->Errors->Count;
					for(long i = 0; i < nCount; i++)
					{
						pErr = pConnection->Errors->GetItem(i);
						sErr=String("Error number: %x\t%s\n", pErr->Number,(LPCSTR) pErr->Description);
						ErrorObject err(sErr);
						err.Show();
						return sErr;
					}
				}							
			}
			break;
		default:
			{
				sErr = String(TR("Could not establish connection to %S ").c_str(), fnDB.sRelative());
				ErrorObject err(sErr);
				err.Show();
				return sErr;
			}						
	}
	return sErr;
}

ForeignFormat *CreateQueryObjectADO() //create query object
{
	return new ADOTable();
}

ForeignFormat *CreateImportObjectADO(const FileName& fnObj, ParmList& pm) //create import object
{
	if ( pm.iSize() < 2 ) // existing object
	{
		String sMethod;
		if ( ObjectInfo::ReadElement("ForeignFormat", "method", fnObj, sMethod) != 0)	// oldstyle expressions
		{
			ForeignFormat *ff = ForeignFormat::Create(sMethod);
			if ( ff )
				ff->ReadParameters(fnObj, pm);
			delete ff;
		}	
	}

	return new ADOTable(fnObj, pm);
}

ADOTable::ADOTable() :
	ForeignFormat(),
    provider(NULL)
{
	provider=NULL;
}

ADOTable::ADOTable(const FileName& fn, ParmList& pm) :
provider(NULL)
{
	int currentSize = pm.iSize();
	String sC = pm.sGet("collection");
	provider = NULL;
	username = pm.sGet("username");
	password = pm.sGet("password");
	tableName = pm.sGet("table");
	FileName fnCollection(sC);
	ForeignCollection db(fnCollection);
	if ( db.fValid()) {
		if ( !pm.fExist("database"))
			pm.Add(new Parm("database", db->sContainer()));
		database = pm.sGet("database");
	}
	sConnectionString = pm.sGet("connectionstring").sUnQuote();
	if ( sConnectionString == "" )
	{
		if ( pm.sGet("provider") == "" ) {
			provider = OLEDBProvider::create("ODBC");
			pm.Add(new Parm("provider","ODBC"));
		}
		else			
		  provider = OLEDBProvider::create(pm.sGet("provider"));
		


		if ( provider )
		{
			String	sInitDir = pm.sGet("initdir") == "" ? fn.sPath() : pm.sGet("initdir");
			sConnectionString = provider->sBuildConnectionString(pm.sGet("database"), sInitDir, pm.sGet("username"),pm.sGet("password"));		
			delete provider;
			provider = NULL;
		}

	}
	if ( pm.sGet("query") == "") { // query  {
		String tbl = tableName == "" ? fn.sFile : tableName;
		sQuery = String("Select * FROM \"%S\"", tbl);								
	}
	else
		sQuery = pm.sGet("query").sUnQuote();	

	if ( pm.fExist("key")  ) // key column present. Create a key domain
	{
		sKeyColumn = pm.sGet("key");
		CreateKeyDomain(pm.sGet("key"));
	}

	if ( db.fValid()) {
		Table tbl;
		FileName fnTbl(pm.sGet("output") != "" ? pm.sGet("output") : fn.sFullPathQuoted());
		tbl.SetPointer(new TablePtr(fnTbl, Domain("none"), pm));
		if ( !pm.fExist("import"))
			tbl->SetUseAs(true);
		else
		{
			tbl->LoadData();
			tbl->SetUseAs(false);			
		}			
		tbl->Store();
		if ( currentSize != pm.iSize() ) {
			Store(tbl);
		}
		db->Add(fnTbl);
	}
	
}

void ADOTable::Store(IlwisObject ob) {
	ForeignFormat::Store(ob);
	ob->WriteElement("ForeignFormat","Method","ADO");
	if ( provider)
		ob->WriteElement("ForeignFormat","Provider",provider->sProvider());
	if ( username != "")
		ob->WriteElement("ForeignFormat","Username",username);
	if ( password != "")
		ob->WriteElement("ForeignFormat","Password",password);
	if ( sQuery != "")
		ob->WriteElement("ForeignFormat","Query",sQuery);
	if ( initialdir != "")
		ob->WriteElement("ForeignFormat","Initialdir", initialdir);
	if ( sKeyColumn != "")
		ob->WriteElement("ForeignFormat","Key",sKeyColumn);
	if ( database != "")
		ob->WriteElement("ForeignFormat","Database",database);
	if ( sConnectionString != "")
		ob->WriteElement("ForeignFormat","ConnectionString",sConnectionString);
	if ( tableName != "")
		ob->WriteElement("ForeignFormat","Table",tableName);

}

void ADOTable::ReadParameter(const FileName& fnObj, ParmList& pm, const String& entry, const String& parmName) {
	String sV = "";
	ObjectInfo::ReadElement("ForeignFormat",entry.c_str(),fnObj,sV);
	if ( sV != "")
		pm.Add(new Parm(parmName,sV));
}

void ADOTable::ReadParameters(const FileName& fnObj, ParmList& pm) {
	ForeignFormat::ReadParameters(fnObj, pm);
	ReadParameter(fnObj, pm, "Provider","provider");
	ReadParameter(fnObj, pm, "UserName","username");
	ReadParameter(fnObj, pm, "Password","Password");
	ReadParameter(fnObj, pm, "Query","query");
	ReadParameter(fnObj, pm, "InitialDir","initialdir");
	ReadParameter(fnObj, pm, "Key","key");
	ReadParameter(fnObj, pm, "Database","database");
	ReadParameter(fnObj, pm, "ConnectionString","connectionstring");
	ReadParameter(fnObj, pm, "Table","table");
	if ( pm.fExist("database") == false)
		pm.Add(new Parm("database",pm.sGet("table")));
}

ADOTable::~ADOTable()
{
	delete provider;
	provider = NULL;
}

// if a key column is selected this will function will create an appropriate domain for the column selected
void ADOTable::CreateKeyDomain(const String& sKeyDomain)
{
	_RecordsetPtr  pRstTable;
	FieldsPtr      pFields ;
	_ConnectionPtr pConnection = NULL;
	FileName fnDom(sKeyDomain, ".dom");
	if ( fnDom.fExist() )
	{
		dmKey = new Domain(fnDom);
		return;
	}		


	try
	{
		_bstr_t strCnn(sConnectionString.c_str());

    pConnection.CreateInstance(__uuidof(Connection));
    pConnection->Mode = adModeShareExclusive;
    pConnection->IsolationLevel = adXactIsolated;
    pConnection->Open(strCnn,"","",adConnectUnspecified);

		pRstTable.CreateInstance(__uuidof(Recordset));
		HRESULT r = pRstTable->Open (sQuery.c_str(), variant_t((IDispatch*) pConnection, true),
			        adOpenForwardOnly, adLockReadOnly, adCmdText);

		pFields = pRstTable->GetFields();	
		if (  pFields )
		{
			int iNumColumns = pFields->GetCount();
			long iNumRecords;
      long rgIndices[2];						
			for(int iCol=0; iCol < iNumColumns; ++iCol)
			{
				_variant_t Index;
	      Index.vt = VT_I2;
	      Index.iVal = iCol;

				String sName((LPSTR)pFields->GetItem(Index)->Name);
	      rgIndices[0] = iCol; 				
				if ( fCIStrEqual(sName, sKeyDomain)) // is the column name equal to the keydomain name
				{
					_variant_t avarRecords;
					avarRecords = pRstTable->GetRows(adGetRowsRest);
					// get the data
					HRESULT hr = SafeArrayGetUBound(avarRecords.parray,2,&iNumRecords);	
					set<String> setDomainItems;
					for( long iRec = 0; iRec < iNumRecords+1; ++iRec)
					{
						rgIndices[1] = iRec;
						_variant_t result;
						result.vt = pFields->GetItem(Index)->Type;;
						hr= SafeArrayGetElement(avarRecords.parray, rgIndices, &result);
						String s((LPCSTR)(_bstr_t)result);
						// the elements are added to a set. We are certain that they are all unique
						setDomainItems.insert(s);
					}	
					if ( setDomainItems.size() != iNumRecords + 1)
					{
						pRstTable->Close();	
						pConnection->Close();								
						throw ErrorObject(TR("Column is not fit as Key domain. All elements must be unique"));
					}						
					vector<String> arItems;
					// transfer set items to vector. set can not go over DLL boundaries (MS bug)
					copy(setDomainItems.begin(), setDomainItems.end(), back_inserter(arItems));
					dmKey = new Domain(fnDom, arItems.size(), dmtID);
					(*dmKey)->pdsrt()->AddValues(arItems);
					(*dmKey)->Store();
					break;
				}					
			}
		}
		pRstTable->Close();	
		pConnection->Close();		
	}		
  catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect = pRstTable->GetActiveConnection();

    // GetActiveConnection returns connect string if connection
    // is not open, else returns Connection object.
		ErrorObject err;
    switch(vtConnect.vt)
    {
        case VT_BSTR:
            PrintComError(e);
            break;
        case VT_DISPATCH:
            PrintProviderError(vtConnect, err);
            break;
        default:
            throw ErrorObject("Errors occured.");
            break;
    }
		if ( pRstTable) pRstTable->Close();	
		if ( pConnection) pConnection->Close();					
		throw err;
	}			

}

// fills a database collection with appropriate tables
void ADOTable::PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm)
{
	_bstr_t strCnn(sConnectionString.c_str());
	FieldsPtr      pFields ;
	_ConnectionPtr pConnection = NULL;
	_RecordsetPtr pRstSchema = NULL;	
	try
	{

	  pConnection.CreateInstance(__uuidof(Connection));
	  pConnection->Mode = adModeShareExclusive;
	  pConnection->IsolationLevel = adXactIsolated;
	  pConnection->Open(strCnn,"","",adConnectUnspecified);
		
	  _RecordsetPtr pRstSchema = pConnection->OpenSchema(adSchemaTables);
		while(!(pRstSchema->EndOfFile))
	  {
			_bstr_t table_name = pRstSchema->Fields->GetItem("TABLE_NAME")->Value;
			_bstr_t table_type = pRstSchema->Fields->GetItem("TABLE_TYPE")->Value;	
			String sType("%s", (LPSTR)table_type);
			// ignore tables of the two type below they contain meta information and are not interesting for the user
			// for excel an exception has to be made, all excel tables are system tables (?, bug)
			String sFormatName = ForeignFormat::sFindFormatName(pm.sGet("database"));
			if ( sFormatName =="Excel" || sType != "SYSTEM TABLE" && sType != "SYSTEM VIEW")
			{
				String sTable("%s.tbt", (LPCSTR)table_name);
				collection->Add(sTable);;
			}			

	    pRstSchema->MoveNext();

		}
		pRstSchema->Close();
		pConnection->Close();
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);	
		if ( !pm.fExist("noshow"))
			getEngine()->Execute(String("open %S", collection->fnObj.sRelativeQuoted()));
		Store(IlwisObject::obj(collection->fnObj));
	}

	catch (_com_error &e)
	{
		_variant_t vtConnect;	
		if ( pRstSchema )
			vtConnect = pRstSchema->GetActiveConnection();

		Com_Error(vtConnect.vt, e, pConnection, collection->fnObj);
		if ( pRstSchema) pRstSchema->Close();
		if ( pConnection) pConnection->Close();
	}
}

bool ADOTable::fIsSupported(const FileName& fn, ForeignFormat::Capability dtType ) const
{
	return true;
}

// this loads the table with the data. It creates the appropriate columns and then fills them
void ADOTable::LoadTable(TablePtr *tbl)
{
	_RecordsetPtr  pRstTable;
	FieldsPtr      pFields ;
	_ConnectionPtr pConnection = NULL;
	
	try
	{
		_bstr_t strCnn(sConnectionString.c_str());
		
		
		
		pConnection.CreateInstance(__uuidof(Connection));
		pConnection->Mode = adModeShareExclusive;
		pConnection->IsolationLevel = adXactIsolated;
		// connect to the database
		pConnection->Open(strCnn,"","",adConnectUnspecified);
		
		pRstTable.CreateInstance(__uuidof(Recordset));
		pRstTable = pConnection->Execute(sQuery.c_str(), 0, 0);
		// open a table in the database
//		HRESULT r = pRstTable->Open (sQuery.c_str(), variant_t((IDispatch*) pConnection, true),
//			adOpenForwardOnly, adLockReadOnly, adCmdText);
		
		pFields = pRstTable->GetFields();	
		if (  pFields )
		{
			int iNumColumns = pFields->GetCount();
			vector<DataTypeEnum> vDataTypes;
			columns.resize(iNumColumns);
			vDataTypes.resize(iNumColumns);
			long iNumRecords;
			int iKeyColumn=iUNDEF;
			for(int iCol=0; iCol < iNumColumns; ++iCol)
			{
				_variant_t Index;
				Index.vt = VT_I2;
				Index.iVal = iCol;
				
				// get the type of the column
				String sName((LPSTR)pFields->GetItem(Index)->Name);
				vDataTypes[iCol] = pFields->GetItem(Index)->Type;	
				DataTypeEnum v = vDataTypes[iCol];
				bool fReadOnly = false;	
				Domain dm;
				if ( fCIStrEqual(sName, sKeyColumn) ) //  skip a key column, but remember it
					iKeyColumn = iCol;
				else //  create new column
				{
					DomainValueRangeStruct dvsColumn = dmTranslateDataTypeToIlwis(vDataTypes[iCol], fReadOnly, sName, tbl);					
					if ((!(dvsColumn.dm().fValid())) || (dvsColumn.dm()->pdnone() == 0))
					{
						columns[iCol] = tbl->colNew(sName, dvsColumn);
						if (columns[iCol].fValid())
						{
							sarForeignCols.insert(sName);
							columns[iCol]->SetReadOnly(fReadOnly);
							columns[iCol]->SetOwnedByTable(false);
						}
					}
				}					
			}
			_variant_t avarRecords;
			avarRecords = pRstTable->GetRows(adGetRowsRest);
			HRESULT hr = SafeArrayGetUBound(avarRecords.parray,2,&iNumRecords);
			iNumRecords++; // points to upperbound counted from 0 so +1 to get the real number of objects			
			if ( tbl->iRecs() == 0) // true for new tables
			{
				tbl->iRecNew(iNumRecords);
			}				
			long rgIndices[2];			
			for(int iCol = 0; iCol < iNumColumns; ++iCol) //  put data in the column
			{
				rgIndices[0] = iCol;
				if ( columns[iCol].fValid() )
				{
					columns[iCol]->SetLoadingForeignData(true);				
					for( long iRec = 0; iRec < iNumRecords; ++iRec) // put data in the field
					{
						rgIndices[1] = iRec;
						_variant_t result;
						result.vt = vDataTypes[iCol];
						hr= SafeArrayGetElement(avarRecords.parray, 
							rgIndices, &result);
						if ( iKeyColumn == iUNDEF ) // no key column
							PutData(columns[iCol], iRec + 1, result);
						else
						{
							if ( iKeyColumn != iCol) // for all but the key column
							{
								_variant_t key;
								key.vt = vDataTypes[iKeyColumn];
								long rgIndices2[2];
								rgIndices2[0] = iKeyColumn;
								rgIndices2[1] = iRec;
								hr= SafeArrayGetElement(avarRecords.parray, rgIndices2, &key);	
								String sKey = (LPCSTR)(_bstr_t)key;
								long iInd = (*dmKey)->pdsrt()->iRaw(sKey);
								PutData(columns[iCol], iInd, result);
							}								
						}
					}
					columns[iCol]->SetLoadingForeignData(false);
				}
			}	
		}
		pRstTable->Close();	
		pConnection->Close();
	}
	catch (_com_error &e)
	{
		// Notify the user of errors if any.
		// Pass a connection pointer accessed from the Recordset.
		_variant_t vtConnect;
		if ( pRstTable)
			vtConnect = pRstTable->GetActiveConnection();
		
		// GetActiveConnection returns connect string if connection
		// is not open, else returns Connection object.
		Com_Error(vtConnect.vt, e, pConnection, tbl->fnObj);
		try
		{
			if ( pRstTable ) pRstTable->Close();
		}
		catch (_com_error)
		{
			// nothing .. seems table wasn't opened
		}
		if ( pConnection->State != adStateClosed )
			if ( pConnection ) pConnection->Close();
		throw ErrorObject();			
	}		
}

void ADOTable::PrintProviderError(_ConnectionPtr pConnection, ErrorObject& err)
{
    // Print Provider Errors from Connection object.
    // pErr is a record object in the Connection's Error collection.
    ErrorPtr    pErr  = NULL;

    if( (pConnection->Errors->Count) > 0)
    {
        long nCount = pConnection->Errors->Count;
        // Collection ranges from 0 to nCount -1.
 //       for(long i = 0; i < nCount; i++)
 //       {
            pErr = pConnection->Errors->GetItem(0L);
           String sErr("Error number: %x\t%s\n", pErr->Number,(LPCSTR) pErr->Description);
					 err = ErrorObject(sErr);
 //       }
    }
}

void ADOTable::PrintComError(_com_error &e)
{
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());

		String sErr("Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
			          e.Error(), e.ErrorMessage(), (LPCSTR) bstrSource, (LPCSTR) bstrDescription);
    // Print Com errors.  
		throw ErrorObject(sErr);
}

void ADOTable::PutData(Column& col, long iRec, _variant_t& result)
{
	switch(result.vt)
	{
		case VT_BSTR:
		case VT_LPSTR:			
			{
				CString str((LPCSTR)(_bstr_t)result);
				col->PutVal(iRec, String(str));
			}				
			break;
		case VT_UI4:			
		case VT_R8:
		case VT_R4:
		case VT_CY:
			{
				double rV = (double)result;
				col->PutVal(iRec, rV);				
			}
			break;
		case VT_DATE:
		{
			COleDateTime dt = (COleDateTime)result;
			//SYSTEMTIME tm;
			//dt.GetAsSystemTime(tm);
			CString sTime = dt.Format();
			col->PutVal(iRec, String(sTime));
			break;
		}	
		case VT_BOOL:
		{
			bool b = (bool)result;
			col->PutVal(iRec, (long)b);
			break;
		}
		case VT_UI2:
		{
			long iV = (long)result;
			col->PutVal(iRec, iV);
			break;
		}		
		case VT_I4:
		case VT_ERROR:			
		{
			long iV = (long)result;
			col->PutVal(iRec, iV);
			break;
		}
		case VT_I2:
		case VT_UI1:			
		{
			short iV = (short)result;
			col->PutVal(iRec, (long)iV);
			break;
		}
		case VT_I1:			
		{
			char iV = (char)(short)result;
			col->PutRaw(iRec, iV);
			break;
		}				
		default:
			break;
	}				
}

DomainValueRangeStruct ADOTable::dmTranslateDataTypeToIlwis(DataTypeEnum dte, bool &fReadOnly, const String& sColName, TablePtr *tbl)
{
	FileName fnDom;
	String sColSection("Col:%S", sColName);
	ObjectInfo::ReadElement(sColSection.c_str(), "Domain", tbl->fnObj, fnDom);
	if ( fnDom.fExist() && !ObjectInfo::fSystemObject(fnDom))
	{
		Domain dom(fnDom);
		if ( dom->pdsrt() )
			return DomainValueRangeStruct(dom);
	}		
	switch(dte)
	{
		case adBoolean:
			return Domain("bool");
		case adBSTR:
		case adVarChar:
		case adLongVarChar:
		case adLongVarWChar:
		case adVarWChar:
		case adWChar:
			return DomainValueRangeStruct(Domain("String"));
		case adDate:
		case adDBDate:
		case adDBTime:
		case adDBTimeStamp:	
		case adFileTime:
		case adGUID:			
			fReadOnly = true;
			return DomainValueRangeStruct(Domain("String"));			

		case adChar:
		case adUnsignedTinyInt:			
			return DomainValueRangeStruct(0, 255, 1);
		case adDecimal:
		case adNumeric:
		case adSingle:
		case adDouble:
			return DomainValueRangeStruct(-1e307, 1e307, 0.0);
		case adCurrency:
			return DomainValueRangeStruct(-1e18, 1e18, 0.01);
		case adError:
		case adInteger:			
			return DomainValueRangeStruct( -LONG_MAX + 2 , LONG_MAX - 2);
		case adUnsignedInt:			
			return DomainValueRangeStruct( 0, LONG_MAX - 2);			
		case adTinyInt:
			return DomainValueRangeStruct(-128, 128);
		case adUnsignedSmallInt:
			return DomainValueRangeStruct( 0, 32768);
		case adSmallInt:
			return DomainValueRangeStruct(-16383, 16383);
	}		
	return Domain("none")	;
}

void ADOTable::PutStringField(const String& sColumn, long iRecord, const String& sValue)
{
	if ( sarForeignCols.size() != 0 && sarForeignCols.find(sColumn) == sarForeignCols.end() ) //nothing to do here
		return;
	_RecordsetPtr  pRstTable;
	FieldsPtr      pFields ;	
	_bstr_t strCnn(sConnectionString.c_str());
	try
	{	
		pRstTable.CreateInstance(__uuidof(Recordset));

		HRESULT r = pRstTable->Open (sQuery.c_str(), strCnn,
				        adOpenKeyset, adLockOptimistic, adCmdText);

		pRstTable->MoveFirst();
		pRstTable->Move(iRecord - 1); //offset 0;
		pFields = pRstTable->GetFields();
		if (  pFields )
		{
			int iNoColumns= pFields->Count;
			bool fAddCols = sarForeignCols.size() == 0;
			for(int iC = 0; iC < iNoColumns; ++iC)
			{
				_variant_t Index;
	      Index.vt = VT_I2;
	      Index.iVal = iC;				
				if ( pFields->GetItem(Index)->Name.GetBSTR() == CString(sColumn.c_str()))
				{
					long iMaxFieldSize = pFields->GetItem(Index)->DefinedSize;
					DataTypeEnum dteType = pFields->GetItem(Index)->Type;
					String sAdaptedValue = sValue;
					switch(dteType)
					{
						case adInteger:
						case adUnsignedTinyInt:	
						case adUnsignedSmallInt:
						case adUnsignedInt:
						case adSmallInt:
						case adTinyInt:							
						{
							long iMaxVal = pow(10.0, iMaxFieldSize);
							long iVal = sValue.iVal();
							if ( iVal > iMaxVal )
								throw ErrorObject(String(TR("Data does not fit in the defined field size (%d)").c_str(), iMaxFieldSize));
						}							
						break;
						case adChar:
						case adVarChar:
						case adLongVarChar:
						{
							if ( sValue.size() > iMaxFieldSize )
								throw ErrorObject(String(TR("Data does not fit in the defined field size (%d)").c_str(), iMaxFieldSize));
						}
						break;
						case adBSTR:
						case adVarWChar:
						case adWChar:	
						{
							if ( sValue.size() * 2 > iMaxFieldSize )
								throw ErrorObject(String(TR("Data does not fit in the defined field size (%d)").c_str(), iMaxFieldSize));
						}
						break;						
					}
					_bstr_t strValue(sAdaptedValue.c_str());					
					pFields->GetItem(Index)->Value = strValue;
					pRstTable->Update();
				}
				if ( fAddCols )
					sarForeignCols.insert(sColumn);
			}				
			pRstTable->Close();
		}	
	}
  catch (_com_error &e)
	{
		_variant_t vtConnect = pRstTable->GetActiveConnection();

		ErrorObject err;
    switch(vtConnect.vt)
    {
        case VT_BSTR:
            PrintComError(e);
            break;
        case VT_DISPATCH:
            PrintProviderError(vtConnect, err);
            break;
        default:
            printf("Errors occured.");
            break;
    }
		//if ( pRstTable) pRstTable->Close();
		throw err;
	}			
}

bool ADOTable::fIsCollection(const String& sForeignObject) const 
{ 
	String sName = ForeignFormat::sFindFormatName(sForeignObject);
	if ( sName == "MSAccess" || sName == "Excel" ) return true;
	return false;
}

// function will test if a type passed from a doctemplate matches the object type of the filename
bool ADOTable::fMatchType(const String& sFileName, const String& sType)
{
	// Templates object type name
	if ( fIsCollection(sFileName))
		return sType == "ILWIS DataBase Collections";
	else
		return sType == "ILWIS Tables";
}

void ADOTable::getImportFormats(vector<ImportFormat>& formats) {
	ImportFormat frm;
	frm.type = ImportFormat::ifTable;
	frm.name = "Excel";
	frm.ext="xls";
	frm.shortName = "Excel";
	frm.method = "ADO";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);

	frm.type = ImportFormat::ifTable;
	frm.name = "dBase IV File Format(DBF)";
	frm.ext="dbf";
	frm.shortName = "DBF";
	frm.method = "ADO";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);

	frm.type = ImportFormat::ifTable;
	frm.name = "Access";
	frm.ext="xls";
	frm.shortName = "Access";
	frm.method = "ADO";
	frm.provider = frm.method;
	frm.useasSuported = true;
	frm.ui = NULL;
	formats.push_back(frm);
}