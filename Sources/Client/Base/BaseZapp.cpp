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
#include "Client\Base\ZappToMFC.h"
#include "Client\Headers\formelementspch.h"

BaseZapp::BaseZapp(FormEntry *f) : _fe(f)
{
}

BaseZapp::~BaseZapp()
{
	_fe=NULL;
}

void BaseZapp::removeNotify(CallBackHandler *cbh, const Message& mp)
{
  NIter cur=Notifies.find(cbh, mp);
	if ( cur == NULL) return;
  if (cur == Notifies.end())
    throw ErrorObject("XXTrying to remove impossible Notification");
  Notifies.erase(cur->first, mp);
}

//processes events by matching the corresponding message with the Notifymap.
// if a match is found the notification function is called with the event as parm.
bool BaseZapp::fProcess(Event& ev)
{
  try
  {
		bool fRet=false;
		bool fProcessed=false;
    NotifyPair &np = Notifies[ev.message()]; // find match
    CallBackList &cbl=np.second;  
    if ( cbl.size()!=0) // if there is a match
    {
      DLIter<CallBackPair> iter(&cbl);
      for (int i=0; i<cbl.size(); ++i, ++iter) // process all defined functions
      {
        NotifyProc np=iter().second;
        CallBackHandler *wnd = iter().first;
        if (wnd!=NULL && np!=NULL)
        {
          (wnd->*np)(&ev);
					fProcessed = true;
				}
        else
          throw ErrorObject("XXUninitialised notification");
      }
      fRet = true;; // something could be processed
    }
		if ( !fProcessed && _fe)
    {
			// exclude some messages to trigger callbacks
			if ( ev.iMessage != WM_MOUSEMOVE &&
				   ev.iMessage != WM_PAINT &&
					 ev.iMessage != WM_DRAWITEM ) 
			   _fe->DoCallBack();
    }
    return fRet;  // no functions found
  }
  catch(ErrorObject& err)
  {
    err.Show();
  }
  return true;
}        

void BaseZapp::setNotify(CallBackHandler *handler, NotifyProc npCh,  const Message& mp)
{
  NotifyPair &np = Notifies[mp]; 
  np.second.insert(CallBackPair(handler, npCh));
  np.first=mp;
}




