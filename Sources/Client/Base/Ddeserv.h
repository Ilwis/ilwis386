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
/* $Log: /ILWIS 3.0/Main/Ddeserv.h $
 * 
 * 5     30-10-01 18:05 Retsios
 * Preserve cwcs: we need it to check if coordinate conversion is
 * necessary to get map value at coordinate crd.
 * 
 * 4     8/29/00 12:41 Retsios
 * Bug 1153 repaired: ';' is now allowed at certain places (between quotes
 * and brackets) in DDE commands.
 * 
 * 3     6/22/00 18:55 Retsios
 * Added UpdateCoordinate()
 * 
 * 2     6/22/00 12:59 Retsios
 * DDE preliminary version .. compiles, but not tested yet.
 * 
 * 1     6/22/00 10:32 Retsios
// Revision 1.2  1998/04/22 17:10:10  Wim
// Added TopicServer::command() function to handle individual commands inside execute()
//
/* ddeserv.h
// DdeServer
// by Wim Koolhoven
// (c) Computer Department ITC
	Last change:  WK   22 Apr 98    5:44 pm
*/
#ifndef ILWDDESERV_H
#define ILWDDESERV_H
//#include "dsp.h"
#include "Client\Base\Ddeclass.h"
//#include "Engine\Base\DataObjects\Dat2.h"

class _export DdeServer;
class _export TopicServerSystem;
class _export TopicServerCalc;
class _export TopicServerCoord;
class _export AdviseLinkCoord;

class _export DdeServer: public zDdeServer
{
public:
  DdeServer();
  ~DdeServer();
  void UpdateCoordinate(CoordWithCoordSystem*);
private:
  TopicServerSystem* tsSystem;
  TopicServerCalc* tsCalc;
  TopicServerCoord* tsCoord;
};

class _export TopicServerSystem: public zTopicServer
{
public:
  TopicServerSystem(zDdeServer*);
  virtual unsigned long request(zRequestDservEvt*);
  virtual unsigned long execute(zExecuteDservEvt*);
private:
  unsigned long command(char*);
	char * parsedStrChr(char * str, int c);
	char * minPtr(char * a, char * b);
};

class _export TopicServerCoord: public zTopicServer
{
public:
  TopicServerCoord(zDdeServer*);
  ~TopicServerCoord();
  virtual unsigned long request(zRequestDservEvt*);
  virtual unsigned long adviseStart(zAdviseDservEvt*);
  int Update(CoordWithCoordSystem);
  CoordWithCoordSystem cwcsValue() const;
private:
  CoordWithCoordSystem cwcs;
};

class StringFromCoord
{
public:
  StringFromCoord(const String&);
  String sValue(const CoordWithCoordSystem&) const;
private:  
  enum { eNONE, eX, eY, eXY, eMAP } eOption;
  BaseMap map;
};

class _export AdviseLinkCoord: public zAdviseLink
{
public:
  AdviseLinkCoord(TopicServerCoord*,zAdviseDservEvt*);
  virtual unsigned long render(zRenderDservEvt*);
private:
  TopicServerCoord* tsCoord;
  StringFromCoord sfc;
};

class _export TopicServerCalc: public zTopicServer
{
public:
  TopicServerCalc(zDdeServer*);
  ~TopicServerCalc();
  virtual unsigned long request(zRequestDservEvt*);
};

#endif // ILWDDESERV_H
