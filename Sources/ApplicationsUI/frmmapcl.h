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
/* FormMapCalc
   Copyright Ilwis System Development ITC
   february 1996, by Wim Koolhoven
	Last change:  WK   16 Jul 97    9:59 am
*/
#ifndef FRMMAPCALC_H
#define FRMMAPCALC_H
#include "Client\Forms\frmmapcr.h"

LRESULT Cmdmapcalc(CWnd *wnd, const String& s);
LRESULT Cmdmapiter(CWnd *wnd, const String& s);
LRESULT Cmdmaplistcalc(CWnd *wnd, const String& s);

class _export FormMapCalc: public FormMapCreate
{
public:
  FormMapCalc(CWnd* mw, const char* sPar);
private:  
  int exec();
  int SetDefaults(Event*);
  String sExpr;
  FieldStringMulti* fsExpr;
};

class _export FormMapIter: public FormMapCreate
{
public:
  FormMapIter(CWnd* mw, const char* sPar);
private:  
  int exec();
  int SetDefaults(Event*);
  String sInitMap, sExpr;
  bool fPropagation;
  int iStopCriterium, iNrIterations;
  FieldStringMulti* fsExpr;
};

class FieldMapList;
class _export FormMapListCalc: public FormMapCreate
{
public:
  FormMapListCalc(CWnd* mw, const char* sPar);
	int iNrMpl();
private:  
  int exec();
	int MapListNrChangeCallback(Event*);
	int MapListChangeCallback(Event*);
  String sExpr;
  FieldStringMulti* fsExpr;
	FieldInt* m_fieldMinBand;
	FieldInt* m_fieldMaxBand;
	FieldInt* m_fiNrMpl;
	long m_iStart;
	long m_iEnd;
	long m_iMinBand;
	long m_iMaxBand;
	long m_iOffset;
	int m_iNrMpl;
	bool keepOriginal;
	vector <String> m_sml;
	vector <FieldMapList*> m_vfml;
	const int iMAX;
};

#endif  // FRMMAPCALC_H





