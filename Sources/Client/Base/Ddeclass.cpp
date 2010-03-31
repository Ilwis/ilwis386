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
/* $Log: /ILWIS 3.0/Main/Ddeclass.cpp $
 * 
 * 6     1/03/01 20:11 Retsios
 * The originally global variables cslist and tlist are now static members
 * of zDdeDev (main DDE class)
 * Also corrected cleanup of:
 * - topics list (cslist) at ~zTopicServer().
 * - advise links list (_links) at ~zTopicServer().
 * - advise links at ~zAdviseLink()
 *
 * 5     03-01-01 19:30 Retsios
 * Checked in by accident .. wrong changes logged and wrong
 * 
 * 4     22-06-00 16:00 Koolhoven
 * Corrections to make ilwis linkable
 * 
 * 3     6/22/00 14:19 Retsios
 * Changed string.GetBuffer(string.GetLength()) to (const char*)string
 * As a result: changed parameter of zTopicServer::findLink from (char*)
 * to (const char*)
 * 
 * 2     6/22/00 12:59 Retsios
 * DDE preliminary version .. compiles, but not tested yet.
 * 
 * 1     6/22/00 10:32 Retsios
// Revision 1.3  1998/09/16 16:26:19  Wim
// 22beta2
//
// Revision 1.2  1997/08/29 16:06:43  Wim
// Added WEP_Sizer() and WEP_DdeClass() which are called in
// WEP() in app.cpp
//
//
//  Copyright(C) 1992-1993 Inmark Development Corporation.
//
//		All rights reserved.
//
//	This material is confidential and may not be reproduced by any means.
//
//	$Revision: 6 $
//	$Date: 1/03/01 20:11 $
//
*/
//#include "app.h"
#include <afxwin.h>
//ZAPP_IMPL_ASSERTS;
#define ZAPPEXPORT 
#include "Client\Base\Ddeclass.h"

#define ZNEW new

///zAdviseTransDlist* zAdviseTrans::tlist=0;
//zAdviseTransDlist* tlist=0; // usage of this global is not clear: failed to move to class member variables
//zEvH * zDdeDev::errorObj=0;
//CCmdTarget * errorObj=0; .. moved to private of zDdeDev
//zDdeErrorProc zDdeDev::errorProc=0;
//zDdeErrorProc errorProc=0; .. moved to private of zDdeDev
//zTopicServerDlist *cslist=0; // usage of this global is not clear: failed to move to class member variables
//zTopicServerDlist *zTopicServer::cslist=0;
unsigned long _servInst=0; // usage of this global is not clear: failed to move to class member variables
//CString _serverAppName; .. moved to public of zDdeDev

CCmdTarget* zDdeDev::errorObj;// belongs to zDdeDev constructor .. this is to solve the static file scope problem
zDdeErrorProc zDdeDev::errorProc;// belongs to zDdeDev constructor .. this is to solve the static file scope problem
zTopicServerDlist * zDdeDev::cslist;// belongs to zDdeDev constructor .. this is to solve the static file scope problem
zAdviseTransDlist * zDdeDev::tlist;// belongs to zDdeDev constructor .. this is to solve the static file scope problem

/* Bas 20 December 2000: code was applicable for ZAPP, moved to destructor
void WEP_DdeClass()
{
    delete tlist;
    tlist = 0;
    delete cslist;
    cslist = 0;
}
*/
zDdeDev::zDdeDev() {
	//c {\c zDdeDev} is the base class for the {\c zDdeClient}
	//  and {\c zDdeServer} classes.
	//c All zApp DDE classes derived from this class are designed
	//  to have only a single object instance per application created.
	//  These objects must be created dynamically.
	//a 2.0
	//f The constructor initializes the DDE status and instance.
	
	status=0;
	_inst=0;
	cslist=0;
	tlist=0;
	errorObj=0;
	errorProc=0;
}

BOOL zDdeDev::isValid() {
	//f This member function returns the status of the DDE object.
	//r Returns TRUE if the device is valid and usable, FALSE if there
	//  has been an error and the device is unusable.
	
	return status;
}

zDdeDev::~zDdeDev() {
	if (status==0) {
		DdeUninitialize(inst());
	}
	if (cslist!=0) {delete cslist; cslist = 0;}
	if (tlist!=0) {delete tlist; tlist = 0;}
}

void zDdeDev::setErrorNotify(CCmdTarget *obj,zDdeErrorProc p) {
	//f This member is used to specify a callback procedure
	//  for DDE errors.  It is similar to other zApp "set"
	//  functions.  The default callback procedure calls
	//  debug("....") with the DDE error.
	//p zEvH * obj - pointer to the object with the member
	//  function that will handle the DDE errors.  The object
	//  must be a descendant of {\c zEvH}.
	//p zDdeErrorProc p - the member function that will be called.
	//s zDdeErrors
	
	errorObj=obj;
	errorProc=p;
}

void zDdeDev::error(zDdeErrors err,int dml) {
	if (errorObj!=0)
		(errorObj->*errorProc)(err,dml);
	else {
		CWnd tempwind;
		char buf [80];
		switch (err) {
		case zNullConnection:
			tempwind.MessageBox("Null connection, unable to start Transaction");
			break;
		case zUnknownAdvise:
			tempwind.MessageBox("Advise for unknown transaction received");
			break;
		case zFailConversationCreate:
			sprintf(buf,"Unable to create DDE conversation %d",dml);
			tempwind.MessageBox(buf);
			break;
		case zFailTransactionCreate:
			sprintf(buf,"Unable to create DDE transaction %d",dml);
			tempwind.MessageBox(buf);
			break;
		case zUnknownTransaction:
			tempwind.MessageBox("Callback for unknown transaction received");
			break;
		default:
			sprintf(buf,"DDE error number(%d,%d)",(int)err,dml);
			tempwind.MessageBox(buf);
		}
	}
}

