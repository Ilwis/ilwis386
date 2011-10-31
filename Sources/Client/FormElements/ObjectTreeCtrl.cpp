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
// ObjectTreeCtrl.cpp: implementation of the ObjectTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\nameedit.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\ObjectTreeCtrl.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Table\tblinfo.h"
#include "Client\ilwis.h"

// copied from afximpl.h
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)
BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#define AFX_WNDCOMMCTL_TREEVIEW_REG     0x00080


BEGIN_MESSAGE_MAP(ObjectTreeCtrl, CTreeCtrl)
  ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
  

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjectTreeCtrl::ObjectTreeCtrl(NameEdit* nameedit)
: ne(nameedit), fFilled(false)
{
	CRect rect(0,0,0,0);

// calling CTreeCtrl::Create() may not be a Popup Window
// partly copied from	CTreeCtrl::Create()
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_TREEVIEW_REG));
	
	DWORD dwStyle = TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|WS_POPUP|WS_BORDER;
	CWnd *p = this;

	BOOL bRet = p->CreateEx(0, WC_TREEVIEW, NULL,
		dwStyle,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		ne->GetSafeHwnd(), (HMENU)(UINT_PTR)0, (LPVOID)0);

	if (bRet && p->GetExStyle() != 0)
	{
		bRet = ModifyStyleEx(0, p->GetExStyle());
	}



	SetImageList(&IlwWinApp()->ilSmall, TVSIL_NORMAL);
	iImgOvlSystem =	IlwWinApp()->iImage("OverlaySystem");
}

ObjectTreeCtrl::~ObjectTreeCtrl()
{
}

void ObjectTreeCtrl::OnKillFocus(CWnd* wnd)
{
	ShowWindow(SW_HIDE);
	iHideTime = GetTickCount();
}

void ObjectTreeCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (0 == (nState & WA_INACTIVE)) {

		bool fFirstTime = false;
		if (!fFilled) {
			Fill();
			fFilled = true;
			fFirstTime = true;
		}

		HTREEITEM hti = TVI_ROOT;
		char sPath[MAXPATH];
		FileName fnCurr(ne->sDir);
		strcpy(sPath, fnCurr.sPath().sVal());
		FileName fnDir;
		fnDir.sExt = "directory";
		size_t iLen = strlen(sPath);
		int iStart = 0;
		for (int i = 2; i < iLen + 1; i++) {
			if ('\\' == sPath[i])
				sPath[i] = '\0';
			if ('\0' != sPath[i])
				continue;
			if (iStart == i)
				continue;
			hti = GetChildItem(hti);
			while (hti) {
				CString str = GetItemText(hti);
				if (0 == str.CompareNoCase(&sPath[iStart]))
					break;
				hti = GetNextSiblingItem(hti);
			}
			ExpandDir(hti);
			sPath[i] = '\\';
			iStart = i + 1;
		}
		if (fFirstTime)	{
			AddSystemObjects(hti);
			TVITEM tvi;
			tvi.mask = TVIF_CHILDREN;
			tvi.cChildren = 1;
			tvi.hItem = hti;
			SetItem(&tvi);
		}

		if (hti == TVI_ROOT) // should never happen, but...
			return;

		Select(hti, TVGN_FIRSTVISIBLE);
		hti = GetChildItem(hti);
		EnsureVisible(hti);
		String sName = ne->sName();
		FileName fn(sName);
		while (hti) {
			int iNr = (int)GetItemData(hti);
			if (fn == afn[abs(iNr)-1])
				break;
			hti = GetNextSiblingItem(hti);
		}
		if (hti) {
			SelectItem(hti);
			EnsureVisible(hti);
		}
	}

	CTreeCtrl::OnActivate(nState, pWndOther, bMinimized);
}

