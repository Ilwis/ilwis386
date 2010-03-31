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
/* $Log: /ILWIS 3.0/Main/Ddeclass.h $
 * 
 * 6     1/10/01 17:40 Retsios
 * Small cleanup: removed definition of WEP_DdeClass()
 * 
 * 5     1/03/01 20:09 Retsios
 * The originally global variables cslist and tlist are now static members
 * of zDdeDev (main DDE class)
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
// Revision 1.3  1998/09/16 16:20:44  Wim
// 22beta2
//
// Revision 1.2  1997/08/29 16:07:28  Wim
// Added WEP_DdeClass()
//
//
//  Copyright(C) 1992 Inmark Development Corporation.
//
//		All rights reserved.
//
//	This material is confidential and may not be reproduced by any means.
//
//	$Revision: 6 $
//	$Date: 1/10/01 17:40 $
//
*/
#include <list>
using namespace std;

#ifndef _ZAPPDDECLASS_INCLUDED
#define _ZAPPDDECLASS_INCLUDED 1

//#define zExchangeFmt CString
//#define ZF_TEXT zExchangeFmt(0)
class zAdviseLink;
class zTopicServer;
class zAdviseTrans;
typedef list<zAdviseLink*> zAdviseLinkDlist;
typedef list<zTopicServer*> zTopicServerDlist; 
typedef list<zAdviseTrans*> zAdviseTransDlist;

//void WEP_DdeClass();

extern "C" {
#include <ddeml.h>
}

#define zDdeAck DDE_FACK
#define zDdeNAck DDE_FNOTPROCESSED
#define zDdeBusy DDE_FBUSY
	
enum zDdeErrors {
	zNullConnection,
	zUnknownAdvise,
	zUnknownTransaction,
	zFailConversationCreate,
	zFailTransactionCreate,
	zUnknownError
};

typedef int (CCmdTarget::*zDdeErrorProc)(zDdeErrors,int);

class zExchangeFmt 
{
public:
	zExchangeFmt(unsigned int id)
		: eid(id)
		{}
	operator unsigned int() const
		{ return id(); }	 
	unsigned int id() const
		{	return eid; }
	~zExchangeFmt()
		{}
private:
	unsigned int eid;
};

class ZAPPEXPORT zDdeDev {
protected:
	unsigned long _inst;
	BOOL status;
//	static zEvH *errorObj;
//	static zDdeErrorProc errorProc;
public:
	zDdeDev();
	~zDdeDev();
	inline unsigned long inst();
	BOOL isValid();
	static void error(zDdeErrors,int=0);
	void setErrorNotify(CCmdTarget*,zDdeErrorProc);
	CString _serverAppName;
	static zTopicServerDlist * cslist;
	static zAdviseTransDlist * tlist;
private:
	static CCmdTarget* errorObj;
	static zDdeErrorProc errorProc;
};

class ZAPPEXPORT zDdeClient:public zDdeDev {
public:
	zDdeClient();
	~zDdeClient();
};

class ZAPPEXPORT zDdeServer:public zDdeDev {
	HSZ serverAppName;
public:
	zDdeServer(const char *);
	~zDdeServer();
};

class ZAPPEXPORT zDdeEvent {
public:
	unsigned long _dSize;
	void *_pData;
	unsigned int type;
	unsigned int fmt;
	HCONV		conv;
	HSZ			sz1,sz2;
	HDDEDATA	hData;
	DWORD		data1,data2;
	inline HSZ hTopic() const;
	zExchangeFmt dataFormat();
};

class ZAPPEXPORT zAdviseDservEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	CString item();
};

class ZAPPEXPORT zRenderDservEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	unsigned long respondWith(void *,long,zExchangeFmt);
	unsigned long respondWith(char *);
};

class ZAPPEXPORT zRequestDservEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	unsigned long respondWith(void *,long len,zExchangeFmt);
	unsigned long respondWith(char *);
	CString item();
};

class ZAPPEXPORT zExecuteDservEvt:public zDdeEvent {
public:
	inline char *cmd();
	inline unsigned long cmdLength();
};

class ZAPPEXPORT zPokeDservEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	inline void* pData();
	inline unsigned long dataSize();
	CString item();
};

class ZAPPEXPORT zConnectDservEvt:public zDdeEvent {
public:
	inline HSZ hServer() const;
	CString topic();
	CString server();
};

