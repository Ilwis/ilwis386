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
// $Log: /ILWIS 3.0/ApplicationForms/frmpntap.h $
 * 
 * 15    5-10-04 22:07 Hendrikse
 * added 	bool fTransformHeights  to allow 3D transformations when the
 * coordsystems (both input and output) have a userdefined Datum,
 * (BursaWolf or Molodensky) or have the WGS84 Datum
 * 
 * 14    6/21/01 10:58a Martin
 * added support for giving the rowcol/coord infor to a submap form
 * together with the filename
 * 
 * 13    26-01-01 18:22 Hendrikse
 * added extra callbacks for each input map:
 * int PointMap_1CallBack(Event*); to   int PointMap_4CallBack(Event*);
 * and  void PointMap_iCallBack(int iCurMap);
 * 
 * added protection fInsideDomCallBack against infinit recursion
 * 
 * 12    1-09-00 12:13p Martin
 * support for coordinate columns in tbltopointmap
 * 
 * 11    3/30/00 8:29p Hendrikse
 * added void HideCoordsAndLatLon();
 * removed FieldCoord
 * fieldDMS
 * 
 * 10    3/28/00 3:03p Hendrikse
 * added
 * void ShowCoords();
 *   void ShowLatLons();
 *   void HideCoordsAndLatLon();
 * to simplify Show and hide calls
 * Removed redundant sCoordSys in PointmapGlue
 * 
 * 9     3/27/00 5:51p Hendrikse
 * added in FormPointMapGlue:
 *  int CallBackMinMaxXY(Event*);
 *   int CallBackMinMaxLatLon(Event*); 
 *   int CallBackClipBoundary(Event*); 
 * and necessary new Fields
 * 
 * 8     3/25/00 12:24p Hendrikse
 * added
 *  FieldPointMap* fldPointMap  and   StaticText* stCsy;
 * FormPointMapGlue hte name of the csy where clipping is referred to
 * 
 * 7     3/24/00 4:24p Hendrikse
 * added 
 * int CallBackMinMaxXY(Event*);
 *  int CallBackMinMaxLatLon(Event*); 
 * and related Fields and members
 * 
 * 6     3/22/00 5:45p Hendrikse
 * added  RadioGroup* rgOptions;
 * 
 * 5     14-03-00 21:01 Hendrikse
 * 
*/
//4     14-03-00 20:58 Hendrikse
//
// 3     14-03-00 20:56 Hendrikse
// badly-printed comments removed
// 
// 2     14-03-00 18:30 Hendrikse
// added include files and functionality to treat segmaps with latlon
// coords
// Revision 1.4  1998/10/08 14:44:26  Wim
// Transform has now callback to indicate source coordsys
//
// Revision 1.3  1998-09-16 18:32:28+01  Wim
// 22beta2
//
// Revision 1.2  1997/07/30 10:50:50  Wim
// Ask for sNewDom in PointMapGlue
//
/* FormPointMapApplications
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK    8 Oct 98    1:09 pm
*/
#ifndef FRMPNTAP_H
#define FRMPNTAP_H
#include "Client\Forms\frmpntcr.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fentdms.h"
#include "SegmentApplications\SEGTRNSF.H"

LRESULT Cmdattribpnt(CWnd *wnd, const String& s);
LRESULT Cmdraspnt(CWnd *wnd, const String& s);
LRESULT Cmdtblpnt(CWnd *wnd, const String& s);
LRESULT Cmdsegpnt(CWnd *wnd, const String& s);
LRESULT Cmdpolpnt(CWnd *wnd, const String& s);
LRESULT Cmdtransfpnt(CWnd *wnd, const String& s);
LRESULT Cmdsubpnt(CWnd *wnd, const String& s);
LRESULT Cmdgluepnt(CWnd *wnd, const String& s);
LRESULT Cmdmaskpnt(CWnd *wnd, const String& s);

class _export FormAttributePointMap: public FormPointMapCreate
{
public:
  FormAttributePointMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int PointMapCallBack(Event*);
  int TblCallBack(Event*);
  int ColCallBack(Event*);
  FieldPointMap* fldPointMap;
  FieldTable* fldTbl;
  FieldColumn* fldCol;
  StaticText* stColRemark;
  String sPointMap;
  String sTbl, sCol;
};