void ObjectTreeCtrl::SelectAttribColumn(const FileName& fnMapWithCol)
{
	// Pre-conditions:
	// 1) fnMapWithCol has an attribute column specified in sCol
	// 2) afn is already filled (e.g. by OnActivate)
	// This function will expand the attribute table and make sure sCol is selected in the tree

	FileName fnMap(fnMapWithCol);
	fnMap.sCol = ""; // fnMap must not have sCol anymore

	// code below is copied from OnActivate() - it "moves" hti in the tree up to the correct path depth

	HTREEITEM hti = TVI_ROOT;
	char sPath[MAXPATH];
	strcpy(sPath, ne->sDir);
	FileName fnDir;
	fnDir.sExt = "directory";
	size_t iLen = strlen(sPath);
	int iStart = 0;
	for (int i = 2; i < iLen + 1; i++) {
		if ('\\' == sPath[i])
			sPath[i] = '\0';
		if ('\0' != sPath[i])
			continue;
		if (iStart == i)
			continue;
		hti = GetChildItem(hti);
		while (hti) {
			CString str = GetItemText(hti);
			if (0 == str.CompareNoCase(&sPath[iStart]))
				break;
			hti = GetNextSiblingItem(hti);
		}
		sPath[i] = '\\';
		iStart = i + 1;
	}

	if (hti == TVI_ROOT) // should never happen, but...
		return;

	// (hopefully) hti points to the last folder before our files
	Select(hti, TVGN_FIRSTVISIBLE);
	hti = GetChildItem(hti);
	EnsureVisible(hti);
	while (hti) {
		int iNr = (int)GetItemData(hti);
		if (fnMap == afn[abs(iNr)-1])
			break;
		hti = GetNextSiblingItem(hti);
	}
	// now hti points to fnMap, which in itself is the class map that has an attribute table
	if (hti)
	{
		int iNr = (int)GetItemData(hti);
		ExpandAttribTable(hti, fnMap); // fill afn with the attribute columns
		SetItemData(hti, -iNr); // "mark" hti as "expanded" (or ExpandAttribTable may be called again by OnItemExpanding)

		// reconstruct our "column" (fnCol) as afn is filled (by ExpandAttribTable(hti, fnMap))
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fnMap);
		FileName fnCol;
		fnCol.sFile = String("%S|%S|%S", fnMap.sFullPath(), fnAttrib.sFullPath(false), fnMapWithCol.sCol);
		fnCol.sExt = "column";

		// done! now find fnCol, select it and show it!

		hti = GetChildItem(hti);
		EnsureVisible(hti);
		while (hti)
		{
			iNr = (int)GetItemData(hti);
			if (fnCol == afn[abs(iNr)-1])
				break;
			hti = GetNextSiblingItem(hti);
		}
		SelectItem(hti);
		EnsureVisible(hti);
	}
}

void ObjectTreeCtrl::Fill()
{
	FileName fnCur = ne->sDir;
	HTREEITEM hCur;

	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 1;

  FileName fn;
	fn.sExt = "drive";
	int iImg = IlwWinApp()->iImage("drive");
	DWORD dwMask = GetLogicalDrives();
	DWORD dwDisk = 1;
	char sDrive[3] = "A:";
	for (int i = 0; i < 32; ++i) 
	{
		if (dwDisk & dwMask) 
		{
			switch(GetDriveType(sDrive) )
			{
				case DRIVE_CDROM:
					iImg  = IlwWinApp()->iImage("CDRomDrive"); break;
				case DRIVE_REMOVABLE:
					iImg  = IlwWinApp()->iImage("FloppyDrive"); break;
				case DRIVE_REMOTE:
					iImg  = IlwWinApp()->iImage("NetworkDrive");break;
				default:
					iImg  = IlwWinApp()->iImage("drive");
			}
			fn.sFile = sDrive;
			tvi.hItem = InsertItem(sDrive, iImg, iImg);
			if (fnCur.sDrive == fn.sDrive)
				hCur = tvi.hItem;
			SetItem(&tvi);
			afn &= fn;
			SetItemData(tvi.hItem, afn.iSize());
		}
		dwDisk <<= 1;
		sDrive[0]++;
	}
}