extern "C" {
HDDEDATA FAR PASCAL //__declspec(dllexport)
ddeProc(WORD type,WORD fmt,HCONV conv,HSZ sz1,HSZ sz2,HDDEDATA data,DWORD data1,DWORD data2) {
	zDdeEvent *Ev;
	zDdeEvent e;
	Ev=&e;
	e.type=type;
	e.fmt=fmt;
	e.data1=data1;
	e.data2=data2;
	e.sz1=sz1;
	e.sz2=sz2;
	e.hData=data;
	e.conv=conv;
	
	unsigned long ret;
	ret=0;
	switch (type) {
		case XTYP_ADVDATA:{
			zAdviseTrans* t;
			t=zAdviseTrans::find((zAdviseDcliEvt*)Ev);
			if (t!=0) {
				if (Ev->hData!=0) Ev->_pData=DdeAccessData(Ev->hData,&(Ev->_dSize));
				ret=t->data((zAdviseDcliEvt*)Ev);
				if (Ev->hData!=0) DdeUnaccessData(Ev->hData);
			} else {
				zDdeDev::error(zUnknownAdvise);
			}
			break;}
		case XTYP_ADVREQ:
		case XTYP_ADVSTART:
		case XTYP_ADVSTOP:
		case XTYP_CONNECT:
		case XTYP_CONNECT_CONFIRM:
		case XTYP_EXECUTE:
		case XTYP_MONITOR:
		case XTYP_POKE:
		case XTYP_REQUEST:
		case XTYP_WILDCONNECT:
			break;
		case XTYP_DISCONNECT:
			break;
		case XTYP_ERROR:
			break;
		case XTYP_REGISTER:
			break;
		case XTYP_UNREGISTER:
			break;
		case XTYP_XACT_COMPLETE:{
			zTransaction* t;
			CONVINFO info;
			info.cb=sizeof(info);
			DdeQueryConvInfo(conv,((zCompleteDcliEvt*)Ev)->tid(),&info);
			t=(zTransaction*)info.hUser;
			if (t!=0) {
				if (Ev->hData!=0 && t->autoDelete()==1)
					Ev->_pData=DdeAccessData(Ev->hData,&(Ev->_dSize));
				ret=t->completed((zCompleteDcliEvt*)Ev);
				if (Ev->hData!=0 && t->autoDelete()==1)
					DdeUnaccessData(Ev->hData);
				if (t->autoDelete()) delete t;
			} else
				zDdeDev::error(zUnknownTransaction);
			break;}
		}

	return (HDDEDATA)ret;
}

HDDEDATA FAR PASCAL //__declspec(dllexport)
ddeServProc(WORD type,WORD fmt,HCONV conv,HSZ sz1,HSZ sz2,HDDEDATA data,DWORD data1,DWORD data2) {
	zDdeEvent *Ev;
	zDdeEvent e;
	Ev=&e;
	e.type=type;
	e.fmt=fmt;
	e.data1=data1;
	e.data2=data2;
	e.sz1=sz1;
	e.sz2=sz2;
	e.hData=data;
	e.conv=conv;
	
	zTopicServer *cs=0;
	unsigned long ret;
	ret=0;
	char *topic=0;
	if (sz1!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,sz1,0,0,CP_WINANSI);
		if (ln>0) {
			topic=ZNEW char[ln+1];
			DdeQueryString(_servInst,sz1,topic,ln+1,CP_WINANSI);
			cs=zTopicServer::find(topic);
		}
	}
	
	switch (type) {
		case XTYP_XACT_COMPLETE:
		case XTYP_ADVDATA:
			break;
		case XTYP_ADVREQ:
			if (cs!=0) {
				zAdviseLink *f;
				zAdviseDservEvt *re;
				re=(zAdviseDservEvt*)Ev;
				CString tmp(re->item());
				f=cs->findLink((const char*)tmp);
				if (f!=0) ret=f->render((zRenderDservEvt*)Ev);
			}
			break;
		case XTYP_ADVSTART:
			if (cs!=0) {
				ret=cs->adviseStart((zAdviseDservEvt*)Ev);
			}
			break;
		case XTYP_ADVSTOP:
			if (cs!=0) {
				ret=cs->adviseStop((zAdviseDservEvt*)Ev);
			}
			break;
		case XTYP_CONNECT:
			if (cs!=0) ret=1;
			break;
		case XTYP_CONNECT_CONFIRM:
			if (cs!=0) {
				cs->connection((zConnectDservEvt*)Ev);
			}
			break;
		case XTYP_EXECUTE:
			if (cs!=0) {
				if (Ev->hData!=0) Ev->_pData=DdeAccessData(Ev->hData,&(Ev->_dSize));
				ret=cs->execute((zExecuteDservEvt*)Ev);
				if (Ev->hData!=0) DdeUnaccessData(Ev->hData);
			}
			break;
		case XTYP_POKE:
			if (cs!=0) {
				DWORD dl;
				if (Ev->hData!=0) Ev->_pData=DdeAccessData(Ev->hData,&dl);
				Ev->_dSize=dl;
				ret=cs->poke((zPokeDservEvt*)Ev);
				if (Ev->hData!=0) DdeUnaccessData(Ev->hData);
			}
			break;
		case XTYP_REQUEST:
			if (cs!=0) {
				ret=cs->request((zRequestDservEvt*)Ev);
			}
			break;
		case XTYP_ERROR:
			break;
		case XTYP_REGISTER:
		case XTYP_UNREGISTER:
		case XTYP_MONITOR:
		case XTYP_WILDCONNECT:
		case XTYP_DISCONNECT:
			break;
		}
	if (topic!=0) delete [] topic;
	return (HDDEDATA)ret;
}
}

zDdeClient::zDdeClient():zDdeDev() {
	//c Creating an instance of {\c zDdeClient} makes an application
	//  capable of being a DDE client.  This function initializes the
	//  application in the Windows DDE system.
	//c A pointer to an instance of this class is a required
	//	parameter for a {\c zConversation}.
	//c There is no need to derive new classes from this class.  It is
	//  used as a DDE handler for various DDE operations.{\i DDE;client}
	//a 2.0
	//f This constructor
	//  initializes the application as a DDE client (setting up the
	//  sets up the DDE client hook procedure etc...)
	//n After constructing this object, check the {\m zDdeDev::isValid()}
	//  member function (which this class inherits) to ensure that
	//  the DDE client subsystem was initialized properly.
	//n Only one instance of this class should be created for an application.

	FARPROC fp;
	fp=MakeProcInstance((FARPROC)ddeProc,appInst()->instance());
	status=DdeInitialize(&_inst,(PFNCALLBACK)fp,APPCMD_CLIENTONLY,0);
	if (status!=0) error(zUnknownError,status);
}

