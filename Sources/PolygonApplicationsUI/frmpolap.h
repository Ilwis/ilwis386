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
/* FormPolygonMapApplications
   Copyright Ilwis System Development ITC
   february 1996, by Wim Koolhoven
	Last change:  WK    8 Oct 98    2:10 pm
*/
#ifndef FRMPOLAP_H
#define FRMPOLAP_H
#include "Client\Forms\frmpolcr.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"

LRESULT Cmdattribpol(CWnd *wnd, const String& s);
LRESULT Cmdlabelpol(CWnd *wnd, const String& s);
LRESULT Cmdsegpol(CWnd *wnd, const String& s);
LRESULT Cmdtransfpol(CWnd *wnd, const String& s);
LRESULT Cmdmaskpol(CWnd *wnd, const String& s);
LRESULT Cmdraspol(CWnd *wnd, const String& s);
LRESULT Cmdidgrid(CWnd *wnd, const String& s);

class _export FormAttributePolygonMap: public FormPolygonMapCreate
{
public:
  FormAttributePolygonMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int PolygonMapCallBack(Event*);
  int TblCallBack(Event*);
  int ColCallBack(Event*);
  FieldPolygonMap* fldPolygonMap;
  FieldTable* fldTbl;
  FieldColumn* fldCol;
  StaticText* stColRemark;
  String sPolygonMap;
  String sTbl, sCol;
};

class _export FormPolygonMapLabels: public FormPolygonMapCreate
{
public:
  FormPolygonMapLabels(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sPolygonMap;
  String sPointMap;
};

class _export FormPolygonMapFromSegment: public FormPolygonMapCreate
{
public:
  FormPolygonMapFromSegment(CWnd* mw, const char* sPar);
private:  
  int exec();
	int CallBack(Event*);
  String sSegmentMap, sMask, sLblPointMap;
  bool fMask, fTopology, fAutoCorrect;
	int iOption;
	CheckBox* cbTopology;
	FieldGroup *fgTop, *fgNonTop;
};

class _export FormPolygonMapTransform: public FormPolygonMapCreate
{
public:
  FormPolygonMapTransform(CWnd* mw, const char* sPar);
private:  
  int MapCallBack(Event*);
	int DensifyCallBack(Event*);

  FieldPolygonMap* fldMap;
  StaticText* stCsy;
  int exec();
  String sPolygonMap, sCoordSys;
	bool fDensify;
	FieldReal* fldMeters;
	FieldDMS* fldDegMinSec;
	CheckBox* cbDensify;
	double rDistance;
	bool fFromLatLon;
};

class _export FormPolygonMapMask: public FormPolygonMapCreate
{
public:
  FormPolygonMapMask(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sPolygonMap, sMask;
};

class _export FormPolygonMapFromRaster: public FormPolygonMapCreate
{
public:
  FormPolygonMapFromRaster(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  bool fSmooth;
  int iConnect;
};

class FieldOneSelectTextOnly;

class _export FormPolygonMapGrid: public FormPolygonMapCreate
{
public:
  FormPolygonMapGrid(CWnd* mw, const char* sPar);
private:  
  int exec();
	int ChangeCSY(Event *);
	int FillOrder(Event *);
	int FillStartCorner(Event *);
	int ChangeGridProperties(Event*);

	FieldDataTypeC *m_fldCSY;
	StaticText *m_stOriCoordLL,*m_stURLL;
	FieldCoord *m_fldOrigin,*m_fldCrdUR,*m_fldCrdP;
	FieldLat *m_fldOriLat,*m_fldURLat,*m_fldDPLat;
	FieldLon *m_fldOriLon,*m_fldURLon,*m_fldDPLon;
	LatLon m_llOri,m_llUR,m_llDP;
	FieldOneSelectTextOnly *m_fldOrder,*m_fldCorner;
	CheckBox *m_chkGridSize;
	CheckBox *m_chkRowCol,*m_chkDirXY,*m_chkDirLL;
	
	String m_sCSY,m_sPrefix,m_sPostfix,m_sTbl,m_sOrder,m_sStartCorner;
	Coord  m_crdOrigin;
	Coord  m_crdSize;
	Coord  m_crdUR;
	Coord  m_crdP;
	double m_rWidth, m_rHeight;
	long	 m_iChkDirXY,m_iChkDirLL;
	long   m_iChkSizeValue,m_iChkRowColValue;
	long	 m_iCols, m_iRows;
	long	 m_iStartNr,m_iSteps;
	long	 m_iTable,m_iLabels;

	bool   m_fOrderFilled,m_fCornerFilled;
	bool	 m_fLatLon;
};

#endif  // FRMPOLAP_H