class ZAPPEXPORT zTopicServer;
//declDlist(zTopicServer);

class ZAPPEXPORT zAdviseLink {
	zTopicServer *dad;
	CString _item;
	HSZ _hItem;
	zExchangeFmt _fmt;
public:
	zAdviseLink(zTopicServer*,zAdviseDservEvt*);
	void setDirty();
	inline CString& item();
	inline zExchangeFmt format();
	virtual unsigned long render(zRenderDservEvt*);
	virtual ~zAdviseLink();
};

//declDlist(zAdviseLink);

class ZAPPEXPORT zTopicServer {
	zDdeServer *_serverDev;
	CString name;
	HSZ hName;
	zAdviseLinkDlist *_links;
protected:
	inline zTopicServerDlist* list();
	inline zDdeServer* serverDev();
public:
	zTopicServer(zDdeServer*,const char *);
	virtual void connection(zConnectDservEvt*);
	virtual unsigned long request(zRequestDservEvt*);
	virtual unsigned long adviseStart(zAdviseDservEvt*);
	virtual unsigned long adviseStop(zAdviseDservEvt*);
	virtual unsigned long execute(zExecuteDservEvt*);
	virtual unsigned long poke(zPokeDservEvt*);
	static zTopicServer* find(char*);
	zAdviseLink* findLink(const char *item);
	inline zAdviseLinkDlist* links();
	inline void links(zAdviseLinkDlist*);
	inline CString topic();
	inline HSZ hTopic();
	~zTopicServer();
};

class ZAPPEXPORT zAdviseDcliEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	inline void* pData();
	inline unsigned long dataSize();
};

class ZAPPEXPORT zCompleteDcliEvt:public zDdeEvent {
public:
	inline HSZ hItem() const;
	inline void* pData();
	inline unsigned long dataSize();
	inline HDDEDATA data();
	inline unsigned long tid();
	inline BOOL isValid();
};

class ZAPPEXPORT zConversation {
	zDdeDev *_clientDev;
	BOOL status;
	HCONV cid;
	char _topic[50],_server[50];
	HSZ csz,csv;
public:
	zConversation(zDdeClient *,const char *server,const char *top);
	BOOL isValid();
	inline zDdeDev* clientDev();
	inline char *topic();
	inline char *server();
	inline operator HCONV();
	inline HSZ convString();
	~zConversation();
};

class ZAPPEXPORT zTransaction {
protected:
	zConversation *_conv;
	unsigned long tid;
	unsigned int fmt;
	HSZ dit;
	char _item[100];
	int autodel;
	zTransaction(zConversation*,zExchangeFmt = CF_TEXT);
public:
	zTransaction(zConversation*,char *item,zExchangeFmt = CF_TEXT);
	inline zConversation* conv();
	inline char *item();
	int autoDelete();
	virtual unsigned long completed(zCompleteDcliEvt*);
	virtual ~zTransaction();
};

class ZAPPEXPORT zExecuteTrans:public zTransaction {
public:
	zExecuteTrans(zConversation*,char *cmds,int wait=0);
};

class ZAPPEXPORT zRequestTrans:public zTransaction {
	unsigned long data;
public:
	zRequestTrans(zConversation*,char *item,zExchangeFmt=CF_TEXT,int wait=0);
	void* getSyncData(unsigned long& dataSize);
	void freeSyncData();
};

class ZAPPEXPORT zAdviseTrans;
//declDlist(zAdviseTrans);

class ZAPPEXPORT zAdviseTrans:public zTransaction {
//	static zAdviseTransDlist *tlist;	
public:
	zAdviseTrans(zConversation*,char *item,zExchangeFmt = CF_TEXT);
	static zAdviseTrans* find(zAdviseDcliEvt*);
	virtual unsigned long data(zAdviseDcliEvt*);
	virtual ~zAdviseTrans();
	static zAdviseTransDlist* adviseList();
};

class ZAPPEXPORT zPokeTrans:public zTransaction {
public:
	zPokeTrans(zConversation*,char *item,char* data,int wait=0);
	zPokeTrans(zConversation*,char *item,void* data,
		unsigned int dataLen,zExchangeFmt = CF_TEXT,int wait=0);
};

#include "ddeclass.inl"






#endif
