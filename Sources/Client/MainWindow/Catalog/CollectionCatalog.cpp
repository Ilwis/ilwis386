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
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\MainWindow\Catalog\ObjectCollectionDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include <afxole.h>
#include "Client\MainWindow\ShowCursorDropSource.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\flddat.h"

IMPLEMENT_DYNCREATE(CollectionCatalog, Catalog)

const POSITION pSTARTPOS=(POSITION)-1;

BEGIN_MESSAGE_MAP(CollectionCatalog, Catalog)
	//{{AFX_MSG_MAP(CollectionCatalog)
	ON_COMMAND(ID_ADD_OBJECT, OnAddObject)
	ON_UPDATE_COMMAND_UI(ID_ADD_OBJECT, OnUpdateAddObject)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_CAT_DEL2, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_CAT_DEL2, OnUpdateDelete)	
	ON_COMMAND(ID_CAT_REMOVE, DeleteItems)
	ON_UPDATE_COMMAND_UI(ID_CAT_REMOVE, OnUpdateDelete)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CollectionCatalog::CollectionCatalog() :
	Catalog()
{
}

CollectionCatalog::~CollectionCatalog()
{}

void CollectionCatalog::OnInitialUpdate()
{
	Catalog::OnInitialUpdate();
	ShowContainerContents(true);
	FillCatalog();
}

// Get the title needed for the add objects form
String CollectionCatalog::sAddObjectTitle()
{
	return TR("Add Object to Collection");
}

// Get the object message needed for the add objects form
String CollectionCatalog::sMsgAddObject()
{
	return TR("Objects");
}

ObjectLister *CollectionCatalog::olActiveLister(const String& sExt)
{
	return new ObjectExtensionLister(0, sExt);
}

void CollectionCatalog::OnAddObject()
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
	CatalogDocument *doc = GetDocument();
	String sExt = doc->sAllowedTypes();

	bool *fDoNotShowError = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fOldVal = *fDoNotShowError;
	*fDoNotShowError = true;
	AddForm frm(this, sAddObjectTitle(), sMsgAddObject(), &sWho, olActiveLister(sExt));
	*fDoNotShowError = fOldVal;

	if ( frm.fOkClicked())
	{
		doc->AddObject(FileName(sWho));

		// It is not enough to only refresh the collection catalog, because the catalog window(s)
		// containing this object collection also need(s) updating; so let mainwindow do it
		MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
		wnd->UpdateCatalogs(doc->obj()->fnObj);
	}
}

void CollectionCatalog::OnCatDel()
{
	POSITION pos = pSTARTPOS;
	FileName fn;
	bool fOK = false;
	CatalogDocument *doc = GetDocument();
	while ((fn = GetNextSelectedFile(pos)) != FileName())
	{
		WritePrivateProfileString("Collection", NULL, NULL, fn.sFullPathQuoted().c_str());
		doc->RemoveObject(fn);
	}

	// It is not enough to only refresh the collection catalog, because the catalog window(s)
	// containing this object collection also need(s) updating; so let mainwindow do it
	MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	wnd->UpdateCatalogs(doc->obj()->fnObj);
}

void CollectionCatalog::OnCopy()
{
	Catalog::OnCopy();
}


class NewNameQuestionForm : public FormWithDest
{
	public:
		NewNameQuestionForm(CWnd *par, const String& sTitle, const FileName& fn, const FileName& fnExist, FileName* _fnObjOld)
			: FormWithDest(par, sTitle, fbsSHOWALWAYS|fbsMODAL| fbsBUTTONSUNDER|fbsNOBEVEL),
			fnObjOld(_fnObjOld)
	{
		IlwisObject obj = IlwisObject::obj(fn);
		String sType = "file";
		if ( obj.fValid() )
			sType = obj->sType();
			
		sNewName = fnObjOld->sFile;
		String sQuestion = String(TR("The %S %S already exists.").c_str(), sType, fnExist.sFileExt());
		new StaticText(root, sQuestion);
		fsN = new FieldString(root, TR("New name") , &sNewName);
		fsN->SetIndependentPos();
		fsN->SetWidth(100);

		create();
	}