zDdeClient::~zDdeClient() {
	//f The destructor for {\c zDdeClient} removes the application
	//  as a DDE client.  It is important that a {\c zDdeClient} object
	//  be explicitly deleted.
}

zDdeServer::zDdeServer(const char *ap):zDdeDev() {
	//c Creating an instance of {\c zDdeServer} makes an application
	//  capable of being a DDE server.  This class contains the hook
	//  function for the fundamental DDE server subsystem.
	//  {\c zDdeServer} also initializes the application as a server
	//  in the Windows DDE system with the name specified in the
	//  char string parameter.{\i DDE;server}
	//c A pointer to an object instance of {\c zDdeServer} is a
	//  required parameter to the {\c zTopicService} constructor.
	//c There is no need to derive new classes form this class.  It is
	//  used as a DDE handler for various DDE operations.
	//a 2.0
	//f This constructor sets up the DDE server hook procedure and
	//  initializes the application as a DDE server.
	//p const char *ap - the server name of the application.  This
	//  name will be visible to other applications.
	//n Only one instance of this class should be created for an application.
	//n If the application is also an OLE server, the application name
	//  specified in the string parameter should be different than that
	//  specified for OLE.
	
	FARPROC fp;
	serverAppName=0;
	fp=MakeProcInstance((FARPROC)ddeServProc,appInst()->instance());
	status=DdeInitialize(&_inst,(PFNCALLBACK)fp,0,0);
	_servInst=_inst;
	_serverAppName=ap;
	if (status!=0)
		error(zUnknownError,status);
	else {
		serverAppName=DdeCreateStringHandle(_inst,(const char*)_serverAppName,CP_WINANSI);
		DdeNameService(_inst,serverAppName,0,DNS_REGISTER);
	}
}

zDdeServer::~zDdeServer() {
	//f The destructor for {\c zDdeServer} removes the application
	//  as a DDE server.  It is important that a {\c zDdeServer} object
	//  be explicitly deleted.
	if (serverAppName!=0) {
		DdeNameService(_inst,serverAppName,0,DNS_UNREGISTER);
		DdeFreeStringHandle(_inst,serverAppName);
	}
}

zTopicServer::zTopicServer(zDdeServer *de,const char *nm) {
	//c {\c zTopicServer} is the main message handling class for
	//  a zApp DDE server application.  This class contains
	//  virtual member functions which correspond to DDE
	//  messages.  The programmer should override these
	//  functions in a derived class.  The derived class from
	//  {\c zTopicServer} is where most of the programmer
	//  defined code should reside.{\i DDE;server}
	//a 2.0
	//f This constructor creates a new {\c zTopicServer} to handle
	//  DDE server messages.
	//p zDdeServer* de - pointer to a {\c zDdeServer} object.  The
	//  {\c zDdeServer} object must be created before the
	//  {\c zTopicServer}.
	//p const char *nm - the name of the dde topic.  This name will
	//  be visible to other applications.
	//s {\c zDdeServer}
	
	
	_serverDev=de;
	name=nm;
	hName=DdeCreateStringHandle(_serverDev->inst(),(const char*)name,CP_WINANSI);
	_links=0;
	//if (cslist==0) cslist=ZNEW zTopicServerDlist;
		if (zDdeDev::cslist==0) zDdeDev::cslist = new zTopicServerDlist;
	//cslist->append(this);
		zDdeDev::cslist->push_back(this);
}

void zTopicServer::connection(zConnectDservEvt* e) {
	//f This virtual function gets called when a DDE client confirms
	//  a connection with this server.
	//p zConnectDservEvt * - the event representing connect to the
	//  DDE server.
	//n This virtual function is called to notify the DDE server.
	//  There is no requirement for any processing to be done
	//  in this function.
	//s {\c zConnectDservEvt}
}

unsigned long zTopicServer::request(zRequestDservEvt* e) {
	//f This virtual function gets called when a DDE client asks for
	//  a request transaction.  This function should be overridden
	//  in a derived class to respond to the request transaction.
	//  In determining a response, this function should check the
	//  data format type specified in the {\c zRequestDservEvt}.{\i DDE;request}
	//r unsigned long - may be {\c zDdeNAck}, {\c zDdeAck}, or
	//  {\c zDdeBusy}.  In the overridden function the programmer
	//  must decide which is the appropriate response.  The default
	//  member returns {\c zDdeNAck}.
	//p zRequestDservEvt* e - the transaction request event.
	//s {\c zRequestDservEvt}
	
	return zDdeNAck;
}

unsigned long zTopicServer::adviseStart(zAdviseDservEvt* e) {
	//f This virtual function initiates an advise link.
	//r unsigned long - may be {\c zDdeNAck}, {\c zDdeAck}, or
	//  {\c zDdeBusy}.  In the overridden function the programmer
	//  must decide which is the appropriate response.  The default
	//  member returns {\c zDdeNAck}.
	//p zAdviseDservEvt *e - the advise event for this adviseStart.
	//s {\c zAdviseDservEvt}
	return 0;
}

unsigned long zTopicServer::adviseStop(zAdviseDservEvt* e) {
	//f This virtual function terminates an advise link, and
	//  deletes the {\c zAdviseLink} object associated with
	//  it.  Normally, there is no need to override this member.
	//r unsigned long - If the link was found and destroyed,
	//  {\c zDdeAck} is returned.  Otherwise, if the link is invalid,
	//  {\c zDdeNAck} is returned.
	//p zAdviseDservEvt *e - the advise event for this adviseStop.
	//s {\c zAdviseLink}, {\c zAdviseDservEvt}
	
	CString tmp(e->item());
	zAdviseLink *f;
	f=findLink((const char*)tmp);
	if (f!=0) {
		delete f;
		return zDdeAck;
	}
	return zDdeNAck;
}

unsigned long zTopicServer::execute(zExecuteDservEvt* e) {
	//f This virtual function is called when a DDE client asks
	//  the DDE server for an execute transaction.  This function
	//  should be overridden in a derived class.{\i DDE;execute}
	//f The DDE excecute command is in the format: "[cmd1][cmd2]...[cmdn]".
	//r unsigned long - may be {\c zDdeNAck}, {\c zDdeAck}, or
	//  {\c zDdeBusy}.  In the overridden function the programmer
	//  must decide which is the appropriate response.  The default
	//  member returns {\c zDdeNAck}.
	//p zExecuteDservEvt* e - the execute transaction event.  Contains
	//  the execute command string.
	//s {\c zExecuteDservEvt}.
	
	return zDdeNAck;
}

