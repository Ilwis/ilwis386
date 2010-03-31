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
/* $Log: /ILWIS 3.0/FormElements/fldmap.h $
 * 
 * 5     27-07-00 13:01 Koolhoven
 * improved layout of Create Vector Map forms
 * 
 * 4     3/24/00 4:14p Hendrikse
 * header comment corrected
 * 
 * 3     3/24/00 3:08p Hendrikse
 * renamed  CallBack(Event*)   to  CallBackXY(Event*);
 /* 
 * 2     3/23/00 1:04p Hendrikse
 * added 
 * FieldLat *fldMinLat;
 *   FieldLat *fldMaxLat;
 *   FieldLon *fldMinLon;
 *   FieldLon *fldMaxLon;
 * in FormCreateVectorMap 
 * to be used in CallBack for wrong LatLon bounds
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/10 16:53:53  Wim
// FormCreateMap tells now about bytes per pixels
//
/* FieldMap
   by Wim Koolhoven, november 1995
   Copyright Ilwis System Development ITC

   5/6/97 Restructured:
   - FormCreateBaseMap
     - FormCreateMap
     - FormCreateVectorMap
       - FormCreateSeg
       - FormCreatePol
       - FormCreatePnt
   All the available forms create the map in the exec() function.

	Last change:  WK   10 Sep 97    6:24 pm
*/
   
#ifndef FIELDMAP_H
#define FIELDMAP_H
#include "Client\FormElements\fentvalr.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Client\FormElements\fldcs.h"
#include "Client\FormElements\fentdms.h"

class FormCreateBaseMap: public FormWithDest
{
protected:
  _export FormCreateBaseMap(CWnd* wPar, const String& sTitle,
    const String& sExt, String* sMap, const String& sDom);
  void initDomain(long dmTypes);
  int exec();
  const String sExt;
  String sNewName;
  String sDescr;
  String sDomain;
  ValueRange vr;
//private:
  String* sMap;
  void initDescr();
  int CallBackName(Event*);
  int DomainCallBack(Event*);
  FieldDataTypeCreate* fmc;
  FieldDomainC* fdc;
  FieldValueRange* fvr;
  StaticText* stRemark;
};

class FormCreateMap: public FormCreateBaseMap
{
public:
  _export FormCreateMap(CWnd* wPar, String* sMap,
    const String& sGrf, const String& sDomain);
private:
  int exec();
  int DomainCallBack(Event*);
  int ValueRangeCallBack(Event*);
  void SetRemarkOnBytesPerPixel();
  String sGeoRef;
};

class FormCreateVectorMap: public FormCreateBaseMap
{
protected:
  _export FormCreateVectorMap(CWnd* wPar, const String& sTitle,  // ask csys and cb
    const String& sExt, String* sMap,
    const String& sCsy, const String& sDom);
  _export FormCreateVectorMap(CWnd* wPar, const String& sTitle,  // ask only cb
    const String& sExt, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDom);
  int exec();
  String sCsys;
  CoordBounds cb;
  LatLon llMin, llMax;
private:
  int CSysCallBack(Event*);
  int CallBackXY(Event*);
  int CallBackLatLon(Event*);
  FieldCoordSystemC *fcsc;
	FieldGroup *fgCoord, *fgLatLon;
  FieldCoord *fldCrdMin, *fldCrdMax;
  FieldLat *fldMinLat;
  FieldLat *fldMaxLat;
  FieldLon *fldMinLon;
  FieldLon *fldMaxLon;
};

class FormCreateSeg: public FormCreateVectorMap
{
public:
  _export FormCreateSeg(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain);
  _export FormCreateSeg(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain);
  int exec();
};

class FormCreatePol: public FormCreateVectorMap
{
public:
  _export FormCreatePol(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain);
  _export FormCreatePol(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain);
  int exec();
};

class FormCreatePnt: public FormCreateVectorMap
{
public:
  _export FormCreatePnt(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain);
  _export FormCreatePnt(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain);
  int exec();
};

#endif // FIELDMAP_H




