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
   $Log: /ILWIS 3.0/ApplicationForms/ImportAsciiTableWizard.h $
 * 
 * 16    11/20/01 8:39a Martin
 * error for choosing an invalid column as table domain is now shown only
 * once
 * 
 * 15    10/22/01 10:07a Martin
 * renamed a member from f_convert to f_useas
 * 
 * 14    9/24/01 10:00a Martin
 * added flag for use-as
 * 
 * 13    9/21/01 10:15a Martin
 * many changes to add tables of a full database to the wizard
 * 
 * 12    8/28/01 4:21p Martin
 * added suport for ADO tables
 * 
 * 11    21/11/00 12:11 Willem
 * Replace the "Key Column" button with selection of a column for the
 * table domain
 * 
 * 10    20/11/00 13:08 Willem
 * - The file selection page now displays a message on the form when a
 * file is binary; also the next button becomes disabled
 * - The file list now only lists non-Ilwis files
 * 
 * 9     25/10/00 18:10 Willem
 * - Removed obsolete code
 * - Moved activation code from constructor to member functions
 * - Adjusted callback function behaviour to handle the additional input
 * page
 * 
 * 8     24/10/00 17:53 Willem
 * - Removed Define Column Width wizard page (now included in Column
 * defintion page
 * - Class/ID domains are now created when Finish is clicked and input
 * table has been fully scanned
 * 
 * 7     19/10/00 10:55 Willem
 * Import wizard now performs a full scan when a key column is specified
 * 
 * 6     13/10/00 14:53 Willem
 * - Added input selector page, for use of the wizard from the command
 * line
 * - Rearrange the scanning of the import table 
 * 
 * 5     30/06/00 15:44 Willem
 * - Table wizard is now language independent
 * - The comamnd line generated now properly quotes long file names
 * - Added callbacks to protect against incomplete information
 * A few smaller changes
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

   ImportAsciiTableWizard.h
   by Willem Nieuwenhuis, 16/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_IMPORTASCIITABLEFORM_H
#define ILW_IMPORTASCIITABLEFORM_H

#include "Engine\DataExchange\TableDelimited.h"

#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")

class AsciiTableWizard;
class SelectDBTable;
class TableColumnSelector;


class DBTableSelector : public BaseSelector
{
public:
	DBTableSelector(FormEntry *par, AsciiTableWizard* atw);
	String sName(int id);
  void StoreData() {}
	void SetColSelector(TableColumnSelector *sel);


protected:
	void create();
	int Selected(Event *);		
	void Fill();
	vector<String> arNames;
	AsciiTableWizard* m_atw;
	TableColumnSelector *m_colsel;	
};

class TableColumnSelector : public BaseSelector
{
	friend class DBTableSelector;
public:
  void StoreData();
  TableColumnSelector(FormEntry* par, AsciiTableWizard *atw, SelectDBTable *pDBT = 0);
  ~TableColumnSelector();
  //int idSelect();
  void Fill();
	String sName(int id);
  //Column col(int id);
protected:
  void create();
	int Selected(Event *ev);		
	bool OpenRecordSet(_ConnectionPtr& pConnection, _RecordsetPtr& pRstTable)	;
	bool fAcceptableDomain(_ConnectionPtr& pConnection, _RecordsetPtr& pRstTable, const String& sSelName, set<String>& setNames)	;
	AsciiTableWizard *m_atw;
	vector<String> arNames;
  SelectDBTable *pDBTable;
	String sIllegalCol; // used to prevent double error messages.This remembers the last illegal vol
};

class FormBaseWizardPage;

class _export AsciiTableWizard : public CPropertySheet
{
	friend class InputSelectPage;
	friend class FormatChoosePage;
	friend class SpecifyColumnsDetailsPage;
	friend class OutputTablePage;
	friend class MethodChoosePage;
	friend class FileViewPage;
	friend class SelectTableDomainPage;
	friend class TableColumnSelector;
	friend class EnterSqlQuery;
	friend class SelectDBTable;
	friend class DBTableSelector;
	
public:
	AsciiTableWizard::AsciiTableWizard(CWnd* wnd);
	AsciiTableWizard(CWnd* wnd, const FileName& fnAscTable, const FileName& fnObject, TableExternalFormat::InputFormat f);
	~AsciiTableWizard();

	void     SetInputTable(const FileName&);
	//void     SetTableName(const FileName&);
	FileName fnInput();
	FileName fnOutput();
	void     SetFormat(TableExternalFormat::InputFormat ifType);
	void	 SetFormat(ImportFormat impf);
	TableExternalFormat::InputFormat GetFormat();

	// build the command line string
	String   sBuildExpression();

	void LoadFromFile(String& s, int iLines = -1);
	void InitScan(String& sErr);
	void ReScan(vector<ClmInfo> &vci, bool fUseColInfo);
	void SwitchToNextPage(FormBaseWizardPage *page);
	static const ULONGLONG iMaxFullScanFileSize;
	INT_PTR DoModal();

private:
	String sBuildADOExpr();
	void SetPages();
	void CleanUp();

	FormBaseWizardPage *ppChooseFormat, *ppFileView;
	FormBaseWizardPage *ppDetermineWidth, *ppColDetails, *ppTableDomain, *ppQuery;
	SelectDBTable *ppSelectDBTable;

	FileName m_fnInput;
	//FileName m_fnOutput;

	int    m_iActiveFormat;
	bool   m_fFullScanExecuted;

	TableExternalFormat::InputFormat m_ifTable;
	TableExternalFormat::DataUsage   m_duTable;
	int    m_iSkipLines;
	int    m_iNrCols;
	int    m_iMaxCol;
	String m_sDomain;
	String m_sKeyCol;
	String m_sDataBaseInputTable; // input database\table
	String m_sDataBaseConnectionString;
	String m_sSQLQuery;

	vector<String> m_vsMarkers;  // Column positions for fixed format
	vector<ClmInfo> m_colInfo;
	bool fADO;
	bool m_fDataBase;
	bool m_fConvert;
};

#endif