unsigned long zTopicServer::poke(zPokeDservEvt* e) {
	//f This virtual function is called when a DDE client sends
	//  data to the server in a poke transaction.  This function
	//  should be overridden in derived classes to handle poke
	//  transactions.  The programmer should check the data{\i DDE;poke}
	//  format type specified in the {\c zPokeDservEvt} parameter
	//  to determine the proper response for this function.
	//r unsigned long - may be {\c zDdeNAck}, {\c zDdeAck}, or
	//  {\c zDdeBusy}.  In the overridden function the programmer
	//  must decide which is the appropriate response.  The default
	//  member returns {\c zDdeNAck}.
	//p zPokeDservEvt* - poke transaction event.
	//s {\c zPokeDservEvt}.
	
	return zDdeNAck;
}

zTopicServer* zTopicServer::find(char *nm) {
	//f This member function searches all registered topic
	//  servers for a match to the specified topic string.
	//r zTopicServer * - pointer to a topic server.  If there
	//  is a match to the requested topic, a valid pointer
	//  is returned, otherwise 0 is returned.
	//p char* nm - the name of the requested topic server.
	
	if (nm==0) return 0;
	CString nam(nm);
	
	if (zDdeDev::cslist!=0) {
		//zTopicServerDlistIter trav(cslist);

		zTopicServer *el;
		for (zTopicServerDlist::iterator trav = zDdeDev::cslist->begin();trav!=zDdeDev::cslist->end();++trav)
		{
			el=*trav;
			if (el->name==nam) return el;
		}
	}

	return 0;
}

zAdviseLink* zTopicServer::findLink(const char *itemName) {
	//f This function searches all active advise links
	//  for the requested item.
	//r zAdviseLink * - a pointer to a valid {\c zAdviseLink}.
	//  If an advise link exists for the requested item, a valid
	//  pointer will be returned, otherwise 0 will be returned.
	//p char* itemName - the item to search for.
	

	if (itemName==0) return 0;
	CString nam(itemName);
	if (_links!=0) {
		//zAdviseLinkDlistIter trav(_links);
	
		zAdviseLink *el;
		for (zAdviseLinkDlist::iterator trav = _links->begin();trav!=_links->end();++trav)
		{
			el=*trav;
			if (el->item()==nam) return el;
		}
	}
	return 0;
}

zTopicServer::~zTopicServer() {
	//f Any {\c zTopicServer} should be explicitly deleted when it
	//  is no longer needed.
	//n Until it is deleted, you may receive additional requests
	//  via DDE.
	
	if (zDdeDev::cslist!=0) {
		//zTopicServerDlistIter trav(cslist);
		/* //This old ZAPP code is replaced with code after this commented part
		zTopicServer *el;
		for (zTopicServerDlist::iterator trav = cslist->begin();trav!=cslist->end();++trav)
		{
			el=*trav;
			if (el==this) {
				cslist->remove(*trav);
				break;
			}
		}
		*/
		zDdeDev::cslist->remove(this);
	}

	if (_links!=0) {
		//zAdviseLinkDlistIter trav(_links);
		/* //This old ZAPP code is replaced with code after this commented part
		zAdviseLink *el;
		for(zAdviseLinkDlist::iterator trav = _links->begin();trav!=_links->end();++trav)
		{
			el=*trav;
			_links->remove(*trav);
			delete el;
		}
		*/
		zAdviseLinkDlist::iterator trav = _links->begin();
		while (trav!=_links->end())
		{
			zAdviseLink *al;
			al = *trav; // This is really necessary!! trav is an iterator, so after the remove its value became invalid, so save it before the remove.
			_links->remove(*trav);
			delete al; // This cleanup is sufficient: it takes over the work of adviseStop for the complete _list.
			trav = _links->begin();
		}
		delete _links;
		_links = 0;
	}
	DdeFreeStringHandle(_servInst,hName);
}

zAdviseLink::zAdviseLink(zTopicServer *pop,zAdviseDservEvt *e)
:dad(pop),_item(e->item()),_fmt(e->dataFormat()) {
	//c {\c zAdviseLink} is the base class for advise links.
	//  The programmer should derive a new class from this class
	//  to create a particular advice link.  A programmer derived
	//  class from {\c zTopicServer} normally creates advise links
	//  in the {\m zTopicServer::adviseStart()} virtual member function.
	//  The {\c zTopicServer} will keep a list of advise links.  This
	//  class also contains the {\m setDirty()} function, and the
	//  {\m render()} virtual function.  {\m setDirty()} should be
	//  the server needs to send new data out.  A call to {\m setDirty()}
	//  will cause an eventual call to the {\m render()} virtual
	//  function.  The programmer should override {\m render()}
	//  to provide (render) the actual data.{\i advise link}
	//a 2.0
	//f Constructor for an advise link.
	//p zTopicServer* pop - the {\c zTopicServer} which owns this
	//  advise link.
	//p zAdviseDservEvt - the advise event.
	//n {\c zAdviseLink}s get deleted automatically in response to
	//  a {\c zAdviseStop} event.
	

	//if (dad->links()==0) dad->links(ZNEW zAdviseLinkDlist);
	if (dad->links()==0) dad->links(new zAdviseLinkDlist);
	dad->links()->push_back(this); // The order of this and the following statement is fine (original ZAPP order): after the following assignment, the links() list correctly contains the latest version of _hItem (checked!).
	_hItem=DdeCreateStringHandle(_servInst,(const char*)_item,CP_WINANSI);
}

void zAdviseLink::setDirty() {
	//f Calling this function marks the data in the advise link
	//  as dirty.  This means that the data needs to be updated.
	//  Calling this function forces the {\m render()} virtual
	//  function to be called for all advise links attached to this
	//  {\c zTopicServer} with the same item.
	//n This function works in a similar fashion to
	//  {\m zDisplay::setDirty()}.
	
	DdePostAdvise(_servInst,dad->hTopic(),_hItem);
}