void ObjectTreeCtrl::ExpandDir(HTREEITEM hti)
{
	if (0 == hti)
		return;
	int iNr = (int)GetItemData(hti);
	// in case smaller than zero expansion already took place
	if (iNr <= 0)	
		return;
	const FileName& fn = afn[iNr-1];
	ExpandPath(hti, fn.sFile);  // ugly: misuse fn.sFile as drive!
	iNr = -abs(iNr);
	SetItemData(hti, iNr);
}

static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the tree control.
	// The lParam of an item is the index in the filename array (afn) of the treeview
	ObjectTreeCtrl* pTree = (ObjectTreeCtrl*) lParamSort;

	return pTree->iCompareByIndex((long)lParam1, (long)lParam2);
}

int ObjectTreeCtrl::iCompareByIndex(long iNdx1, long iNdx2)
{
	const FileName& fn1 = afn[abs(iNdx1)-1];
	const FileName& fn2 = afn[abs(iNdx2)-1];
	bool fIsDir1 = fn1.sExt == "directory";
	bool fIsDir2 = fn2.sExt == "directory";
	bool fIsCont1 = FileName::fIsContainer(fn1); // String(".mpl.ioc").find(fn1.sExt.toLower()) != String::npos;
	bool fIsCont2 = FileName::fIsContainer(fn2); // String(".mpl.ioc").find(fn2.sExt.toLower()) != String::npos;
	if ((fIsDir1 && fIsDir2) ||   // both directories
		(fIsCont1 && fIsCont2) || // both ILWIS containers
		(!fIsDir1 && !fIsDir2 && !fIsCont1 && !fIsCont2))  // both are files
		return _strcmpi(fn1.sFile.c_str(), fn2.sFile.c_str());  // plain sort when the types of both are the same

	// We reach here when both items are different types (Dir + File, Dir + Cont, Cont + File)
	// Check for exactly one directory, directories go first!
	if (fIsDir1)
		return -1;
	else if (fIsDir2)
		return 1;
	// Check for exactly one container, containers go before files!
	else if (fIsCont1)
		return -1;
	else // fIsCont2 == true
		return 1;
}

void ObjectTreeCtrl::ExpandPath(HTREEITEM hti, const String& sP)
{
	String sPath = sP;
	int iImgDir = IlwWinApp()->iImage("directory");
	bool fChildren = false;
	bool fObjects = false;
	string sIlwisObjExt = IlwisObject::sAllExtensions();
	String sExt = ne->ol->sFileExt();
	if (sExt == ".*")   
		sExt = sIlwisObjExt;
	
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	
	sPath &= "\\*.*";
	CFileFind finder;
	BOOL fFound = finder.FindFile(sPath.c_str());
	while (fFound)
	{
		fFound = finder.FindNextFile();
		if (finder.IsHidden())
			continue;
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
		{
			fChildren = true;
			FileName fn;
			fn.sExt = "directory";
			fn.sFile = (const char*)finder.GetFilePath();
			tvi.cChildren = 1;
			tvi.hItem = InsertItem(finder.GetFileName(), iImgDir, iImgDir, hti);
			SetItem(&tvi);
			afn &= fn;
			SetItemData(tvi.hItem, afn.iSize());
		}
		else // object collection/maplist/table
		{
			String s = finder.GetFilePath();
			FileName fn(s);
			
			if (fn.sExt.toLower() == ".mpl" || fn.sExt.toLower() == ".mpr")
				tvi.cChildren = 0;  // dummy
			
			// Check if fn is a container for extensions in sExt
			bool fIsContainer = FileName::fIsContainerFor(fn, sExt);
			
			// only allow extensions from the ObjectLister and appropriate container types
			if (!fIsContainer && sExt.find(fn.sExt.toLower()) == string::npos)
				continue;
			
			// Only allow object passed by the ObjectLister and appropriate container types
			if (!fIsContainer && !ne->ol->fOK(fn))
				continue;
			
			if (fIsContainer)
				tvi.cChildren = 1;
			else if ( ObjectInfo::fnAttributeTable(fn) != FileName())
			{
				if (ne->ol->fHasChildren(fn))
					tvi.cChildren = 1;
				else
					tvi.cChildren = 0;
			}
			else
				tvi.cChildren = 0;
			fChildren = true;
			fObjects = true;
			String str = fn.sExt.toLower();
			int iImg = IlwWinApp()->iImage(str);
			tvi.hItem = InsertItem(fn.sFile.c_str(), iImg, iImg, hti);
			SetItem(&tvi);
			afn &= fn;
			int iNr = afn.iSize();
			if (0 == tvi.cChildren)
				iNr *= -1;
			SetItemData(tvi.hItem, iNr);
		}
	} 
	TVSORTCB tvs;
	
	// Sort the tree control's items using my
	// callback procedure.
	tvs.hParent = hti;
	tvs.lpfnCompare = MyCompareProc;
	tvs.lParam = (LPARAM) this;
	
	SortChildrenCB(&tvs);
	
	if (!fChildren)
	{
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
	}
}

