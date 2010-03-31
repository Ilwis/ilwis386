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
/* $Log: /ILWIS 3.0/Tools/TRANQ.CPP $
 * 
 * 16    10-05-04 18:42 Retsios
 * Now sending the appropriate message to the report window when the
 * tranquilizer text has changed. With the "old style tranquilizers" this
 * was not needed as the text was always re-displayed with an fUpdate
 * call.
 * 
 * 15    11/12/03 3:37p Martin
 * fAbort flag was never set in the Start function. It retained the value
 * of the previous run while it needed to be reset
 * 
 * 14    10/09/03 3:42p Martin
 * ProgressID can only be assigned once, multiple starts on a trq will not
 * change the ProgressID, else the administration at the
 * progresslistwindow gets confused.
 * Second, wrong message was sent for textonly trq's, is corrected
 * 
 * 13    10/03/03 2:17p Martin
 * tranquilizer differentiates the use of the type of reportwindow by
 * setting the iProgressID
 * 
 * 12    6/28/01 12:55p Martin
 * it is now possible to tell the trq that it should only show the gauge
 * (with the normal string) but not the counter string. Sometimes this
 * takes too much space and it is counter productive
 * 
 * 11    1-11-00 4:04p Martin
 * trq's can now exist wothout a stop button ( on reportwindow )
 * 
 * 10    28-04-00 11:42 Koolhoven
 * Stopping the tranquilizer deletes the reportwindow, so set the pointer
 * to zero in the Stop() function
 * 
 * 9     22/03/00 17:03 Willem
 * Removed obsoleted functions (fWarning() and Message() etc)
 * 
 * 8     2-12-99 11:15 Wind
 * debugged deadlock situation with  report window when there is a very
 * short period between Start() and Stop()
 * 
 * 7     17-11-99 14:01 Wind
 * bug for title and for not displaying progress bar
 * 
 * 6     10-11-99 12:14p Martin
 * Sending of messages has changed. Most go to the report window
 * 
 * 5     23-07-99 18:09 Koolhoven
 * Added sleep(1) calls, to prevent that the main thread gets not enough
 * time slices to do the redrawing of the ReportWindow.
 * So each call to fUpdate etc. costs at least 1 ms
 * 
 * 4     2-06-99 11:01 Koolhoven
 * Corrected for multithreading
 * 
 * 3     27-04-99 16:47 Koolhoven
 * Header comment
 * 
 * 2     3/31/99 12:17p Martin
 * Blocked any attempt to send messages as long as iRepWin==0
// Revision 1.2  1997/07/24 13:55:48  Wim
// Set helptopic with ReportWindow
//
/* app/tranq.c
   Tranquilizer Interface for ILWIS 2.0
   october 1995, by Jelle Wind
	Last change:  WK   28 Sep 98    1:11 pm
*/
#include "Headers\toolspch.h"
#include "Headers\messages.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\Tranq.h"

unsigned short Tranquilizer::iTranqIDCounter = 0;

void Tranquilizer::Init()
{
  fAbort = false;
  fStartUpdating = fDelayedShow = true;
  iVal = 0;
  iMax = 0;
  reportWindow = 0;
  fStarted = false;
  fNoStopButton = false;
  fOnlyGauge = false;
  iProgressID = 0;
}

Tranquilizer::Tranquilizer()
{
  Init();
}

Tranquilizer::Tranquilizer(const String& sTitl, const HelpTopic& _htp)
{
  Init();
  sTitle = sTitl;
  htp = _htp;
  Start();
}


Tranquilizer::~Tranquilizer()
{
  Stop();
  // wait till ReportWindow is finished
	TRACE("entering lock for tranquilizer =>");
  ILWISSingleLock cslTranq(&csTranq, TRUE,SOURCE_LOCATION);
	TRACE("leaving lock for tranquilizer \n");
}