unsigned long zAdviseLink::render(zRenderDservEvt*) {
	//f This virtual function should be overridden to handle rendering
	//  of the data.{\i rendering}
	//r unsigned long - Should return 0 if the data is unavailable, or return
	//  zRenderDservEvt::respondWith(...).  By default it returns 0.
	//p zRenderDservEvt* - the event received to tell about this request
	//  for rendering.
	//s {\c zRenderDservEvt}
	return 0;
}

zAdviseLink::~zAdviseLink() {
	//zAdviseLinkDlistIter trav(dad->links());
	/* //This old ZAPP code is replaced with code after this commented part
	zAdviseLink *el;
	for (zAdviseLinkDlist::iterator trav = dad->links()->begin();trav!=dad->links()->end();++trav)
	{
		el=*trav;
		if (el==this) {
			dad->links()->remove(*trav);
			break;
		}
	}
	*/
	dad->links()->remove(this);

	DdeFreeStringHandle(_servInst,_hItem);
}

zConversation::zConversation(zDdeClient*d,const char *server,const char *topi) {
	//c This class represents a conversation for a DDE client.
	//  {\c zConversation} is used as one of the parameters in all
	//  DDE client transactions.{\i DDE;conversation}
	//a 2.0
	//f This constructor creates a DDE conversation between the
	//  specified DDE client server and the DDE server specified
	//  in the char *server.  The name of the topic is specified
	//  in the third parameter.
	//p zDdeClient *d - pointer to the DDE client.  This must be
	//  a valid pointer to a previously created {\c zDdeClient}.
	//p char *server - the name of the server to connect to.
	//p char *topi - the name of the topic for this conversation.
	//n There should be no need to derive a new class from this class.
	//  All DDE conversations may be created directly from this class.
	//n Be sure to call the {\m isValid()} member, before using the object
	//  in any transactions, it is very possible for a DDE conversation
	//  start to fail.
	
	_clientDev=d;
	strcpy(_topic,topi);
	strcpy(_server,server);
	HSZ serv,top;
	serv=DdeCreateStringHandle(_clientDev->inst(),server,CP_WINANSI);
	top=DdeCreateStringHandle(_clientDev->inst(),topi,CP_WINANSI);
	csz=top;
	csv=serv;
	cid=DdeConnect(_clientDev->inst(),serv,top,0);
	if (cid!=0)
		status=1;
	else {
		status=0;
		unsigned int err;
		err=DdeGetLastError(_clientDev->inst());
		zDdeDev::error(zFailConversationCreate,err);
	}
}

BOOL zConversation::isValid() {
	//f This member function checks to see if the DDE conversation
	//  started properly.
	//r BOOL - returns TRUE if the conversation started, FALSE
	//  otherwise.
	
	return status;
}

zConversation::~zConversation() {
	//f A {\c zConversation} should be explicitly deleted.
	if (isValid()) {
		DdeDisconnect(cid);
	}
	DdeFreeStringHandle(_clientDev->inst(),csz);
	DdeFreeStringHandle(_clientDev->inst(),csv);
}

zTransaction::zTransaction(zConversation*con,char *it,zExchangeFmt df) {
	//c This is the base class for all DDE client transactions.
	//  Under the event-driven model, asynchronous transactions are
	//  preferred and highly recommended.
	//c Normally this class is not used directly, only use derived ones.
	//a 2.0
	//f This constructor is used by all derived classes which have items.
	//p zConversation* con - a DDE conversation.
	//p char* it - the item for the transaction.
	//p zExchangeFmt df - the exchange data format for the transaction.
	
	autodel=1;
	strcpy(_item,it);
	tid=0;
	_conv=con;
	fmt=df.id();
	if (con==0) {
		zDdeDev::error(zNullConnection);
		dit=0;
		return;
	}
	dit=DdeCreateStringHandle(conv()->clientDev()->inst(),it,CP_WINANSI);
}

zTransaction::zTransaction(zConversation* con,zExchangeFmt df) {
	//f This constructor is used for transactions without items,
	//  such as execute transactions.
	//p zConversation* con - a DDE conversation.
	//p zExchangeFmt df - the exchange data format for the transaction.
	
	autodel=1;
	tid=0;
	dit=0;
	_conv=con;
	fmt=df.id();
	if (con==0) {
		zDdeDev::error(zNullConnection);
		dit=0;
		return;
	}
}

unsigned long zTransaction::completed(zCompleteDcliEvt*) {
	//f This virtual function is called when a transaction is
	//  completed.  Derived classes should override this function
	//  to retrieve the completion status and any data for
	//  request transactions.
	//r unsigned long - should be zDdeAck.
	//p zCompleteDcliEvt* - the event for the completed transaction.
	//n For an advise transaction, this function will be called only
	//  after the advise link has been terminated.
	//s {\c zCompleteDcliEvt}
	
	return 0;
}

int zTransaction::autoDelete() {
	return autodel;
}

zTransaction::~zTransaction() {
	//f This destructor is automatically called when the transaction is
	//  complete.  Deleting an asynchronous transaction before it is completed
	//  automatically aborts the transaction.

	if (tid!=0) {
		unsigned int ret;
		ret=DdeAbandonTransaction(conv()->clientDev()->inst(),*conv(),tid);
		if (ret==0) {
			unsigned int err;
			err=DdeGetLastError(conv()->clientDev()->inst());
			if (err==0x4011) {
				ret=(unsigned int)(unsigned long)DdeClientTransaction(0,0,*conv(),dit,fmt,XTYP_ADVSTOP,5000,&tid);
/*
				if (ret==0) {
					unsigned int err;
					err=DdeGetLastError(conv()->clientDev()->inst());
					debug("Unable to terminate advise loop, error=%x",err);
				}
	Last change:  WK   29 Aug 97    6:06 pm
*/
			}
/*			 else
			debug(" !=0x4011 ~Transaction abandon error=%x",err);
*/
		}
	}
	if (dit!=0) DdeFreeStringHandle(conv()->clientDev()->inst(),dit);
}

