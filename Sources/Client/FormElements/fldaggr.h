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
/* Interface for FieldAggregate
   by Wim Koolhoven, january 1997
   (c) Ilwis System Development ITC
	Last change:  WK   29 May 98    5:03 pm
*/
#ifndef FIELDAGGREGATE_H
#define FIELDAGGREGATE_H

class FieldAggrFuncSimple;
class FieldRegressionFuncSimple;
class FieldAggrLastFuncSimple;

class FieldAggrFunc: public FieldGroup
{
public:
  _export FieldAggrFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault);
  virtual void _export SetCallBack(NotifyProc np); 
  virtual void _export SetCallBack(NotifyProc np, CallBackHandler* cb); 
private:
  FieldAggrFuncSimple* fafs;
};

class _export FieldRegressionFunc: public FieldGroup
{
public:
  FieldRegressionFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault);
  virtual void SetCallBack(NotifyProc np); 
  virtual void SetCallBack(NotifyProc np, CallBackHandler* cb); 
private:
  FieldRegressionFuncSimple* frfs;
};

class _export FieldAggrLastFunc: public FieldGroup
{
public:
  FieldAggrLastFunc(FormEntry* parent, const String& sQuestion, String** sFunc, const String& sDefault);
  virtual void SetCallBack(NotifyProc np);
  virtual void  SetCallBack(NotifyProc np, CallBackHandler* cb); 
private:
  FieldAggrLastFuncSimple* fafs;
};

#endif FIELDAGGREGATE_H
