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
/* FormMatrixMapApplications
   Copyright Ilwis System Development ITC
   february 1996, by Wim Koolhoven
	Last change:  WK   10 Feb 97    2:17 pm
*/
#ifndef FRMMATAP_H
#define FRMMATAP_H
#include "Client\Forms\frmmatcr.h"
#include "Engine\Map\Raster\MapList\maplist.h"

class FieldMapList;
class FieldInt;

LRESULT Cmdprinccmp(CWnd *wnd, const String& s);
LRESULT Cmdfactanal(CWnd *wnd, const String& s);
LRESULT Cmdmatvarcov(CWnd *wnd, const String& s);
LRESULT Cmdmatcorr(CWnd *wnd, const String& s);

class _export FormMatrixPrincCmp: public FormMatrixCreate
{
public:
  FormMatrixPrincCmp(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapListCallBack(Event*);
  int OutBandsCallBack(Event*);
  String sMapList;
  int iOutBands;
  FieldMapList* fldMpl;
  FieldInt* fldOutBands;
  MapList mpl;
};

class _export FormMatrixFactorAnal: public FormMatrixCreate
{
public:
  FormMatrixFactorAnal(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapListCallBack(Event*);
  int OutBandsCallBack(Event*);
  String sMapList;
  int iOutBands;
  FieldMapList* fldMpl;
  FieldInt* fldOutBands;
  MapList mpl;
};

class _export FormMatrixVarCov: public FormMatrixCreate
{
public:
  FormMatrixVarCov(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMapList;
};

class _export FormMatrixCorr: public FormMatrixCreate
{
public:
  FormMatrixCorr(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMapList;
};

#endif  // FRMMATAP_H





