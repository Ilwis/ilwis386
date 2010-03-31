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
// NavigatorTreeCtrl.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Engine\Base\File\Directory.h"
#include "Client\ilwis.h"
#include "Engine\Base\File\Directory.h"
#include "Client\MainWindow\NavigatorTreeCtrl.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mainwind.hs" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// copied from afximpl.h
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)
BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#define AFX_WNDCOMMCTL_TREEVIEW_REG     0x00080

class NavigatorTreeCtrlDropTarget: public COleDropTarget
{
public:
	BOOL Register(NavigatorTreeCtrl* acttc)
	{
		ntc = acttc;
		return COleDropTarget::Register(ntc);
	}
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return ntc->OnDragEnter(pDataObject, dwKeyState, point);
	}
	void OnDragLeave(CWnd* pWnd)
	{
		ntc->OnDragLeave();
	}
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return ntc->OnDragOver(pDataObject, dwKeyState, point);
	}
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
	{
		return ntc->OnDrop(pDataObject, dwKeyState, point);
	}
private:
	NavigatorTreeCtrl* ntc;
};


BEGIN_MESSAGE_MAP(NavigatorTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(NavigatorTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()	
	ON_WM_RBUTTONDOWN()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


NavigatorTreeCtrl::NavigatorTreeCtrl(CWnd* wParent)
{
	CRect rect(0,0,0,0);
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_TREEVIEW_REG));

	odt = new NavigatorTreeCtrlDropTarget;	
	DWORD dwStyle = TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|WS_CHILD|WS_BORDER;
	CreateEx(0, dwStyle, rect, wParent, 0);
	odt->Register(this);	
	SetImageList(&IlwWinApp()->ilSmall, TVSIL_NORMAL);
	iFmtCopy = RegisterClipboardFormat("IlwisCopy");
	IlwisSettings settings("MainWindow\\WMSNodes");
	int n = settings.iValue("NumberOfWMS");
	for(int i=0; i< n; ++i) {
		String key("url%d",i);
		String url = settings.sValue(key);
		AddToWMSNode(url);

	}
	Refresh();
}

NavigatorTreeCtrl::~NavigatorTreeCtrl()
{
	IlwisSettings settings("MainWindow\\WMSNodes");
	if ( lsWMSUrls.size() == 0)
		settings.DeleteKey();
	else {
		int count = 0;
		for(list<String>::const_iterator cur = lsWMSUrls.begin(); cur != lsWMSUrls.end(); ++cur)
		{
			String url = (*cur);
			settings.SetValue(String("url%d",count++),url);
		}
		settings.SetValue("NumberOfWMS",count);
	}
	delete odt;

}

void NavigatorTreeCtrl::Refresh()
{
	afn.Reset();
	sLastScrollDir = "";	
	//LockWindowUpdate();
	SendMessage(WM_SETREDRAW, FALSE, 0);
	DeleteAllItems();
	int iImgHis = IlwWinApp()->iImage("History");
	htiHistory = InsertItem("History", iImgHis, iImgHis);
	int iImgInet = IlwWinApp()->iImage("WMF");
	htiWMS = InsertItem("WMS", iImgInet, iImgInet);
	for(list<Directory>::const_iterator cur = lsHistory.begin(); cur != lsHistory.end(); ++cur)
	{
		AddToHistoryNode((*cur).sFullPath());
	}

	for(list<String>::const_iterator cur = lsWMSUrls.begin(); cur != lsWMSUrls.end(); ++cur)
	{
		AddToWMSNode(*cur);
	}
	
	Fill();

	OnUpdateCmdUI();
	SendMessage(WM_SETREDRAW, TRUE, 0);	
	//UnlockWindowUpdate();
}