	FormEntry *CheckData()
	{
		fsN->StoreData();
		FileName fnNew(sNewName + fnObjOld->sExt);
		if ( fnNew.fExist() )
		{
			FileName fnNew(String("Copy of %S", sNewName), fnObjOld->sExt);
			fsN->SetVal(fnNew.sFile);
			return fsN;
		}			
		return NULL;
	}
	
	int exec()
	{
		FormWithDest::exec();
		FileName fnNew(sNewName + fnObjOld->sExt, *fnObjOld); // make sure the correct path is used
		*fnObjOld = fnNew;
		return 1;
	}

	private:
		FileName *fnObjOld;
		String sNewName;
		FieldString *fsN;
};
	
void CollectionCatalog::OnPaste()
{
	vector<FileName> arFiles;
	Catalog::GetFromClipboard(arFiles);
	ObjectCollectionDoc *doc = dynamic_cast<ObjectCollectionDoc *>(GetDocument());

	for( vector<FileName>::iterator cur = arFiles.begin(); cur != arFiles.end(); ++cur)
	{
		if ( fOK(*cur))
			AddFile(*cur, doc);
	}

	MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	wnd->UpdateCatalogs(doc->obj()->fnObj);
}

BOOL CollectionCatalog::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
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
	ObjectCollectionDoc *doc = dynamic_cast<ObjectCollectionDoc *>(GetDocument());
	for( vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
	{
		if (fOK(*cur))
			AddFile(*cur, doc);
	}
	doc->UpdateAllViews(0);
	return TRUE;
}

void CollectionCatalog::AddFile(const FileName &fn, ObjectCollectionDoc *doc)
{
	if ( doc->fAlreadyInCollection(fn ))
	{
		FileName fnNew(String("Copy of %S", fn.sFile), fn.sExt);			
		NewNameQuestionForm frm(this, TR("Copy files"), fn, fn, &fnNew);	
		if ( frm.fOkClicked())
		{
			ObjectCopierUI::CopyFiles(fn, fnNew, true);
			doc->AddObject(fnNew);
		}				
	}	
	else
		doc->AddObject(fn);		

}

DROPEFFECT CollectionCatalog::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	CatalogDocument *doc = GetDocument();
	if (!doc->fEditable())
		return DROPEFFECT_NONE;
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	bool fOk = false;
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    FileName fn(sFileName);
			return DROPEFFECT_COPY;
		}
	}
	return DROPEFFECT_NONE;
}


bool CollectionCatalog::fOK(const FileName& fn)
{
	return true;
}

void CollectionCatalog::DeleteItems()
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	if ( iNr <= 0 ) return;


	POSITION pos = lc.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	int id = lc.GetNextSelectedItem(pos);

	CatalogDocument *doc = GetDocument();					
	for (int i = 0; i < iNr && id >=0; ++i) 
	{
		FileName fnSel =  vfnDisplay[viIndex[id]];
		doc->RemoveObject(fnSel);
		id = lc.GetNextSelectedItem(pos);
	}

	// It is not enough to only refresh the collection catalog, because the catalog window(s)
	// containing this object collection also need(s) updating; so let mainwindow do it
	MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	wnd->UpdateCatalogs(doc->obj()->fnObj);
}

void CollectionCatalog::OnDelete()
{
	Catalog::DeleteItems();

	// It is not enough to only refresh the collection catalog, because the catalog window(s)
	// containing this object collection also need(s) updating; so let mainwindow do it
	MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	wnd->UpdateCatalogs(GetDocument()->obj()->fnObj);
}

void CollectionCatalog::OnUpdateDelete(CCmdUI* pCmdUI)
{
	CListCtrl& lc = GetListCtrl();
	int iNr = lc.GetSelectedCount();
	CatalogDocument *doc = GetDocument();
	if (iNr > 0 && doc->fEditable())
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);	
}

void CollectionCatalog::OnUpdateAddObject(CCmdUI* pCmdUI)
{
	CatalogDocument *doc = GetDocument();
	pCmdUI->Enable(doc->fEditable());
}
