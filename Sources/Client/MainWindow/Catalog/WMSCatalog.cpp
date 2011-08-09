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
#include "Client\Headers\formelementspch.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Base\Framewin.h"
#include "Client\Editors\Utils\OwnerHeaderCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Client\MainWindow\Catalog\WMSCatalog.h"
#include "Client\MainWindow\Catalog\ForeignCollectionDoc.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\Base\Menu.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include <afxole.h>
#include "Client\MainWindow\ShowCursorDropSource.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\flddat.h"

IMPLEMENT_DYNCREATE(WMSCatalog, CollectionCatalog)

const POSITION pSTARTPOS=(POSITION)-1;

BEGIN_MESSAGE_MAP(WMSCatalog, CollectionCatalog)
	//{{AFX_MSG_MAP(WMSCatalog)
	ON_COMMAND(ID_ADD_OBJECT, OnAddObject)
//	ON_UPDATE_COMMAND_UI(ID_ADD_OBJECT, OnUpdateAddObject)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)	
	ON_WM_CONTEXTMENU()	
	ON_COMMAND(ID_OPEN_WITH_QUERY, OnOpenQuery)
	ON_COMMAND(ID_CAT_SHOW,OnShow)
	ON_COMMAND(ID_CAT_PROP,OnProp)	
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)	
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)	
	ON_UPDATE_COMMAND_UI(ID_ADD_OBJECT, OnUpdateAddObject)	
	ON_UPDATE_COMMAND_UI(ID_CAT_DEL2, OnUpdateDelete)	
	ON_UPDATE_COMMAND_UI(ID_CAT_REMOVE, OnUpdateDelete)	
	ON_WM_MDIACTIVATE()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



WMSCatalog::WMSCatalog() :
	CollectionCatalog()
{
}

WMSCatalog::~WMSCatalog()
{}

void WMSCatalog::OnInitialUpdate()
{
	Catalog::OnInitialUpdate();
	ShowContainerContents(true);
	FillCatalog();
}

// Get the title needed for the add objects form
String WMSCatalog::sAddObjectTitle()
{
	return TR("Add Object to Collection");
}

// Get the object message needed for the add objects form
String WMSCatalog::sMsgAddObject()
{
	return TR("Objects");
}

ObjectLister *WMSCatalog::olActiveLister(const String& sExt)
{
	return new ObjectExtensionLister(0, sExt);
}

void WMSCatalog::OnAddObject()
{
	class AddForm: public FormWithDest
	{
	public:
		AddForm(CWnd* wPar, const String& sTitle, const String& sMsg, String *sName, ObjectLister *ol)
			: FormWithDest(wPar, sTitle)
		{
			fd = new FieldDataType(root, sMsg, sName, ol, true);
			create();
		}
		FormEntry* feDefaultFocus()
		{
			return fd;
		}
	private:
		FieldDataType* fd;
	};
	
	String sWho;
	ObjectCollectionDoc *doc = GetDocument();
	String sExt = doc->sAllowedTypes();
	AddForm frm(this, sAddObjectTitle(), sMsgAddObject(), &sWho, olActiveLister(sExt));
	if ( frm.fOkClicked())
	{
		doc->AddObject(FileName(sWho));
		FillCatalog();
	}
}

void WMSCatalog::OnCopy()
{
	//Catalog::OnCopy();
}

void WMSCatalog::OnPaste()
{
}

void WMSCatalog::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void WMSCatalog::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void WMSCatalog::OnUpdateAddObject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void WMSCatalog::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

BOOL WMSCatalog::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
/*	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;

	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	vector<FileName> afn;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    afn.push_back(FileName(sFileName));
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	ObjectCollectionDoc *doc = GetDocument();
	for( vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
	{
		if (fOK(*cur))
			doc->AddObject((*cur));
	}
	doc->UpdateAllViews(0);
	return TRUE;*/
	return FALSE;
}

DROPEFFECT WMSCatalog::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
/*	ObjectCollectionDoc *doc = GetDocument();
	if (!doc->fEditable())
		return DROPEFFECT_NONE;
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	bool fOk = false;
	if (hDrop) 
	{
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i)
		{
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
			FileName fn(sFileName);
			if ( IlwisObject::iotObjectType(fn) != IlwisObject::iotTABLE)
				return DROPEFFECT_NONE;
			return DROPEFFECT_COPY;
		}
	}*/
	return DROPEFFECT_NONE;
}


bool WMSCatalog::fOK(const FileName& fn)
{
	if ( IlwisObject::iotObjectType(fn) != IlwisObject::iotTABLE)
		return false;
	return true;
}

void WMSCatalog::DeleteItems()
{
	CollectionCatalog::DeleteItems();
}

