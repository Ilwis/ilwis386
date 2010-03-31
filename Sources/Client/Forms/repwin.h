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

#ifndef REPWIN_H
#define REPWIN_H

#undef IMPEXP
#ifdef REPWIN_C
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP ReportWindow: public CDialog
{
	friend UINT ReportWindow::UpdateInThread(LPVOID data);

public:
	ReportWindow(Tranquilizer* tranq);
  void Create(Tranquilizer*);
  ~ReportWindow();
  void SetTitle();
  void SetText();
  BOOL OnInitDialog();
  void OnCancel();
  void OnHelp();
  afx_msg void OnSysCommand(UINT, LPARAM);
	void Stop();

private:
  String sUpdate();
  void Update();
	LONG OnIlwisReportMsg(UINT iRepCode, LONG lp);
	void DelayedShow();
  void UpdateIconic();
	static UINT UpdateInThread(LPVOID data);
  Tranquilizer* tr;
  CStatic stText;
  CProgressCtrl prCtrl;
  CButton butAbort, butHelp;
  ILWISSingleLock cslTranq;
	HANDLE threadHandle;
	time_t  iFirstTime;
	bool fUpdateLoop;
	void CheckPosition(int& iX, int& iY, int& iW, int& H);
	bool fShown;
	static int iNrReportWindows; // Number of report windows on the screen
	int iMyNumber; // A number that represents the howmanieth report window I am
	DECLARE_MESSAGE_MAP()
};

//class ReportThread : public CWinThread
//{
//	public:
//		ReportThread(Tranquilizer *tr);
//
//
//	private:
//		virtual BOOL InitInstance();
//		virtual int ExitInstance();
//
//		Tranquilizer *trq;
//		ReportWindow repWin;
//};

#endif // REPWIN_H