void NavigatorTreeCtrl::Fill()
{
	HTREEITEM hCur;

	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.cChildren = 1;

  FileName fn, fnCur;
	fn.sExt = "drive";
	int iImgDrive = IlwWinApp()->iImage("drive");
	int iImgCD = IlwWinApp()->iImage("CDRomDrive");
	int iImgNet = IlwWinApp()->iImage("NetworkDrive");	
	int iImgFloppy = IlwWinApp()->iImage("FloppyDrive");	
	DWORD dwMask = GetLogicalDrives();
	DWORD dwDisk = 1;
	int iImg;
	char sDrive[3] = "A:";
	for (int i = 0; i < 32; ++i) {
		if (dwDisk & dwMask) 
		{
			fn.sFile = sDrive;
			switch(GetDriveType(sDrive) )
			{
				case DRIVE_CDROM:
					iImg = iImgCD; break;
				case DRIVE_REMOVABLE:
					iImg = iImgFloppy; break;
				case DRIVE_REMOTE:
					iImg = iImgNet;break;
				default:
					iImg = iImgDrive;
			}				
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

void NavigatorTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
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

void NavigatorTreeCtrl::ExpandDir(HTREEITEM hti)
{
	if (0 == hti)
		return;
	int iNr = GetItemData(hti);
	// in case smaller than zero expansion already took place
	if (iNr <= 0)	
		return;
	const FileName& fn = afn[iNr-1];
	ExpandPath(hti, fn.sFile);
	iNr = -abs(iNr);
	SetItemData(hti, iNr);
}

void NavigatorTreeCtrl::ExpandPath(HTREEITEM hti, const String& sP)
{
	String sPath = sP;
	int iImgDir = IlwWinApp()->iImage("directory");
	int iImgOpenDir = IlwWinApp()->iImage("opendirectory");
	bool fChildren = false;
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	
	sPath &= "\\*.*";
  CFileFind finder;
  BOOL fFound = finder.FindFile(sPath.scVal());
	while (fFound) {
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
			if (Directory(String(finder.GetFilePath())).fValid() == false)
				continue;
			tvi.cChildren = 1;
			tvi.hItem = InsertItem(finder.GetFileName(), iImgDir, iImgOpenDir, hti);
			SetItem(&tvi);
			afn &= fn;
			SetItemData(tvi.hItem, afn.iSize());
		}
  } 
	SortChildren(hti);
	if (!fChildren) {
		tvi.cChildren = 0;
		tvi.hItem = hti;
		SetItem(&tvi);
	}
}

void NavigatorTreeCtrl::OnUpdateCmdUI()
{
	String sDir = IlwWinApp()->sGetCurDir();
	if ( sDir == "" || sDir == "\\" ) 
	{ 
		TRACE("Warning: empty current directory!\n");
		return;
	}
	if (sDir == sLastScrollDir)
		return;
	sLastScrollDir = sDir;

	HTREEITEM hti = TVI_ROOT;
	char sPath[MAXPATH];
	strcpy(sPath, sDir.scVal());
	FileName fnDir;
	fnDir.sExt = "directory";
	int iLen = strlen(sPath);
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
	if (hti)
	{
		EnsureVisible(hti);
		SelectItem(hti);
	}
}

void NavigatorTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	HTREEITEM hti = HitTest(point);
	int iNr = GetItemData(hti);
	if (iNr == 0) { // history!
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}
	// copy needed, because Expand changes afn
	FileName fn = afn[abs(iNr)-1];
	if ("drive" == fn.sExt || "directory" == fn.sExt)
		Expand(hti, TVE_TOGGLE);
	if ( URL::isUrl(fn.sFile)) {
		URL u = completeWMSUrl(fn.sFile);
		String cmd = "open " + u.sVal() + " -method=WMS";
		IlwWinApp()->Execute(cmd);
	}
	else
		IlwWinApp()->OpenDirectory(fn.sFile.scVal());
}

BOOL NavigatorTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) 
		{
			case VK_RETURN: 
			{
				HTREEITEM hti = GetSelectedItem();
				int iNr = GetItemData(hti);
				const FileName& fn = afn[abs(iNr)-1];
				IlwWinApp()->OpenDirectory(fn.sFile.scVal());
			} // fall through
			case VK_ESCAPE:
//				ShowWindow(SW_HIDE);
//				GetParent()->SetFocus();
				return TRUE;
		}
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void NavigatorTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{									
	UINT uFlags;
	HTREEITEM hti = HitTest(point, &uFlags);
	if (uFlags & TVHT_ONITEM) {
		int iNr = GetItemData(hti);
		if (iNr == 0) {
			CTreeCtrl::OnLButtonDown(nFlags, point);
			return;
		}
		const FileName& fn = afn[abs(iNr)-1];
		SelectItem(hti);
		SetFocus();
		CDocument *doc = IlwWinApp()->OpenDirectory(fn.sFile.scVal());
		if ( doc == NULL )
			IlwWinApp()->OpenNewDirectory(fn.sFile.scVal());
	}
	else
		CTreeCtrl::OnLButtonDown(nFlags, point);
}

