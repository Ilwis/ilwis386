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

#include "Headers\toolspch.h"
#include "Headers\htmlhelp.h"
#include "Client\Help\ChmFinder.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\messages.h"
#include "Client\FormElements\ReportListView.h"
#include "Client\Forms\ProgressListWindow.h"

const int ID_PROGRESS_LIST_HEADER = 589;

CCriticalSection ReportListCtrl::cs;

ReportListCtrlItem::ReportListCtrlItem() :
	trq(NULL),
	rlcList(NULL),
	pcProgress(NULL),
	iListItem(0),
	fDrawStaticColumns(true)
{
	
}

ReportListCtrlItem::ReportListCtrlItem(ReportListCtrl *rlc, Tranquilizer *tr, int iItem) :
	trq(tr),
	rlcList(rlc),
	iListItem(iItem),
	fDrawStaticColumns(true)
{
	pcProgress = new CProgressCtrl();		
	pcProgress->Create(WS_CHILD | PBS_SMOOTH | WS_BORDER, CRect(0,0,0,0), rlc, 100 + tr->iGetProgressID());
	pcProgress->SetRange(0,1000);
	pcProgress->ShowWindow(FALSE);

	CRect rctItem;
	rlcList->GetSubItemRect(iListItem, 1, LVIR_BOUNDS, rctItem);
	if(rctItem.Width() > 0 && rctItem.Height() > 0)
	{
		rctItem.InflateRect(1, 1, 1, 1);
		pcProgress->MoveWindow(rctItem);
	}
}

void ReportListCtrlItem::Update(String& sNumber)
{
	ILWISSingleLock sl(&trq->cs, TRUE, SOURCE_LOCATION);
	if ( trq->fGetOnlyGauge() )
	{

		sNumber = "";

	}		
	if (trq->iVal == iUNDEF) 
	{
		sNumber = "";
	}		
	else //if (trq->iMax == 0)
	{
		sNumber = String("%li", trq->iVal);
	}		
//	else
//	{
//		sNumber = String("%4.1f%%",(trq->iVal * 100.0) / trq->iMax);
//	}	
}

void ReportListCtrlItem::SetDrawStaticColumns(bool fSet)
{
	fDrawStaticColumns = fSet;
}

void ReportListCtrlItem::ShowHelp()
{
	//String sHelpFile (ChmFinder::sFindChmFile("ilwis.chm"));

	//int iHtp = trq->htp.iTopic;
	//HtmlHelp(::GetDesktopWindow(), sHelpFile.sVal(), HH_HELP_CONTEXT, iHtp);
	//HtmlHelp(0, sHelpFile.sVal(), HH_DISPLAY_TOC, 0);
}

void ReportListCtrlItem::DrawProgressBar()
{
	CRect rctItem;
	rlcList->GetSubItemRect(iListItem, 1, LVIR_BOUNDS, rctItem);
	if(rctItem.Width() > 0 && rctItem.Height() > 0)
	{
		rctItem.InflateRect(1,1,1,1);
		pcProgress->MoveWindow(rctItem);
	}		
}
unsigned short ReportListCtrlItem::iGetProgressID()
{
	if ( trq)
		return trq->iGetProgressID();
	return 0;
}

void ReportListCtrlItem::Update()
{

	trq->cs.Lock();
	String sNumber;
	Update(sNumber);
	long iVal = trq->iVal;
	long iMax = trq->iMax;
	trq->cs.Unlock();

	if ( iVal == 0)
		fDrawStaticColumns = true;

	if ( fDrawStaticColumns )
	{
		String sName =  trq->sTranqText;	
		rlcList->SetItemText(iListItem,3, sName.sVal());
		rlcList->SetItemText(iListItem,0,trq->sTitle.sVal());
		fDrawStaticColumns = false;
    }
	if ( sOldNumber != sNumber)
		rlcList->SetItemText(iListItem,2, sNumber.sVal());
	sOldNumber = sNumber;	

	if ((iMax > 0) && (iVal != iUNDEF))
	{
		pcProgress->ShowWindow(TRUE);		
		double rVal = iVal;
		rVal /= iMax;
		rVal *= 1000;
		if ( pcProgress)
			pcProgress->SetPos(rVal);
	}
}

