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
/* $Log: /ILWIS 3.0/ApplicationForms/frmtblap.h $
 * 
 * 9     24-10-03 16:48 Willem
 * - Changed: TableChangeDomain form now handles the default aggregation
 * selection itself instead of the field. This prevents creating
 * references to temporaries
 * 
 * 8     6-02-03 21:00 Hendrikse
 * changed FieldDataTypeCreate into FieldCoordsys, because it selects an
 * existing one (perhaps make more flexible?)
 * 
 * 7     5-02-03 14:53 Hendrikse
 * adde int FormTableBursaWolf::TableCallBack(Event*)
 * 
 * 6     5-02-03 13:56 Hendrikse
 * added class _export FormTableBursaWolf: public FormTableCreate
 * 
 * 5     7-10-02 14:17 Hendrikse
 * added bppo fSphericalDistance for FormTableCrossVariogram and for
 * FormTableSpatCorr
 * 
 * 4     28-08-01 11:43 Koolhoven
 * FormTableSpatCorr has got a member pntmap to load the Pointmap in the
 * callback to prevent too often constructing one
 * 
 * 3     9-09-99 2:39p Martin
 * ported 2.22 stuff
 * 
 * 2     8-09-99 12:30p Martin
 * Added 2.22 stuff
// Revision 1.3  1998/09/16 17:32:28  Wim
// 22beta2
//
// Revision 1.2  1997/08/13 07:55:09  Wim
// AutoCorr uses FieldDataType
//
/* FormTableApplications
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   24 Jun 98   10:51 am
*/
#ifndef FRMTBLAP_H
#define FRMTBLAP_H
#include "Client\Forms\frmtblcr.h"
#include "Client\FormElements\fldcol.h"

LRESULT Cmdhistogram(CWnd *wnd, const String& s);
LRESULT Cmdautocorr(CWnd *wnd, const String& s);
LRESULT Cmdspatcorr(CWnd *wnd, const String& s);
LRESULT Cmdpattanal(CWnd *wnd, const String& s);
LRESULT Cmdhistnbpol(CWnd *wnd, const String& s);
LRESULT Cmdhistsegdir(CWnd *wnd, const String& s);
LRESULT Cmdcross(CWnd *wnd, const String& s);
LRESULT Cmdtranspose(CWnd *wnd, const String& s);
LRESULT Cmdtblchdom(CWnd *wnd, const String& s);
LRESULT Cmdcrossvariogram(CWnd *wnd, const String& s);
LRESULT Cmdbursawolf(CWnd *wnd, const String& s);
LRESULT Cmdtblglue(CWnd *wnd, const String& s);

class _export FormTableHistogram: public FormTableCreate
{
public:
  FormTableHistogram(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableAutoCorr: public FormTableCreate
{
public:
  FormTableAutoCorr(CWnd* mw, const char* sPar);
private:  
  int exec();
  FieldDataType* fldMap;
  FieldInt* fiMaxShift;
  int CallBack(Event*);
  String sMap;
  int iPixShift;
};

class _export FormTableSpatCorr: public FormTableCreate
{
public:
  FormTableSpatCorr(CWnd* mw, const char* sPar);
private:  
  int exec();
  FieldPointMap* fldMap;
  FieldColumn* fldCol;
  int CallBack(Event*);
  String sMap;
  String sCol;
  double rLagLength;
  int iOmniBi;
  bool fBandWidth;
  double rDir, rTol, rBandWidth;
  PointMap pntmap;
	bool fSphericalDistance;
};

class _export FormTablePattAnal: public FormTableCreate
{
public:
  FormTablePattAnal(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableNeighbourPol: public FormTableCreate
{
public:
  FormTableNeighbourPol(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableLinAnal: public FormTableCreate
{
public:
  FormTableLinAnal(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormMapCross: public FormTableCreate
{
public:
  FormMapCross(CWnd* mw, const char* sPar);
private:  
  int exec();
  bool fMap, fIgnoreUndef1, fIgnoreUndef2;
  String sMap1, sMap2, sOutMap;
};

class _export FormTableHistogramSeg: public FormTableCreate
{
public:
  FormTableHistogramSeg(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableHistogramPol: public FormTableCreate
{
public:
  FormTableHistogramPol(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableHistogramPnt: public FormTableCreate
{
public:
  FormTableHistogramPnt(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormTableTranspose: public FormTableCreate
{
public:
  FormTableTranspose(CWnd* mw, const char* sPar);
private:  
  int exec();
  int CallBack(Event*);
  FieldTable* ftbl;
  FieldDomain* fdom;
  String sTable, sDomain;
  ValueRange vr;
};

class _export FormTableChangeDomain: public FormTableCreate
{
public:
  FormTableChangeDomain(CWnd* mw, const char* sPar);
private:  
  int exec();
  int TblCallBack(Event*);
  String sTable, sDomain, sCol;
  int iColDom;
  bool fAggregate;
  String *sFunc, m_sDefault;
  FieldTable* fldTbl;
  FieldColumn* fldCol;
};

class _export FormTableGlue: public FormTableCreate
{
public:
  FormTableGlue(CWnd* mw, const char* sPar);
private:  
  int exec();
  int TblsCallBack(Event*);
  int TblCallBack(Event*);
  RadioGroup* rgTbls;
  FieldTable *fldTbl1, *fldTbl2, *fldTbl3, *fldTbl4;
  StaticText* stRemark;
  CheckBox* cbVertical;
  int iTbls;
  String sTbl1, sTbl2, sTbl3, sTbl4;
  bool fVertical;
};


class _export FormTableCrossVariogram: public FormTableCreate
{
public:
  FormTableCrossVariogram(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  FieldPointMap* fldMap;
  FieldColumn *fldColA, *fldColB;
  String sMap, sColA, sColB;
  double rLagSpacing;
	bool fSphericalDistance;
};

class _export FormTableBursaWolf: public FormTableCreate
{
public:
  FormTableBursaWolf(CWnd* mw, const char* sPar);
private:  
  int exec();
  FieldTable* fldTblIn;
  FieldColumn *fldColCrds, *fldColHeights;
  FieldCoordSystem* fcs;
  String sTbl, sColCrds, sColHeights;
  double m_rX; 
  double m_rY; 
  double m_rZ; 
  double m_rXrot; 
  double m_rYrot; 
  double m_rZrot;			
  double m_rScaleDiff; 
  String m_sCsyOut;
	int TableCallBack(Event*);
  int CallBackCsyName(Event*);
};

#endif  // FRMTBLAP_H





