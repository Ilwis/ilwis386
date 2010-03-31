//
//  Copyright(C) 1992 Inmark Development Corporation.
//
//		All rights reserved.
//
//	This material is confidential and may not be reproduced by any means.
//
//	$Revision: 2 $
//	$Date: 22-06-00 16:00 $
//

inline HSZ zDdeEvent::hTopic() const {
	return sz1;
}

inline HSZ zAdviseDservEvt::hItem() const {
	return sz2;
}

inline HSZ zRenderDservEvt::hItem() const {
	return sz2;
}

inline HSZ zRequestDservEvt::hItem() const {
	return sz2;
}

inline char* zExecuteDservEvt::cmd() {
	return (char*)_pData;
}

inline unsigned long zExecuteDservEvt::cmdLength() {
	return _dSize;
}

inline HSZ zPokeDservEvt::hItem() const {
	return sz2;
}

inline void* zPokeDservEvt::pData() {
	return _pData;
}

inline unsigned long zPokeDservEvt::dataSize() {
	return _dSize;
}

inline HSZ zConnectDservEvt::hServer() const {
	return sz2;
}

inline HSZ zAdviseDcliEvt::hItem() const {
	return sz2;
}

inline void* zAdviseDcliEvt::pData() {
	return _pData;
}

inline unsigned long zAdviseDcliEvt::dataSize() {
	return _dSize;
}

inline HSZ zCompleteDcliEvt::hItem() const {
	return sz2;
}

inline void* zCompleteDcliEvt::pData() {
	return _pData;
}

inline unsigned long zCompleteDcliEvt::dataSize() {
	return _dSize;
}

inline HDDEDATA zCompleteDcliEvt::data() {
	return hData;
}

inline unsigned long zCompleteDcliEvt::tid() {
	return data1;
}

inline BOOL zCompleteDcliEvt::isValid() {
	return (hData!=0);
}

inline unsigned long zDdeDev::inst() {
	return _inst;
}

inline CString& zAdviseLink::item() {
	return _item;
}

inline zExchangeFmt zAdviseLink::format() {
	return _fmt;
}
/*
inline zTopicServerDlist* zTopicServer::list() {
	return cslist;
}
*/
inline zDdeServer* zTopicServer::serverDev() {
	return _serverDev;
}

inline zAdviseLinkDlist* zTopicServer::links() {
	return _links;
}

inline void zTopicServer::links(zAdviseLinkDlist*d) {
	_links=d;
}

inline CString zTopicServer::topic() {
	return name;
}

inline HSZ zTopicServer::hTopic() {
	return hName;
}

inline zDdeDev* zConversation::clientDev() {
	return _clientDev;
}

inline char* zConversation::topic() {
	return &(_topic[0]);
}

inline char* zConversation::server() {
	return &(_server[0]);
}

inline zConversation::operator HCONV() {
	return cid;
}

inline HSZ zConversation::convString() {
	return csz;
}

inline zConversation* zTransaction::conv() {
	return _conv;
}

inline char* zTransaction::item() {
	return &(_item[0]);
}




