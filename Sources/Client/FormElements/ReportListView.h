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
#ifndef REPLISTCTRL_H
#define REPLISTCTRL_H

const int ID_REPLIST = 1897;

class ReportListCtrl;
class ProgressListWindow;

class ReportListCtrlItem
{
public:
	ReportListCtrlItem(ReportListCtrl *, Tranquilizer *tr, int iItem);
	ReportListCtrlItem();
	void Update();
	void DrawProgressBar();
	void Update(String& sNumber);
	unsigned short iGetProgressID();
	void SetListItemIndex(int iIndex);
	int iGetListItemIndex();
	void SetTextOnly();
	void UpdateText();
	void SetTitle();
	void ShowHelp();
	void StopOperation();
	~ReportListCtrlItem();
	void SetDrawStaticColumns(bool fSet);

private:	
	CProgressCtrl *pcProgress;
	Tranquilizer *trq;
	ReportListCtrl *rlcList;
	int iListItem;
	bool fDrawStaticColumns;;
	long iOldVal;
};

#include <afxcmn.h>

class ProgressListHeaderCtrl : public CHeaderCtrl
{
	DECLARE_MESSAGE_MAP()
public:
	ProgressListHeaderCtrl();
	ProgressListHeaderCtrl(ReportListCtrl *rlc);
	virtual ~ProgressListHeaderCtrl();
	BOOL OnHeaderSize(NMHDR* pNMHDR, LRESULT* pResult);	

private:
	ReportListCtrl	*list;
};


class ReportListCtrl : public CListCtrl
{
	public:
		ReportListCtrl();
		~ReportListCtrl();

		int Create(CWnd *wnd, CRect rct);
		void AddTopItem(Tranquilizer *trq);
		void RemoveItem(unsigned short i);
		void Update(unsigned short iID);
		void DrawProgressBar();
		void UpdateItems(ProgressListWindow *);
		int  iNumberOfItems();
		unsigned short iGetRelevantProgressID(int iListIndex);
		void SetTextOnly(unsigned short iProgressId);
		void UpdateText(unsigned short iProgressId);
		void SetTitle(unsigned short iProgressId);
		void ShowHelp();
		void SaveSettings(IlwisSettings& settings);

	private:
		void MoveListIDOfItems(int iIndex);
		
		map<unsigned short, ReportListCtrlItem *> mpListItems;
		static CCriticalSection cs;
		ProgressListHeaderCtrl *rlcHeader;

	DECLARE_MESSAGE_MAP()
};


#endif
