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
#include "Client\MainWindow\Catalog\MapListDoc.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\MapListCatalog.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\MapListWindow.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include <afxole.h>
#include "Client\MainWindow\ShowCursorDropSource.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ASSOCSEL.H"
#include "Client\MainWindow\ACTPAIR.H"
#include "Client\FormElements\objlist.h"

IMPLEMENT_DYNCREATE(MapListCatalog, CollectionCatalog)

const POSITION pSTARTPOS=(POSITION)-1;

/*void MoveMouse(short xInc, short yInc) 
{
	mouse_event(MOUSEEVENTF_MOVE, xInc, yInc, 0, 0); 
}*/

BEGIN_MESSAGE_MAP(MapListCatalog, CollectionCatalog)
	//{{AFX_MSG_MAP(MapListCatalog)
	ON_NOTIFY(LVN_BEGINDRAG, AFX_IDW_PANE_FIRST, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_CATVIEW_OPTIONS, OnUpdateDisable)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MWD_STARTSELBUT, ID_MWD_ENDSELBUT, OnSelButUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MapListCatalog::MapListCatalog() :
	CollectionCatalog()
{
	iSortColumn = -1;  // no sorting, otherwise moving files around will not work
	farColumns.resize(farColumns.size() + 1); 
}

MapListCatalog::~MapListCatalog()
{
}

MapListDoc* MapListCatalog::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(MapListDoc)));
	return (MapListDoc*)m_pDocument;
}

String MapListCatalog::sAddObjectTitle()
{
	return TR("Add Map to Maplist");
}

// Get the object message needed for the add objects form
String MapListCatalog::sMsgAddObject()
{
	return TR("&Raster Map");
}

void MapListCatalog::OnInitialUpdate()
{
	CollectionCatalog::OnInitialUpdate();	
	
	MapListDoc *doc = GetDocument();
	CListCtrl& lvCtrl = GetListCtrl();
	farColumns[Catalog::ctNONE] = LVColumnInfo(true, 30, Catalog::ctNONE );
	lvCtrl.InsertColumn(1, "order", LVCFMT_LEFT, farColumns[Catalog::ctNONE].iWidth);

}

bool MapListCatalog::fOK(const FileName& fn)
{
	MapListDoc *doc = GetDocument();
	if (!doc->dm().fValid()) return true; //probalbly still empty
	Map mp(fn);

	return ( doc->dm() == mp->dm() && doc->gr() == mp->gr()) ;
}

bool MapListCatalog::fIsManualSortAvail()
{
	return true;
}

String MapListCatalog::sFieldValue(int iRow, int iCol)
{
	if (iCol > 0)
	{
		iCol -= 1;
		if (0 == iCol)
		{
			if (iRow < 0 || iRow >= vfnDisplay.size())
				return "";
			MapListDoc *doc = GetDocument();
			const FileName& fn = vfn[iRow];
			int iIndex = doc->iFindIndex(fn);
			return String("%d", iIndex);
		}
	}
	else if (iCol == 0)
	{
		if (iRow >= 0 && iRow < vfnDisplay.size())
		{
			const FileName& fnDsp = vfnDisplay[iRow];
			const FileName& fn = vfn[iRow];

			if (ObjectInfo::fSystemObject(fn))
				return String("  %S", fnDsp.sFile);
			if (ObjectInfo::fUseAs(fn) )
				return String("  %S", fnDsp.sFile);
		}
	}
	return CollectionCatalog::sFieldValue(iRow, iCol);
}

void MapListCatalog::SaveCatalogView(const String& sDocKey, int iNr, bool fAsTemplate)
{
	CollectionCatalog::SaveCatalogView(sDocKey, iNr, fAsTemplate);
}

void MapListCatalog::LoadViewSettings(const String sDocKey, int iViewNr, bool fAsTemplate)
{
	CollectionCatalog::LoadViewSettings(sDocKey, iViewNr, fAsTemplate);
}

ObjectLister *MapListCatalog::olActiveLister(const String& sExt)
{
	MapListDoc *doc = GetDocument();

	FileName fnDom;
	if (doc->maplist()->iSize() > 0)
		fnDom = doc->dm()->fnObj;
	
	return new MapListerDomainAndGeoRef(doc->gr()->fnObj, doc->rcSize(), fnDom);
}

