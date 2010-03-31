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
/* FormHydroFlowFunctions
   Copyright Ilwis System Development ITC
   March 2003, by Lichun Wang
*/
#ifndef FRMMAPHYDROFLOW_H
#define FRMMAPHYDROFLOW_H

//#include "ApplicationForms\frmmapcr.h"
//#include "FormElements\FldOneSelectTextOnly.h"
//#include "FormElements\fldcol.h"
//#include "FormElements\fldtbl.h"
//#include "RasterApplication\MapDEMOptimization.h"
#include "Client\Forms\frmmapcr.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldtbl.h"
//#include "Applications\Raster\MapDEMOptimization.h"

LRESULT Cmdfillsinks(CWnd *wnd, const String& s);
LRESULT Cmdflowaccumulation(CWnd *wnd, const String& s);
LRESULT Cmddemoptimization(CWnd *wnd, const String& s);
LRESULT Cmdflowdirection(CWnd *wnd, const String& s);

class FormMapFillSinks : public FormMapCreate  
{
public:
  _export FormMapFillSinks(CWnd* mw, const char* sPar);
private:  
  int exec();
  int m_iMethod;
	String m_sMap;
};

class FormMapFlowDirection : public FormMapCreate  
{
public:
  _export FormMapFlowDirection(CWnd* mw, const char* sPar);
private:  
  int exec();
	
	int m_iMethod;
  String sMap;
  bool m_fParallelDrainageCorrection;
  CheckBox *m_chkParallelDrainageCorrection;
  int PDCOnSelect(Event *);
};

class FormMapFlowAccumulation : public FormMapCreate  
{
public:
  _export FormMapFlowAccumulation(CWnd* mw, const char* sPar);
private:  
  int exec();
	String sMap;
};

class FormMapDemOptimization : public FormMapCreate  
{
public:
  _export FormMapDemOptimization(CWnd* mw, const char* sPar);
	static ValueRange _export vrDefault(const Map& map);
private:
  //DrainageMapType m_DrainageMapType;
	enum DrainageMapType{dmtSegment, dmtPolygon} m_DrainageMapType;
	void ShowCols();
	void ShowVals();
	void FillCols();
	int exec();
	int DrainageMap(Event *);
	int TableFlagOnSelect(Event *);
	int TableOnSelect(Event *);
	int SetDefDomain(Event *);

	String m_sMap;
	String m_sDrainageMap;
	String m_sTbl;
	
	StaticText *m_stBufDist;
	StaticText *m_stSmooth;
	FieldMap *m_fldmp;
	CheckBox *m_chkAttTbl;
	bool m_fChkAttTbl;	//flag for attribute table
	FieldDataType *m_fdtDrainage;
	FieldTable *m_ftbl;
	FieldColumn *m_fldColBufDist,*m_fldColSmooth, *m_fldColSharp;;
	String m_sBufDist, m_sSmooth, m_sSharp; //column name
	FieldReal *m_fldBufDist,*m_fldSmooth, *m_fldSharp; 
	double m_rBufDist, m_rSmooth, m_rSharp; //value of parameters

	SegmentMap m_sm;
};

#endif // FRMMAPHYDROFLOW_H