void ObjectTreeCtrl::ExpandAttribTable(HTREEITEM hti, const FileName& fnM)
{
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 0;
  bool fChildren = true;
	FileName fnMap(fnM);
	try {
		vector<String> arColumns;
		FillWithColumns(fnMap, arColumns);
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fnMap);

		for (int i = 0; i < arColumns.size(); ++i) 
		{
			fChildren = true;
			String sName = arColumns[i];
			int iImg = IlwWinApp()->iImage("column");
			tvi.hItem = InsertItem(sName.c_str(), iImg, iImg, hti);
			SetItem(&tvi);
			FileName fn;
			fn.sFile = String("%S|%S|%S", fnMap.sFullPath(), fnAttrib.sFullPath(false), arColumns[i]);
			fn.sExt = "column";
			afn.push_back(fn);
			int iNr =  -afn.iSize();
			SetItemData(tvi.hItem, iNr);
		}
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
	if (!fChildren) {
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
	}

}

void ObjectTreeCtrl::FillWithColumns(const FileName& fnMap, vector<String>& arColumns)
{
  String sCol;
	FileName fnTbl = ObjectInfo::fnAttributeTable(fnMap);
  if (fnTbl.fValid()) 
	{
    TableInfo tblinf(fnTbl);
    for (int i = 0; i < tblinf.iCols(); ++i) 
		{
			String sCol = tblinf.aci[i].sName();
			if ( ne->ol->fOK(fnMap, sCol))
				arColumns.push_back(sCol);
    }
  }
}

void ObjectTreeCtrl::ExpandObjectCollection(HTREEITEM hti)
{
	if (0 == hti)
		return;
	int iNr = GetItemData(hti);
	// in case smaller than zero expansion already took place
	if (iNr <= 0)	
		return;
	const FileName& fn = afn[iNr-1];

	bool fChildren = false; // will be set to true if one suitable child is found
	TVITEM tvi; // for browsing through elements and setting "children" properties
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 0;

	try {
		ObjectCollection obc(fn);
		// if one map in the maplist is ok, all are ok
		if (obc.fValid()) // can be invalid, which is not caught by "catch" below
			for (int i = 0; i < obc->iNrObjects(); ++i) 
			{
				FileName fno = obc->fnObject(i);
				String sName = fno.sRelative(false);
				int iImg = IlwWinApp()->iImage(fno.sExt.c_str());
				IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fno); // was fn since May 2000!!
				if (ne->ol->fOK(fno) || type == IlwisObject::iotMAPLIST || type == IlwisObject::iotOBJECTCOLLECTION)
				{
					fChildren = true;
					tvi.cChildren = (type == IlwisObject::iotMAPLIST || type == IlwisObject::iotOBJECTCOLLECTION)?1:0;
					tvi.hItem = InsertItem(sName.c_str(), iImg, iImg, hti);
					SetItem(&tvi);
					afn &= fno;
					int iNr = (type != IlwisObject::iotMAPLIST && type != IlwisObject::iotOBJECTCOLLECTION) ? -afn.iSize() : afn.iSize();
					SetItemData(tvi.hItem, iNr);
				}
			} // end for
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
	if (!fChildren) {
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
	}
}