void NavigatorTreeCtrl::AddToHistoryNode(const String& sFn)
{
	int iImgDir = IlwWinApp()->iImage("directory");
	TVITEM tvi;
	tvi.hItem = InsertItem(sFn.scVal(), iImgDir, iImgDir, htiHistory, TVI_FIRST);
	FileName fn;
	fn.sExt = "directory";
	fn.sFile = sFn;
	afn &= FileName(fn);
	SetItemData(tvi.hItem, afn.iSize());
}

void NavigatorTreeCtrl::AddToWMSNode(const String& sUrl)
{
	for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur) {
		if ( fCIStrEqual((*cur).sFile,sUrl))
			return;
	}
	if ( ! URL::isUrl(sUrl))
		return;
	lsWMSUrls.push_front(sUrl);
	int iImgDir = IlwWinApp()->iImage(".mpl");
	TVITEM tvi;
	tvi.hItem = InsertItem(sUrl.scVal(), iImgDir, iImgDir, htiWMS, TVI_FIRST);
	FileName fn;
	fn.sExt = "url";
	fn.sFile = sUrl;
	afn &= FileName(fn);
	SetItemData(tvi.hItem, afn.iSize());
}

void NavigatorTreeCtrl::AddToUrlNode(HTREEITEM hti, const FileName& fn)
{
	for(vector<FileName>::iterator cur = afn.begin(); cur != afn.end(); ++cur) {
		if ( fn == (*cur))
			return;
	}
	int iImgDir = IlwWinApp()->iImage(".ioc");
	TVITEM tvi;
	tvi.hItem = InsertItem(fn.sFile.scVal(), iImgDir, iImgDir, hti, TVI_FIRST);
	afn &= FileName(fn);
	SetItemData(tvi.hItem, afn.iSize());
}

void NavigatorTreeCtrl::AddToHistory(const Directory& dirAdd)
{
  if ( find(lsHistory.begin(), lsHistory.end(), dirAdd) == lsHistory.end())
	{
		lsHistory.push_front(dirAdd);
		AddToHistoryNode(dirAdd.sFullPath());
	}
}

DROPEFFECT NavigatorTreeCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return DROPEFFECT_NONE;
	HTREEITEM hti = HitTest(point);
	if (0 == hti)
		return DROPEFFECT_NONE;
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	if (0 == hDrop)
		return DROPEFFECT_NONE;
	bool fOk = true;

	char sFileName[MAX_PATH+1];
	int iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
	if (0 == iFiles)
		return DROPEFFECT_NONE;
	for (int i = 0; i < iFiles; ++i) 
	{
		DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	  FileName fn(sFileName);
		if ( fn.sExt == "Drive" || fn.sExt == "Directory" )
			fOk = false;
		GlobalUnlock(hDrop);
	}

	GlobalFree(hnd);

	if (fOk)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

