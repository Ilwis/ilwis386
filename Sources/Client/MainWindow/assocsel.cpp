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
#include "Client\Base\Res.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\MainWindow\ASSOCSEL.H"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CollectionCatalog.h"
#include "Client\MainWindow\Catalog\DataBaseCatalog.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Client\MainWindow\ACTPAIR.H"

//#define pmadd(ID) pm->addStr(ID, sMen(ID))
#define sMen(ID) ILWSF("men",ID).scVal()
#define pmadd(ID) AppendMenu(MF_STRING, ID, sMen(ID)); 

void AssociationSelector::AddOperMenuItems(CMenu& men, const ActionList* actList, const ActionPair* ap, const String& sExt, int& iDfltOption)
{
	CMenu menSub;
	int iLastOpt;
	int id = ID_OPERATION1;
	String sLastOpt = "-";
	String sLastSubOpt = "-";
	int iSub = 0;
	for (SLIterCP<Action> iter(actList); iter.fValid(); ++iter, ++id) {
		Action* act = iter();
		if (!act->fExtension(sExt))
			continue;
		if (0 == iDfltOption && 0 != ap && ap->fAction(act))
			iDfltOption = id;
		if ("-" == act->sMenOpt() ||
				"open" == act->sAction() ||
				"tbl" == act->sAction())
			continue;    
		if (sLastOpt == act->sMenOpt()) {
			String s = act->sSubOpt();
			if ( s == "") continue;
			s &= "...";
			iSub += 1;
			menSub.AppendMenu(MF_STRING, id, s.scVal());
		}  
		else {
			if (menSub.m_hMenu) {
				if (iSub == 1) {
					String s = sLastSubOpt;
					if ( s != "") {
						s &= "...";
						men.AppendMenu(MF_STRING, iLastOpt, s.scVal());
						menSub.DestroyMenu();
					}
				}
				else {
					men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sLastOpt.scVal()); 
					menSub.Detach();
				}
				iSub = 0;
			}
			if ("" == act->sMenOpt()) {
				String s = act->sSubOpt();
				if ( s == "") continue;
				s &= "...";
				men.AppendMenu(MF_STRING, id, s.scVal());
				sLastOpt = "-";
				iLastOpt = id;
			}
			else {
				menSub.CreateMenu();
				sLastOpt = act->sMenOpt();
				sLastSubOpt = act->sSubOpt();
				iSub += 1;
				String s = act->sSubOpt();
				if ( s == "") continue;
				s &= "...";
				menSub.AppendMenu(MF_STRING, id, s.scVal());
				iLastOpt = id;
			}
		}
	}
	if (menSub.m_hMenu) {
		if (iSub == 1) {
			String s = sLastSubOpt;
			if ( s == "") {
			s &= "...";
				men.AppendMenu(MF_STRING, iLastOpt, s.scVal());
				menSub.DestroyMenu();
			}
		}
		else {
			men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sLastOpt.scVal()); 
			menSub.Detach();
		}
	}
}

AssociationSelector::AssociationSelector(CWnd* w, zPoint p,
		 const ActionList& aList, const FileName& fn, AssociationSelector** as, int iDfltOption)
		 : fnAct(fn), actList(aList)
{
	if (as)
		*as = this;

	String sExt = fn.sExt;
	if (".mpl" == fn.sExt && fn.sSectionPostFix != "") 
		sExt = ".mpr";
	
/*
	Catalog* cat = dynamic_cast<Catalog*>(w);
	if (cat) {
		cat->assSel = this;
	}
*/
	String s;
	ActionPair* ap = IlwWinApp()->apl()->ap(s, fn);

	CreatePopupMenu();

	String sType;
	pmadd(ID_CAT_SHOW);

	AddOperMenuItems(*this, &aList, ap, sExt, iDfltOption);

	if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION)
		pmadd(ID_CAT_ASLAYER);
	if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION || IOTYPE(fn) == IlwisObject::iotMAPLIST)
		pmadd(ID_CAT_ASANIMATION);

	AppendMenu(MF_SEPARATOR);

	bool fOpenAs = false;
	if ((".dom" != sExt) && ObjectInfo::fDomain(fn, false)) 
	{
		fOpenAs = true;
		pmadd(ID_CAT_SHOWASDOM);
	}
	if ((".rpr" != sExt) && ObjectInfo::fRepresentation(fn))
	{
		fOpenAs = true;
		pmadd(ID_CAT_SHOWASRPR);
	}
	if ((".tbt" != sExt) && (".ta2" != sExt) && 
			(".his" != sExt) && (".hsa" != sExt) && 
			(".hss" != sExt) && (".hsp" != sExt) && 
			(".atx" != sExt) && (".dom" != sExt) &&
			(".mps" != sExt) && (".mpa" != sExt) &&
			ObjectInfo::fTable(fn)) 
	{
		fOpenAs = true;
		pmadd(ID_CAT_SHOWASTBL);
	}
	if (fOpenAs) 
		AppendMenu(MF_SEPARATOR);

	pmadd(ID_CAT_PROP);
	if ( ".tbt" == sExt) {
		DataBaseCatalog *cat = dynamic_cast<DataBaseCatalog *>(w);
		if ( cat) {
			pmadd(ID_CAT_TABLEVIEW);
		}

	}
	if (".mpr" == sExt || ".mpa" == sExt || 
			".mps" == sExt || ".mpp" == sExt) {
		pmadd(ID_CAT_EDIT);  
		if (_access(fn.sFullPath(true).scVal(),2)!=0 || ObjectInfo::fVirtual(fn))
			EnableMenuItem(ID_CAT_EDIT, MF_GRAYED|MF_BYCOMMAND);
	}  
	if (".stp" == sExt) {
		pmadd(ID_CAT_EDIT);  
		if (_access(fn.sFullPath(true).scVal(),2)!=0 || !ObjectInfo::fVirtual(fn))
			EnableMenuItem(ID_CAT_EDIT, MF_GRAYED|MF_BYCOMMAND);
	}  
	pmadd(ID_EDIT_COPY);