void WMSCatalog::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem = nmlv->iItem;
	if (iItem == -1)
		return;
	iItem = viIndex[iItem];
	FileName& fn = vfn[iItem];
	String s;
	ObjectCollectionDoc *doc = GetDocument();
	//String sDBName = doc->obj()->fnObj.sRelative();
	String sCmd;

	if ( fn.sExt == ".mpr")
		sCmd = String("open %S\\%S -method=WMS", doc->obj()->fnObj.sRelativeQuoted(), fn.sRelativeQuoted());
	else
		sCmd = String("open %S", fn.sRelativeQuoted());
	//
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

ObjectCollectionDoc* WMSCatalog::GetDocument()
{
	//ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ForeignCollectionDoc)));
	return (ObjectCollectionDoc*)m_pDocument;
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
#define addsub(ID) menSub.AppendMenu(MF_STRING, ID, sMen(ID)); 

void WMSCatalog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FileName fn, fnFileSub;
	int iNr, iSub;
	bool fReport;
	CalcMenuProps(pWnd, point, fn, fnFileSub, iNr, iSub, fReport);

	CListCtrl& lc = GetListCtrl();
	if (iNr > 0) 
	{
		POSITION pos = lc.GetFirstSelectedItemPosition();
		FileName fn;
		bool fToPixInfo = false;
		String file = "";
		String rest = "";
		int count =0;
		while (pos) 
		{
			int id = lc.GetNextSelectedItem(pos);
			if (id >= 0) 
			{
				id = viIndex[id];
				fn = vfn[id];
				if ( fn.sExt == ".mpr") {
					if ( count == 0)
						file = fn.sRelativeQuoted();
					else
						rest += fn.sRelativeQuoted() + "+";
					++count;
				}
			}
		}
		rest = rest.sLeft(rest.length() -1);
		String sCmd="";
		CMenu men;
		men.CreatePopupMenu();
		pmadd(ID_CAT_SHOW);
		men.AppendMenu(MF_SEPARATOR);
		pmadd(ID_EDIT_COPY);
		pmadd(ID_EDIT_PASTE);
		pmadd(ID_CAT_DEL );
		switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
		{
			case ID_CAT_SHOW:
				sCmd = String("open %S\\%S -method=WMS", GetDocument()->obj()->fnObj.sRelativeQuoted(), file);
				if ( rest != "")
					sCmd += " -extras=" + rest;
				break;
		}
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
		return;
	}
}

void WMSCatalog::OnShow()
{
	CListCtrl& lc = GetListCtrl();
	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int id = lc.GetNextSelectedItem(pos);	
	int iItem = viIndex[id];
	FileName fn = vfn[iItem];

	ObjectCollectionDoc *doc = GetDocument();
/*	String sDBName = doc->sForeign();

	String sCmd;
	if ( sQuery != "")
		sCmd = String("open %S\\%S -query=%S", sDBName, fn.sFile, sQuery.sQuote());
	else
		sCmd = String("open %S\\%S", sDBName, fn.sFile);

	if ( sViewName != "")
	{
		FileName fnView(sViewName, ".tbt");
		sCmd += String(" -output=%S", fnView.sRelative());
		doc->AddObject(fnView);
	}		
	
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	*/		
}

void WMSCatalog::OnOpenQuery()
{
/*	class QueryForm: public FormWithDest
	{
	public:	
		QueryForm(CWnd *par, String& sQ, String& sNewName) :
				FormWithDest(par, TR("Enter SQL Query"))
		{
			FieldStringMulti *fs = new FieldStringMulti(root, &sQ);
			fs->SetWidth(200);
			fs->SetHeight(75);		
			FieldString *fn = new FieldString(root, "View name", &sNewName);
			fn->SetIndependentPos();
			create();
		}
				
	};

	POSITION pos = pSTARTPOS;
	FileName fn = GetNextSelectedFile(pos);
	String sFiles;
	fn = GetNextSelectedFile(pos);
	bool fSetFile = fn != FileName(); 
	
	do
	{
		sQuery = String("Select * from %S", fSetFile ? fn.sFile : "?");
		sViewName = String("%Sview", fSetFile ? fn.sFile : "Table");
		QueryForm frm(this, sQuery, sViewName);
		if ( frm.fOkClicked() == false )
			return;
		OnShow();
		fn = GetNextSelectedFile(pos)	;	
	}	while ((fn = GetNextSelectedFile(pos)) != FileName());*/
}

// a databsecollection contains initially only names, no real ILWIS tables exists. This creates them
void WMSCatalog::CreateTable(const FileName& fn)
{
/*	Table tbl;	
	ParmList pm;
	ForeignCollectionDoc *doc = GetDocument();
	pm.Add(new Parm(0, String("%S\\%S", doc->sForeign(), fn.sFile)));	
	pm.Add(new Parm("collection", doc->obj()->fnObj.sRelative()));
	ForeignCollection::CreateImplicitObject(fn, pm);*/		
}			


