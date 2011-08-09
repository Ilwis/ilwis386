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
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Client\MainWindow\Catalog\ForeignCatalog.h"
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

IMPLEMENT_DYNCREATE(ForeignCatalog, CollectionCatalog)

const POSITION pSTARTPOS=(POSITION)-1;

BEGIN_MESSAGE_MAP(ForeignCatalog, CollectionCatalog)
	//{{AFX_MSG_MAP(ForeignCatalog)
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



ForeignCatalog::ForeignCatalog() :
	CollectionCatalog()
{
}

ForeignCatalog::~ForeignCatalog()
{}

void ForeignCatalog::OnInitialUpdate()
{
	Catalog::OnInitialUpdate();
	ShowContainerContents(true);
	FillCatalog();
}

// Get the title needed for the add objects form
String ForeignCatalog::sAddObjectTitle()
{
	return TR("Add Object to Collection");
}

// Get the object message needed for the add objects form
String ForeignCatalog::sMsgAddObject()
{
	return TR("Objects");
}

ObjectLister *ForeignCatalog::olActiveLister(const String& sExt)
{
	return new ObjectExtensionLister(0, sExt);
}

void ForeignCatalog::OnAddObject()
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

void ForeignCatalog::OnCopy()
{
	//Catalog::OnCopy();
}

void ForeignCatalog::OnPaste()
{
}

void ForeignCatalog::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void ForeignCatalog::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void ForeignCatalog::OnUpdateAddObject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

void ForeignCatalog::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}

BOOL ForeignCatalog::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	return FALSE;
}

DROPEFFECT ForeignCatalog::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return DROPEFFECT_NONE;
}


bool ForeignCatalog::fOK(const FileName& fn)
{
	if ( IlwisObject::iotObjectType(fn) != IlwisObject::iotTABLE)
		return false;
	return true;
}

void ForeignCatalog::DeleteItems()
{
	CollectionCatalog::DeleteItems();
}

void ForeignCatalog::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NMLISTVIEW* nmlv = (NMLISTVIEW*) pNMHDR;
	int iItem = nmlv->iItem;
	if (iItem == -1)
		return;
	iItem = viIndex[iItem];
	FileName& fn = vfn[iItem];
	String s;
	ForeignCollectionDoc *doc = GetDocument();
	String sDBName = doc->obj()->fnObj.sRelative();
	String sCmd;

	sCmd = String("open %S\\%S", sDBName.sQuote(), fn.sRelativeQuoted());
	
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

ForeignCollectionDoc* ForeignCatalog::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ForeignCollectionDoc)));
	return (ForeignCollectionDoc*)m_pDocument;
}

void ForeignCatalog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FileName fn, fnFileSub;
	int iNr, iSub;
	bool fReport;
	CalcMenuProps(pWnd, point, fn, fnFileSub, iNr, iSub, fReport);

	try
	{
		int iCurCols;
		ObjectInfo::ReadElement("Table", "Columns", fn, iCurCols);	// see if it exists
		if ( !fn.fExist() || iCurCols == 0)
			CreateTable(fn);

		IlwWinApp()->ShowPopupMenu(this, point, fn, &assSel);	
	}
	catch (...)
	{
		// silent catch: CreateTable already displays a message
	}
}

void ForeignCatalog::OnShow()
{
	CListCtrl& lc = GetListCtrl();
	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int id = lc.GetNextSelectedItem(pos);	
	int iItem = viIndex[id];
	FileName fn = vfn[iItem];

	ForeignCollectionDoc *doc = GetDocument();
	String container = doc->sContainer();

	String sCmd;
	if ( sQuery != "")
		sCmd = String("open %S\\%S -query=%S", container, fn.sFile, sQuery.sQuote());
	else
		sCmd = String("open %S\\%S", container, fn.sFile);

	if ( sViewName != "")
	{
		FileName fnView(sViewName, ".tbt");
		sCmd += String(" -output=%S", fnView.sRelative());
		doc->AddObject(fnView);
	}		
	
	char* str = sCmd.sVal();
	IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);			
}

void ForeignCatalog::OnOpenQuery()
{
	class QueryForm: public FormWithDest
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
	}	while ((fn = GetNextSelectedFile(pos)) != FileName());
}

// a databsecollection contains initially only names, no real ILWIS tables exists. This creates them
void ForeignCatalog::CreateTable(const FileName& fn)
{
	Table tbl;	
	ParmList pm;
	ForeignCollectionDoc *doc = GetDocument();
	pm.Add(new Parm(0, String("%S\\%S", doc->sContainer(), fn.sFile)));	
	pm.Add(new Parm("collection", doc->obj()->fnObj.sRelative()));
	ForeignCollection::CreateImplicitObject(fn, pm);		
}			


