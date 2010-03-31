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
#include "Client\Base\NfyMap.h"
#include "Engine\Base\AssertD.h"


//--[ CallBackList ]-------------------------------------------------------------------------
//For every message there is a list of functions that should be performed when an event is triggered.
//The Callbacklist contains the list of functions plus the handler object. (CallBackPairs)
void CallBackList::insert(const CallBackPair& cbp)
{
  PRECONDITION(fINotEqual, cbp.first, (CallBackHandler *)NULL);

  // search for handler; if exists than replace the old function; if not add a new pair
  CallBackPair *oldCbp = find(cbp.first); 

  if ( oldCbp != NULL)
    oldCbp->second = cbp.second;
  else
    append(cbp);

  POSTCONDITION(fIGreater, iSize(), 0L);

}

CallBackPair *CallBackList::find(CallBackHandler *cbh)
{
  PRECONDITION(fINotEqual, cbh, (CallBackHandler *)NULL);

  DLIter<CallBackPair> iter(this);
  for(int i=0; i<size(); ++i, ++iter)
  {
    CallBackPair& curPair = iter();
    if ( curPair.first == cbh )
      return &curPair;
  }
  return NULL;
}


//--[ NotifyMap ]-------------------------------------------------------------------------
NotifyMap::NotifyMap(int size)
{
	arNotifies.Resize(size);
}

NotifyPair& NotifyMap::operator [] (const Message& mp)
{
  int iFirstEmpty=-1;
  int i=0;
  for(; i < arNotifies.iSize(); ++i)
	{
    NotifyPair &nopa=arNotifies[i];
		if ( nopa.second.size() != 0 && nopa.first == mp)
			return nopa;
    if ( iFirstEmpty==-1 && nopa.second.size()== 0) //store first empty as new message can be stored there
      iFirstEmpty=i;
    
 	}
	if (i == arNotifies.iSize() && iFirstEmpty==-1) // array is full, no match found
	{
    iFirstEmpty=arNotifies.iSize();
		arNotifies.Resize(arNotifies.iSize() + 5);
  }
	return arNotifies[iFirstEmpty];
}

CallBackPair* NotifyMap::find(CallBackHandler *cbh, const Message& mp)
{
  for(int i=0; i < arNotifies.iSize(); ++i)
	{
		if ( arNotifies[i].second.size() != 0 && arNotifies[i].first == mp)
    {
			return arNotifies[i].second.find(cbh);
    }
	}
   return NULL;
  
}

void NotifyMap::erase(CallBackHandler *cbh, const Message& mp)
{
  PRECONDITION(fINotEqual, cbh, (CallBackHandler *)NULL);

  for(int i=0; i < arNotifies.iSize(); ++i)
	{
		if ( arNotifies[i].second.size() != 0 && arNotifies[i].first == mp)
    {
      CallBackList cbl=arNotifies[i].second;
      DLIter<CallBackPair> iter(&cbl);
      for (int i=0; i<cbl.size(); ++i, ++iter)
      {
        if (iter().first == cbh )
        {
          iter.remove();
          break;
        }
      }
    }
  }
}