class _export FormPointMapFromRas: public FormPointMapCreate
{
public:
  FormPointMapFromRas(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class _export FormPointMapFromTable: public FormPointMapCreate
{
public:
  FormPointMapFromTable(CWnd* mw, const char* sPar);
private:  
  int exec();
  int TblCallBack(Event*);
  int OptionCallBack(Event*);
	int ColCoordCallBack(Event*);
  FieldTable *fldTbl;
  FieldColumn *fldColX, *fldColY, *fldColAttrib, *fldColCoord;
  RadioGroup *rg, *rgDmNone;
	CheckBox *cc;
	FieldCoordSystemC *feCs;
  String sTbl, sColX, sColY, sColCoord, sColAttrib, sCsy, sPrefix;
  int iOption;
	bool fUseCoords;
};

class _export FormPointMapSegments: public FormPointMapCreate
{
public:
  FormPointMapSegments(CWnd* mw, const char* sPar);
private:  
  int exec();
	StaticText* stCsy;
	int SegmentMapCallBack(Event*);
	double rDefaultDensifyDistance(const SegmentMap& segmap);
	FieldSegmentMap* fldSegmentMap;
	RadioButton* rbMeters;
	RadioButton* rbDegr;
  FieldReal* fldMeters;
	FieldDMS* fldDegMinSec;
	RadioGroup* rgOptions;
	bool fFromLatLon;
  String sMap;
  int iOption;
	double rDist;
};

class _export FormPointMapPolygons: public FormPointMapCreate
{
public:
  FormPointMapPolygons(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  int iOption;
  bool fIncludeUndefs;
};

class _export FormPointMapTransform: public FormPointMapCreate
{
public:
  FormPointMapTransform(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  FieldPointMap* fldMap;
  StaticText* stCsy;
  String sPointMap, sCoordSys;
	bool fTransformHeights;
};

class _export FormPointMapSubMap: public FormPointMapCreate
{
public:
  FormPointMapSubMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int PointMapCallBack(Event*);
  int CallBackMinMaxXY(Event*);
  int CallBackMinMaxLatLon(Event*); 
  FieldPointMap* fldPointMap;
  
  FieldCoordSystemC *fcsc;
  StaticText* stCsy;
  FieldCoord *fcMin, *fcMax;
  FieldLat *fldMinLat;
  FieldLat *fldMaxLat;
  FieldLon *fldMinLon;
  FieldLon *fldMaxLon;

  void HideCoordsAndLatLon();
  String sPointMap;
  Coord cMin, cMax;
  LatLon llMin, llMax;
  bool fFromLatLon;
	bool fPreset;
};

class _export FormPointMapGlue: public FormPointMapCreate
{
public:
  FormPointMapGlue(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapsCallBack(Event*);
  int PointMap_1CallBack(Event*);
	int PointMap_2CallBack(Event*);
	int PointMap_3CallBack(Event*);
	int PointMap_4CallBack(Event*);
	void PointMap_iCallBack(int iCurMap);
  int CallBackMinMaxXY(Event*);
  int CallBackMinMaxLatLon(Event*); 
  int CallBackClipBoundary(Event*); 
  int DomCallBack(Event*);
	bool fInsideDomCallBack;
	/*
	void SetOKButton(); 
	bool fEnoughInputMaps(); 
	bool fDomainsOK();
	bool fDomsOK;
	bool fXYBoundariesOK(); 
	bool fLatLonBoundariesOK();
	bool fNewDomGiven();
	bool fOutPutMapGiven();
	StaticText* stRemarkWhyNotOK;
  
  FieldPointMap* fldPointMap; */
  StaticText* stCsy;

  void ShowCoords();
  void ShowLatLons();
  void HideCoordsAndLatLon();

  RadioGroup* rgMaps;
  int iMaps;
	Array<String> asMaps;
  FieldGroup *fg1, *fg2, *fg3, *fg4;
  FieldCoord *fldCrdMin, *fldCrdMax;
  FieldLat *fldMinLat;
  FieldLat *fldMaxLat;
  FieldLon *fldMinLon;
  FieldLon *fldMaxLon;
  CheckBox* cbClip;
  CheckBox* cbDom;
  FieldDataTypeCreate* fldDom;
  StaticText* stRemark;
  String sMap1, sMap2, sMap3, sMap4;
  String sMask1, sMask2, sMask3, sMask4;
  bool fClipBoundary;
  Coord cMin, cMax;
  LatLon llMin, llMax;
  bool fFromLatLon;
  bool fNewDom;
  String sNewDom;
};

class _export FormPointMapMask: public FormPointMapCreate
{
public:
  FormPointMapMask(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sPointMap, sMask;
};


#endif  // FRMPNTAP_H





