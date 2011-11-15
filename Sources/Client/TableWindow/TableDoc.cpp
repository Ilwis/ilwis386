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
// TableDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"
#include "Client\TableWindow\TableDoc.h"
#include "Engine\Table\tblview.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Scripting\Calc.h"
#include "Engine\Scripting\Script.h"
#include "Engine\DataExchange\TableForeign.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Engine\Applications\COLVIRT.H"
//#include "Engine\Table\COLCALC.H"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Domain\DomainTime.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\TableWindow\ColumnPropForm.h"
#include "Engine\Base\File\Directory.h"
#include "Client\TableWindow\TableWindow.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Headers\Hs\Table.hs"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TableDoc

IMPLEMENT_DYNCREATE(TableDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(TableDoc, IlwisDocument)
	//{{AFX_MSG_MAP(TableDoc)
	ON_COMMAND(ID_ADDCOLUMN, OnAddColumn)
	ON_UPDATE_COMMAND_UI(ID_ADDCOLUMN, OnUpdateAddColumn)
	ON_COMMAND(ID_GRAPH, OnNewGraph)
	ON_COMMAND(ID_ROSE, OnNewRoseDiagram)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateSaveAs)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)	
	//	ON_COMMAND(ID_SORT, OnSort)
	//	ON_COMMAND(ID_UPDATEALLCOLS, OnUpdateAllColumns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



TableDoc::TableDoc()
: tvw(0)
{
}

