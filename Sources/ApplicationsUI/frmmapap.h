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

#ifndef FRMMAPAP_H
#define FRMMAPAP_H
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\Base\Algorithm\semivar.h"
#include "Client\Forms\frmmapcr.h"
#include "Applications\Raster\MAPITPCN.H"

class FieldAggrFunc;
LRESULT Cmddistance(CWnd *parent, const String& cmd);
LRESULT Cmdeffectdistance(CWnd *parent, const String& cmd);
LRESULT Cmdareanumb(CWnd *parent, const String& s);
LRESULT Cmdattribras(CWnd *parent, const String& s);
LRESULT Cmdslicing(CWnd *parent, const String& s);
LRESULT Cmdaggregate(CWnd *wnd, const String& s);
LRESULT Cmdglueras(CWnd *wnd, const String& s);
LRESULT Cmdvariogramsurface(CWnd *wnd, const String& s);
LRESULT Cmdkrigingras(CWnd *wnd, const String& s);
LRESULT Cmdpntras(CWnd *wnd, const String& s);
LRESULT Cmdnearestpnt(CWnd *wnd, const String& s);
LRESULT Cmdmovaverage(CWnd *wnd, const String& s);
LRESULT Cmdmovsurface(CWnd *wnd, const String& s);
LRESULT Cmdtrendsurface(CWnd *wnd, const String& s);
LRESULT Cmdkriging(CWnd *wnd, const String& s);
LRESULT Cmdunivkriging(CWnd *wnd, const String& s);
LRESULT Cmdanisotrkriging(CWnd *wnd, const String& s);
LRESULT Cmdcokriging(CWnd *wnd, const String& s);
LRESULT Cmdsegras(CWnd *wnd, const String& s);
LRESULT Cmdsegdensity(CWnd *wnd, const String& s);
LRESULT Cmdpolras(CWnd *wnd, const String& s);
LRESULT Cmdinterpolseg(CWnd *wnd, const String& s);
LRESULT Cmddensras(CWnd *wnd, const String& s);
LRESULT Cmdresample(CWnd *wnd, const String& s);
LRESULT Cmdapply3d(CWnd *wnd, const String& s);
LRESULT Cmdsubras(CWnd *wnd, const String& s);
LRESULT Cmdpntdensity(CWnd *parent, const String& s) ;
LRESULT Cmdattribtoras(CWnd *parent, const String& s);

class FormDensifyMap: public FormMapCreate
{
public:
 _export FormDensifyMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  int MethodCallBack(Event*);
  FieldDataType* fldMap;
  StaticText* stMapRemark;
  RadioGroup* rg;
  RadioButton *rbLin, *rbCub;
  String sMap;
  double rEnl; 
  int iMeth;
};

LRESULT Cmddistance(CWnd *parent, const String& cmd);

class FormDistanceMap: public FormMapCreate
{
public:
  _export FormDistanceMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  bool fWeightMap;
  String sSourceMap, sWeightMap;
  FieldDataType *fldSourceMap, *fldWeightMap;
  StaticText* stWeightMapRemark;
  int CallBack(Event*);
  int WeightMapCallBack(Event*);
  bool fThiessenMap;
  String sThiessenMap;
};

class FormEffectDistanceMap: public FormMapCreate
{
public:
  _export FormEffectDistanceMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sSourceMap;
  String m_sDEM;
  FieldDataType *m_fldDEMMap;
  bool m_fDirectionFromSource;
  bool m_fUpslopeFromSource;
  bool m_fDownslopeFromSource;
  CheckBox* m_chkDirectionFromSource;
  CheckBox* m_chkUpslopeFromSource;
  CheckBox* m_chkDownslopeFromSource;
  FieldOneSelectTextOnly* m_fsDirection;
  FieldOneSelectTextOnly* m_fsUpslope;
  FieldOneSelectTextOnly* m_fsDownslope;
  bool m_AddString;
  int OnSelect(Event *);
  int OnDirectionSelect(Event *);
  int OnUpslopeSelect(Event *);
  int OnDownslopeSelect(Event *);
  void initWeights();
  String m_sDirectionFromSourceMap;
  String m_sUpslopeFromSourceMap;
  String m_sDownslopeFromSourceMap;
  String m_sDirection;
  double m_rDist;
  double m_rNorth;
  double m_rNorthEast;
  double m_rEast;
  double m_rSouthEast;
  double m_rSouth;
  double m_rSouthWest;
  double m_rWest;
  double m_rNorthWest;
};