void ObjectTreeCtrl::ExpandMapList(HTREEITEM hti)
{
	if (0 == hti)
		return;
	int iNr = GetItemData(hti);
	// in case smaller than zero expansion already took place
	if (iNr <= 0)	
		return;
	const FileName& fn = afn[iNr-1];

	int iImgMap = IlwWinApp()->iImage(".mpr");
	bool fChildren = false;
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 0;

	try {
		MapList mpl(fn);
		int iLo = mpl->iLower();
		int iHi = mpl->iUpper();
		// if one map in the maplist is ok, all are ok (also check if there are any maps at all!)
		if (iLo<=iHi && ne->ol->fOK(mpl[iLo]->fnObj))
		{
			for (int i = iLo; i <= iHi; ++i) {
				fChildren = true;
				String sName = mpl->sDisplayName(i);
				tvi.hItem = InsertItem(sName.c_str(), iImgMap, iImgMap, hti);
				SetItem(&tvi);
				afn &= mpl[i]->fnObj;
				int iNr = afn.iSize();
				SetItemData(tvi.hItem, -iNr);
			}
		}
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
	if (!fChildren) {
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
	}
}

void ObjectTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* nmtv = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hti = nmtv->itemNew.hItem;
	*pResult = 0;
	int iNr = GetItemData(hti);
	if (iNr > 0) {
		try {
			const FileName& fn = afn[iNr-1];
			if ("drive" == fn.sExt || "directory" == fn.sExt)
				ExpandDir(hti);
			else if (".mpl" == fn.sExt)
					ExpandMapList(hti);
			else if (".ioc" == fn.sExt )
				ExpandObjectCollection(hti);
			else if ( ObjectInfo::fnAttributeTable(fn) != FileName() )
			{
				ExpandAttribTable(hti, fn);
			}
			SetItemData(hti, -iNr);
		}
		catch (ErrorObject& err) {
			err.Show();
			TVITEM tvi;
			tvi.mask = TVIF_CHILDREN;
			tvi.cChildren = 0;
			tvi.hItem = hti;
			SetItem(&tvi);
		}
	}
}

void ObjectTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hti = HitTest(point);
	SelectItem(hti);
// do not call base class, because it prevents often LButtonUp receive
//	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void ObjectTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	// if a key or something is pressed do not close
	if (0 != nFlags)
		return;
	TVHITTESTINFO tvi;
	tvi.pt = point;
	HTREEITEM hti = HitTest(&tvi);
	if ( tvi.flags & TVHT_ONITEMBUTTON ) // user clicked on '+'
		Expand(hti,TVE_TOGGLE);
	else // user selected an item
	{
		int iNr = GetItemData(hti);
		const FileName& fn = afn[abs(iNr)-1];
		if ( fn.sFile.find("|") != -1 ) // attrib column
		{		
			String sName("%S.clm" ,fn.sFile);
			ne->SetVal(sName);
		}
		else
		{
			if (!ne->ol->fOK(fn, sCHECK_OBJECT_ONLY))
				return;

			IlwisObject::iotIlwisObjectType type = IlwisObject::iotObjectType(fn);
			String sExtSelectable = ne->ol->sFileExt();
			String sExtCurrentFile = fn.sExt;
  		// do not close on directories, maplists, oc's and items with columns
			// unless the maplist and oc is the right type for selecting
			if ((sExtSelectable.find(sExtCurrentFile.toLower()) == -1)
				&& (ItemHasChildren(hti)
						|| type == IlwisObject::iotMAPLIST
						|| type == IlwisObject::iotOBJECTCOLLECTION)
					 )
  			return;
			ne->SetVal(fn);
		}
		ShowWindow(SW_HIDE);
		ne->SetFocus();
	}
}

