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
/* DefForm
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   23 Jul 97    8:53 am
*/

#ifndef DEFFORM_H
#define DEFFORM_H

#include "Client\FormElements\formbase.h"

UINT ShowDefForm(LPVOID lpObjectStruct);

class _export ObjectDefForm: public FormWithDest
{
public:
	bool fShow;
protected:
  ObjectDefForm(CWnd* wPar, const IlwisObject& obj, const String& sTitle);
  afx_msg void OnShow();
  afx_msg void OnDefine();
  DECLARE_MESSAGE_MAP();
};

class _export MapDefForm: public ObjectDefForm
{
public:
  MapDefForm(CWnd* wPar, Map&, bool fShowButtons, bool fBreakDep);
	~MapDefForm();
private:
  int exec();
  ValueRange vr;
  String sExpr, sDomain, sGeoRef, sUiExpr; 
  int CallBackDomainChange(Event*);
  int ValueRangeCallBack(Event*);   
//  int CallBackStepChange(zEvent*);
  int SetDefaults(Event*);
  int CallBackGeoRefChange(Event*);
  virtual FormEntry* feDefaultFocus();
  Map mp;
  FieldDomainC* fdm;
  StaticText* stDomain;
  FieldValueRange* frValueRange;
//  FieldReal* frStep;
  FieldGeoRefExisting* fgr;
  StaticText* stGeoRef;
  FieldString* fsExpr, *fsDesc;
  Map mpTmp;
  bool fSetDescFromExpr;
  StaticText* stRemark;
};
  
class _export SegmentMapDefForm: public ObjectDefForm
{
public:
  SegmentMapDefForm(CWnd* wPar, SegmentMap&);
private:
  SegmentMap mp;
};
  
class _export PolygonMapDefForm: public ObjectDefForm
{
public:
  PolygonMapDefForm(CWnd* wPar, PolygonMap&);
private:
  PolygonMap mp;
};
  
class _export PointMapDefForm: public ObjectDefForm
{
public:
  PointMapDefForm(CWnd* wPar, PointMap&);
private:
  PointMap mp;
};
  
class _export TableDefForm: public ObjectDefForm
{
public:
  TableDefForm(CWnd* wPar, const Table&);
private:
  Table tbl;
};

/*
class _export MatrixDefForm: public ObjectDefForm
{
public:
  MatrixDefForm(CWnd* wPar, MatrixObject&);
private:
  MatrixObject mat;
};
*/
 
#endif // DEFFORM_H
 
