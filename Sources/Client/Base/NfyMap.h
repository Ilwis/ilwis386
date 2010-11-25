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
#ifndef NFYMAP_H
#define NFYMAP_H

#include <utility>

using namespace std;

// implementatie van een map met een interface als de stl map
// Studio genereert irritante warnings op een STL map. Totdat deze weg zijn gebruik ik deze
// voor de notifies

typedef CCmdTarget CallBackHandler;
typedef int (CallBackHandler::*NotifyProc)(Event *);
typedef void (CCmdTarget::*NotifyRectProc)(CRect);

// handler and function
class CallBackPair
{
  public:
    CallBackPair() : first(NULL), second(NULL) {}
    CallBackPair(CallBackHandler *caller, NotifyProc prc) : first(caller), second(prc) {}

    CallBackHandler* first;
    NotifyProc      second;
};

// all functions belonging to a certain message (pair)
class CallBackList : public DList<CallBackPair>
{
  public:
    //push_front(CallBackPair);
    int             size() 
                      { return iSize(); }
    void            insert(const CallBackPair& cbp);
    CallBackPair    *find(CallBackHandler *cbh);
    CallBackPair    *find();
};

// to every Message (pair) a number of functions may be tied. These are stored in the list
typedef pair<Message, CallBackList> NotifyPair;
typedef CallBackPair* NIter;

class NotifyMap
{
  public:
    _export NotifyMap(int iSize=5);

    NotifyPair      &operator[](const Message& );
    CallBackPair*   find(CallBackHandler *cbh, const Message& );
    CallBackPair    *end() { return NULL; }
    void            erase(CallBackHandler *cur, const Message& );
  
  private:
    Array<NotifyPair> arNotifies;
};

#endif