void NavigatorTreeCtrl::OnDragLeave() 
{
	// TODO: Add your specialized code here and/or call the base class
}

DROPEFFECT NavigatorTreeCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return OnDragEnter(pDataObject, dwKeyState, point);
}

BOOL NavigatorTreeCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!pDataObject->IsDataAvailable(CF_HDROP))
		return FALSE;
	HTREEITEM hti = HitTest(point);
	if (0 == hti)
		return FALSE;
	int iNr = abs((int)GetItemData(hti));
	String sDest = afn[iNr-1].sFile;	
	if ( sDest [ sDest.size() - 1 ] != '\\' )
		sDest &= "\\";
	
	HGLOBAL hnd = pDataObject->GetGlobalData(CF_HDROP);
	HDROP hDrop = (HDROP)GlobalLock(hnd);
	int iFiles = 0;
	FileName* afn = 0;
	String sFiles;	
	if (hDrop) {
		char sFileName[MAX_PATH+1];
		iFiles = DragQueryFile(hDrop,	(UINT)-1, NULL, 0);
		for (int i = 0; i < iFiles; ++i) {
			DragQueryFile(hDrop, i, sFileName, MAX_PATH+1);
	    FileName fn = FileName(sFileName);
			if ( fn.fExist() )
			{
				sFiles &= fn.sFullPathQuoted() + " ";
			}				
		}
		GlobalUnlock(hDrop);
	}
	GlobalFree(hnd);
	// if send by SendMessage() prevent deadlock
	ReplyMessage(0);

	if ( sFiles != "" )
	{
		Directory dir( sDest);
		sFiles &= dir.sFullPathQuoted();
		String sCmd("copy %S", sFiles);
		IlwWinApp()->Execute( sCmd);
	}			
	return TRUE;
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID));
class NameForm : public FormWithDest
{
public:
	NameForm(CWnd *par, String& sUrl, String& version) :
	  FormWithDest(par, SMWMSTitle)
	  {

		FieldStringMulti *fs = new FieldStringMulti(root, SMWMsgWMSName, &sUrl);
		zDimension dim = fs->Dim("gk");
		fs->SetWidth((short)(13 * dim.width()));
		fs->SetHeight((short)(4 * dim.height()));
		create();

	  }							
};

void NavigatorTreeCtrl::handleWMSMenu(CPoint point) {

	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_NEW_WMS);
	switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this)) {
		case ID_NEW_WMS:
			String sUrl, sVersion="1.1.1";
			NameForm frm(this, sUrl, sVersion);
			if ( frm.fOkClicked()) {
				sUrl = sUrl.sTrimSpaces();
				String sCleanedUrl="";
				for(int i = 0; i< sUrl.length(); ++i) {
					char c = sUrl[i];
					if ( c < 32)
						continue;
					sCleanedUrl += c;
				}
				AddToWMSNode(sCleanedUrl);
				OnUpdateCmdUI();
				SendMessage(WM_SETREDRAW, TRUE, 0);
			}
			break;

	}
}

URL NavigatorTreeCtrl::completeWMSUrl(const String& txt) {
	URL u(txt);
	if ( u.getQueryValue("VERSION") == "")
		u.setQueryValue("VERSION","1.1.1");
	if (u.getQueryValue("SERVICE")=="")
		u.setQueryValue("Service","WMS");
	return u;

}