BOOL ObjectTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) 
		{
			case VK_RETURN: 
			{
				HTREEITEM hti = GetSelectedItem();
				int iNr = GetItemData(hti);
				const FileName& fn = afn[abs(iNr)-1];
				if (!ne->ol->fOK(fn)) {	
					ExpandDir(hti);
					hti = GetChildItem(hti);
					if (hti)
						EnsureVisible(hti);
					return TRUE;
				}
				ne->SetVal(fn);
			} // fall through
			case VK_ESCAPE:
				ShowWindow(SW_HIDE);
				ne->SetFocus();
				return TRUE;
		}
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void ObjectTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnMouseMove(nFlags, point);
	if (nFlags & MK_LBUTTON) 
	{
		HTREEITEM hti = HitTest(point);
		if (hti) 
			SelectItem(hti);
	}
}

void ObjectTreeCtrl::AddSystemObjects(HTREEITEM hti)
{
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 0;

  CFileFind finder;
	String sPath = IlwWinApp()->Context()->sStdDir();
	if (sPath[sPath.length()] != '\\')
		sPath &= "\\*.*";
	else
		sPath &= "*.*";

  BOOL fFound = finder.FindFile(sPath.c_str());
	while (fFound) {
		fFound = finder.FindNextFile();
		if (finder.IsHidden())
			continue;
		if (finder.IsDirectory()) 
			continue;

		String s = finder.GetFilePath(); 
		FileName fn(s); 
		if (!ne->ol->fOK(fn))
			continue;
		int iImg = IlwWinApp()->iImage(fn.sExt);
		tvi.hItem = InsertItem(fn.sFile.c_str(), iImg, iImg, hti);
		SetItem(&tvi);
		afn &= fn;
		int iNr = afn.iSize();
		SetItemData(tvi.hItem, -iNr);
//		SetItemState(tvi.hItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK);
  } 
}

void ObjectTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	HTREEITEM hti = HitTest(pt);
	if (hti == 0)
		return;
	int iNr = GetItemData(hti);
	FileName fn = afn[abs(iNr)-1];
	IlwWinApp()->ShowPopupMenu(pWnd, point, fn, 0, 0);
}

void ObjectTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}

void ObjectTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVCUSTOMDRAW* pcd = (NMTVCUSTOMDRAW*) pNMHDR;
	switch (pcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYPOSTPAINT;
			return;
		case CDDS_ITEMPOSTPAINT:
		{
			*pResult = CDRF_DODEFAULT;
			int iNr = pcd->nmcd.lItemlParam;
			const FileName& fn = afn[abs(iNr)-1];
			if (fn.sExt == "drive" || fn.sExt == "directory")
				return;
			if (ObjectInfo::fSystemObject(fn)) {
				CDC cdc;
				cdc.Attach(pcd->nmcd.hdc);
				CRect rect = pcd->nmcd.rc;
				HTREEITEM hti = HitTest(rect.TopLeft());
				GetItemRect(hti, &rect, TRUE);
				CPoint pt = rect.TopLeft();
				pt.x -= 38;
/*
				int iIndent = GetIndent();
				int iLevel = abs(pcd->iLevel) + 1;
				pt.x += iIndent * iLevel;
*/
				IlwWinApp()->ilSmall.Draw(&cdc, iImgOvlSystem, pt, ILD_TRANSPARENT); 				
				cdc.Detach();
			}
			return;
		}
	}
}

void ObjectTreeCtrl::Reset()
{
	if (fFilled) 
		DeleteAllItems();
	fFilled = false;
}