int ReportListCtrlItem::iGetListItemIndex()
{
	return iListItem;
}

void ReportListCtrlItem::SetListItemIndex(int iIndex)
{
	iListItem = iIndex;
}

ReportListCtrlItem::~ReportListCtrlItem()
{
	delete pcProgress;
	pcProgress = 0;
}

void ReportListCtrlItem::SetTextOnly()
{
	pcProgress->ShowWindow(SW_HIDE);		
	trq->cs.Lock();
	rlcList->SetItemText(iListItem,0,trq->sTitle.sVal());
	rlcList->SetItemText(iListItem, 3, trq->sTranqText.sVal());
	trq->cs.Unlock();
}

void ReportListCtrlItem::SetTitle()
{
	pcProgress->ShowWindow(SW_HIDE);		
	trq->cs.Lock();
	rlcList->SetItemText(iListItem,0,trq->sTitle.sVal());
	trq->cs.Unlock();
}

void ReportListCtrlItem::StopOperation()
{
	trq->SetAborted();
}


BEGIN_MESSAGE_MAP(ProgressListHeaderCtrl, CHeaderCtrl)
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGED, OnHeaderSize)
END_MESSAGE_MAP()

BOOL ProgressListHeaderCtrl::OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* pnmh = (NMHEADER*)pNMHDR;
	list->DrawProgressBar();

	return TRUE;
}


ProgressListHeaderCtrl::ProgressListHeaderCtrl(ReportListCtrl *rlc) :
	list(rlc)

{

}

ProgressListHeaderCtrl::~ProgressListHeaderCtrl()
{
}

BEGIN_MESSAGE_MAP(ReportListCtrl, CListCtrl)
END_MESSAGE_MAP()


ReportListCtrl::ReportListCtrl() :
	CListCtrl(),
	rlcHeader(NULL)		
{

}

ReportListCtrl::~ReportListCtrl()
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	list<unsigned short> lst;
	// collect progress id in a seperate loop. Else you are erasing stuff in the list you are just checking
	for(map<unsigned short, ReportListCtrlItem *>::iterator cur=mpListItems.begin(); cur != mpListItems.end(); ++cur)
	{
		lst.push_front((*cur).second->iGetProgressID()); 			
	}
	//remove collected progress id
	for(list<unsigned short>::iterator cur2 = lst.begin(); cur2 != lst.end(); ++cur2)
		RemoveItem(*cur2);
	
	delete rlcHeader;

}

int ReportListCtrl::Create(CWnd *parent, CRect rct)
{
	int iRet = CListCtrl::Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_SINGLESEL | LVS_REPORT | WS_HSCROLL, rct, parent, ID_REPLIST);
	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	rlcHeader = new ProgressListHeaderCtrl(this);
	
	CHeaderCtrl *hdr = GetHeaderCtrl();

	rlcHeader->SubclassWindow(hdr->GetSafeHwnd());
	IlwisSettings settings("ProgressWindow");
	CRect rect;
	rect = settings.rctValue("Operation");
	int iW = rect.Width() > 0 ? rect.Width() : 150;
	InsertColumn(0, "Operation", LVCFMT_LEFT, iW);

	rect = settings.rctValue("Progress");
	iW = rect.Width() > 0 ? rect.Width() : 80;
	InsertColumn(1, "Progress", LVCFMT_LEFT, iW);

	rect = settings.rctValue("Completed");
	iW = rect.Width() > 0 ? rect.Width() : 60;
	InsertColumn(2, "Completed", LVCFMT_LEFT, iW);

	rect = settings.rctValue("Info");
	iW = rect.Width() > 0 ? rect.Width() : 250;
	InsertColumn(3, "Info", LVCFMT_LEFT, 250);

	return iRet;
}

void ReportListCtrl::AddTopItem(Tranquilizer *tr)
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
    item.pszText = tr->sTitle.sVal();
	item.lParam = tr->iGetProgressID();
	item.iItem = mpListItems.size();
	item.iSubItem = 0;
	InsertItem(&item);
	mpListItems[tr->iGetProgressID()] = new ReportListCtrlItem(this, tr, item.iItem);	

}