void NavigatorTreeCtrl::handleUrl(CPoint point, HTREEITEM hti) {
	int iNr = abs((int)GetItemData(hti)); 
	String url = afn[iNr-1].sFile;
	CMenu men;
	men.CreatePopupMenu();
	pmadd(ID_OPEN_WMS);
	pmadd(ID_REMOVE_WMS);
	pmadd(ID_EDIT_WMS);
	switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this)) {
		case ID_OPEN_WMS:{
			URL u = completeWMSUrl(url);
			FileName fn = u.toFileName();
			//AddToUrlNode(hti,fn);
			IlwWinApp()->Execute("open " + u.sVal() + " -method=WMS");
     		}
			break;
		case ID_REMOVE_WMS:
			afn.Remove(iNr - 1, 1);
			lsWMSUrls.remove(url);
			DeleteItem(hti);
			break;
		case ID_EDIT_WMS:
			String sUrl, sVersion="1.1.1";
			NameForm frm(this, url, sVersion);
			if ( frm.fOkClicked()) {
				url = url.sTrimSpaces();
				String sCleanedUrl="";
				for(int i = 0; i< url.length(); ++i) {
					char c = url[i];
					if ( c < 32)
						continue;
					sCleanedUrl += c;
				}
				AddToWMSNode(sCleanedUrl);
				OnUpdateCmdUI();
				SendMessage(WM_SETREDRAW, TRUE, 0);
			}
			break;
	}
}
void NavigatorTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	HTREEITEM hti = HitTest(pt);
	SelectItem(hti);
	SetFocus();	
	if (hti) 
	{
		if ( htiWMS == hti ) {
			handleWMSMenu(point);
			return;
		}
		int iNr = abs((int)GetItemData(hti));
		if ( iNr == 0)
			return;
		FileName fn = afn[iNr-1];
		String sDest = afn[iNr-1].sFile;
		if (URL::isUrl(sDest)) {
			handleUrl(point, hti);
			return;
		}
		String sType;
		ObjectInfo::ReadElement("ObjectCollection","Type",fn,sType);
		if ( sType == "WMSCollection") {
			IlwWinApp()->ShowPopupMenu(this, point, fn, NULL);
			return;
		}
		if ( sDest [ sDest.size() - 1 ] != '\\' )
			sDest &= "\\";
		if (sDest != "\\") 
		{
			CMenu men;
			men.CreatePopupMenu();
			pmadd(ID_CAT_NEW2);
			men.AppendMenu(MF_SEPARATOR);			
			pmadd(ID_NAV_CREATEDIR);
			men.AppendMenu(MF_SEPARATOR);			
			pmadd(ID_EDIT_PASTE);
			bool fPossible = IsClipboardFormatAvailable(iFmtCopy)? true:false;			
			men.EnableMenuItem(ID_EDIT_PASTE,  fPossible ? MF_ENABLED : MF_GRAYED);			
		  switch (men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y, this))
			{
				case ID_CAT_NEW2:
				{ 
					String sCmd("opendir %S", sDest);
					IlwWinApp()->Execute(sCmd);	
				} break;
				case ID_NAV_CREATEDIR:
				{
					class NameForm : public FormWithDest
					{
					public:
						NameForm(CWnd *par, String& sDirName) :
							FormWithDest(par, SMSTitleNewDirectory)
						{
							FieldString *fs = new FieldString(root, SMWMsgDirName, &sDirName);
							create();

						}							
					};
					String sName;
					NameForm frm(this, sName);
					if ( frm.fOkClicked())
					{
						String sDir = sDest + sName;
						String sCmd("md %S", sDir.sQuote());
						IlwWinApp()->Execute(sCmd);	
						String sCD = IlwWinApp()->sGetCurDir();
					}						

				}	
				break;
				case ID_EDIT_PASTE:
				{
					vector<FileName> arFiles;
					Catalog::GetFromClipboard(arFiles);
					if ( arFiles.size() == 0 ) return;

					vector<FileName> arCmd;
					for(vector<FileName>::iterator cur = arFiles.begin(); cur != arFiles.end(); ++cur)
						arCmd.push_back(*cur);
					ObjectCopierUI copier(this, arFiles, Directory(sDest));
					copier.Copy();
				}
				break;
			}				
		}
	}
}

void NavigatorTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{ // just here to prevent that default implementation 
	// prevents a WM_CONTEXTMENU to be sent
}
