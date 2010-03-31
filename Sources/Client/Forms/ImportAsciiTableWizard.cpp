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
/*-----------------------------------------------
   $Log: /ILWIS 3.0/ApplicationForms/ImportAsciiTableWizard.cpp $
 * 
 * 67    18-04-05 10:15 Retsios
 * [bug=6391]
 * The filename of the output table is altered (if needed) so that it
 * always works.
 * 
 * 66    10-03-03 16:35 Retsios
 * Clear columns array when user chooses a new table - solves bug#6396
 * 
 * 65    3/06/03 12:39p Martin
 * last quoting ptoblem (yeah right). This time also for databases
 * 
 * 64    3/05/03 8:27a Martin
 * another quoting problem
 * 
 * 63    2/27/03 12:33p Martin
 * format for ilwis branch of imports could not change after a default was
 * choosen because a piece of comment was commented including the
 * assignment of the format.
 * 
 * 62    26-02-03 16:21 Willem
 * - Added: The output wizard page now also checks if the output directory
 * is read/only
 * 
 * 61    2/06/03 12:05p Martin
 * default output dir is now the cur dir, not the dir where the input file
 * is located. This is inline with the map import and forstall problems
 * with people who try to import things from cd roms.
 * 
 * 60    2/06/03 9:05a Martin
 * if the file extension is not db, mdb, xls the default will siwtch to
 * use ILWIS import. I do not skip this page because it is legal (and
 * sometimes possible) that for example a dbf file has not a dbf
 * extension. Here you still have the chance to switch
 * 
 * 59    11/26/02 10:41a Martin
 * prevented the use of illegal columns names
 * 
 * 58    11/25/02 11:56a Martin
 * added support for excel in the wizard
 * 
 * 57    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 56    20-02-02 17:48 Willem
 * Default output name now uses correct extension for new (unique) name,
 * avoiding the "__1" postfix the first time
 * 
 * 55    20-02-02 14:51 Retsios
 * StoreData() was forgotten for the "skip lines". Callback added to do
 * the job.
 * 
 * 54    12/12/01 1:08p Martin
 * output name of ado expression can have only one .tbt
 * 
 * 53    12/07/01 10:33a Martin
 * changed title of page
 * 
 * 52    12/05/01 9:20a Martin
 * now a proper check if a filename exists. files without extension where
 * not correctly checked
 * 
 * 51    12/04/01 9:44a Martin
 * sql queries seem not to be able ti handle paths. now use sFileExt to g
 * et the correct name. Disbaled network databases
 * 
 * 52    11/30/01 3:51p Martin
 * strip \n \r from sql queries. seems not to work correctly
 * 
 * 51    11/30/01 11:53a Martin
 * sql queries seem not to be able ti handle paths. now use sFileExt to
 * get the correct name. Disbaled network databases
 * 
 * 50    11/21/01 12:44p Martin
 * changed order in the choose method page
 * 
 * 49    11/21/01 10:30a Martin
 * Open from recordset is not very happy with paths with spaces in it. So
 * the curdir is first set then the open is done and then the curdir is
 * setback
 * 
 * 48    11/21/01 9:55a Martin
 * added extra htp numbers for pages of the wizard
 * 
 * 47    11/20/01 4:46p Martin
 * last page setwizardnext function returns -1 and not 0; prevent extra
 * step.
 * error is directly shown and not thrown. prevents com errors
 * 
 * 46    11/20/01 9:08a Martin
 * output names through the ADO track now have a name based on the table
 * and not the database and it is unique
 * 
 * 45    11/20/01 8:39a Martin
 * error for choosing an invalid column as table domain is now shown only
 * once
 * 
 * 44    11/19/01 4:34p Martin
 * now uses a function to check if a file is dbf
 * 
 * 43    11/19/01 3:44p Martin
 * added browse button for output and solved some alignment problemsd
 * 
 * 42    11/19/01 9:47a Martin
 * useas checkbox does not show if it is not an ADO type import
 * 
 * 41    11/19/01 8:56a Martin
 * changed the the text string in the input page
 * 
 * 40    11/12/01 1:00p Martin
 * WChars added as type of the possible columns to select a domain from
 * 
 * 39    11/12/01 12:27p Martin
 * forgot to quote the output filename when creating the expression
 * 
 * 38    11/12/01 12:18p Martin
 * added error when no connection could be established
 * 
 * 37    10/31/01 9:13a Martin
 * too much copy and paste. An index was wrongly checked for the
 * DBTableSelector because that code was copied from somewhere else
 * 
 * 36    10/17/01 4:07p Martin
 * changed the string for useas and reversed the flag
 * 
 * 35    10/05/01 10:56a Martin
 * after change in the InitScan the else branche of dbf was incorrect,
 * should return immediately
 * 
 * 34    10/05/01 10:15a Martin
 * changed strings for the import ADO table part
 * 
 * 33    10/04/01 11:35a Martin
 * support for file based databases (like access)
 * 
 * 32    10/04/01 9:01a Martin
 * added some COM error handling. Error should not happen but it did so
 * the error handling was needed
 * 
 * 31    9/27/01 11:17a Martin
 * wrong name was used if a column was wished for a tabledomain
 * 
 * 30    9/24/01 10:00a Martin
 * added flag for use-as
 * 
 * 29    9/21/01 10:15a Martin
 * many changes to add tables of a full database to the wizard
 * 
 * 28    8/28/01 4:21p Martin
 * added suport for ADO tables
 * 
 * 27    5/22/01 12:54 Willem
 * The combo box for the selection of the table domain column is now
 * maintained properly. The selection is now preserved correctly
 * 
 * 26    18-04-01 14:33 Koolhoven
 * given the pages of the import table wizard seperate htp numbers
 * 
 * 25    17-04-01 11:48 Koolhoven
 * added htp for import table wizard
 * 
 * 24    21/03/01 13:59 Willem
 * After a column number change the Next button status (enabled/disabled)
 * is now also updated
 * 
 * 23    13/03/01 14:14 Willem
 * - Added extra remark message when column width is not correct (Fixed
 * format only)
 * - FillSelector() now only tries to access existing column information
 * 
 * 22    12/03/01 17:27 Willem
 * - Simplified code for ColumnSpecify page somewhat
 * - Removed maximum column check
 * - Initialized all page members
 * - The fields are now aligned vertically instead of horizontally
 * 
 * 21    13/02/01 10:40 Willem
 * Added extra check for array bounds
 * 
 * 20    19/01/01 11:41 Willem
 * - Added a remark field for display of warning messages
 * - Empty column name or empty domain name now disable the next button
 * and also display an appropriate message
 * 
 * 19    6-12-00 12:40 Koolhoven
 * Table Wizard now has a Table Icon
 * 
 * 18    29/11/00 13:44 Willem
 * - The key column is now passed properly to the command line
 * - The list of columns to be used for table domain is now limited to
 * DomainID or DomainClass columns
 * 
 * 17    23/11/00 17:27 Willem
 * For Ilwis 1.4 import the lines to skip is now properly set to 1 in the
 * "edit column details" page
 * 
 * 16    23/11/00 15:48 Willem
 * The output table name is now by default set on the input name (without
 * extension)
 * 
 * 15    21/11/00 16:22 Willem
 * The command line creation added a right paranthesis ")" to much to the
 * end of the column info, causing too few columns to be imported
 * 
 * 14    21/11/00 12:11 Willem
 * Replace the "Key Column" button with selection of a column for the
 * table domain
 * 
 * 13    20/11/00 13:08 Willem
 * - The file selection page now displays a message on the form when a
 * file is binary; also the next button becomes disabled
 * - The file list now only lists non-Ilwis files
 * 
 * 12    10-11-00 17:51 Koolhoven
 * added try/catch block in NameChange()
 * 
 * 11    25/10/00 18:10 Willem
 * - Removed obsolete code
 * - Moved activation code from constructor to member functions
 * - Adjusted callback function behaviour to handle the additional input
 * page
 * 
 * 10    24/10/00 17:53 Willem
 * - Removed Define Column Width wizard page (now included in Column
 * defintion page
 * - Class/ID domains are now created when Finish is clicked and input
 * table has been fully scanned
 * 
 * 9     19/10/00 10:55 Willem
 * Import wizard now performs a full scan when a key column is specified
 * 
 * 8     13/10/00 14:53 Willem
 * - Added input selector page, for use of the wizard from the command
 * line
 * - Rearrange the scanning of the import table 
 * 
 * 7     30/06/00 15:44 Willem
 * - Table wizard is now language independent
 * - The comamnd line generated now properly quotes long file names
 * - Added callbacks to protect against incomplete information
 * A few smaller changes
 * 
 * 6     22/06/00 15:47 Willem
 * Added pushbutton to toggle key column
 * 
 * 5     21/06/00 18:15 Willem
 * Improved version of the import table wizard
 * - better column selection handling
 * - scan is done each time the format selection is changed
 * 
 * 4     31/05/00 17:34 Willem
 * Work in progress:
 * - Added new functionality. 
 * - The Columlister inplace editors are OK
 * 
 * 3     24/05/00 9:11 Willem
 * Added the object output name as parameter to the wizard
 * 
 * 2     19/05/00 14:40 Willem
 * Added Column display to import ascii table wizard
 * 
 * 1     18/05/00 14:00 Willem
 * Working skeleton wizard for Import Ascii Tables

   ImportAsciiTableWizard.cpp
   by Willem Nieuwenhuis, 16/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\FormElements\NonIlwisObjectLister.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\FieldColumnLister.h"
#include "Client\FormElements\FieldList.h"
#include "Client\FormElements\flddat.h"
#include "Engine\DataExchange\OLEDBProvider.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Editors\Utils\SBDestination.h"
#include "Client\ilwis.h"
#include <atlbase.h>
#include "Engine\Base\File\Directory.h"
extern CComModule _Module;
#include <atlcom.h>
#include <comutil.h>
#include <atldbcli.h>
#include <atldbsch.h>
#include <set>
#include "Client\Forms\ImportAsciiTableWizard.h"
#include "Engine\DataExchange\TableIlwis14.h"
#include "Engine\DataExchange\TableDBF.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\TabelWizard.hs"
#include "Headers\Hs\Appforms.hs"

//#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
//    no_namespace rename("EOF", "EndOfFile")

class SelectDBTable : public FormBaseWizardPage
{
public:
	SelectDBTable(AsciiTableWizard* atw) : FormBaseWizardPage(STWTitleDBTableDomain)
	{
		m_atw = atw;
		FormEntry* root = feRoot();	
		StaticText *txt = new StaticText(root, STWDatabaseTables);
		StaticText *txt2 =new StaticText(root, STWKeyColumns);
		txt2->Align(txt, AL_AFTER);
		dbSel = new DBTableSelector(root, m_atw);
		dbSel->Align(txt, AL_UNDER);
		colSel = new TableColumnSelector(root, m_atw, this);	
		dbSel->SetColSelector(colSel);
		colSel->Align(txt2, AL_UNDER);		
	}
  virtual BOOL OnSetActive() 
	{
		FormBaseWizardPage::OnSetActive();
		return TRUE;
	}	
	LRESULT OnWizardNext()
	{
		SwitchToNextPage(m_atw->ppQuery);
		return -1;
	}
	LRESULT OnWizardBack()
	{
		EnablePreviousPage();
		return -1;
	}		
	AsciiTableWizard *m_atw;		
private:
	DBTableSelector *dbSel;
	TableColumnSelector	*colSel;
};


DBTableSelector::DBTableSelector(FormEntry *par, AsciiTableWizard* atw) :
	BaseSelector(par),
	m_atw(atw)
{
	style = WS_VSCROLL | LBS_WANTKEYBOARDINPUT;
  psn->iMinWidth *= 1.5;		
}

String DBTableSelector::sName(int id)
{
	return arNames[id] + ".tbt";
}

void DBTableSelector::SetColSelector(TableColumnSelector *sel)
{
	m_colsel = sel;
}

void DBTableSelector::create()
{
	BaseSelector::create();
	Fill();
  lb->setNotify(this, (NotifyProc)&DBTableSelector::Selected, Notify(LBN_SELCHANGE));
}

void DBTableSelector::Fill()
{
  lb->ResetContent();

	_bstr_t strCnn(m_atw->m_sDataBaseConnectionString.c_str());
	FieldsPtr      pFields ;
	_ConnectionPtr pConnection = NULL;	

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
		// for excel an exception has to be made, all excel tables are system tables (?, bug)
		String sFormatName = m_atw->m_sDataBaseConnectionString.find(".xls") != string::npos ? String("Excel") : String();
		if ( sFormatName =="Excel" || sType != "SYSTEM TABLE" && sType != "SYSTEM VIEW")		
		{
			String sTable("%s", (LPCSTR)table_name);
			arNames.push_back(sTable);
		}			

    pRstSchema->MoveNext();

	}
	m_atw->m_sDataBaseInputTable = arNames[0];
	for(vector<String>::iterator cur = arNames.begin(); cur != arNames.end(); ++cur)
	{
		String sColName("%S.tbt", *cur);
		lb->AddString(sColName.c_str());
	}	

	lb->SetCurSel(0);

//	m_atw->m_fnOutput = FileName(arNames[0], ".tbt");
	pRstSchema->Close();
	pConnection->Close();
}

int DBTableSelector::Selected(Event *)
{
	int iIndex;
	if ( arNames.size() != 0  && (iIndex = lb->GetCurSel()) != -1)
	{
//		if ( iIndex == 0)
//			return 1;
		String sSelTable = arNames[iIndex];	
		m_atw->m_sDataBaseInputTable = sSelTable;
		if ( m_colsel)
			m_colsel->Fill();
		m_atw->m_sSQLQuery = "";
//		m_atw->m_fnOutput = FileName(sSelTable, ".tbt");
	}	
	return 1;
}	
	

//----------------------------------------------------
TableColumnSelector::TableColumnSelector(FormEntry* par, AsciiTableWizard* atw, SelectDBTable *pDBT) :
	BaseSelector(par),
	m_atw(atw),
	pDBTable(pDBT)
{
	style = WS_VSCROLL | LBS_WANTKEYBOARDINPUT;
  psn->iMinWidth *= 1.5;
}

TableColumnSelector::~TableColumnSelector()
{
}

String TableColumnSelector::sName(int id)
{
	if ( id == 0 && m_atw->m_sDataBaseInputTable != "")
		return "none.dom";
  else		
		return arNames[id] + ".clm";
}

void TableColumnSelector::create()
{
	BaseSelector::create();
	Fill();
  lb->setNotify(this, (NotifyProc)&TableColumnSelector::Selected, Notify(LBN_SELCHANGE));
}

bool TableColumnSelector::fAcceptableDomain(_ConnectionPtr& pConnection, _RecordsetPtr& pRstTable, const String& sSelName, set<String>& setNames )
{
	FieldsPtr pFields = pRstTable->GetFields();	
	if (  pFields )
	{
		int iNumColumns = pFields->GetCount();									
		for(int iCol=0; iCol < iNumColumns; ++iCol)
		{	
			_variant_t Index;
			Index.vt = VT_I2;
			Index.iVal = iCol;					
			String sColName((LPSTR)pFields->GetItem(Index)->GetName());		
			if ( sSelName == sColName )
			{
				long iNumRecords;
				_variant_t avarRecords;
				avarRecords = pRstTable->GetRows(adGetRowsRest);
				HRESULT hr = SafeArrayGetUBound(avarRecords.parray,2,&iNumRecords);
				iNumRecords++; // points to upperbound counted from 0 so +1 to get the real number of objects								
				long rgIndices[2];								
		    rgIndices[0] = iCol; 
				for( long iRec = 0; iRec < iNumRecords; ++iRec)
				{
			    rgIndices[1] = iRec;
			    _variant_t result;
			    result.vt = pFields->GetItem(Index)->Type;
			    hr= SafeArrayGetElement(avarRecords.parray, rgIndices, &result);
					String str;
					if ( result.vt != VT_NULL)
						str = String((LPCSTR)(_bstr_t)result);
					else
						return false;
					if ( setNames.find(str) == setNames.end() )
						setNames.insert(str);
					else
						return false;
						
				}	
				return true;
			}
		}	
	}	
	return false;
}

int TableColumnSelector::Selected(Event *)
{
	_ConnectionPtr pConnection;
	_RecordsetPtr pRstTable	;			
	try
	{
		int iIndex;
		if ( arNames.size() != 0  && (iIndex = lb->GetCurSel()) != -1)
		{
			if ( iIndex == 0)
				return 1;
			
			String sSelCol = arNames[iIndex];
			set<String> setNames;
			if ( iIndex == 0)
				return 1;

			if ( !OpenRecordSet(pConnection, pRstTable) )
			{
				return 0;
			}				
			// prevent double error messages (event gets send twice), remember last column with error
			if ( sIllegalCol == sSelCol)
				return 1;
			else
				sIllegalCol = "";

			if ( fAcceptableDomain(pConnection, pRstTable, sSelCol, setNames) == false)
			{
				sIllegalCol = sSelCol;
				m_atw->m_sKeyCol = "none";
				lb->SetSel(0);
				frm()->wnd()->MessageBox(STWErrDomainNotPossible.scVal(), STWTilteDomainError.scVal(), MB_OK | MB_ICONSTOP);
			}
			else
				m_atw->m_sKeyCol = sSelCol;
			
			pRstTable->Close();	
			pConnection->Close();		
			
		}
	}		
  catch (_com_error &e)
	{
		_variant_t vtConnect = pRstTable->GetActiveConnection();

     switch(vtConnect.vt)
    {
        case VT_BSTR:
					{
						_bstr_t bstrSource(e.Source());
						_bstr_t bstrDescription(e.Description());

						String sErr("Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
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
								String sErr("Error number: %x\t%s\n", pErr->Number,(LPCSTR) pErr->Description);
								throw ErrorObject(sErr);
							}
						}							
					}
          break;
        default:
            throw ErrorObject("Errors occured.");
            break;
    }
	}				
	return 1;
}

bool TableColumnSelector::OpenRecordSet(_ConnectionPtr& pConnection, _RecordsetPtr& pRstTable)
{
	try
	{
		if (  pDBTable == 0)
		{
			OLEDBProvider *prov = OLEDBProvider::create("ODBC");
			FileName fnInput = m_atw->fnInput();
			if ( fnInput == FileName() )
				return false;
			m_atw->m_sDataBaseConnectionString = prov->sBuildConnectionString(fnInput.sFullPath(), "", "", "");
			delete prov;
			m_atw->m_sDataBaseInputTable = fnInput.sFullName();
		}

		_bstr_t strCnn(m_atw->m_sDataBaseConnectionString.c_str());

	  pConnection.CreateInstance(__uuidof(Connection));
	  pConnection->Mode = adModeShareExclusive;
	  pConnection->IsolationLevel = adXactIsolated;
	  pConnection->Open(strCnn,"","",adConnectUnspecified);
		pRstTable.CreateInstance(__uuidof(Recordset));	

		FileName fn(m_atw->m_sDataBaseInputTable);
		//v.SetString(m_atw->m_sDataBaseInputTable.scVal());
		String sTableName = "\"" + m_atw->m_sDataBaseInputTable + "\"";
		String sCurDir = IlwWinApp()->sGetCurDir();

		if ( fn.sExt != "" )
		{
			IlwWinApp()->SetCurDir(fn.sPath());
			sTableName = "\"" + fn.sFileExt() + "\""; 
		}			
	 	HRESULT r = pRstTable->Open (sTableName.scVal(), variant_t((IDispatch*) pConnection, true),
					adOpenForwardOnly, adLockReadOnly, adCmdTable);	

		IlwWinApp()->SetCurDir(sCurDir);
		if ( r == 0 )
			return true;
		else
			return false;

	}
	catch (_com_error &e)
	{
		_variant_t vtConnect;		
		if ( pRstTable )
			vtConnect = pRstTable->GetActiveConnection();

     switch(vtConnect.vt)
    {
        case VT_BSTR:
					{
						_bstr_t bstrSource(e.Source());
						_bstr_t bstrDescription(e.Description());

						String sErr("Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
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
								String sErr("Error number: %x\t%s\n", pErr->Number,(LPCSTR) pErr->Description);
		            ErrorObject err(sErr);
								err.Show();
								return false;
							}
						}							
					}
          break;
        default:
				{
						String sErr(STWErrNoConnection.scVal(), m_atw->fnInput().sRelative());
            ErrorObject err(sErr);
						err.Show();
            return false;
				}						
    }
	}
	return false;
}

void TableColumnSelector::Fill()
{
	if ( lb == 0 )
		return;
	sIllegalCol = "";
	
  lb->ResetContent();

	_ConnectionPtr pConnection;
	_RecordsetPtr pRstTable	;

	if ( ! OpenRecordSet(pConnection, pRstTable) )
	{
		return;
	}				


	set<String> setNames;	
	FieldsPtr pFields = pRstTable->GetFields();	
	if (  pFields )
	{
		int iNumColumns = pFields->GetCount();									
		for(int iCol=0; iCol < iNumColumns; ++iCol)
		{	
			_variant_t Index;
			Index.vt = VT_I2;
			Index.iVal = iCol;					
			DataTypeEnum deType =pFields->GetItem(Index)->Type;
			_bstr_t sColName = pFields->GetItem(Index)->GetName();
			if ( deType == adChar || deType == adLongVarChar || deType == adVarChar|| deType == adVarWChar)
				setNames.insert(String((LPSTR)sColName));
		}
	}

	arNames.clear();
	arNames.push_back("none");
	copy(setNames.begin(), setNames.end(), back_inserter(arNames));
	// Clean up objects before exit.
	pRstTable->Close();	
  pConnection->Close();
    	
	//pRstTable.CreateInstance(__uuidof(Recordset));
	for(vector<String>::iterator cur = arNames.begin(); cur != arNames.end(); ++cur)
	{
		String sColName("%S.clm", *cur);
		lb->AddString(sColName.c_str());
	}	
	lb->SetCurSel(0);
}

//---------------------------------------------------------------	

void TableColumnSelector::StoreData()
{
}


class SelectTableDomainPage: public FormBaseWizardPage
{
public:
	SelectTableDomainPage(AsciiTableWizard* atw) : FormBaseWizardPage(STWTitleTableDomain)
	{
		m_atw = atw;
		new TableColumnSelector(root,atw);
	}	
	LRESULT OnWizardNext()
	{
		SwitchToNextPage(m_atw->ppQuery);
		return -1;
	}
	LRESULT OnWizardBack()
	{
		EnablePreviousPage();
		return -1;
	}		
private:	
	AsciiTableWizard *m_atw;
};

class EnterSqlQuery : public FormBaseWizardPage
{
public:
	EnterSqlQuery(AsciiTableWizard* atw) : FormBaseWizardPage(STWTitleSQLQuery)
	{
		m_atw = atw;
		fs = new FieldStringMulti(root, &(m_atw->m_sSQLQuery));
		fs->SetWidth(150);
		fs->SetHeight(50);
	}
  virtual BOOL OnSetActive() 
	{
		FormBaseWizardPage::OnSetActive();
		m_atw->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
		if ( m_atw->m_sSQLQuery == "")
		{
			String sInput = m_atw->m_fDataBase ? m_atw->m_sDataBaseInputTable : m_atw->fnInput().sFileExt();
			m_atw->m_sSQLQuery = String("Select * from \"%S\"", sInput);
			fs->SetVal(m_atw->m_sSQLQuery);
		}			

		return TRUE;
	}	
	LRESULT OnWizardBack()
	{
		EnablePreviousPage();
		return -1;
	}			
private:
	AsciiTableWizard *m_atw;	
	FieldStringMulti *fs;
};


class FileViewPage : public FormBaseWizardPage
{
public:
	FileViewPage(AsciiTableWizard* atw) : FormBaseWizardPage(STWTitlePageFileView)
	{
		m_atw = atw;

		FormEntry* root = feRoot();
		(new StaticText(root, STWMsgContentsInput))->SetIndependentPos();

		DWORD style = WS_GROUP|WS_TABSTOP|ES_MULTILINE|ES_READONLY|
						ES_AUTOVSCROLL|WS_VSCROLL|WS_HSCROLL|WS_BORDER;

		fsmFile = new FieldStringMulti(root, &m_sMemo, style);
		fsmFile->SetFixedFont();
		fsmFile->SetWidth(200);
		fsmFile->SetHeight(175);
	}
	LRESULT OnWizardNext()
	{
		SwitchToNextPage(m_atw->ppColDetails);		
		return -1;
	}
		
	virtual BOOL OnSetActive()
	{
		FormBaseWizardPage::OnSetActive();
		m_atw->SetWizardButtons(PSWIZB_NEXT); 
		if (File::fExist(m_atw->fnInput()))
		{
			if (m_fnLoaded != m_atw->fnInput() || !m_fnLoaded.fValid())
			{
				m_fnLoaded = m_atw->fnInput();

				m_atw->LoadFromFile(m_sMemo, 100);
				fsmFile->SetVal(m_sMemo);
			}
		}

		return TRUE;
	}

private:
	AsciiTableWizard* m_atw;
	String m_sMemo;
	FileName m_fnLoaded;

	FieldStringMulti *fsmFile;
};

class SpecifyColumnsDetailsPage : public FormBaseWizardPage
{
public:
	SpecifyColumnsDetailsPage(AsciiTableWizard* atw) : FormBaseWizardPage(STWTitlePageColumnsDetails)
	{
		m_atw = atw;
		m_iKey = -1; // no column is selected as table domain
		m_fHasKey = false;
		m_piSkip = &m_atw->m_iSkipLines;
		m_iCols = m_atw->m_iNrCols;
		fInColCountChange = false;

		FormEntry* root = feRoot();
		(new StaticText(root, STWMsgColumnProperties))->SetIndependentPos();
		fclColumn = new FieldColumnLister(root, m_atw->m_colInfo);
		fclColumn->SetIndependentPos();
		fclColumn->SetCallBack((NotifyProc)&SpecifyColumnsDetailsPage::ColumnNamesChange);
		m_cbTableDomain = new CheckBox(root, STWUiTableDomain, &m_fHasKey);
		m_cbTableDomain->SetCallBack((NotifyProc)&SpecifyColumnsDetailsPage::ToggleTableDomain);
		m_fos = new FieldOneSelectTextOnly(m_cbTableDomain, &m_sKey);
		m_fos->SetCallBack((NotifyProc)&SpecifyColumnsDetailsPage::KeyColChange);
		m_fos->Align(m_cbTableDomain, AL_AFTER);
		m_fos->SetWidth(80);

		m_fiCols = new FieldInt(root, STWUiColumns, &m_iCols, ValueRange(1, 9999), true);  // use with spin control
		m_fiCols->SetCallBack((NotifyProc)&SpecifyColumnsDetailsPage::ColCountChange);
		m_fiSkip = new FieldInt(root, STWUiHeaderSkipLines, m_piSkip, ValueRange(0, 9999), true);  // use with spin control
		m_fiSkip->SetCallBack((NotifyProc)&SpecifyColumnsDetailsPage::SkipLinesChange);
		String s('x', 60);
		m_stRemark = new StaticText(root, s);
		m_stRemark->SetIndependentPos();
	}

	LRESULT OnWizardNext()
	{
		//m_atw->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
		//SwitchToNextPage(m_atw->ppOutput);		
		return -1;
	}
	LRESULT OnWizardBack()
	{
		EnablePreviousPage();
		return -1;
	}	
	
	void FillSelector()
	{
		if (!m_fos->ose->IsWindowVisible())
			return;

		m_fos->ose->ResetContent();

		int iKey = -1;
		for (int i = 0; i < fclColumn->iNrCols(); i++)
		{
			ClmInfo& ci = fclColumn->ciColumn(i);
			if (ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID)
			{
				if (ci.fKeyAllowed)
					m_fos->AddString(ci.sColumnName);
				if (i == m_iKey)
					iKey = m_fos->ose->GetCount() - 1;
			}
		}

		if (iKey >= 0 && iKey < m_iCols)
			m_fos->ose->SetCurSel(iKey);  // reselect the item
	}
	// Check column info and appropriately disable NEXT button
	void CheckValidColumnInfo()
	{
		DWORD dwButtons = PSWIZB_BACK | PSWIZB_FINISH;
		int i;
		String sRem;
		for (i = 0; i < m_iCols; i++)
		{
			ClmInfo& ci = fclColumn->ciColumn(i);
			if (ci.sColumnName.length() == 0)
			{
				dwButtons &= ~PSWIZB_FINISH;
				sRem = String(STWRemColumnNameMissing_i.scVal(), i + 1);
				break;
			}
			if (!ci.fnDomain.fValid())
			{
				dwButtons &= ~PSWIZB_FINISH;
				sRem = String(STWRemDomainnameMissing_S.scVal(), ci.sColumnName);
				break;
			}
			if (m_atw->GetFormat() == TableExternalFormat::ifFixed)
			{
				if (ci.iColumnWidth == iUNDEF)
				{
					dwButtons &= ~PSWIZB_FINISH;
					sRem = String(STWRemIncorrectColumnWidth_S.scVal(), ci.sColumnName);
					break;
				}
			}
		}
		m_atw->SetWizardButtons(dwButtons);
		m_stRemark->SetVal(sRem);
	}
    virtual BOOL OnSetActive() 
	{
		FormBaseWizardPage::OnSetActive();
		m_fiSkip->SetVal(m_atw->m_iSkipLines);
		
		if (m_atw->m_iNrCols != m_iCols)
		{
			m_iCols = m_atw->m_iNrCols;
			m_fiCols->SetVal(m_iCols); // synchronize with DetermineColumnWidthPage
		}

		if (m_atw->GetFormat() == TableExternalFormat::ifDBF)
		{
			m_fiCols->Hide();
			m_fiSkip->Hide();
		}
		m_atw->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

		return TRUE;
	}
	virtual BOOL OnKillActive()
	{
		FormBaseWizardPage::OnKillActive();

		m_atw->m_iNrCols = m_iCols;

		if (m_iKey >= 0)
			fclColumn->ciColumn(m_iKey).fKey = true;

		return TRUE;
	}
	int ToggleTableDomain(Event*)
	{
		m_cbTableDomain->StoreData();
		if (!m_fHasKey)
		{
			m_iKey = -1;  // disable table domain
			fclColumn->ToggleAsKey(m_iKey); // visual feedback
		}
		else
		{
			FillSelector();
			KeyColChange(0);
		}
		
		return 0;
	}
	int ColumnNamesChange(Event*)
	{
		m_cbTableDomain->StoreData();
		// only refill the list of columns that are candidates for the
		// table domain if the selector is visible
		if (m_fHasKey)  
			FillSelector();

		CheckValidColumnInfo();

		return 0;
	}
	int KeyColChange(Event*)
	{
		int iKey = m_fos->ose->GetCurSel();
		CString scColName;
		m_fos->ose->GetWindowText(scColName);
		String sCol(scColName);
		if (iKey != LB_ERR)
		{
			for (int i = 0; i < fclColumn->iNrCols(); ++i)
			{
				ClmInfo& ci = fclColumn->ciColumn(i);
				if (fCIStrEqual(ci.sColumnName, sCol))
				{
					m_iKey = i;
					break;
				}
			}
			
			KeyCheck();
		}

		return 0;
	}
	int ColCountChange(Event*)
	{
		m_fiCols->StoreData();

		// SetRowCount() sets the visible rows in the listcontrol to m_iCols
		// It also increases the size of the m_atw->m_colInfo when m_iCols
		// is larger than the size of the m_atw->m_colInfo array
		if (m_iCols >= 0)
			fclColumn->SetRowCount(m_iCols);

		m_cbTableDomain->StoreData();
		// only refill the list of columns that are candidates for the
		// table domain if the selector is visible
		if (m_fHasKey)  
			FillSelector();

		// Number of columns has changed so recheck the validity of 
		// available information in the ColumnListCtrl fldColumn
		CheckValidColumnInfo();

		return 1;
	}
	int SkipLinesChange(Event*)
	{
		m_fiSkip->StoreData();
		return 0;
	}
	void KeyCheck()
	{
		if (!m_atw->m_fFullScanExecuted)
		{
			CWaitCursor wc;
			bool fScanNeeded = false;
			for (int i = 0; i < fclColumn->iNrCols(); ++i)
			{
				ClmInfo& ci = fclColumn->ciColumn(i);
				if (ci.fKey)
				{
					fScanNeeded = true;
					break;
				}
			}

			if (fScanNeeded)
			{
				vector<ClmInfo> vci;
				m_atw->ReScan(vci);
				for (int i = 0; i < fclColumn->iNrCols(); ++i)
				{
					ClmInfo& ci = fclColumn->ciColumn(i);
					ci.fKeyAllowed = vci[i].fKeyAllowed;  // only thing needed here
					ci.Strings.swap(vci[i].Strings);      // retain the scanned strings
					if (i == m_iKey && !ci.fKeyAllowed)
						m_iKey = -1;
				}
				m_atw->m_fFullScanExecuted = true;
			}
		}

		fclColumn->ToggleAsKey(m_iKey); // visual feedback
	}

private:
	FieldColumnLister      *fclColumn;
	FieldOneSelectTextOnly *m_fos;
	CheckBox               *m_cbTableDomain;
	StaticText             *m_stRemark;

	FieldInt *m_fiSkip;
	FieldInt *m_fiCols;
	bool     fInColCountChange;

	bool     m_fHasKey;
	int      m_iKey;
	String   m_sKey;

	int      *m_piSkip;
	int      m_iCols;
	AsciiTableWizard *m_atw;
};



AsciiTableWizard::AsciiTableWizard(CWnd* wnd) : CPropertySheet(STWTitleTableWizard.scVal(), wnd)  ,
		m_fFullScanExecuted(false), 
		m_sKeyCol("none"),
		m_fDataBase(false),
		m_fConvert(false),
		fADO(true),
		m_iNrCols(0),
		m_iMaxCol(0),
		m_iSkipLines(0),
		m_ifTable(TableExternalFormat::ifUnknown),
		m_duTable(TableExternalFormat::duUnknown),
		m_iActiveFormat(m_ifTable),
		ppTableDomain(NULL),
		ppQuery(NULL),
		ppSelectDBTable(NULL),
		ppFileView(NULL),
		ppColDetails(NULL),
		ppDetermineWidth(NULL)
{

}

// The table wizard implementation
AsciiTableWizard::AsciiTableWizard(CWnd* wnd, const FileName& fnAscTable, const FileName& fnObject, TableExternalFormat::InputFormat _format) 
		: CPropertySheet(STWTitleTableWizard.scVal(), wnd), 
		m_fFullScanExecuted(false), 
		m_sKeyCol("none"),
		m_fDataBase(false),
		m_fConvert(false),
		fADO(true),
		m_ifTable(_format)
{
	m_fnInput = fnAscTable;

	// Initialize the wizard
	SetPages();
			
}

void AsciiTableWizard::CleanUp() {
	delete ppTableDomain;
	delete ppQuery;
	delete ppSelectDBTable;
	delete ppFileView;
	delete ppColDetails;
	delete ppDetermineWidth;

	ppTableDomain = NULL;
	ppQuery = NULL;
	ppSelectDBTable = NULL;
	ppFileView = NULL;
	ppColDetails = NULL;
	ppDetermineWidth = NULL;

}
void AsciiTableWizard::SetPages() {
	SetWizardMode();
//	CleanUp();

	m_ifTable = TableExternalFormat::ifUnknown;
	m_iActiveFormat = m_ifTable;
	m_duTable = TableExternalFormat::duUnknown;
	m_iSkipLines = 0;
	m_iNrCols = 0;
	m_iMaxCol = 0;

	if (m_fnInput.fValid())
	{
		String sErr;
		InitScan(sErr);
	}

	if ( ppTableDomain != NULL)
		return;
	ppTableDomain = new SelectTableDomainPage(this);
	ppTableDomain->SetMenHelpTopic(htpImportTableWizardSelectTableDomain);
	ppTableDomain->create();
	AddPage(ppTableDomain);
	ppTableDomain->SetPPDisable();	

	ppQuery = new EnterSqlQuery(this);
	ppQuery->SetMenHelpTopic(htpImportTableWizardSQLQuery);
	ppQuery->create();
	AddPage(ppQuery);	
	ppQuery->SetPPDisable();	

	ppSelectDBTable = new SelectDBTable(this);
	ppSelectDBTable->SetMenHelpTopic(htpImportTableWizardSelectTableDomain);
	ppSelectDBTable->create();
	AddPage(ppSelectDBTable);
	ppSelectDBTable->SetPPDisable();			

	// file contents viewer
	ppFileView = new FileViewPage(this);
	ppFileView->SetMenHelpTopic(htpImportTableWizardView);
	ppFileView->create();
	AddPage(ppFileView);
	ppFileView->SetPPDisable();				

	// choose all column properties
	ppColDetails = new SpecifyColumnsDetailsPage(this);
	ppColDetails->SetMenHelpTopic(htpImportTableWizardColumns);
	ppColDetails->create();
	AddPage(ppColDetails);
	ppColDetails->SetPPDisable();					

}

INT_PTR AsciiTableWizard::DoModal() {
	SetPages();
	ReScan(m_colInfo);
	if ( m_fDataBase) {
		SetActivePage(ppSelectDBTable);
		fADO = true;
	}
	else if ( m_ifTable == TableExternalFormat::ifDBF)
		SetActivePage(ppColDetails);
	else if ( m_ifTable == TableExternalFormat::ifDBFADO) {
		fADO = true;
		SetActivePage(ppTableDomain);
	}
	else { 
		SetActivePage(ppFileView);
	}

	return CPropertySheet::DoModal();
}

AsciiTableWizard::~AsciiTableWizard()
{
}

void AsciiTableWizard::SetInputTable(const FileName& fn)
{
	m_fnInput = fn;
}

void AsciiTableWizard::SetFormat(ImportFormat impf) { 
	fADO = false;
	if ( impf.shortName == "DBF") {
		m_ifTable = TableExternalFormat::ifDBF;
		if ( impf.method == "ADO"){
			m_ifTable = TableExternalFormat::ifDBFADO;
			fADO = true;
		}
	}
	else if ( impf.shortName == "Ilwis 1.4")
		m_ifTable = TableExternalFormat::ifIlwis14;
	else if ( impf.shortName == "CSV")
		m_ifTable = TableExternalFormat::ifComma;
	else if ( impf.shortName == "Space delimeted")
		m_ifTable = TableExternalFormat::ifSpace;
	else if ( impf.shortName == "Fixed format")
		m_ifTable = TableExternalFormat::ifFixed;
	else
		m_ifTable = TableExternalFormat::ifUnknown;
}

//void AsciiTableWizard::SetTableName(const FileName& fn)
//{
//	m_fnOutput = fn;
//}

FileName AsciiTableWizard::fnInput()
{
	return m_fnInput;
}

FileName AsciiTableWizard::fnOutput()
{
	return FileName();
}

TableExternalFormat::InputFormat AsciiTableWizard::GetFormat()
{
	return m_ifTable;
}

void AsciiTableWizard::SetFormat(TableExternalFormat::InputFormat ifType)
{
	m_ifTable = ifType;
}

// read lines from input File: -1 = read all
void AsciiTableWizard::LoadFromFile(String& s, int iLines)
{
	File fil(m_fnInput);
	if (iLines == -1)
	{
		char *ps = new char(fil.iSize());
		fil.Read(fil.iSize(), ps);
		s = String(ps);
		delete [] ps;
	}
	else
	{
		s = "";
		String sLine;
		for (int i = 0; i < iLines; ++i)
		{
			if (fil.fEof())
				break;
			fil.ReadLnAscii(sLine);
			s &= sLine;
			s &= "\r\n";
		}
	}
}


void AsciiTableWizard::InitScan(String& sErr)
{
	sErr = String();
	m_fDataBase = false;
	m_colInfo.clear();
	if (TableDBF::fIsDBF(m_fnInput))
	{
		if ( fADO)
			m_ifTable = TableExternalFormat::ifDBFADO;
		else
			m_ifTable = TableExternalFormat::ifDBF;
		return;
	}		
	else
	{
		OLEDBProvider *prov = OLEDBProvider::create("ODBC");
		if ( prov )
		{
			String sSupportedExtensions = prov->sSupportedFormatExtensions();
			if ( sSupportedExtensions.find(m_fnInput.sExt) != string::npos )	
			{
				m_fDataBase = prov->fIsDataBaseFormat(m_fnInput.sFullPath());
				m_sDataBaseConnectionString = prov->sBuildConnectionString(m_fnInput.sFullPath(), "", "", "");				
				delete prov;
				return;
			}
			delete prov;			
		}
	}
	if (fCIStrEqual(m_fnInput.sExt, ".tbl"))
	{
		m_ifTable = TableExternalFormat::ifIlwis14;
		m_iSkipLines = 1;
	}
	else // Scan input file to determine default for input table type, column info is added later
	{
		if (File::fIsBinary(m_fnInput))
		{
			sErr = String(STWErrNoTextFile_S.scVal(), m_fnInput.sShortName());
			return;
		}

		int iSpecFields = 0;  // count header fields with specifiers (ilwis 1.4)
		TableIlwis14::Scan(m_fnInput, m_iSkipLines, iSpecFields, m_colInfo);
		if (m_colInfo.size() == 0 || m_colInfo.size() != iSpecFields)
		{
			m_ifTable = TableExternalFormat::ifUnknown;
			TableDelimited::GetFormatType(m_fnInput, m_ifTable, m_iSkipLines, m_iNrCols, true); // true: only read top of file
		}
		else
		{
			m_ifTable = TableExternalFormat::ifIlwis14;
			m_iSkipLines = 1;
		}
		m_colInfo.clear();  // avoid listing columns twice
	}
}

void AsciiTableWizard::ReScan(vector<ClmInfo> &vci)
{
	m_iActiveFormat = GetFormat();

	bool fFull = vci.size() == 0;
	for (unsigned int i = 0; i < vci.size(); ++i)
		fFull = fFull || vci[i].fKey;

	fFull = fFull && !m_fFullScanExecuted;

	int iSpecFields = 0;
	bool fUseColInfo = vci.size() > 0;
	switch (m_iActiveFormat)
	{
		case TableExternalFormat::ifDBF:
		case TableExternalFormat::ifDBFADO:
			TableDBF::Scan(m_fnInput, vci, fFull, fUseColInfo);
			break;
		case TableExternalFormat::ifIlwis14:
			TableIlwis14::Scan(m_fnInput, m_iSkipLines, iSpecFields, vci, fFull, fUseColInfo);
			break;
		case TableExternalFormat::ifComma:
		case TableExternalFormat::ifSpace:
		case TableExternalFormat::ifFixed:
			TableDelimited::Scan(m_fnInput, m_iSkipLines, m_ifTable, vci, fFull, fUseColInfo);  // scan top of file only
			break;
	}
	m_iNrCols = vci.size();
	m_iMaxCol = m_iNrCols * 2;
}

String AsciiTableWizard::sBuildADOExpr()
{
	String sDataBaseName;
	if ( m_sDataBaseConnectionString != "" )
	{
		int iWhere = m_sDataBaseConnectionString.find("Data Source=");
		if ( iWhere != string::npos)
		{
			int iUntil = m_sDataBaseConnectionString.find(";", iWhere);
			iWhere += String("Data Source=").size();
			sDataBaseName = m_sDataBaseConnectionString.substr(iWhere , iUntil - iWhere);	
		}	
		iWhere = m_sDataBaseConnectionString.find("DBQ=");				
		if ( iWhere != string::npos )
		{
			int iUntil = m_sDataBaseConnectionString.find(";", iWhere);
			iWhere += String("DBQ=").size();
			sDataBaseName = m_sDataBaseConnectionString.substr(iWhere , iUntil - iWhere);
		}			
	}
	String sInput = m_fDataBase ? String("%S\\%S", sDataBaseName.sQuote(), m_sDataBaseInputTable) : fnInput().sRelativeQuoted();
	FileName fnOut(fnOutput(), ".tbt");
	//String sOutTemp = fnInput().sPath() != fnOutput().sPath() ? fnOutput().sFullPathQuoted() : fnOutput().sRelativeQuoted();	
	String sExpr(" %S -noshow ", sInput);
	if ( m_sKeyCol != "none" )
		sExpr += String(" -key=%S", m_sKeyCol);
	if ( m_sSQLQuery != "")
	{
		String sStrippedQuery;
		for(unsigned int i=0; i< m_sSQLQuery.size(); ++i)
		{
			if ( m_sSQLQuery[i] != '\n' && m_sSQLQuery[i] != '\r')
				sStrippedQuery += m_sSQLQuery[i];
		}				
		sExpr += String(" -query=%S", sStrippedQuery.sQuote());
	}		
	if ( m_sDataBaseConnectionString != "" )
		sExpr += String(" -connectionstring=%S", m_sDataBaseConnectionString.sQuote());
	if ( m_fConvert )
		sExpr += " -import";

	return sExpr;
}

String AsciiTableWizard::sBuildExpression()
{
	String sEx = String("%S=table(%S,", fnOutput().sRelativeQuoted(true), m_fnInput.sRelativeQuoted(true));
	if ( fADO )
	{
		return sBuildADOExpr();
	}		
	switch (m_ifTable)
	{
		case TableExternalFormat::ifDBF     : sEx &= "DBF,";     break;
		case TableExternalFormat::ifIlwis14 : sEx &= "Ilwis14,"; break;
		case TableExternalFormat::ifSpace   : sEx &= "Space,";   break;
		case TableExternalFormat::ifComma   : sEx &= "Comma,";   break;
		case TableExternalFormat::ifFixed   : sEx &= "Fixed,";   break;
	}
	sEx &= "Convert,";
	if (m_ifTable != TableExternalFormat::ifDBF && m_iSkipLines > 0)
		sEx &= String("%i,", m_iSkipLines);
	sEx &= m_sDomain.length() > 0 ? m_sDomain : "none";

	for (int i = 0; i < m_iNrCols; ++i)
	{
		String sColInfo = "";
		ClmInfo& ci = m_colInfo[i];
		if (m_ifTable == TableExternalFormat::ifFixed)
			sColInfo &= String(",%i", ci.iColumnWidth);
		if (ci.sUndefValue.length() > 0)
			sColInfo &= String(",%S", ci.sUndefValue);
		if (ci.sColumnName.find("-") != string::npos || ci.sColumnName.find("+") != string::npos || ci.sColumnName.find("(") != string::npos ||
			  ci.sColumnName.find(")") != string::npos || ci.sColumnName.find("=") != string::npos || ci.sColumnName.find("/") != string::npos)
			throw ErrorObject(STWErrIllegalColumnNames);
		sColInfo &= String(",%S(", ci.sColumnName);
		if (ci.dtDomainType != dmtSTRING && ci.fCreate == false)
			sColInfo &= ci.fnDomain.sRelativeQuoted(true);
		else if (ci.dtDomainType != dmtSTRING && ci.fCreate == true)
			sColInfo &= fnOutput().sPath() + ci.fnDomain.sFileExt();		
		if (ci.dtDomainType == dmtVALUE)
		{
			if (!ci.fnDomain.fValid())
				sColInfo &= "value";
			RangeReal rr = ci.dvs.vr()->rrMinMax();
			sColInfo &= String("{%g:%g:%g}", rr.rLo(), rr.rHi(), ci.dvs.rStep());
		}
		else if (ci.dtDomainType == dmtCLASS || ci.dtDomainType == dmtID)
		{
			String sDm;
			if (ci.fCreate)
			{
				sDm = Domain::sDomainType(ci.dtDomainType);
				if (ci.vcStrings.size() > 0)  // already create domain
				{
					Domain dm(ci.fnDomain, 0, ci.dtDomainType);
					dm->pdsrt()->dsType = DomainSort::dsMANUAL;
					dm->pdsrt()->AddValues(ci.vcStrings);
					dm->pdsrt()->dsType = DomainSort::dsALPHANUM;
					dm->pdsrt()->ReSort();
				}
			}
			if (ci.fExtend)
			{
				if (sDm.length() > 0)
					sDm &= ",";
				sDm &= "Extend";
			}
			if (ci.fKey)
			{
				if (sDm.length() > 0)
					sDm &= ",";
				sDm &= "Key";
			}
			if (sDm.length() > 0)
				sColInfo &= String("{%S}", sDm);
		}
		else if (ci.dtDomainType == dmtSTRING)
			sColInfo &= Domain::sDomainType(ci.dtDomainType);

		sColInfo &= ")";

		sEx &= sColInfo;
	}
	sEx &= ")";

	return sEx;
}	
// The wizard callbacks