void ReportListCtrl::SetTextOnly(unsigned short iProgressId)
{
	map<unsigned short, ReportListCtrlItem *>::iterator where = mpListItems.find(iProgressId);
	if ( where != mpListItems.end())	
	{
		(*where).second->SetTextOnly();
	}		
}

void ReportListCtrl::SetTitle(unsigned short iProgressId)
{
	map<unsigned short, ReportListCtrlItem *>::iterator where = mpListItems.find(iProgressId);
	if ( where != mpListItems.end())	
	{
		(*where).second->SetTitle();
	}		
}

void ReportListCtrl::RemoveItem(unsigned short iProgressId)
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);	
	int iIndex = 0;
	for(; iIndex < mpListItems.size(); ++iIndex)
	{
		LVITEM item;
		item.mask = LVIF_PARAM;
		item.iItem = iIndex;
		item.iSubItem = 0;
		GetItem(&item);
		if ( item.lParam == iProgressId)
			break;
	}		
	if ( iIndex != mpListItems.size())
	{
		DeleteItem(iIndex);
		MoveListIDOfItems(iIndex);
		map<unsigned short, ReportListCtrlItem *>::iterator where = mpListItems.find(iProgressId);
		if ( where != mpListItems.end())
		{
			mpListItems[iProgressId]->StopOperation();
			delete mpListItems[iProgressId];		
			mpListItems.erase(iProgressId);
		}			
	}
}

void ReportListCtrl::MoveListIDOfItems(int iIndex)
{
	for(map<unsigned short, ReportListCtrlItem *>::iterator cur=mpListItems.begin(); cur != mpListItems.end(); ++cur)
	{
		int id = (*cur).second->iGetListItemIndex();
		if ( id > iIndex)
		{
			(*cur).second->SetListItemIndex(--id);
			(*cur).second->SetDrawStaticColumns(true);			
			(*cur).second->DrawProgressBar();			
		}			
	}		
}

unsigned short ReportListCtrl::iGetRelevantProgressID(int iListIndex)
{
	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem = iListIndex;
	item.iSubItem = 0;
	GetItem(&item);	
	return item.lParam;
}

void ReportListCtrl::Update(unsigned short iID)
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	map<unsigned short, ReportListCtrlItem *>::iterator where = mpListItems.find(iID);
	if ( where != mpListItems.end())
		mpListItems[iID]->Update();
}

void ReportListCtrl::DrawProgressBar()
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);
	for(map<unsigned short, ReportListCtrlItem *>::iterator cur=mpListItems.begin(); cur != mpListItems.end(); ++cur)
	{
		(*cur).second->DrawProgressBar();
	}	
}

void ReportListCtrl::UpdateItems(ProgressListWindow *plw)
{
	ILWISSingleLock lock(&cs, TRUE, SOURCE_LOCATION);	
	int iS = mpListItems.size();
	for(map<unsigned short, ReportListCtrlItem *>::iterator cur=mpListItems.begin(); cur != mpListItems.end(); ++cur)
	{
		plw->PostMessage(ID_ILWISREP, MAKELONG(ILWREP_UPDATE, (LPARAM)(*cur).second->iGetProgressID()), 0); 			
	}		
}

int ReportListCtrl::iNumberOfItems()
{
	return mpListItems.size();
}

void ReportListCtrl::ShowHelp()
{
	if ( GetSelectedCount() > 0 )
	{
		
		POSITION pos = GetFirstSelectedItemPosition();
		if (pos == NULL)
			return ; // nothing selected

		int id = GetNextSelectedItem(pos);
		unsigned short iProgressID =  iGetRelevantProgressID(id);
		mpListItems[iProgressID]->ShowHelp();
	}	
}

void ReportListCtrl::SaveSettings(IlwisSettings& settings)
{
	CRect rctItem;
	CHeaderCtrl *hdr = GetHeaderCtrl();
	hdr->GetItemRect(0, rctItem);
	settings.SetValue("Operation", rctItem);
	GetSubItemRect(0, 1, LVIR_BOUNDS, rctItem);	
	settings.SetValue("Progress", rctItem);	
	GetSubItemRect(0, 2, LVIR_BOUNDS, rctItem);	
	settings.SetValue("Completed", rctItem);
	GetSubItemRect(0, 3, LVIR_BOUNDS, rctItem);	
	settings.SetValue("Info", rctItem);		

}