BOOL TableDoc::OnNewDocument()
{
	if (!IlwisDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

TableDoc::~TableDoc()
{
	delete tvw;
}


/////////////////////////////////////////////////////////////////////////////
// TableDoc diagnostics

#ifdef _DEBUG
void TableDoc::AssertValid() const
{
	IlwisDocument::AssertValid();
}

void TableDoc::Dump(CDumpContext& dc) const
{
	IlwisDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// TableDoc serialization

void TableDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// TableDoc commands

const Table& TableDoc::table() const
{
	return tbl;
}

IlwisObject TableDoc::obj() const
{
	return tbl;
}

BOOL TableDoc::OnOpenDocument(LPCTSTR lpszPath, ParmList& pm, int os) 
{
	String loc = pm.sGet(0);
	String tableName = loc.sTail("\\");
	String databaseName = loc.sHead("\\").sUnQuote();
	if ( databaseName != "" && pm.fExist("database") == false && FileName(databaseName).sExt != ".ioc") {
		pm.Add(new Parm("database",databaseName));
	}
	if (tableName != "" && pm.fExist("table") == false) {
		tableName = tableName.sHead(".");
		pm.Add(new Parm("table",tableName));
	}
	FileName fn(pm.sGet("output") != "" ? pm.sGet("output") : lpszPath);

	String sC = pm.sCmd();
	if ( fn.fExist() == false && pm.fExist("collection") ) // implicit object
	{
		DataBaseCollection::CreateImplicitObject(fn, pm);
	}
	else if ( fn.fExist() == false && pm.fExist("table")) // databse table
	{
		TableForeign::CreateDataBaseTable(fn, pm);
	}	
	else if ( fn.fExist() && IlwisObject::iotObjectType(fn) == IlwisObject::iotANY)		
	{
		fn = FileName(fn, ".tbt");
		fn = FileName::fnUnique(fn);
		TableForeign::CreateDataBaseTable(fn, pm);		
	}		
	if (!IlwisDocument::OnOpenDocument(fn.sRelative().c_str()))
		return FALSE;
	Table table(fn);
	return OnOpenDocument(table);
}

BOOL TableDoc::OnOpenDocument(LPCTSTR lpszOpenString, int os) 
{
	ParmList pm(lpszOpenString);
	FileName fn = IlwisObjectPtr::fnCheckPath(pm.sGet(0));	
	if (!IlwisDocument::OnOpenDocument(fn.sRelative().c_str()))
		return FALSE;
	Table table(fn);
	return OnOpenDocument(table);
}

BOOL TableDoc::OnOpenDocument(const Table& table)
{
	tbl = table;
	if (!tbl.fValid())
		return FALSE;
	if (!tbl->fCalculated()) {
		CWaitCursor wc;
		tbl->Calc();
		if (!tbl->fCalculated())
			return FALSE;
	} 
	String s = tbl->sTypeName();
	if (s != tbl->sDescription && "" != tbl->sDescription)
		s = String("%S - %S", s, tbl->sDescription);
	SetTitle(s.c_str());
	if ( tvw )
		delete tvw;
	tvw = new TableView(tbl);
	return TRUE;
}

zIcon TableDoc::icon() const
{
	return zIcon("TableIcon");
}

int TableDoc::iCols() const
{
	if (0 == tvw)
		return 1;
	return tvw->iCols();
}

long TableDoc::iRows() const
{
	if (0 == tvw)
		return 1;
	return tvw->iNettoRecs();
}

long TableDoc::iRow(const String sRec) const
{
	if (0 == tvw)
		return 1;
	return tvw->iRow(sRec);
}

ColumnView& TableDoc::cv(int i)
{
	return tvw->cv(i);
}

const ColumnView& TableDoc::cv(int i) const
{
	return tvw->cv(i);
}

ColumnView& TableDoc::cvKey()
{
	return tvw->cvKey();
}

const ColumnView& TableDoc::cvKey() const
{
	return tvw->cvKey();
}

String TableDoc::sRow(long iRow) const
{
	return tvw->sRow(iRow);
}

String TableDoc::sValue(short iCol, long iRow, short iWidth, short iDec) const
{
	return tvw->sValue(iCol, iRow, iWidth, iDec);
}

void TableDoc::MakeUsable(short iCol) const
{
	const_cast<TableView*>(tvw)->MakeUsable(iCol);
}

void TableDoc::CheckNrRecs()
{
	tvw->CheckNrRecs();
}

static char* sComposed[]  = { ":=", 0}; // hmmm

bool TableDoc::fCmdCalc(const String sCom)
{
	CWaitCursor wc;
	String sCmd = sCom.sTrimSpaces();
	if (sCmd == "")
		return true;
	bool fSemicolon = sCmd.sRight(1) == ";";

	TextInput ip(sCmd);
	TokenizerBase tokenize(&ip);
	tokenize.SetComposed(sComposed);

	Token tok = tokenize.tokGet();
	String sCol = tok.sVal();

	Domain dom;
	ValueRange vr;
	tok = tokenize.tokGet();
	if (tok == "[") {
		Calculator calc(tbl, sCmd);
		calc.ExecColumnIndexStore();
		return true;
	}
	bool fDom = tok.sVal() == "{";
	if (fDom)
		ScriptPtr::fScanDomainValueRange(tok, tokenize, dom, vr);
	if ((tok.sVal() != "=") && (tok.sVal() != ":=")) 
		return false;
	bool fFreeze = tok.sVal() == ":=";
	String sExpres = tokenize.sCurrentLine(); // rest of line
	sExpres = sExpres.sTrimSpaces();
	bool fShowForm = true;
	bool fNewColumn = false;
	String sOldExpr;
	DomainValueRangeStruct dvrsOld;
	if (tbl.ptr()) {
		try {
			// check if column already exists
			Column col = tbl->col(sCol);
			if (col.fValid()) {
				fShowForm = col->fDependent(); //0 == col->pcs();
				sOldExpr = col->sExpression();
				dvrsOld = col->dvrs();
			}
			else
				fNewColumn = true;
		}
		catch (const ErrorObject&) {
			fNewColumn = true;
		}
	}
	if (fSemicolon)
		fShowForm = false;
	try {
		Table tblCol;
		Column col;
		if (tbl.ptr()) {
			String sCol1;
			String sTable1;
			Column::SplitTableAndColumn(tbl, sCol, sTable1, sCol1);
			try {
				if (sTable1.length() != 0) {
					tblCol = Table(sTable1);
					col = Column(tblCol, sCol, sExpres);
				}
				else {
					col = Column(tbl, sCol, sExpres);
				}

			}
			catch (const ErrorObject& err) {
				err.Show(tvw->sName());
				col = Column();
				return false;
			}
		}
		if ((fNewColumn || fShowForm) && col.fValid()) {
			if (col->fnTbl == tbl->fnObj) {  
				int iCol = iUNDEF;
				ColumnView cv;
				if (fNewColumn)
					cv = ColumnView(0, col);
				else {
					iCol = tvw->iAddCol(col);
					cv = tvw->cv(iCol);
					// make sure we have valid and the correct domain/vr
					// dom and vr (coming from command line) take precedence
					if (!dom.fValid())
						dom = cv->dm();
					if (!vr.fValid())
						vr = cv->vr();
				}
				if (col->fDependent()) {
					if (dom.fValid())
						col->SetDomainValueRangeStruct(DomainValueRangeStruct(dom, vr));
					else if (vr.fValid())
						col->SetValueRange(vr);
					String s = col->sName();
					s &= " = ";
					s &= col->sExpression();
					col->SetDescription(s);
				}  
				// tbpn->ColButtonPressed(iCol);
				int iPos = iCol == iUNDEF ? tvw->iCols() + 1: iCol + 1;
				bool f = true;
				if (fShowForm) {
					//					UpdateAllViews(0);
					ColumnPropForm frm(wndGetActiveView(), cv, &iPos, true);
					wc.Restore();
					f = frm.fOkClicked();
				}
				if (fNewColumn) {
					if (f) 
					{
						if ( fFreeze)
							col->BreakDependency();
						int iCol = tvw->iAddCol(col);
						ColumnView& cvn = tvw->cv(iCol);
						cvn.iWidth = cv.iWidth;
						cvn.iDec = cv.iDec;
						cvn.sTitle = cv.sTitle;
						SetColPos(iCol, iPos);
						UpdateAllViews(0);
						return true;
					}
					else 
						tbl->RemoveCol(col);
				}
				else
				{
					if (!f)
					{
						col = Column(tbl, sCol, sOldExpr);
						col->SetDomainValueRangeStruct(dvrsOld);
					}
				}
			}
		}
		if (col.fValid()) {
			col->Calc();
			col->CalcMinMax();  // on special request of Jelle
			if (fFreeze)
				tvw->VirtualToStore(col);
			UpdateAllViews(0);
		}
		else
			return false;
	}
	catch (ErrorObject& err) 
	{
		err.Show(tvw->sName());
		return false;
	}  
	return true;
}

void TableDoc::MoveCol(int iFrom, int iTo)
{
	if (iTo < 0)
		iTo = 0;
	if (iTo >= tvw->iCols())
		iTo = tvw->iCols() - 1;
	if (iFrom == iTo)
		return;
	tvw->MoveCol(iFrom,iTo);
	UpdateAllViews(0);
}

void TableDoc::SetColPos(int iCol, int iPos)
{
	iPos -= 1;
	if (iPos != iCol) {
		if (iPos >= tvw->iCols())
			iPos = tvw->iCols() - 1;
		MoveCol(iCol,iPos);
	}
}

void TableDoc::OnAddColumn()
{
	class NewColumnForm: public FormWithDest
	{
	public:
		NewColumnForm(CWnd* parent)
			: FormWithDest(parent, TR("Add Column")),
			vr(0,100,1)
		{
			sDomName = "value.dom";
			new FieldString(root, TR("&Column Name"), &sColName, Domain(), false);
			fdc = new FieldDomainC(root, TR("&Domain"), &sDomName, dmCLASS|dmIDENT|dmGROUP|dmVALUE|dmIMAGE|dmBOOL|dmCOORD|dmCOLOR|dmSTRING|dmPICT|dmTIME);
			fvr = new FieldValueRange(root, TR("Value &Range"), &vr, fdc);
			ftiv = new FieldTimeInterval(root,"Interval",&tiv,fdc);

			fdc->SetCallBack((NotifyProc)&NewColumnForm::DomainCallBack);
			fvr->Align(fdc, AL_UNDER);  
			StaticText* st = new StaticText(root,TR("&Description:"));
			st->psn->SetBound(0,0,0,0);
			st->SetIndependentPos();
			FieldString* fs = new FieldString(root, "", &sDescr);
			st->Align(fvr, AL_UNDER);
			fs->SetWidth(120);
			fs->SetIndependentPos();
			fvr->Hide();
			ftiv->Hide();

			SetMenHelpTopic("ilwismen\\add_column_to_table.htm");
			create();
		}
		String sColName;
		String sDomName;
		String sDescr;
		ValueRange vr;
		ILWIS::TimeInterval tiv;
	private:
		int DomainCallBack(Event *ev) {
			fdc->StoreData();
			String sDom = fdc->sName();
			Domain dm;
			if ("" == sDom)
				return 0;
			try {
				dm = Domain(sDom);
			}
			catch (const ErrorObject&) {
				return 0;
			}
			if (!dm.fValid())
				return 0;
			ftiv->Hide();
			fvr->Hide();
			if ( dm->pdtime()){
				ftiv->DomainCallBack(0);
				ftiv->setMode(dm->pdtime()->getMode());
				ftiv->Show();
			}
			else if ( dm->pdv()) {
				fvr->DomainCallBack(0);
				fvr->Show();
			}
			return 1;

		}
		FieldDomainC* fdc;
		FieldValueRange* fvr;
		FieldTimeInterval *ftiv;
	};
	if (tbl->fDataReadOnly()) 
		return;  
	try {
		NewColumnForm frm(wndGetActiveView());
		if (frm.fOkClicked()) {
			if ("" == frm.sColName)
				return;
			Domain dm(frm.sDomName);
			if (!dm.fValid())
				return;
			DomainValueRangeStruct dvrs(dm, dm->pdtime() ? ValueRange(frm.tiv) : frm.vr);
			Column col;
			try
			{
				col = Column(tbl, frm.sColName, dvrs);
			}
			catch (CMemoryException* err)
			{
				AfxMessageBox(TR("Could not create column (not enough memory)").c_str());
				err->Delete();
			}
			if (!col.fValid()) 
				return;
			col->SetDescription(frm.sDescr);  
			if (tbl.ptr())
				tbl->AddCol(col);
			col->Fill();
			int iCol = tvw->iAddCol(col);
			UpdateAllViews(0);
		}    
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}


const Domain& TableDoc::dm() const
{
	return tbl->dm();
}

bool TableDoc::fDelColumn(int iCol)
{
	Column col = tvw->cv(iCol);
	if (!col.fValid())
		return false;
	if (col.fUsedInOpenColumns()) {
		String s(TR("Column %S is used in other columns\ndeleting it is not possible").c_str(), col->sNam);
		wndGetActiveView()->MessageBox(s.sVal(), TR("Delete Column").c_str(),
			MB_OK|MB_ICONSTOP);
		return false;
	}
	if (col->fOwnedByTable()) {
		String s(TR("Column %S is owned by its table\ndeleting it is not possible").c_str(), col->sNam);
		wndGetActiveView()->MessageBox(s.sVal(), TR("Delete Column").c_str(),
			MB_OK|MB_ICONSTOP);
		return false;
	}
	if (col->fReadOnly()) {
		String s(TR("Column %S is read-only\ndeleting it is not possible").c_str(), col->sNam);
		wndGetActiveView()->MessageBox(s.sVal(), TR("Delete Column").c_str(),
			MB_OK|MB_ICONSTOP);
		return false;
	}
	String s(TR("Delete Column %S").c_str(), col->sNam);
	int iRet = wndGetActiveView()->MessageBox(s.sVal(), TR("Delete Column").c_str(),
		MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2);
	if (IDYES == iRet) {
		tvw->RemoveCol(iCol);
		return true;
	}    
	else if (IDCANCEL == iRet)
		return false;
	else
		return true;
}


void TableDoc::OnUpdateAddColumn(CCmdUI* pCmdUI)
{
	bool fEnable = !tvw->fReadOnly();
	pCmdUI->Enable(fEnable);
}

void TableDoc::OnNewGraph()
{
	IlwWinApp()->OpenDocumentAsGraph(obj()->sNameQuoted(true).c_str());
}

void TableDoc::OnNewRoseDiagram()
{
	IlwWinApp()->OpenDocumentAsRoseDiagram(obj()->sNameQuoted(true).c_str());
}

void TableDoc::OnFileSaveAs()
{
	FileName fn(obj()->fnObj);
	fn.sExt = ".tbt";
	CString sNewName(fn.sRelative().c_str());
	CDocTemplate* pTemplate = GetDocTemplate();	
	if (!AfxGetApp()->DoPromptFileName(sNewName,
		TRUE ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
		return ;     	
	IlwisDocument::DoSave(sNewName, TRUE);
}

BOOL TableDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	tbl->DoNotStore(true); // we do not want any stores at this moment
	bool *fNoUpdate = new bool(true);
	FileName fnFrom(tbl->fnObj);
	FileName fnTo(lpszPathName);
	if (fnFrom != fnTo) // save copy as
	{
		if (obj()->fUseAs() && !fCIStrEqual(fnFrom.sPath(), fnTo.sPath()))
		{
			String sMessage(TR("Cannot copy use-as table %S to another directory").c_str(), fnFrom.sRelative(false));
			throw ErrorObject(sMessage);
		}

		IlwWinApp()->Context()->SetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG, fNoUpdate);
		FileName fnTempTbl = FileName::fnUnique(fnFrom);

		ObjectCopierUI cop2(NULL, fnFrom, fnTempTbl); // save the orignal table to a temporary, changes in data may not end up the original
		// if a save as is done. So temporary rename it, will be placed back at the end
		ObjectTime tim = tbl->objtime;
		cop2.Copy(true);
		tbl->DoNotStore(false);
		tbl->Store();
		ObjectCopierUI cop(NULL, fnFrom, fnTo);
		cop.Copy(true);
		ObjectInfo::WriteElement("Ilwis", "Type", fnTo, "Table"); // transform new object to table
		OnOpenDocument(fnTo.sFullPathQuoted().c_str());
		tbl->SetUseAs(false);
		for (int i=0; i < tbl->iCols(); ++i )
		{
			if ( tbl->col(i).fValid() && tbl->col(i)->dm() != Domain("none") )
				tbl->col(i)->Store();
		}

		tbl->Store();
		ObjectCopierUI cop3(NULL, fnTempTbl, fnFrom);
		cop3.Copy(true, true);
		ObjectInfo::WriteElement("Ilwis", "Time", fnFrom, tim);
		Table tb(fnTempTbl);
		tb->fErase = true;
		bool *fNoUpdate2 = new bool(false);
		IlwWinApp()->Context()->SetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG, fNoUpdate2);		
		ObjectInfo::RemoveSection(fnTo, "DataBaseCollection");
		ObjectInfo::RemoveSection(fnTo, "Collection");
		FileName* pfn = new FileName(fnTo);		
		AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)pfn);		
		return TRUE;
	}	
	else
		throw ErrorObject(TR("Filename must be different from original"));
	return FALSE;
}

void TableDoc::OnUpdateSaveAs(CCmdUI* pCmdUI) 
{
	if ( tbl->iRef <= 3)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
