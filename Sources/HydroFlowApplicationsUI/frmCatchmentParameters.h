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
/* Forms for Catchment Parameter Extraction Functions 
   Copyright Ilwis System Development ITC
   Oct 2004, by Lichun Wang
*/
#ifndef FRMMAPCATCHMENTPARAMETERS_H
#define FRMMAPCATCHMENTPARAMETERS_H

#include "Client\Forms\frmmapcr.h"
#include "Client\Forms\frmtblcr.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldtbl.h"

LRESULT Cmdtopologicaloptimization(CWnd *wnd, const String& s);
LRESULT Cmddrainagenetworkextraction(CWnd *wnd, const String& s);
LRESULT Cmddrainagenetworkordering(CWnd *wnd, const String& s);
LRESULT Cmdcatchmentextraction(CWnd *wnd, const String& s);
LRESULT Cmdcatchmentmerge(CWnd *wnd, const String& s);
LRESULT Cmdflowlengthtooutlet(CWnd *wnd, const String& s);
LRESULT Cmdoverlandflowlength(CWnd *wnd, const String& s);
LRESULT Cmdhortonplots(CWnd *wnd, const String& s);
LRESULT Cmdflowpathlongitudinalprofile(CWnd *wnd, const String& s);
LRESULT Cmdvariablethresholdcomputation(CWnd *wnd, const String& s);

class FormDrainageNetworkExtraction : public FormMapCreate  
{
public:
  _export FormDrainageNetworkExtraction(CWnd* mw, const char* sPar);
private:  
  int exec();
	int ThresholdMapOnSelect(Event *);
  String m_sMap;
	String m_sThresholdMap;
	String m_sFlowMap;
	long m_iThreshold;
	FieldInt *m_fldThreshold;

	bool m_fChkThresholdMap; //flag for threshold map
	CheckBox *m_chkThresholdMap;
	FieldMap *m_fldThresholdMap;
	FieldDataType *m_fltFlowMap;
};

class FormDrainageNetworkOrdering : public FormMapCreate  
{
public:
  _export FormDrainageNetworkOrdering(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sMap;
	String m_sFlowMap;
	String m_sDrainageMap;
	double m_rThreshold;
	
};

class FormCatchmentExtraction : public FormMapCreate  
{
public:
  _export FormCatchmentExtraction(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sFlowMap;
	String m_sDrainageMap;
};

class FormCatchmentMerge : public FormMapCreate  
{
public:
  _export FormCatchmentMerge(CWnd* mw, const char* sPar);
private:  
  int exec();
  int m_iMergeOption;
  int m_iMax;
  String m_sFlowMap;
  String m_sFacc;
  String m_sDEM;
  String m_sDrainageMap;
  String m_sOutletMap;
  String m_sLongestFlowSegmentMap;
  
  int m_iOrderSystem;
  long m_iStreamOrderd;
  bool m_fExtractOriginalOrder;
  bool m_fExtractStreamNetwork;
  bool m_fLongestSegmentMap;
  CheckBox *m_chkSegExtractedMap;
  bool m_fUndefined;
  StaticText *m_stSegmentMapRemark;

  int ExtractSegmentMapOnSelect(Event *);
};


class FormSlopeLengths : public FormMapCreate  
{
public:
  _export FormSlopeLengths(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sFlowMap;
	String m_sDrainageMap;
};

class FormFlowLength2Outlet : public FormMapCreate  
{
public:
  _export FormFlowLength2Outlet(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sFlowMap;
	String m_sDrainageMap;
};

class FormHortonPlots : public FormTableCreate  
{
public:
  _export FormHortonPlots(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sCatchmentMap;
  String m_sMergedCatchmentMap;
	String m_sDrainageMap;
};

class FormFlowPathLongitudinalProfile : public FormTableCreate  
{
public:
  _export FormFlowPathLongitudinalProfile(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sSegMap;
    long m_iSegID;
	double m_rDist;
	String m_sAttMap;
};

class FormTopologicalOptimization : public FormMapCreate  
{
public:
  _export FormTopologicalOptimization(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sFlowMap;
	String m_sDrainageMap;
  String m_sStreamSegMap;
  String m_sModifiedFlowMap;
};

class FieldOneSelectTextOnly;
class FormVariableThresholdComputation : public FormMapCreate  
{
public:
  _export FormVariableThresholdComputation(CWnd* mw, const char* sPar);
private:  
  int exec();
	String m_sMap;
	int m_iFilterSize;
  int m_iMax;
  int m_iClasses;
  bool m_AddString;
  String m_sClasses;
  FieldOneSelectTextOnly *m_fldClasses;
  vector<FieldRowCol*> m_vFld;
  vector<RowCol> m_vRowCol;
  vector<double> m_vFldVal;
  int NrofClassesOnChange(Event *);
  bool m_fReliefMap;
};


#endif // FormVariableThresholdComputation
