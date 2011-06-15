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

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldonesl.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\MainWindow\Catalog\CatalogPropertySheet.h"

CatalogPropertySheet::CatalogPropertySheet(Catalog *cat) :
	CPropertySheet(SMSTitleCustomizeCatalog.scVal()),
  catalog(cat)
{}

BOOL CatalogPropertySheet::OnInitDialog()
{
	return CPropertySheet::OnInitDialog();
}

//------------------------------------------------------------

class CatItemSelector: public BaseSelector
{
public:
  CatItemSelector(FormEntry* fe, vector<NameExt>& arr)
  : BaseSelector(fe), ane(arr)
  {
    psn->iMinWidth = 200;
    psn->iMinHeight = psn->iHeight = 200;
    style = LBS_EXTENDEDSEL | WS_VSCROLL;
  }
  String sName(int);
  void StoreData();
private:
  void create();
  vector<NameExt>& ane;
};

void CatItemSelector::create()
{
  BaseSelector::create();
  int iMax = ane.size();
  for (int i = 0; i < iMax; ++i) 
	{
		String sExt = (ane[i].sExt != "drive" && ane[i].sExt != "directory") ? ane[i].sExt : String(".%S",ane[i].sExt);
    String s("%S%S", ane[i].sName, sExt);
    int ind = lb->AddString(s.scVal());
		lb->SetItemData(ind, (LPARAM)&ane[i]);
		lb->SetSel(ind, ane[i].fShow);
  }
	lb->SetCaretIndex(0,0);
}

void CatItemSelector::StoreData()
{
  int iNr = lb->GetCount();
	map<IlwisObject::iotIlwisObjectType, String> Expr;
	

  int iCount=0;
	for (int id = 0; id < iNr; ++id) 
	{
		ane[id].fShow = lb->GetSel(id) != 0 ? true : false;
	}
}


String CatItemSelector::sName(int id)
{
  NameExt* ne = (NameExt*)lb->GetItemData(id);
	String sExt = (ne->sExt != "drive" && ne->sExt != "directory") ? ne->sExt : String(".%S",ne->sExt);
  String s("%S%S", ne->sName, sExt);
  return s;
}

SimpleQueryPropPage::SimpleQueryPropPage(CatalogPropertySheet& sh, vector<NameExt>& ane) :
	FormBasePropertyPage(SMSUiSimpleQuery.scVal()),
  sheet(sh)
{
	new CatItemSelector(root, ane);

	Catalog* cat = sheet.catalog;
	fAll = cat->fIncludeSystemObjects();
	new CheckBox(root, SMSUiInclSysObj, &fAll);  

	fHideCollCont = !cat->fShowContainerContents();
	CatalogDocument* doc = cat->GetDocument();
	FileName fn(doc->sName());
	if (!(IlwisObject::iotObjectType(fn) == IlwisObject::iotOBJECTCOLLECTION))
		new CheckBox(root, SMSUiIncludeCollectionContents, &fHideCollCont);

	new FieldBlank(root, 0.2);
	sExtensions = cat->sGetExternalFileExtensions();
	FieldString *fs = new FieldString(root, SMSUiExtFileExtensions, &sExtensions);
	fs->SetIndependentPos();

  SetMenHelpTopic("ilwismen\\customize_catalog_selection.htm");
  create();	
}

int SimpleQueryPropPage::exec()
{
	FormBasePropertyPage::exec();

	Catalog* cat = sheet.catalog;
	cat->IncludeSystemObjects(fAll);
	cat->ShowContainerContents(!fHideCollCont);
	cat->SetExternalFileExtensions(sExtensions);
	FormBasePropertyPage *page = dynamic_cast<FormBasePropertyPage *>(sheet.GetActivePage());
	if (sheet.GetActivePage() == this)
	{
		IlwWinApp()->Execute("dir");
		cat->GetDocument()->UpdateAllViews(0);
	}
	else
		page->OnOK();

	return 1;
}

FormEntry* SimpleQueryPropPage::CheckData()
{
	return 0;
}
//-------------------------------------------------------------
class VisibleColSelector: public BaseSelector
{
public:
  VisibleColSelector(FormEntry* fe, Catalog *c, vector<bool> &fVis)
  : BaseSelector(fe), 
	  cat(c),
		fVisibleColumns(fVis)
  {
    psn->iMinWidth = 200;
    psn->iMinHeight = psn->iHeight = 200;
    style = LBS_EXTENDEDSEL | WS_VSCROLL;
  }
  String sName(int);
  void StoreData();
private:
	vector<bool> &fVisibleColumns;
  void create();
	Catalog *cat;
};