zRequestTrans::zRequestTrans(zConversation*con,char *item,zExchangeFmt df,int wait):zTransaction(con,item,df) {
	//c This transaction class represents a request to a DDE server
	//  for data.  An open conversation is required for this transaction.{\i DDE;request}
	//a 2.0
	//f This constructor begins a request for the given item and data
	//  format.
	//p zConversation* con - an open DDE conversation.
	//p char* item - the name of a data item.
	//p zExchangeFmt df - the data format type required.
	//p int wait - seconds to wait for a synchronous transaction.  If
	//  no number is specified, an asynchronous transaction will be
	//  created.  Asynchronous is recommended.
	
	unsigned long ret;
	data=0;
	if (con==0) {
		zDdeDev::error(zNullConnection);
		return;
	}
	ret=(unsigned long)DdeClientTransaction(0,0,*conv(),dit,fmt,XTYP_REQUEST,
		(wait==0) ? TIMEOUT_ASYNC:(wait*1000),&tid);
	if (ret!=0) {
		if (wait==0) {
			ret=DdeSetUserHandle(*con,tid,(unsigned long)this);
		} else {
			data=ret;
			return;
		}
	}
	if (ret==0) {
		unsigned int err;
		err=DdeGetLastError(conv()->clientDev()->inst());
		zDdeDev::error(zUnknownError,err);
	}
}

void* zRequestTrans::getSyncData(unsigned long& dataSize) {
	//f This function retrieves any data returned from a synchronous DDE request
	//  transaction.
	//r Returns the pointer to any data returned by the request.  This will be
	//  zero if the request failed, or if no data was returned.  The
	//  {\p dataSize} parameter is also set by this function.
	//p unsigned long& dataSize - this output value will be set to the size
	//  of the accessed data.
	//n You must call {\m zRequestTrans::freeSyncData} to free this memory
	//  block.

	return (void*)DdeAccessData((HDDEDATA)data,&dataSize);
}

void zRequestTrans::freeSyncData() {
	//f This function will free any data that was locked by the
	//  {\m zRequestTrans::getSyncData} member function.
	
	DdeUnaccessData((HDDEDATA)data);
}

zAdviseTrans::zAdviseTrans(zConversation*con,char*item,zExchangeFmt df)
:zTransaction(con,item,df) {
	//c Creation of this transaction class starts an advise link
	//  with a DDE server.  The programmer should check the validity
	//  of this transaction, and not assume that it will always work.
	//  Also, the {\m completed()} virtual member should be overridden
	//  to access data updates.{\i advise link}
	//a 2.0
	//f Constructor creates an advise link with a DDE server.
	//p zConversation* con - an open DDE conversation.
	//p char* item - the item to query the DDE server about.
	//p zExchangeFmt df - the exchange data type format requested.  Default
	//  is ZFMT_TEXT.
	
	autodel=0;
	//if (tlist==0) tlist=ZNEW zAdviseTransDlist;
	if (zDdeDev::tlist==0) zDdeDev::tlist=new zAdviseTransDlist;
	zDdeDev::tlist->push_back(this);
	unsigned int ret;
	if (con==0) {
		zDdeDev::error(zNullConnection);
		return;
	}
	ret=(unsigned int)(unsigned long)DdeClientTransaction(0,0,*conv(),dit,fmt,XTYP_ADVSTART,TIMEOUT_ASYNC,&tid);
	if (ret!=0) {
		ret=DdeSetUserHandle(*con,tid,(unsigned long)this);
	}
	if (ret==0) {
		unsigned int err;
		err=DdeGetLastError(conv()->clientDev()->inst());
		zDdeDev::error(zFailTransactionCreate,err);
	}
}

zAdviseTrans* zAdviseTrans::find(zAdviseDcliEvt* ev) {
	if (zDdeDev::tlist==0) return 0;

	//zAdviseTransDlistIter trav(tlist);

	zAdviseTrans *me;
	for (zAdviseTransDlist::iterator trav = zDdeDev::tlist->begin();trav!=zDdeDev::tlist->end();++trav)
	{
		me=*trav;
		if (DdeCmpStringHandles(ev->hItem(),me->dit)==0 &&
			DdeCmpStringHandles(ev->hTopic(),me->conv()->convString())==0)
				return me;
	}

	return 0;
}

zAdviseTransDlist* zAdviseTrans::adviseList() {
	//f This member function returns a list of all active
	//  advise links for the DDE client.
	//r zAdviseTransDlist * - list of advise links.
	
	return zDdeDev::tlist;
}

zAdviseTrans::~zAdviseTrans() {
	if (zDdeDev::tlist!=0) {
		//zAdviseTransDlistIter trav(tlist);

		zAdviseTrans *me;
		for (zAdviseTransDlist::iterator trav = zDdeDev::tlist->begin();trav!=zDdeDev::tlist->end();++trav)
		{
			me=*trav;
			if (DdeCmpStringHandles(dit,me->dit)==0 &&
				DdeCmpStringHandles(conv()->convString(),me->conv()->convString())==0)  {
				zDdeDev::tlist->remove(*trav);
					break;
				}
			}
		}
}

unsigned long zAdviseTrans::data(zAdviseDcliEvt *ev) {
	//f This virtual function should be overridden to
	//  retrieve data.
	//r unsigned long  Should be:
	///     zDdeAck		- if data processed.
	///		zDdeBusy	- if too busy to do process data.
	///     zDdeNAck	- if the server does not wish to process this data.
	//p zAdviseDcliEvt *ev - the event received which provides access to
	//  the data being received in this advise update.
	return zDdeAck;
}

zExecuteTrans::zExecuteTrans(zConversation*con,char *cmds,int wait):zTransaction(con) {
	//c This class is used to send an execute command to a DDE server.{\i DDE;execute}
	//a 2.0
	//f This constructor creates an execute transaction, which sends
	//  an execute command to a DDE server.  It a wait time is not
	//  specified, an asynchronous transaction will be created.  Otherwise
	//  a synchronous transaction will be created with the specified wait
	//  time.  Asynchronous is recommended.
	//p zConversation* con - an open DDE conversation.
	//p char* cmds - a string representing the execute command.
	//p int wait - the number of seconds to wait for a synchronous
	//  transaction.  If no wait time is specified, an asynchronous
	//  transaction will be created.
	
	unsigned int ret;
	if (cmds==0) return;
	if (con==0) {
		zDdeDev::error(zNullConnection);
		return;
	}
	ret=(unsigned int)(unsigned long)DdeClientTransaction((unsigned char*)cmds,strlen(cmds)+1,*conv(),0,fmt,XTYP_EXECUTE,
		(wait==0) ? TIMEOUT_ASYNC:(wait*1000),&tid);
	if (ret!=0) {
		if (wait==0) {
			ret=DdeSetUserHandle(*con,tid,(unsigned long)this);
		} else {
			return;
		}
	}
	if (ret==0) {
		unsigned int err;
		err=DdeGetLastError(conv()->clientDev()->inst());
		zDdeDev::error(zUnknownError,err);
	}
}


