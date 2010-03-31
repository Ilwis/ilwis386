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
// $Log: /ILWIS 3.0/ApplicationForms/frmsegap.h $
 * 
 * 8     6/21/01 10:58a Martin
 * added support for giving the rowcol/coord infor to a submap form
 * together with the filename
 * 
 * 7     1-02-01 17:33 Hendrikse
 * added int SegmentMap_4CallBack(Event*); etc
 * 	void SegmentMap_iCallBack(int iCurMap);
 * and fInsideDomCallBack() for better domain control
 * 
 * 6     27-11-00 11:22 Koolhoven
 * in FormSegmentMapPolBoundaries removed paramters for the choosing of a
 * domain to be created
 * 
 * 5     4/03/00 5:56p Hendrikse
 * added in FormSubMap and FormGlue:
 *  int CallBackMinMaxXY(Event*);
 *   int CallBackMinMaxLatLon(Event*); 
 *   StaticText* stCsy;
 *   void HideCoordsAndLatLon();
 *   FieldLat *fldMinLat;
 *   FieldLat *fldMaxLat;
 *   FieldLon *fldMinLon;
 *   FieldLon *fldMaxLon;
 *   LatLon llMin, llMax;
 *   bool fFromLatLon;
 * to deal with latlons
 * 
 * and in FormGlue also a calbackClip  and  fClip member
 * 
 * 4     14-03-00 13:15 Hendrikse
 * added callbacks ets for FieldDMS* use with densif dist in degrees
 * 
 * 3     17-11-99 21:14 Wind
 * comment
 * 
 * 2     16-11-99 10:21 Wind
 * implemented wish (bug 1280), optional domain for
 * segmentmapfrompolboundaries
*/
// Revision 1.4  1998/10/08 14:44:26  Wim
// Transform has now callback to indicate source coordsys
//
// Revision 1.3  1998-09-16 18:32:28+01  Wim
// 22beta2
//
// Revision 1.2  1997/07/30 14:48:29  Wim
// Added New Domain option
// for FormSegmentMapGlue
//
/* FormSegmentMapApplications
   Copyright Ilwis System Development ITC
   february 1996, by Wim Koolhoven
	Last change:  WK    8 Oct 98    2:10 pm
*/
#ifndef FRMSEGAP_H
#define FRMSEGAP_H
#include "Client\Forms\frmsegcr.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fentdms.h"

LRESULT Cmdattribseg(CWnd *wnd, const String& s);
LRESULT Cmdtunnelseg(CWnd *wnd, const String& s);
LRESULT Cmdlabelseg(CWnd *wnd, const String& s);
LRESULT Cmdmaskseg(CWnd *wnd, const String& s);
LRESULT Cmdcleanseg(CWnd *wnd, const String& s);
LRESULT Cmdtransfseg(CWnd *wnd, const String& s);
LRESULT Cmdsubseg(CWnd *wnd, const String& s);
LRESULT Cmddensseg(CWnd *wnd, const String& s);
LRESULT Cmdglueseg(CWnd *wnd, const String& s);
LRESULT Cmdpolseg(CWnd *wnd, const String& s);
LRESULT Cmdrasseg(CWnd *parent, const String& s);

class _export FormAttributeSegmentMap: public FormSegmentMapCreate
{
public:
  FormAttributeSegmentMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int SegmentMapCallBack(Event*);
  int TblCallBack(Event*);
  int ColCallBack(Event*);
  FieldSegmentMap* fldSegmentMap;
  FieldTable* fldTbl;
  FieldColumn* fldCol;
  StaticText* stColRemark;
  String sSegmentMap;
  String sTbl, sCol;
};

class _export FormSegmentMapTunneling: public FormSegmentMapCreate
{
public:
  FormSegmentMapTunneling(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap;
  double rTunnelWidth;
  bool fRemoveNodes;
};

class _export FormSegmentMapLabels: public FormSegmentMapCreate
{
public:
  FormSegmentMapLabels(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap, sLabels;
};

class _export FormSegmentMapMask: public FormSegmentMapCreate
{
public:
  FormSegmentMapMask(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap, sMask;
};

class _export FormSegmentMapCleaning: public FormSegmentMapCreate
{
public:
  FormSegmentMapCleaning(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap;
};

class _export FormSegmentMapTransform: public FormSegmentMapCreate
{
public:
  FormSegmentMapTransform(CWnd* mw, const char* sPar);
private:  
  int MapCallBack(Event*);
	int DensifyCallBack(Event*);

  FieldSegmentMap* fldMap;
  StaticText* stCsy;
  int exec();
  String sSegmentMap, sCoordSys;
	bool fDensify;
	FieldReal* fldMeters;
	FieldDMS* fldDegMinSec;
	CheckBox* cbDensify;
	double rDistance;
	bool fFromLatLon;
};

class _export FormSegmentMapSubMap: public FormSegmentMapCreate
{
public:
  FormSegmentMapSubMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap;
  int SegmentMapCallBack(Event*);
  int CallBackMinMaxXY(Event*);
  int CallBackMinMaxLatLon(Event*); 
  FieldSegmentMap* fldSegmentMap;
  StaticText* stCsy;
  void HideCoordsAndLatLon();
  FieldCoord *fldCrdMin, *fldCrdMax;
  FieldLat *fldMinLat;
  FieldLat *fldMaxLat;
  FieldLon *fldMinLon;
  FieldLon *fldMaxLon;

  Coord cMin, cMax;
  LatLon llMin, llMax;
  bool fFromLatLon;
	bool fPreset;
};

class _export FormSegmentMapDensify: public FormSegmentMapCreate
{
public:
  FormSegmentMapDensify(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSegmentMap;
	int SegmentMapCallBack(Event*);
	FieldSegmentMap* fldMap;
  FieldReal* fldMeters;
	FieldDMS* fldDegMinSec;
	double rDistance;
	bool fFromLatLon;
};

class _export FormSegmentMapGlue: public FormSegmentMapCreate
{
public:
  FormSegmentMapGlue(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapsCallBack(Event*);
	int SegmentMap_1CallBack(Event*);
	int SegmentMap_2CallBack(Event*);
	int SegmentMap_3CallBack(Event*);
	int SegmentMap_4CallBack(Event*);
	void SegmentMap_iCallBack(int iCurMap);
  //int SegmentMapCallBack(Event*);
  int CallBackMinMaxXY(Event*);
  int CallBackMinMaxLatLon(Event*); 
  int CallBackClipBoundary(Event*); 
  int DomCallBack(Event*);
	bool fInsideDomCallBack;

  FieldSegmentMap* fldSegmentMap;
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

class _export FormSegmentMapPolBoundaries: public FormSegmentMapCreate
{
public:
  FormSegmentMapPolBoundaries(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sPolygonMap, sMask;
  bool fSingleName;
  StaticText* stRemark;
};

class _export FormSegmentMapFromRaster: public FormSegmentMapCreate
{
public:
  FormSegmentMapFromRaster(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  int iType;
  bool fSmooth;
  int iConnect;
  bool fSingleName;
};


#endif  // FRMSEGAP_H





