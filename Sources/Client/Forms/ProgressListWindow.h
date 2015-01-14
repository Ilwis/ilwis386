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
/* ReportWindow
reponsible for the user interface part of a tranquilizer
Last change:  WK   14 Jan 99   10:22 am
*/

#ifndef PROGRESSLIST_H
#define PROGRESSLIST_H

#undef IMPEXP
#ifdef PROGRESSLIST_C
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP ProgressListWindow: public CFrameWnd
{
	friend UINT ProgressListWindow::UpdateInThread(LPVOID data);

public:
	ProgressListWindow();
	void Create();
	~ProgressListWindow();
	void SetTitle(unsigned short);
	void SetText(unsigned short);
	void UpdateText(unsigned short);
	BOOL OnInitDialog();
	void OnCancel();
	BOOL OnEraseBkgnd(CDC *);
	void OnSize(UINT nType, int cx, int cy);
	void OnHelp();
	afx_msg void OnSysCommand(UINT, LPARAM);
	void Stop(unsigned short iProgressID);
	void Start(Tranquilizer *tr);
	void UpdateItems();
	BOOL DestroyWindow();	
protected:
	String help;

private:
	void sUpdate(String& sName, String& sNumber);
	void Update(int iID);
	LONG OnIlwisReportMsg(UINT iRepCode, LONG lp);
	static UINT UpdateInThread(LPVOID data);
	void OnTimer(UINT iIDEvent);
	void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	bool fDelayedShow();
	void Init();  
	CStatic stText;
	CButton butAbort, butHelp;

	ReportListCtrl rlvTrqList;
	HANDLE threadHandle;
	time_t  iFirstTime;
	bool fUpdateLoop;
	bool fShown;
	static int iNrReportWindows; // Number of report windows on the screen
	int iMyNumber; // A number that represents the howmanieth report window I am

	DECLARE_MESSAGE_MAP()
};

#endif // REPWIN_H