void Tranquilizer::Start()
{
//	cs.Lock();
	if (!fStarted ) // to protect against multiple, incorrect use of a tranquilizer
	{
		iProgressID = ++iTranqIDCounter;	
	}		
	fStarted = true;
	fAbort = false; //  if you start you are not aborted
	if (reportWindow == 0 )
	{
		int	iMessage = MAKELONG(ILWREP_START, iProgressID);		
		reportWindow = reinterpret_cast<CWnd*> (AfxGetApp()->GetMainWnd()->SendMessage(ID_ILWISREP, iMessage, (LPARAM)this));				
	}	  

//	cs.Unlock();
}

void Tranquilizer::SetNoStopButton(bool fYesNo)
{
	fNoStopButton = fYesNo;
}

void Tranquilizer::Stop()
{
//  cs.Lock();
	if (fStarted && reportWindow != 0 && reportWindow->GetSafeHwnd() != NULL)
	{
		int iMessage = MAKELONG(ILWREP_STOP, iProgressID);
		AfxGetApp()->GetMainWnd()->SendMessage(ID_ILWISREP, iMessage, (LPARAM)reportWindow);
	}		
	fStarted = false;
	reportWindow = 0;
//  cs.Unlock();
}

void Tranquilizer::SetTitle(const String& sTitl)
{
	cs.Lock();
	sTitle = sTitl;
	cs.Unlock();
	if (reportWindow != 0 && reportWindow->GetSafeHwnd() != NULL)
	{
		int	iMessage = MAKELONG(ILWREP_TITLE, iProgressID);
		reportWindow->PostMessage(ID_ILWISREP, iMessage);
	}		
}

void Tranquilizer::SetText(const String& sText)
{
  cs.Lock();
  sTranqText = sText;
  iVal = iUNDEF;	
  cs.Unlock();
	if (reportWindow != 0 && reportWindow->GetSafeHwnd() != NULL)
	{
		int	iMessage = MAKELONG(ILWREP_TEXT, iProgressID);
		reportWindow->PostMessage(ID_ILWISREP, iMessage);
	}
}

void Tranquilizer::SetHelpTopic(const HelpTopic& _htp)
{
  cs.Lock();
  htp = _htp;
  cs.Unlock();
}

bool Tranquilizer::fUpdate(long _iVal, long _iMax) // return true if stop has been pressed
{
  if (0 == reportWindow && !fStarted)
    Start();
  cs.Lock();
  iVal = _iVal;
  iMax = _iMax;
  cs.Unlock();
	if (reportWindow != 0 && reportWindow->GetSafeHwnd() != NULL && fStartUpdating)
	{
		fStartUpdating = false;
		int iMessage = MAKELONG(ILWREP_UPDATE, iProgressID);
		reportWindow->PostMessage(ID_ILWISREP, iMessage);
	}
  cs.Lock();
  bool fAb = fAbort;
  cs.Unlock();
  return fAb;
}

bool Tranquilizer::fText(const String& sText) // return true if stop has been pressed
{
  if (0 == reportWindow && !fStarted)
    Start();
  cs.Lock();
  sTranqText = sText;
  iVal = iUNDEF;	
  cs.Unlock();
  if (reportWindow != 0 && reportWindow->GetSafeHwnd() != NULL )
  {
	int iMessage = MAKELONG(ILWREP_TEXT, iProgressID);	  
	reportWindow->PostMessage(ID_ILWISREP, iMessage);
  }
  cs.Lock();
  bool fAb = fAbort;
  cs.Unlock();
  return fAb;
}


bool Tranquilizer::fAborted()
{
  cs.Lock();
  bool fAb = fAbort;
  cs.Unlock();
	//Sleep(1);
  return fAb;
}

void Tranquilizer::ResetAborted()
{
  cs.Lock();
  fAbort = false;
  cs.Unlock();
	//Sleep(1);
}

void Tranquilizer::SetAborted()
{
  cs.Lock();
  fAbort = true;
  cs.Unlock();
	//Sleep(1);
}

void Tranquilizer::SetDelayShow(bool fD)
{
	fDelayedShow = fD;
}

void Tranquilizer::SetOnlyGauge(bool fChoice)
{
	fOnlyGauge = fChoice;
}

bool Tranquilizer::fGetOnlyGauge()
{
	return fOnlyGauge;
}

unsigned short Tranquilizer::iGetProgressID()
{
	return iProgressID;
}