void VisibleColSelector::create()
{
  BaseSelector::create();

	// skip 0 (ctNAME)
  for (int i = 1; i < Catalog::ctALL; ++i) 
	{
    String s("%S", cat->sGetColumnName(i));
    if ("" == s)
      break;
    int ind = lb->AddString(s.scVal());
//		lb->SetItemData(ind, (LPARAM)&ane[i]);
		lb->SetSel(ind, fVisibleColumns[i]);
  }
	lb->SetCaretIndex(0,0);
}

void VisibleColSelector::StoreData()
{
  int iNr = lb->GetCount();
	
	fVisibleColumns[0] = true; // ctNAME
	for (int id = 0; id < iNr; ++id) 
	{
		fVisibleColumns[1+id] = lb->GetSel(id) != 0 ? true : false;
	}
}

String VisibleColSelector::sName(int id)
{
	return String("%S.clm", cat->sGetColumnName(1+id));
}


VisibleColumnsPropPage::VisibleColumnsPropPage(Catalog *c) :
	FormBasePropertyPage(SMSUiVisibleColumns.scVal()),
	cat(c),
	fShowGrid(c->fGrid())
{
	fVisibleColumns.resize(Catalog::ctALL);
	for ( int  i = 0; i < Catalog::ctALL; ++i)
		fVisibleColumns[i] = cat->fColVisible(i);

	new StaticText(root, SMSUiColumns);
	new VisibleColSelector(root, cat, fVisibleColumns);

	FieldBlank *fb = new FieldBlank(root);
	new CheckBox(root, SMSGrid, &fShowGrid);
	PushButton *pb1 = new PushButton(root, SMSUiSaveTemplate, (NotifyProc)&VisibleColumnsPropPage::SetDefault);
	PushButton *pb2 = new PushButton(root, SMSUiRestoreDefault, (NotifyProc)&VisibleColumnsPropPage::RestoreDefault);
	pb2->Align(pb1, AL_AFTER);
  SetMenHelpTopic("ilwismen\\customize_catalog_details_view.htm");
	create();
}

int VisibleColumnsPropPage::SetDefault(Event *)
{
	String sDocKey("MainWindow\\CatalogTemplate");
	cat->SaveCatalogView(sDocKey, -1, true);
	return 1;
}

int VisibleColumnsPropPage::RestoreDefault(Event *)
{
	String sDocKey("MainWindow\\CatalogTemplate");
	IlwisSettings settings(sDocKey);
	settings.DeleteKey();
	return 1;
}

int VisibleColumnsPropPage::exec()
{
	FormBasePropertyPage::exec();

	cat->SetVisibleColumns(fVisibleColumns);
	cat->SetGridOnOf(fShowGrid);
	cat->FillCatalog(); // needed because new columns can be made visible
	return 1;
}

FormEntry* VisibleColumnsPropPage::CheckData()
{
	return 0;
}

//-------------------------------------------------------------
QueryPropPage::QueryPropPage(CatalogPropertySheet& sh, map<String, String>& mpQ) :
	FormBasePropertyPage(SMSUiQuery.scVal()),
  mpQueries(mpQ),
	sheet(sh)
{
	StaticText *st = new StaticText(root, SMSUiQueries);
	st->SetCallBack((NotifyProc)&QueryPropPage::FillAll);
  fld = new FieldOneSelectTextOnly(root, &sChoice, true);
	fld->SetCallBack((NotifyProc)&QueryPropPage::ChangeQuery);
	fld->SetWidth(120);
	fld->Align(st, AL_AFTER);

	new FieldBlank(root);
	DWORD style = WS_GROUP|WS_TABSTOP|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL|WS_VSCROLL|WS_BORDER|ES_AUTOHSCROLL;   
	fsExpr = new FieldStringMulti(root, &sExpr, style);
	fsExpr->SetIndependentPos();
	fsExpr->SetWidth(180);
	fsExpr->SetHeight(150);
	pbSave  = new PushButton(root, SMSUiReplaceQuery, (NotifyProc)&QueryPropPage::Replace);
	pbSaveAs =  new PushButton(root, SMSUiAddQuery, (NotifyProc)&QueryPropPage::Add);
	pbDelete =  new PushButton(root, SMSUiDeleteQuery, (NotifyProc)&QueryPropPage::Delete);
	pbSave->SetIndependentPos();
	pbSave->Align(fsExpr, AL_UNDER);
	pbSaveAs->SetIndependentPos();
	pbSaveAs->Align(pbSave, AL_AFTER);
	pbDelete->Align(pbSaveAs, AL_AFTER);
  SetMenHelpTopic("ilwismen\\customize_catalog_query.htm");
	create();	
}