void MapListCatalog::SortColumn(int i)
{
	if ( i ==	1)
		CollectionCatalog::SortColumn(farColumns.size() - 1); // == last column
	else
		CollectionCatalog::SortColumn(i);
}

DROPEFFECT MapListCatalog::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	MapListDoc *doc = GetDocument();
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
			Map mp;
			try
			{
				if (fn.sExt == ".mpr")
					mp = Map(fn);

				if (mp.fValid())
				{
					// Added rcSize check in case of GeoRefNone
					// All domains are allowed if the maplist contains no maps
					// otherwise the domain of the maplist (== domain of maps already in maplist)
					//   must be equal to domain of the map to be dropped
					bool fDomOk = doc->maplist()->iSize() == 0 || (doc->dm() == mp->dm());
					if (fDomOk && doc->gr() == mp->gr() && doc->rcSize() == mp->rcSize())
						return DROPEFFECT_COPY;
				}
			}
			catch (const ErrorNotFound&)
			{
				// catch if fn is not a Map
			}
		}
	}
	return DROPEFFECT_NONE;
}

BOOL MapListCatalog::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
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
	CListCtrl& lvCtrl = GetListCtrl();
	int iInsertIndex = lvCtrl.HitTest(point);
	bool fInsert = fInOrder() && iInsertIndex >= 0;
	MapListDoc *doc = GetDocument();
	for( vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur)
	{
		Map mp((*cur));
		MapListDoc *doc = GetDocument();
		// All domains are allowed if the maplist contains no maps
		// otherwise the domain of the maplist (== domain of maps already in maplist)
		//   must be equal to domain of the map to be added
		bool fDomOk = doc->maplist()->iSize() == 0 || (doc->dm() == mp->dm());
		if (!fDomOk || doc->gr() != mp->gr())
			return FALSE;

		if ( fInsert)
			doc->MoveMap(iInsertIndex++, *cur);
		else
			doc->AddObject((*cur));
	}

	MainWindow * wnd = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	wnd->UpdateCatalogs(doc->obj()->fnObj);
	return TRUE;
}

bool MapListCatalog::fInOrder()
{
	MapListDoc *doc = GetDocument();
	MapListPtr *mpl = dynamic_cast<MapListPtr*>(doc->obj().pointer());
	for(int i=0; i<vfn.size(); ++i)
	{
		if ( i >= vfnDisplay.size()) 
			return false;

		FileName fnReal = vfn[i];
		FileName fnDisp = mpl->fnRealName(vfnDisplay[i]);
		if ( fnReal != fnDisp ) 
			return false;
	}
	return true;
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
void MapListCatalog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FileName fnFileSub;
	CListCtrl& lc = GetListCtrl();

	int iNr = lc.GetSelectedCount();
	if (iNr > 0) {
		CollectionCatalog::OnContextMenu(pWnd, point);
		return;
	}
				 
	FileName fn = GetDocument()->obj()->fnObj;
	String s;
	ActionPair* ap = IlwWinApp()->apl()->ap(s, fn);

	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_VIEW_LIST);
	pmadd(ID_VIEW_DETAILS);
	men.AppendMenu(MF_SEPARATOR);
	int iDummy = -1;
	const ActionList& actList = *IlwWinApp()->acl();
	AssociationSelector::AddOperMenuItems(men, &actList, ap, ".mpl", iDummy);
	men.AppendMenu(MF_SEPARATOR);
	pmadd(ID_PROP);

	LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= LVS_TYPEMASK;
	if (dwStyle == LVS_LIST)
		men.CheckMenuRadioItem(ID_VIEW_LIST,ID_VIEW_LIST,ID_VIEW_LIST,MF_BYCOMMAND);
	else if (dwStyle == LVS_REPORT)
		men.CheckMenuRadioItem(ID_VIEW_DETAILS,ID_VIEW_DETAILS,ID_VIEW_DETAILS,MF_BYCOMMAND);
  int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

void MapListCatalog::OnUpdateDisable(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void MapListCatalog::OnSelButUI(CCmdUI* pCmdUI)
{
	int id = pCmdUI->m_nID;
	id -= ID_MWD_STARTSELBUT;
	if (id == IlwisObject::iotRASMAP) 
		pCmdUI->SetCheck(TRUE);
	else {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
}

