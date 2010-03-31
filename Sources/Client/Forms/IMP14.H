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
/* 
  Import14
  by Wim Koolhoven
  (c) ILWIS System Development, ITC
	Last change:  JEL  20 Feb 97    1:20 pm
*/

int _export Import14(CWnd* wPar);
int _export Import14(CWnd* wPar, const String& sMask);

class _export Import14Form: public FormWithDest
{
public:
  int DomCallBack(Event*);
protected:
  Import14Form(CWnd* wPar, const String& sTitle);
  RadioGroup* rg;  
  FieldDataTypeCreate* fldDom;
  int iDom;
  String sDom;
  String sDescr;
  ValueRange vr;
public:
  ~Import14Form();
};

class _export Import14MaskForm: public Import14Form
{
public:
  Import14MaskForm(CWnd* wPar, String* sMask);
};

class _export Import14MapForm: public Import14Form
{
public:
  Import14MapForm(CWnd* wPar, const FileName& fn, const String& sDef="");
  int import();
private:
  int CallBack(Event*);
  FileName fnMap;
  bool fClass, fId, fValue, fPicture, fImage, fBit;
  String sGrf, sOutMap;
};

class _export Import14SegmentMapForm: public Import14Form
{
public:
  Import14SegmentMapForm(CWnd* wPar, const FileName& fn, const String& sDef="");
private:
  int exec();
  int CallBack(Event*);
  FileName fnMap;
  String sOutMap;
};

class _export Import14PolygonMapForm: public Import14Form
{
public:
  Import14PolygonMapForm(CWnd* wPar, const FileName& fn, const String& sDef="");
private:
  int exec();
  int CallBack(Event*);
  FileName fnMap;
  String sOutMap;
};

class _export Import14PointMapForm: public Import14Form
{
public:
  Import14PointMapForm(CWnd* wPar, const FileName& fn, const String& sDef="");
private:
  int exec();
  int CallBack(Event*);
  FileName fnMap;
  bool fNameCol;
  String sAttTbl;
  String sOutMap;
};

class _export Import14TableForm: public Import14Form
{
public:
  Import14TableForm(CWnd* wPar, const FileName& fn, const String& sDef="");
private:
  int exec();
  FileName fnTable;
  bool fNameCol;
  String sOutTbl;
};