zPokeTrans::zPokeTrans(zConversation*con,char *item,char *data,int wait):zTransaction(con,item) {
	//c This class is used to send data to a DDE server.{\i DDE;poke}
	//a 2.0
	//f This constructor is used to send a string to the DDE server, with
	//  an exchange data format of {\c ZFMT_TEXT}.  If no wait time is
	//  specified, an asynchronous transaction will be created.  Otherwise
	//  a synchronous transaction will be created with the specified wait
	//  time.  Asynchronous is recommended.
	//p zConversation* con - an open DDE conversation.
	//p char* item - the DDE server item.
	//p char* data - the string data to send to the DDE server.
	//p int wait - the number of seconds to wait for a synchronous
	//  transaction.  If no wait time is specified, an asynchronous
	//  transaction will be created.

	unsigned int ret;
	if (data==0) return;
	if (con==0) {
		zDdeDev::error(zNullConnection);
		return;
	}
	autodel=2;
	ret=(unsigned int)(unsigned long)DdeClientTransaction(
		(unsigned char*)data,strlen(data)+1,*conv(),dit,fmt,
		XTYP_POKE,(wait==0) ? TIMEOUT_ASYNC:(wait*1000),&tid);
	if (ret!=0) {
		if (wait==0) {
			ret=DdeSetUserHandle(*con,tid,(unsigned long)this);
		} else {
			return;
		}
	}
	if (ret==0) {
		unsigned int err;
		err=DdeGetLastError(conv()->clientDev()->inst());
		zDdeDev::error(zUnknownError,err);
	}
}

zPokeTrans::zPokeTrans(zConversation*con,char *item,void *data,unsigned int dataLen,zExchangeFmt df,int wait):zTransaction(con,


item,df) {
	//f This constructor is used to send any type of data to the DDE server.
	//  If not wait time is specified, an asynchronous transaction will be created.
	//  Otherwise, a synchronous transaction will be created with the
	//  specified wait time.  Asynchronous is recommended.{\i DDE;poke}
	//p zConversation* con - an open DDE conversation.
	//p char* item - the DDE server item.
	//p char* data - the data to send to the DDE server.
	//p unsigned int dataLen - length of the data sent to the DDE server.
	//p zExchangeFmt df - the exchange data type format.
	//p int wait - the number of seconds to wait for a synchronous
	//  transaction.  If no wait time is specified, an asynchronous
	//  transaction will be created.
	
	unsigned int ret;
	if (data==0) return;
	if (con==0) {
		zDdeDev::error(zNullConnection);
		return;
	}
	autodel=2;
	ret=(unsigned int)(unsigned long)DdeClientTransaction((unsigned char*)data,
		dataLen,*conv(),dit,fmt,XTYP_POKE,
		(wait==0) ? TIMEOUT_ASYNC:(wait*1000),&tid);
	if (ret!=0) {
		if (wait==0) {
			ret=DdeSetUserHandle(*con,tid,(unsigned long)this);
		} else {
			return;
		}
	}
	if (ret==0) {
		unsigned int err;
		err=DdeGetLastError(conv()->clientDev()->inst());
		zDdeDev::error(zUnknownError,err);
	}
}


CString zConnectDservEvt::topic() {
	//c A dde server receives this event when a connection request is received.{\i events;DDE events}
	//a 2.0
	//f This member returns the topic name for the {\c zConnectDservEvt}.
	//r zString - the topic name.
	
	char *txt=0;
	CString val;
	if (hTopic()!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,hTopic(),0,0,CP_WINANSI);
		if (ln>0) {
			txt=ZNEW char[ln+1];
			DdeQueryString(_servInst,hTopic(),txt,ln+1,CP_WINANSI);
			val=txt;
			delete txt;
		}
	}
	return val;
}

CString zConnectDservEvt::server() {
	//f This member returns the server name for the
	//  {\c zConnectDservEvt}.
	//r zString - the server name.

	char *txt=0;
	CString val;
	if (hServer()!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,hServer(),0,0,CP_WINANSI);
		if (ln>0) {
			txt=ZNEW char[ln+1];
			DdeQueryString(_servInst,hServer(),txt,ln+1,CP_WINANSI);
			val=txt;
			delete txt;
		}
	}
	return val;
}

unsigned long zRequestDservEvt::respondWith(void *obj, long len,zExchangeFmt df) {
	//c An instance of this class is created and sent to a dde server
	//  when a dde request is received.{\i events;DDE events}{\i DDE;request}
	//a 2.0
	//f This member function should be used by a server to return data
	//  to a client for a DDE request.  This function can return any
	//  kind of data of any length.
	//p void *obj - pointer to the data to return.
	//p long len - the length of the data.
	//p zExchangeFmt df - the format of the data.
	//r unsigned long - Handle to the data to return to the Client.
	
	return (unsigned long)
		DdeCreateDataHandle(_servInst,(unsigned char*)obj,len,0,hItem(),df.id(),0);
}

unsigned long zRequestDservEvt::respondWith(char *str) {
	//f This member function should be used by a server to return string
	//  data to a client for a DDE request.  This member can only return
	//  text strings.
	//p char *str - the string data to return.
	//r unsigned long - Handle to the data to return to the Client.

	if (str!=0)
		return (unsigned long)
			DdeCreateDataHandle(_servInst,(unsigned char*)str,strlen(str)+1,0,hItem(),CF_TEXT,0);
	return 0;
}

CString zRequestDservEvt::item() {
	//f This member returns the item name for the {\c zRequestDservEvt}.
	//r zString - the item name.
	
	char *txt=0;
	CString val;
	if (hItem()!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,hItem(),0,0,CP_WINANSI);
		if (ln>0) {
			txt=ZNEW char[ln+1];
			DdeQueryString(_servInst,hItem(),txt,ln+1,CP_WINANSI);
			val=txt;
			delete txt;
		}
	}
	return val;
}

