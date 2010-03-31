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
/* ui/fentdms.h
   Interface for DMS formentries
   by Wim Koolhoven, oct 1996
   (c) Ilwis System Development ITC
	Last change:  WK   11 Aug 98   10:38 am
*/
#ifndef FENTDMS_H
#define FENTDMS_H

#include "Engine\SpatialReference\Coordsys.h"

class _export FieldDMS: public FormEntry
{
public:
  FieldDMS(FormEntry* parent, const String& sQuestion, double* rDMS, 
           int iMaxDeg=360, bool fAutoAlign = true);
  ~FieldDMS();
  void SetVal(double);
  double rVal();
  void create();             
  void StoreData();          
  FormEntry* CheckData();    
  void SetFocus();           
  void SetHelpTopic(const HelpTopic& htp);
  void SetCallBack(NotifyProc np);
  void SetCallBack(NotifyProc np, CallBackHandler* cb);
  String sGetText();
protected:
  double* rValue;
  int iDeg, iMin;
  double rSec;
  FormEntry* parGroup;
  StaticTextSimple *st, *stDeg, *stMin, *stSec;
  FieldIntSimple *fiDeg, *fiMin;
  FieldRealSimple *frSec;
};

class _export FieldDMSS: public FieldDMS
{
protected:
  FieldDMSS(FormEntry* parent, const String& sQuestion, double* rDMS, 
            int iMaxDeg, bool fAutoAlign);
  void Init();
public:
  ~FieldDMSS();
  double rVal();
  void StoreData();          
  void SetHelpTopic(const HelpTopic& htp);
  void SetCallBack(NotifyProc np);
  void SetCallBack(NotifyProc np, CallBackHandler* cb);
  String sGetText();
protected:
  String str;
  FieldStringSimple* fss;  
};

class _export FieldLat: public FieldDMSS
{
public:
  FieldLat(FormEntry* parent, const String& sQuestion, double* rLat);
  FieldLat(FormEntry* parent, double* rLat);
  void SetVal(double);
  FormEntry* CheckData();    
};

class _export FieldLon: public FieldDMSS
{
public:
  FieldLon(FormEntry* parent, const String& sQuestion, double* rLon);
  FieldLon(FormEntry* parent, double* rLon);
  void SetVal(double);
  FormEntry* CheckData();    
};

class _export FieldLatLon: public FormEntry
{
public:
  FieldLatLon(FormEntry* parent, const String& sQuestion,
                 LatLon *ll);
  FieldLatLon(FormEntry* parent, LatLon *ll);
  ~FieldLatLon();
  void SetVal(const LatLon& ll);
  LatLon llVal();      
  void create();             // overriden
  FormEntry* CheckData();
  void StoreData();          // overriden
  void SetFocus();           // overriden
  void SetHelpTopic(const HelpTopic& htp) // overriden
    { fLat->SetHelpTopic(htp); fLon->SetHelpTopic(htp); }
  void SetCallBack(NotifyProc np) // overriden
    { fLat->SetCallBack(np); fLon->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
    { fLat->SetCallBack(np,cb); fLon->SetCallBack(np,cb); }
  String sGetText();
private:
  FormEntry *st;
  FieldLat* fLat;
  FieldLon* fLon;
  LatLon *_ll;
  LatLon ll;
};

class _export FieldCoordAskLatLon: public FieldLatLon
{
public:
  FieldCoordAskLatLon(FormEntry* parent, const String& sQuestion,
                      const CoordSystem& csy, Coord *c);
  FieldCoordAskLatLon(FormEntry* parent, const CoordSystem& csy, Coord *c);
  void SetVal(const Coord& crd);
  Coord cVal();
  void StoreData();
private:
  CoordSystem cs;
  Coord* crd;
  LatLon ll;  
};




#endif // FENTDMS_H