//	pmadd(ID_CAT_COPY);
	pmadd(ID_CAT_DEL );
//	EnableMenuItem(ID_CAT_DEL, MF_GRAYED|MF_BYCOMMAND);
	pmadd(ID_CAT_HELP);
//  pmadd(ID_CAT_REN );

	if (0 == iDfltOption)
		iDfltOption = ID_CAT_SHOW;
	SetDefaultItem(iDfltOption);

  int iCmd = TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, w);
	ProcessCmd(iCmd, fn);
	if (as)
		*as = 0;
}

AssociationSelector::AssociationSelector(CWnd* w, zPoint p,
		 const ActionList& aList, const Action* action)
		 : actList(aList)
{
	CreatePopupMenu();
  String sType;
  pmadd(ID_ACT_RUN);
  pmadd(ID_ACT_HELP);
  if (action->iHelp() <= 0)
		EnableMenuItem(ID_ACT_HELP, MF_GRAYED|MF_BYCOMMAND);
  TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, w);
}

void AssociationSelector::ProcessCmd(int iCmd, const FileName& fn)
{
	String sExt = fn.sExt;
	if (".mpl" == fn.sExt && fn.sSectionPostFix != "") 
		sExt = ".mpr";

	String sCmd;
	switch (iCmd) {
		case 0:
			return;
		case ID_CAT_OPENHERE:
			sCmd = String("cd %S", fn.sFile);
			break;
		case ID_CAT_SHOW:
			if ("drive" == sExt || "directory" == sExt) 
				sCmd = String("opendir %S", fn.sFile);
			else
				sCmd = String("show %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_ASLAYER:
			sCmd = String("open %S -layer", fn.sFullPathQuoted());
			break;
		case ID_CAT_ASANIMATION:
			sCmd = String("open %S -animation", fn.sFullPathQuoted());
			break;
		case ID_CAT_SHOWASDOM:
			sCmd = String("showasdom %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_SHOWASRPR:
			sCmd = String("showasrpr %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_SHOWASTBL:
			sCmd = String("showastbl %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_EDIT:
			sCmd = String("edit %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_PROP:
			sCmd = String("prop %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_COPY:
			sCmd = String("copy %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_DEL:
			sCmd = String("del %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_HELP:
			sCmd = String("popuphelp %S", fn.sFullPathQuoted());
			break;
		case ID_CAT_TABLEVIEW:
			sCmd = String("createview %S", fn.sFullPathQuoted());
			break;
		case ID_EDIT_COPY:
			{
				int	iFmtCopy = RegisterClipboardFormat("IlwisCopy");
				String sFiles = fn.sFullPathQuoted() + "|";
				zClipboard clip(AfxGetMainWnd());
				clip.clear();
				clip.add(sFiles.sVal(), iFmtCopy);
			} break;
		default:
	    sCmd = actList[iCmd-ID_OPERATION1]->sExec(fn);
			break;
	}
	if (sCmd == "")
		return;
	IlwisWinApp* iwa = IlwWinApp();
	char* str = sCmd.sVal();
	iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

void AssociationSelector::OnMenuSelect(CWnd* wnd, UINT nItemID, UINT nFlags)
{
	CFrameWnd* pFrameWnd = wnd->GetTopLevelFrame();
	CWnd* pMessageBar = pFrameWnd->GetMessageBar();
	String sMsg;

	if (nFlags & (MF_SEPARATOR|MF_POPUP)) {
		// do nothing!
	}
  else if (nItemID >= ID_OPERATION1 && nItemID < ID_OPERATION1 + 499) 
	{
    String sFileName("\"%S\"", fnAct.sShortName(false));
    String sDscS = actList[nItemID-ID_OPERATION1]->sDescriptionS();
    sMsg = String(sDscS.scVal(), sFileName);
  }
  else if (nItemID >= ID_CAT_START && nItemID <= ID_CAT_LAST) 
	{
    String sBase = ILWSF("dsc", nItemID);
    String sDescr;
    ObjectInfo::ReadElement("Ilwis", "Type", fnAct, sDescr);
    if (sDescr == "BaseMap")
      ObjectInfo::ReadElement("BaseMap", "Type", fnAct, sDescr);
    else if (sDescr == "Function")
      ObjectInfo::ReadElement("Function", "Type", fnAct, sDescr);
    String str("%S \"%S\"", sDescr, fnAct.sFile); 
    sMsg = String(sBase.scVal(), str);
  }

	if (pMessageBar)
		pMessageBar->SetWindowText(sMsg.sVal());
}