CString zPokeDservEvt::item() {
	//c An instance of this class is sent to a dde server when a poke
	//  transaction is received.{\i events;DDE events}{\i DDE;poke}
	//a 2.0
	//f This member returns the item name for the {\c zPokeDservEvt}.
	//r zString - the item name.

	char *txt=0;
	CString val;
	if (hItem()!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,hItem(),0,0,CP_WINANSI);
		if (ln>0) {
			txt=ZNEW char[ln+1];
			DdeQueryString(_servInst,hItem(),txt,ln+1,CP_WINANSI);
			val=txt;
			delete txt;
		}
	}
	return val;
}

zExchangeFmt zDdeEvent::dataFormat() {
	//c This is the base class for all DDE events.{\i events;DDE events}
	//a 2.0
	//f This member returns the {\c zExchangeFmt} for
	//  the {\c zDdeEvent}.
	//r zExchangeFmt - id of the exchange data type.
	//n This public member is inherited by all zApp DDE events.

	return zExchangeFmt(fmt);
}

CString zAdviseDservEvt::item() {
	//c Class is passed to a dde server and provides access to information
	//  regarding advises.{\i events;DDE events}
	//a 2.0
	//f This member returns the item name for the {\c zAdviseDservEvt}.
	//r zString - the item name.	

	char *txt=0;
	CString val;
	if (hItem()!=0) {
		int ln;
		ln=(int)DdeQueryString(_servInst,hItem(),0,0,CP_WINANSI);
		if (ln>0) {
			txt=ZNEW char[ln+1];
			DdeQueryString(_servInst,hItem(),txt,ln+1,CP_WINANSI);
			val=txt;
			delete txt;
		}
	}
	return val;
}

unsigned long zRenderDservEvt::respondWith(void *obj,long len,zExchangeFmt df) {
	//c An instance of this class is created and sent to a dde server
	//  when data needs to be rendered.{\i events;DDE events}
	//a 2.0
	//f This member function should be used by a server to return data
	//  to a client for a DDE render request.  This function can return any
	//  kind of data of any length.
	//p void *obj - pointer to the data to return.
	//p long len - the length of the data.
	//p zExchangeFmt df - the format of the data.
	//r unsigned long - Global handle to the data.

	
	return (unsigned long)
		DdeCreateDataHandle(_servInst,(unsigned char*)obj,len,0,hItem(),df.id(),0);
}

unsigned long zRenderDservEvt::respondWith(char *str) {
	//f This member function should be used by a server to return string
	//  data to a client for a DDE render request.  This member can only return
	//  text strings.
	//p char *str - the string data to return.
	//r unsigned long - Handle to the data to return to the Client.
	
	if (str!=0)
		return (unsigned long)
			DdeCreateDataHandle(_servInst,(unsigned char*)str,strlen(str)+1,0,hItem(),CF_TEXT,0);
	return 0;
}

#ifdef DOCONLY

HSZ zDdeEvent::hTopic() const {
	return sz1;
}

HSZ zAdviseDservEvt::hItem() const {
	return sz2;
}

HSZ zRenderDservEvt::hItem() const {
	return sz2;
}

HSZ zRequestDservEvt::hItem() const {
	return sz2;
}

char* zExecuteDservEvt::cmd() {
	//c This class is passed to a dde server when a Execute transaction is
	//  received.{\i events;DDE events}{\i DDE;execute}
	//a 2.0
	//f This member provides access to the command string
	//  for a DDE execute event, {\c zExecuteDservEvt}.
	//r char * - the command string sent to the DDE server.
	//  It is of the format: "[cmd1][cmd2]...[cmdn]".

	return (char*)_pData;
}

unsigned long zExecuteDservEvt::cmdLength() {
	//f This member returns the length of the command string
	//  passed to the DDE execute event, {\c zExecuteDservEvt}.
	//r unsigned long - the length of the command string.
	//s {\m zExecuteDservEvt::cmd()}.
	
	return _dSize;
}

HSZ zPokeDservEvt::hItem() const {
	return sz2;
}

void* zPokeDservEvt::pData() {
	return _pData;
}

unsigned long zPokeDservEvt::dataSize() {
	return _dSize;
}

HSZ zConnectDservEvt::hServer() const {
	return sz2;
}

HSZ zAdviseDcliEvt::hItem() const {
	return sz2;
}

void* zAdviseDcliEvt::pData() {
	//c Class is passed to a dde client advise transaction and provides access
	//  to information returned from the server.
	//a 2.0
	//f This function returns the pointer to the data that was sent by the dde
	//  server.
	//r Returns the data pointer provided by the dde server.
	
	return _pData;
}

unsigned long zAdviseDcliEvt::dataSize() {
	return _dSize;
}

HSZ zCompleteDcliEvt::hItem() const {
	return sz2;
}

void* zCompleteDcliEvt::pData() {
	return _pData;
}

unsigned long zCompleteDcliEvt::dataSize() {
	return _dSize;
}

HDDEDATA zCompleteDcliEvt::data() {
	return hData;
}

unsigned long zCompleteDcliEvt::tid() {
	return data1;
}

BOOL zCompleteDcliEvt::isValid() {
	return (hData!=0);
}

unsigned long zDdeDev::inst() {
	return _inst;
}

CString& zAdviseLink::item() {
	return _item;
}

zExchangeFmt zAdviseLink::format() {
	return _fmt;
}

zTopicServerDlist* zTopicServer::list() {
	return zDdeDev::cslist;
}

zDdeServer* zTopicServer::serverDev() {
	return dev;
}

zAdviseLinkDlist* zTopicServer::links() {
	return _links;
}

void zTopicServer::links(zAdviseLinkDlist*d) {
	_links=d;
}

CString zTopicServer::topic() {
	return name;
}

HSZ zTopicServer::hTopic() {
	return hName;
}

zDdeDev* zConversation::clientDev() {
	return dev;
}

char* zConversation::topic() {
	return &(_topic[0]);
}

char* zConversation::server() {
	return &(_server[0]);
}

zConversation::operator HCONV() {
	return cid;
}

HSZ zConversation::convString() {
	return csz;
}

zConversation* zTransaction::conv() {
	return _conv;
}

char* zTransaction::item() {
	return &(_item[0]);
}

zTopicServerDlist* zTopicServer::list() {
	return zDdeDev::cslist;
}

#endif
