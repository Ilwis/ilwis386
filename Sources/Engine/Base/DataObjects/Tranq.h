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
/* app/tranq.h
   Tranquilizer Interface for ILWIS 2.0
   october 1995, by Jelle Wind
	Last change:  WK   28 Sep 98    8:15 pm
*/

#ifndef ILWTRANQ_H
#define ILWTRANQ_H

#include "Engine\Base\System\HTP.H"
#include <Afxmt.h>

class Tranquilizer
{
  friend class ReportWindow;
  friend class ReportListCtrlItem;
  friend class ProgressListWindow;
  friend class ReportListCtrl;

public:
  _export Tranquilizer(); // Note:: use Start() to show report window
  _export Tranquilizer(const String& sTitle, const HelpTopic& htp = HelpTopic());
  _export ~Tranquilizer();

  void _export SetTitle(const String& sTitle);
  void _export setHelpItem(const HelpTopic& htp);
  void _export setHelpItem(const String& htp);
  void _export SetText(const String& sText);

  bool _export fUpdate(long iVal, long iMax = 0); // return true if stop has been pressed
  bool _export fText(const String& sText); // return true if stop has been pressed

  bool _export fAborted();
  void _export ResetAborted();
  void _export SetAborted();
  _export unsigned short iGetProgressID();

  void HelpEnable(bool) {} // dummy, should be handled by SetHelpItem()
	void _export SetDelayShow(bool fD);

  void _export Start();
  void _export Stop();
	void _export SetNoStopButton(bool fYesNo);
	void _export SetOnlyGauge(bool fChoice);
	bool _export fGetOnlyGauge();

private:
	void Init();
	String sTitle;
	String sTranqText;
	HelpTopic htp;
	bool fAbort;
	bool fStartUpdating;
	bool fDelayedShow;
	bool fStarted; // to prevent superflous calls to Start();
	bool fNoStopButton;
	bool fOnlyGauge;
	long iVal, iMax;
	String help;
	CCriticalSection cs, csTranq;
	CWnd *reportWindow;
	unsigned short iProgressID;
	static unsigned short iTranqIDCounter;
	bool fServerMode() const;
};

#endif







