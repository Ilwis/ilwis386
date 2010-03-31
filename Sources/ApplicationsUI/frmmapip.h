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
/* Form Map Applications Image Processing
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   23 May 97    2:24 pm
*/
#ifndef FRMMAPIP_H
#define FRMMAPIP_H
#include "Engine\Function\FILTER.H"
#include "Client\Forms\frmmapcr.h"
#include "Client\Forms\frmtblcr.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"

class FieldFilter;
class FieldFilterC;
class FieldFilterType;
class FieldSampleSet;
class FieldOneSelectTextOnly;

LRESULT OpenMapListColorComp(CWnd *wnd, const String& s);
LRESULT Cmdclassify(CWnd *wnd, const String& s);
LRESULT Cmdstretch(CWnd *wnd, const String& s);
LRESULT Cmdcluster(CWnd *wnd, const String& s);
LRESULT Cmdcolorsep(CWnd *wnd, const String& s);
LRESULT Cmdmapliststatistics(CWnd *wnd, const String& s);

LRESULT Cmdfilter(CWnd *wnd, const String& s);

class FormFilterMap: public FormMapCreate
{
public:
  _export FormFilterMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap, sFil;
  int MapCallBack(Event*);
  int FilTypeCallBack(Event*);
  int FilLinCallBack(Event*);
  int FilBinCallBack(Event*);
  int FilMajPrdCallBack(Event*);
  int FilRnkPrdCallBack(Event*);
  int FilMajCallBack(Event*);
  int FilRnkCallBack(Event*);
  int FilAvgCallBack(Event*);
  int FilMedCallBack(Event*);
  int FilPatCallBack(Event*);
  int CallBack(Event*);
  int DomainCallBack(Event*);
  void EnableOutFields();
  void DisableOutFields();
  FieldDataType* fldMap;
  StaticText* stMapRemark;
  long iType;
  int iRgVal;
  RowCol rc;
  int iRank;
  bool fPreDef, fThreshold, fUndefOnly;
  double rThreshold;
  
  FieldFilterType* fft;
  FieldGroup *fgLin, *fgBin, *fgAvg, *fgMaj, *fgRnk, *fgMed, *fgPat;
  FieldGroup *fgUMaj, *fgURnk;
  FieldFilterC* fldFilLin;
  FieldFilter *fldFilBin, *fldFilMaj, *fldFilRnk;
  StaticText *stFilLin, *stFilBin, *stFilMaj, *stFilRnk;
  
  RadioGroup* rg;
  RadioButton *rbAverage, *rbRankOrder, *rbMedian, *rbPattern;
  FieldInt *fiRows, *fiCols;
  Filter flt;
  Map mp;
};

class FormColorComp: public FormMapCreate
{
public:  
  _export FormColorComp(CWnd* mw, const char* sPar);
private:
  int exec();
  bool f24Bit, fPerc;
  int iRgbHsi, iMethod, iInterpol;
  String sMapRed, sMapGreen, sMapBlue, sMapHue, sMapSat, sMapInt;
  int iColors;
  RangeReal rrRed, rrGreen, rrBlue;
  double rPercRed, rPercGreen, rPercBlue;
  int CallBack24Bit(Event*);
  int RgbHsiCallBack(Event*);
  int StretchCallBack(Event*);
  int PercCallBack(Event*);
  CheckBox* cb24Bit, *cbPerc;
  RadioGroup *rgRgbHsi, *rgMethod;
  FieldGroup *fgStretch, *fgRGB, *fgHSI;
  FieldRangeReal *fRangeRed, *fRangeGreen, *fRangeBlue;
  FieldReal *fPercRed, *fPercGreen, *fPercBlue;
};  

class FormClassifyMap: public FormMapCreate
{
public:
  _export FormClassifyMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  RadioGroup* rg;
  int ClfCallBack(Event*);
	int PriorProbTblChanged(Event*);
	int SampleSetChanged(Event*);
	FieldSampleSet *fssMap;
  FieldReal *frFact;
  CheckBox *cbDist;
  int iClf;
  bool fDist;
  double rFact, rDist;
	String sTable, sCol;
	FieldTable* fldPriorProbTbl;
  FieldColumn* fldPriorProbCol;
};

class FormMapStretch: public FormMapCreate
{
public:
  _export FormMapStretch(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  int MethodCallBack(Event*);
  FieldDataType* fldMap;
  RadioGroup* rgMethod;
  FieldReal* frPerc;
  FieldRangeReal* frrMinMax;
  String sMap;
  Map mp;
  int iMethod, iFrom;
  RangeReal rr;
  double rPerc;
  long iIntervals;
};

class FormMapCluster: public FormMapCreate
{
public:  
  _export FormMapCluster(CWnd* mw, const char* sPar);
private:
  int exec();
  int MapsCallBack(Event*);
  int iClasses;
  int iMaps;
  RadioGroup* rgMaps;
  FieldDataType *fm1, *fm2, *fm3, *fm4;
  String sMap1, sMap2, sMap3, sMap4;
	FieldTableCreate* ftc;
  String sOutTable;
  bool fStatTable;
};  

class FormMapColorSep: public FormMapCreate
{
public:
  _export FormMapColorSep(CWnd* mw, const char* sPar);
private:
  int exec();
  String sMap;
  int iOption;
};

class FormMapMaplistStatistics: public FormMapCreate
{
public:  
    _export FormMapMaplistStatistics(CWnd* mw, const char* sPar);

private:
    int exec();
    int MaplistCallBack(Event*);
    int StaticCallback(Event*);
		
		static String sFuncConvert(String sFunc);

    String m_sMaplist;
    String* m_psFunction;
    long m_iStart, m_iEnd;
		long m_iOffset;

    long m_iMinBand, m_iMaxBand;
    bool m_FunctionListInitialized;
		bool m_fNewMaplist;

    FieldDataType *m_fieldMaplist;
    FieldInt *m_fieldMinBand, *m_fieldMaxBand;
};  


#endif  // FRMMAPIP_H





