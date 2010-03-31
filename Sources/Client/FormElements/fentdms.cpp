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
/* ui/fentdms.c
   DMS formentries
   by Wim Koolhoven, oct 1996
   (c) Ilwis System Development ITC
	Last change:  WK   11 Aug 98   10:40 am
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fentdms.h"

FieldDMS::FieldDMS(FormEntry* p, const String& sQuestion, double* rDMS, 
                   int iMaxDeg, bool fAutoAlign)
  : FormEntry(p, 0, fAutoAlign)
{
  rValue = rDMS;
  parGroup = this;
  if (sQuestion.length() != 0) {
    st = new StaticTextSimple(this, sQuestion);
    parGroup = new FieldGroupSimple(this);
    parGroup->Align(st, AL_AFTER);
  }
  else {
//    parGroup = new FieldGroupSimple(this);
    st = 0;
  }  
  parGroup->SetIndependentPos();
  
  double rVal = *rDMS;
  rVal = abs(rVal);
  if (rVal > 360)
    rVal = 0;
    
  iDeg = floor(rVal);
  rVal -= iDeg;
  rVal *= 60;
  iMin = floor(rVal);
  rVal -= iMin;
  rVal *= 60;
  rSec = rVal;
  if (rSec > 59.99) {
    iMin += 1;
    rSec = 0;
  }
  if (iMin > 59) {
    iDeg += 1;
    iMin -= 60;
  }
    
//  iMaxDeg -= 1;
  fiDeg = new FieldIntSimple(parGroup, &iDeg, ValueRange(0L,iMaxDeg),false);
  fiDeg->SetWidth(15);
  stDeg = new StaticTextSimple(parGroup, "°");
	stDeg->SetIndependentPos();
  stDeg->Align(fiDeg, AL_AFTER,1);
  fiMin = new FieldIntSimple(parGroup, &iMin, ValueRange(0L,59L),false);
  fiMin->SetWidth(11);
  fiMin->Align(fiDeg, AL_AFTER, 10);
  stMin = new StaticTextSimple(parGroup, "\'");
	stMin->SetIndependentPos();
  stMin->Align(fiMin, AL_AFTER,1);
  frSec = new FieldRealSimple(parGroup, &rSec, ValueRange(0,60,0.01));
  frSec->SetWidth(20);
  frSec->Align(fiMin, AL_AFTER, 10);
  stSec = new StaticTextSimple(parGroup, "\"");
	stSec->SetIndependentPos();
  stSec->Align(frSec, AL_AFTER,1);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
}

FieldDMS::~FieldDMS()
{}

void FieldDMS::SetVal(double rVal)
{
  if ( rVal != rUNDEF)
  {	
	  rVal = abs(rVal);
	  iDeg = floor(rVal);
	  rVal -= iDeg;
	  rVal *= 60;
	  iMin = floor(rVal);
	  rVal -= iMin;
	  rVal *= 60;
	  rSec = rVal;
	  if (rSec > 59.99) {
		iMin += 1;
		rSec = 0;
	  }
	  if (iMin > 59) {
		iDeg += 1;
		iMin -= 60;
	  }
  }
  else
  {	
	  iDeg = iMin = iUNDEF;
	  rSec = rUNDEF;
  }	
  fiDeg->SetVal(iDeg);
  fiMin->SetVal(iMin);
  frSec->SetVal(rSec);
}

double FieldDMS::rVal()
{
  fiDeg->StoreData();
  fiMin->StoreData();
  frSec->StoreData();
  double rVal = rSec;
  rVal /= 60;
  rVal += iMin;
  rVal /= 60;
  rVal += iDeg;
  return rVal;
}

String FieldDMS::sGetText()
{
  if ( !fShow() ) return "";
  fiDeg->StoreData();
  fiMin->StoreData();
  frSec->StoreData();
  String s("%3i°%2i\'%5.2f\"", iDeg, iMin, rSec);
  return s;
}

void FieldDMS::create()
{
  CreateChildren();
}
             
FormEntry* FieldDMS::CheckData()
{
  FormEntry* fe;
  fe = fiDeg->CheckData();
  if (fe) return fe;
  fe = fiMin->CheckData();
  if (fe) return fe;
  fe = frSec->CheckData();
  if (fe) return fe;
  return FormEntry::CheckData();
}
    
void FieldDMS::StoreData()
{
  *rValue = rVal();
  FormEntry::StoreData();
}
          
void FieldDMS::SetFocus()
{
  fiDeg->SetFocus();
}
           
void FieldDMS::SetHelpTopic(const HelpTopic& htp)
{
  fiDeg->SetHelpTopic(htp);
  fiMin->SetHelpTopic(htp);
  frSec->SetHelpTopic(htp);
}

void FieldDMS::SetCallBack(NotifyProc np)
{
	FormEntry::SetCallBack(np); 
  fiDeg->SetCallBack(np);
  fiMin->SetCallBack(np);
  frSec->SetCallBack(np);
}

void FieldDMS::SetCallBack(NotifyProc np, CallBackHandler* cb)
{
	FormEntry::SetCallBack(np,cb);   
	fiDeg->SetCallBack(np,cb);
  fiMin->SetCallBack(np,cb);
  frSec->SetCallBack(np,cb);
}

FieldDMSS::FieldDMSS(FormEntry* parent, const String& sQuestion, double* rDMS, 
                     int iMaxDeg, bool fAutoAlign)
: FieldDMS(parent, sQuestion, rDMS, iMaxDeg, fAutoAlign)
{
}

FieldDMSS::~FieldDMSS()
{}

void FieldDMSS::Init()
{
  fss = new FieldStringSimple(parGroup, &str, Domain(), false);
  fss->SetWidth(10);
  fss->Align(frSec, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
}

double FieldDMSS::rVal()
{
  double rVal = FieldDMS::rVal();
  fss->StoreData();
  char c = str[0];
  if (c == 'S' || c == 's' || c == 'W' || c == 'w')
    rVal *= -1;
  return rVal;   
}

String FieldDMSS::sGetText()
{
  if ( !fShow() ) return "";
  String s = FieldDMS::sGetText();
  fss->StoreData();
  char c = str[0];
  String sVal("%S %c", s, c);
  return sVal;
}

void FieldDMSS::StoreData()          
{
  *rValue = rVal();
}

void FieldDMSS::SetHelpTopic(const HelpTopic& htp)
{
  FieldDMS::SetHelpTopic(htp);
  fss->SetHelpTopic(htp);
}

void FieldDMSS::SetCallBack(NotifyProc np)
{
  FieldDMS::SetCallBack(np);
  fss->SetCallBack(np);
}

void FieldDMSS::SetCallBack(NotifyProc np, CallBackHandler* cb)
{
  FieldDMS::SetCallBack(np,cb);
  fss->SetCallBack(np,cb);
}

FieldLat::FieldLat(FormEntry* parent, const String& sQuestion, double* rLat)
: FieldDMSS(parent, sQuestion, rLat, 90, true)
{
  if (*rLat >= 0 || *rLat == rUNDEF)
    str = String("N");
  else
    str = String("S");
  Init();    
}

FieldLat::FieldLat(FormEntry* parent, double* rLat)
: FieldDMSS(parent, "", rLat, 90, false)
{
  if (*rLat >= 0 || *rLat == rUNDEF)
    str = String("N");
  else
    str = String("S");
  Init();    
}

void FieldLat::SetVal(double rVal)
{
  if (rVal >= 0 || rVal == rUNDEF)
    str = String("N");
  else
    str = String("S");
  FieldDMS::SetVal(rVal);  
  fss->SetVal(str);
}

FormEntry* FieldLat::CheckData()
{
  FormEntry* fe = FieldDMSS::CheckData();
  if (fe) return fe;
  fss->StoreData();
  switch (str[0]) {
    case 's':
      fss->SetVal(String('S',1));
    case 'S':
      return 0;
    case 'n':
      fss->SetVal(String('N',1));
    case 'N':
      return 0;
    default:
      fss->SetVal(String('N',1));
      return fss;  
  }
}
    
FieldLon::FieldLon(FormEntry* parent, const String& sQuestion, double* rLon)
: FieldDMSS(parent, sQuestion, rLon, 360, true)
{
  if (*rLon >= 0 || *rLon == rUNDEF)
    str = String("E");
  else
    str = String("W");
  Init();    
}

FieldLon::FieldLon(FormEntry* parent, double* rLon)
: FieldDMSS(parent, "", rLon, 360, false)
{
  if (*rLon >= 0 || *rLon == rUNDEF)
    str = String("E");
  else
    str = String("W");
  Init();    
}

void FieldLon::SetVal(double rVal)
{
  if (rVal >= 0 || rVal == rUNDEF)
    str = String("E");
  else
    str = String("W");
  FieldDMS::SetVal(rVal);  
  fss->SetVal(str);
}

FormEntry* FieldLon::CheckData()
{
  FormEntry* fe = FieldDMSS::CheckData();
  if (fe) return fe;
  fss->StoreData();
  switch (str[0]) {
    case 'w':
      fss->SetVal(String('W',1));
    case 'W':
      return 0;
    case 'e':
      fss->SetVal(String('E',1));
    case 'E':
      return 0;
    default:
      fss->SetVal(String('E',1));
      return fss;  
  }
}
    
////// FieldLatLon

FieldLatLon::FieldLatLon(FormEntry* p, const String& sQuestion,
                               LatLon* latlon)
  : FormEntry(p, 0)
{
  _ll = latlon;
  ll = *latlon;
  FormEntry* parGroup = this;
  fLat = new FieldLat(parGroup, sQuestion, &ll.Lat);
  fLon = new FieldLon(parGroup, "", &ll.Lon);
  fLon->Align(fLat, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
}

FieldLatLon::FieldLatLon(FormEntry* p, LatLon* latlon)
  : FormEntry(p, 0, true), st(0)
{
  _ll = latlon;
  ll = *latlon;
  fLat = new FieldLat(this, &ll.Lat);
  fLon = new FieldLon(this, &ll.Lon);
  fLon->Align(fLat, AL_AFTER, 10);
  if (_npChanged)
    SetCallBack(_npChanged,_cb);
}

FieldLatLon::~FieldLatLon()
{
 // children are removed by ~FormEntry
}

void FieldLatLon::create()
{
  CreateChildren();
}

void FieldLatLon::SetVal(const LatLon& ll)
{
  fLat->SetVal(ll.Lat);
  fLon->SetVal(ll.Lon);
}

LatLon FieldLatLon::llVal()
{
  LatLon ll;
  ll.Lat = fLat->rVal();
  ll.Lon = fLon->rVal();
  return ll;
}

String FieldLatLon::sGetText()
{
  if ( !fShow() ) return "";
  LatLon ll = llVal();
  return ll.sValue();
}

FormEntry* FieldLatLon::CheckData()
{ FormEntry *fe;
  fe = fLat->CheckData();
  if (fe) return fe;
  fe = fLon->CheckData();
  if (fe) return fe;
  return FormEntry::CheckData();
}

void FieldLatLon::StoreData()
{
  fLat->StoreData();
  fLon->StoreData();
  if (_ll != 0) 
    *_ll = ll;

  FormEntry::StoreData();
}


void FieldLatLon::SetFocus()
{
  fLat->SetFocus();
  _frm->ilwapp->SetHelpTopic(htp());
}

FieldCoordAskLatLon::FieldCoordAskLatLon(FormEntry* parent, 
                    const String& sQuestion,
                    const CoordSystem& csy, Coord *c)
: FieldLatLon(parent, sQuestion, (ll = csy->llConv(*c), &ll)),
cs(csy), crd(c)
{
  ll = cs->llConv(*crd);
}                        
  
FieldCoordAskLatLon::FieldCoordAskLatLon(FormEntry* parent, 
                    const CoordSystem& csy, Coord *c)
: FieldLatLon(parent, (ll = csy->llConv(*c), &ll)),
cs(csy), crd(c)
{
  ll = cs->llConv(*crd);
}                        
  
void FieldCoordAskLatLon::SetVal(const Coord& crd) 
{ 
  FieldLatLon::SetVal(cs->llConv(crd)); 
}

Coord FieldCoordAskLatLon::cVal()
{ 
  return cs->cConv(llVal()); 
}
  
void FieldCoordAskLatLon::StoreData() 
{ 
  FieldLatLon::StoreData();
  *crd = cs->cConv(ll);
}  