int QueryPropPage::ChangeQuery(Event*)
{ 
	int iIndex = fld->ose->GetCurSel();
	if ( iIndex <= 0 ) 
	{
		// this is a hack. In the release version the exec function will not be executed unless something is
		// in the multilin edit. Well a space is something
		fsExpr->SetVal(" ");
		return 0;
	}		
	bool fDefault = fld->ose->GetItemData(iIndex) != 0;
	String sPrefix;
	if ( fDefault )
	{
			pbSave->disable();
			pbDelete->disable();
			sPrefix = "DEF:";
	}
	else
	{
			pbSave->enable();
			pbDelete->enable();
	}

	String sExpr = fld->sGetText();
	sChoice = sPrefix + sExpr;
	String sExpr2 = mpQueries[sChoice];
	fsExpr->SetVal(sExpr2);

	return 1; 
}

int QueryPropPage::Replace(Event*)
{
	map<String, String>::iterator where = mpQueries.find(sChoice);
	if ( where == mpQueries.end()) 
	{
		return Add(0)	;
	}		

	mpQueries[sChoice] = fsExpr->sVal();
	return 1; 
}

int QueryPropPage::Add(Event*)
{ 

	String sName;
	NameForm frm(this, sName);
	if ( frm.fOkClicked())
	{
		fld->ose->ResetContent();
		if ( sName != "")
			mpQueries[sName] = fsExpr->sVal();
		FillAll(0);
	}
	return 1; 
}

int QueryPropPage::FillAll(Event *)
{
	if ( fld->ose->GetCount() > 0 ) return 0;
	int i=0;
	for ( map<String, String>::iterator cur = mpQueries.begin(); cur != mpQueries.end(); ++cur)
	{
		String sQuery = (*cur).first;
		if ( sQuery.substr(0,4) != "DEF:" )
		{
			fld->AddString( sQuery );
			fld->ose->SetItemData(i, 0);
		}
		else
		{
			fld->AddString( sQuery.sTail(":"));
			fld->ose->SetItemData(i, 1);
		}
		++i;
	}
	return 1;
}

int QueryPropPage::exec()
{
	FormBasePropertyPage::exec();
	IlwisSettings settings(String("%S\\Queries", MainWindow::sBaseRegKey()));
	int i=0;
	for ( map<String, String>::iterator cur = mpQueries.begin(); cur != mpQueries.end(); ++cur)
	{
		String sQuery = (*cur).first;
		if ( sQuery.substr(0,4) != "DEF:" )
		{
			settings.SetValue(String("query%d", i++), String("%S|%S", (*cur).first, (*cur).second));
		}

	}
	settings.SetValue("NrOfQueries", i);
	
	String sQuery = String("DEF:%S", sChoice);
	map<String, String>::iterator where = mpQueries.find(sQuery);
	if ( where != mpQueries.end() )
		sheet.catalog->SetCurrentQuery( sQuery);
	else
	sheet.catalog->SetCurrentQuery( sChoice);

	if ( sheet.GetActivePage() == this)
	{
		IlwWinApp()->Execute(String("dir %S", sExpr));
	}

	return 1;
}
int QueryPropPage::Delete(Event*)
{
	if ( fld->ose->GetCurSel() == -1 ) return 0;
	String sExpr = fld->sGetText();
	map<String, String>::iterator where = mpQueries.find(sExpr);
	if ( where != mpQueries.end() )
		mpQueries.erase(where);

	return 1;
}

FormEntry* QueryPropPage::CheckData()
{
	return 0;
}

BOOL QueryPropPage::OnApply()
{
	return TRUE;
}

