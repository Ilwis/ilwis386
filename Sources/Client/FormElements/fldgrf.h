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
/* FieldGeoRef
   by Wim Koolhoven, june 1995
   Copyright Ilwis System Development ITC
	Last change:  JH   19 Jan 98   11:14 am
*/
   
#ifndef FIELDGEOREF_H
#define FIELDGEOREF_H
#include "Engine\SpatialReference\Grcornrs.h"
#include "Client\FormElements\fldcs.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fentdms.h"

class _export FieldGeoRefC: public FieldDataTypeC
{
public:
  FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, long types = grALL | grNONONE | grNO3D, 
               CoordSystem csys=CoordSystem(), 
               CoordBounds cbnds=CoordBounds());
  FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, const GeoRef& grf);
  FieldGeoRefC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef, const GeoRef& grf, const Map& mp, bool fStartEdit=false);
  void SetBounds(const CoordSystem& csys, const CoordBounds& cbnd);             
  void SetNewName(const String& sName) { sNewName = sName; }
private:  
  String sNewName;
  Map map;
  CoordSystem cs;
  CoordBounds cb;
  int CreateGeoRef(void *);
  bool fEditStart;
};

class _export FieldGeoRef3DC: public FieldDataTypeC
{
public:
  FieldGeoRef3DC(FormEntry* fe, const String& sQuestion, 
               String* sGeoRef); 
private:  
  int CreateGeoRef(void*);
  String sNewName;
};

class _export FormCreateGeoRef: public FormWithDest
{
public:
	FormCreateGeoRef(CWnd* wPar, String* sGrf, 
		CoordSystem cs, CoordBounds cb, bool fOnlyCorners);
	~FormCreateGeoRef();
private:  
	int exec();
	int GeoRefTypeChange(Event*);
	int CallBackCorners(Event*);
	int CSysCallBack(Event*);
	int CallBackGrfSubPixelChange(Event*);
	void ShowHide(bool fLatLon, bool fHideAll);
	void SetOkButton();
		
	FormEntry* feDefaultFocus();  
	String* sGeoRef;
	String sNewName;
	String sDescr;
	String sCoordSys;
	double rPixSize;
	double rPixSizeDMS;
	int iOption;
	bool fCoC;
	bool fSubPixelPrecise;
	bool fCallBackGrfSubPixelCalled;
	RowCol rcSize;
	Coord crdMin, crdMax;
	LatLon llMin, llMax;
	FieldCoordSystemC *fcsc;
	FieldGroup* fgCsyMeters;
	FieldGroup* fgCsyLatLons;
	FieldReal* fldPixInMeters;
	FieldDMS* fldPixInDegMinSec;
	FieldCoord *fldCrdMin, *fldCrdMax;
	CheckBox* cbCoC;
	CheckBox* cbSubPixel;
	FieldLat *fldMinLat;
	FieldLat *fldMaxLat;
	FieldLon *fldMinLon;
	FieldLon *fldMaxLon;
	FieldDataTypeCreate* fgr;  
	RadioGroup* rg;
	FieldGroup* fgCorners;
	String sRefMap, sDTM;
	StaticText* stRemark;
	CWnd* wParent;
	bool m_fInShowHide, m_fInSetVal;
	
	bool m_fNameOK;
	bool m_fBoundsOK;
};

class _export FormCreateGeoRefRC: public FormWithDest
{
public:
  FormCreateGeoRefRC(CWnd* wPar, String* sGrf, const Map& mp, 
                     CoordSystem cs, CoordBounds cb, 
                     bool fOnlyTiepoints, bool fStartEdit=false);
private:  
  int exec();
  int GeoRefTypeChange(Event*);
  int CallBackCorners(Event*);
  int CallBackGrfSubPixelChange(Event*);
  int CSysCallBack(Event*);
  void ShowHide(bool fLatLon, bool fHideAll);
  void SetOkButton();
  FormEntry* feDefaultFocus();  
  String* sGeoRef;
  String sNewName;
  String sDescr;
  String sCoordSys;
  String sDTM;
  int iOption;  // Corners, Tiepoints
  bool fCoC, fEditStart, fOnlyTiepoints;
  bool fSubPixelPrecise;
  bool fCallBackGrfSubPixelCalled;
  CheckBox* cbCoC;
  CheckBox* cbSubPixel;
  Map map;
  RowCol rcSize;
  Coord crdMin, crdMax;
  LatLon llMin, llMax;
  FieldDataTypeCreate* fgr;
  RadioGroup* rg;
  FieldGroup *fgCorners;
  FieldGroup* fgCsyMeters;
  FieldGroup* fgCsyLatLons;
  FieldCoordSystemC *fcsc;
  FieldCoord *fldCrdMin, *fldCrdMax;
  FieldLat *fldMinLat;
  FieldLat *fldMaxLat;
  FieldLon *fldMinLon;
  FieldLon *fldMaxLon;
  StaticText* stRemark;
  bool m_fInShowHide, m_fInSetVal;

  bool m_fNameOK;
  bool m_fBoundsOK;
};

class _export FieldGeoRefExisting: public FieldDataType
{
public:
  FieldGeoRefExisting(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".GRF",
                    true)
    {}
  FieldGeoRefExisting(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".GRF",
                    true)
    {}
};


#endif // FIELDGEOREF_H