class FormAreaNumbering: public FormMapCreate
{
public:
  _export FormAreaNumbering(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  CheckBox* cbDom;
  FieldDataType *fdtMapIn;
  FieldDataTypeCreate* fldDom;
  String sMap;
  int iConnect;
};

class FormRasterize: public FormMapCreate
{
public:
	  int MapCallBack(Event*);
protected:  
  _export FormRasterize(CWnd*, const String& sTitle);
  void initAsk(long dmTypes);
  int exec();
  FormEntry* feMap;
  FormEntry* feAttrib;
  FieldColumn* fldCol;
  StaticText* stMapRemark;
  String sMap, sCol;
  bool fAttrib, fOnlyValue;
  bool fWiderValRange;
  BaseMap bmap;
	bool fSphericalDistance;
};

class FormRasPoint: public FormRasterize
{
public:
  _export FormRasPoint(CWnd* mw, const char* sPar);
private:  
  int exec();
  int iPointSize;
};

class FormRasPointCount: public FormRasterize
{
public:
  _export FormRasPointCount(CWnd* mw, const char* sPar);
private:  
  int exec();
  int iPointSize;
};

class FormRasSegment: public FormRasterize
{
public:
   _export FormRasSegment(CWnd* mw, const char* sPar);
private:  
  int exec();
};

class FormMapSegmentDensity: public FormRasterize
{
public:
  _export FormMapSegmentDensity(CWnd* mw, const char* sPar);
private:  
  bool fMask;
  String sMask;
  int exec();
};

class FormInterpolMap: public FormRasterize
{
public:
  _export FormInterpolMap(CWnd* mw, const char* sPar);
private:  
  int MapCallBack(Event*);
  int exec();
};

class FormRasPolygon: public FormRasterize
{
public:
  _export FormRasPolygon(CWnd* mw, const char* sPar);

private:  
  int exec();
};

class FormAttributeMap: public FormMapCreate
{
public:
  _export FormAttributeMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  int TblCallBack(Event*);
  int ColCallBack(Event*);
  FieldMap* fldMap;
  FieldTable* fldTbl;
  FieldColumn* fldCol;
  StaticText* stColRemark;
  String sMap, sTbl, sCol;
};

class FormAttributeToMap: public FormMapCreate
{
public:
  _export FormAttributeToMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int TblCallBack(Event*);
  FieldMap* fldMap;
  FieldTable* fldTbl;
  FieldColumn* fldCol, *fldCol2;
  StaticText* stColRemark;
  String sMap, sTbl, sColSource, sColTarget;
  bool mustLink;
};

class FormResampleMap: public FormMapCreate
{
public:
  _export FormResampleMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  int MethodCallBack(Event*);
	int ChangeGeoRef(Event*);
  FieldDataType* fldMap;
  StaticText* stMapRemark;
  StaticText* stRemGeoRef;
  String sMap;
  int iMeth;
  RadioGroup* rg;
  RadioButton *rbNearest, *rbBiLin, *rbBiCub;
  Map mp;
};

class FormMapApply3D: public FormMapCreate
{
public:
  _export FormMapApply3D(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
};

class FormMapNearestPoint: public FormRasterize
{
public:
  _export FormMapNearestPoint(CWnd* mw, const char* sPar);
private:  
  int exec();
};

class FormMapGridding: public FormRasterize
{
protected:
  _export FormMapGridding(CWnd*, const String& sTitle, const char *sPar);
  int exec();
};

class FormMapGriddingWeighted: public FormMapGridding
{
protected:
	_export FormMapGriddingWeighted(CWnd*, const String& sTitle, const char* sPar);
	int exec();
	String sWeight;
	int MapCallBack(Event*);
	int DistanceMethodCallBack(Event*);

private:
  FieldReal* frLimDist;
	int        iWeight;
  double     rExp;
  double     rLimDist;
	CheckBox*  cbSphDis;
};

class FormMapMovingAverage: public FormMapGriddingWeighted
{
public:
  _export FormMapMovingAverage(CWnd* mw, const char* sPar);
private:
  int exec();
};

class FormMapTrendSurface: public FormMapGridding
{
public:
  _export FormMapTrendSurface(CWnd* mw, const char* sPar);
private:  
  int exec();
  long iSurface;
};

class _export FormMapMovingSurface: public FormMapGriddingWeighted
{
public:
  FormMapMovingSurface(CWnd* mw, const char *sPar);
private:  
  int exec();
  long iSurface;
};

class FormMapKriging: public FormRasterize
{
public:
  _export FormMapKriging(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  SemiVariogram smv;
  bool fErrorMap, fDuplicRemoval;
  String sErrorMap;
  int iSimpleOrdinary, iDuplicRemoval;
  RangeInt riMinMax;
  double rLimDist, rTolerance;
};



class FormMapSlicing: public FormMapCreate
{
public:
  _export FormMapSlicing(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  int MapCallBack(Event*);
  FieldDataType *fdtMapIn;
};

long Cmdmirror(CWnd *parent, const String& s);

class FormMapMirrorRotate: public FormMapCreate
{
public:
  _export FormMapMirrorRotate(CWnd* mw, const char* sPar);
private:  
  int exec();
  String sMap;
  RadioGroup* rg;
  int iMethod;
};

class FormMapSubMap: public FormMapCreate
{
public:
  _export FormMapSubMap(CWnd* mw, const char* sPar);
private:  
  int MapCallBack(Event*);
  int MethodCallBack(Event*);
	void UpdateSize();
  int exec();
  String sMap;
  long iFirstLine, iFirstCol, iLines, iCols, iLastLine, iLastCol;
  Coord crd1, crd2;
  FieldDataType* fldMap;
	FieldInt *feFirstLine, *feFirstCol, *feNrLines, *feNrCols;
	FieldInt *feFirstLineCrnr, *feFirstColCrnr, *feLastLineCrnr, *feLastColCrnr;
  FieldCoord* fcFirst, *fcSecond;
  RadioGroup* rgMethod;
  RadioButton* rbCoords;
  int iMethod;
	bool fPreSet;
};

class FormMapAggregate: public FormMapCreate
{
public:
  _export FormMapAggregate(CWnd* mw, const char* sPar);
private:  
  int exec();
  int CallBack(Event*);
  FieldDataType* fldMap;
  FieldInt* fldGrf;
  FieldAggrFunc* fldAgf;
  String sMap;
  int iFactor;
  bool fGroup, fOffset;
  RowCol rcOffset;
  String *sFunc, m_sDefault;
};

class FormMapGlue: public FormMapCreate
{
public:
  _export FormMapGlue(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapsCallBack(Event*);
  int MapCallBack(Event*);
  int DomCallBack(Event*);
  RadioGroup* rgMaps;
  FieldMap *fldMap1, *fldMap2, *fldMap3, *fldMap4;
  CheckBox* cbDom;
  FieldDataTypeCreate* fldDom;
  int iMaps;
  String sMap1, sMap2, sMap3, sMap4;
  bool fReplaceAll;
  bool fNewDom;
  String sNewDom;
  bool fGeoRef;
  String sGeoRef;
};

class _export FormMapUniversalKriging: public FormRasterize
{
public:
  FormMapUniversalKriging(CWnd* mw, const char* sPar);
private:  
  int exec();
	int MapCallBack(Event*);
  int TrendCallBack(Event*);
  RadioGroup* rgTrend;
  FieldRangeInt* fmm;
  SemiVariogram smv;
  int iTrend;
  bool fErrorMap, fDuplicRemoval;
  int iDuplicRemoval;
  String sErrorMap;
  RangeInt riMinMax;
  double rLimDist, rTolerance;
};

class _export FormMapAnisotropicKriging: public FormRasterize
{
public:
  FormMapAnisotropicKriging(CWnd* mw, const char* sPar);
private:  
  int exec();
	int MapCallBack(Event*);
  SemiVariogram smv;
  double rRatio, rAngle;
  bool fErrorMap, fDuplicRemoval;
  int iDuplicRemoval;
  String sErrorMap;
  RangeInt riMinMax;
  double rLimDist, rTolerance;
};

class _export FormMapVariogramSurface: public FormMapCreate
{
public:
  FormMapVariogramSurface(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  FieldDataType* fldMap;
  FieldReal* frLagSpacing;
  String sMap;
  double rLagSpacing;
  int iLags;
};

class _export FormMapCoKriging: public FormRasterize
{
public:
  FormMapCoKriging(CWnd* mw, const char* sPar);
private:  
  int exec();
	int MapCallBack(Event*);
  SemiVariogram smv[3];
  String as[3];
  String sMapCoVar;
  bool fErrorMap, fDuplicRemoval;
  int iDuplicRemoval;
  String sErrorMap;
  RangeInt riMinMax;
  double rLimDist, rTolerance;
};

class _export FormMapKrigingFromRaster: public FormMapCreate
{
public:
  FormMapKrigingFromRaster(CWnd* mw, const char* sPar);
private:  
  int exec();
	int MeterEditCallBack(Event*);
	int PixelEditCallBack(Event*);
	int UnitsCallBack(Event*);
	int MapCallBack(Event*);

  FieldDataType* fldMap;
  RadioGroup* rgUnits;
	FieldReal* frMeter;
	FieldInt*  fiPixels;

	String sMap;
	bool fWiderValRange;
	SemiVariogram smv;
	bool fErrorMap;
  String sErrorMap;
  RangeInt riMinMax;
  double rRadius, rPixS;
	int iPixels, iUnitChoice;
	//RadiusUnits ruUnits;
};

#endif  // FRMMAPAP_H





