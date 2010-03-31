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
#ifndef FRMAPPIA_H
#define FRMAPPIA_H

//#include "Client\MainWindow\mainwind.h"
#include "Client\FormElements\fentdms.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\DATUM.H"

LRESULT Cmdtransformheight(CWnd *parent, const String& s);
LRESULT Cmdtransform(CWnd *parent, const String& s);
LRESULT Cmdtransformtogeocentric(CWnd *parent, const String& s);
LRESULT Cmdtransformfromgeocentric(CWnd *parent, const String& s);
LRESULT Cmdtransformviageocentric(CWnd *parent, const String& s);

class _export FormInteractiveApplication: public FormBaseDialog
{
public:
  FormInteractiveApplication(CWnd* parent, const String& sTitle);
  ~FormInteractiveApplication();
  int exec();  
};

class _export FormTransformCoordinates: public FormInteractiveApplication
{
public:
  FormTransformCoordinates(CWnd* mw, const char* sPar);
private:  
  Coord crd;
  LatLon ll;
  String sCsyInput, sCsyOutput;
  CoordSystem csInput, csOutput;
  int CsyInputCallBack(Event*);
  int CsyOutputCallBack(Event*);
  int CoordInputCallBack(Event*);
	int DDDCallBack(Event*);
  FieldCoord *fldCrd;
  FieldLatLon *fldLL;
	FieldCoord *fldLLDDD;
  FieldCoordSystem *fcsInput, *fcsOutput;
  StaticText *stCsyInput, *stCsyOutput, *stResult;
	CheckBox* cbDDD;
	bool fDDD;
	Coord crdLL;
	FieldString* fsResult;
	String sFill64;
	String sXout, sYout;
};

class _export FormTransformCoordinatesH: public FormInteractiveApplication
{
public:
  FormTransformCoordinatesH(CWnd* mw, const char* sPar);
private:  
  Coord crd;
  LatLon ll;
  String sCsyInput, sCsyOutput;
  CoordSystem csInput, csOutput;
  int CsyInputCallBack(Event*);
  int CsyOutputCallBack(Event*);
  int CoordInputCallBack(Event*);
	int DDDCallBack(Event*);
	int HeightInCallBack(Event*);
	FieldGroup* fgLLDDD;
  FieldCoord *fldCrd;
  FieldLatLon *fldLLDMS;
	FieldCoord *fldLLDDD;
	FieldReal* frLatIn;
	FieldReal* frLonIn;
  FieldCoordSystem *fcsInput, *fcsOutput;
  StaticText *stCsyInput, *stCsyOutput;//, *stResult, *stHeightOut;
	CheckBox* cbDDD;
	bool fDDD;
	CheckBox* cbHeight;
	bool fHeight;
	FieldReal* frHeightIn;
	String sOutpHeight;
	double rHeightIn;
	double rLatIn, rLonIn;
	double rHeightOut;
	CoordSystemViaLatLon* csvll1;
	CoordSystemViaLatLon* csvll2;
	CoordSystemProjection* cspr1;
	CoordSystemProjection* cspr2;
	bool fProj1;
	bool fProj2;
	Datum* datum1;
	Datum* datum2; 
	String	sEllipsoid1, sEllipsoid2;
	String	sDatum1, sDatum2;
	BursaWolfDatum* bwdat1;
  BursaWolfDatum* bwdat2;
  BadekasDatum* badat1;
  BadekasDatum* badat2;
  bool fBursaWolf1;
  bool fBursaWolf2;
  bool fBadekas1;
  bool fBadekas2;
  StaticText    *stDatumName1; 
  StaticText    *stDatumName2;
  int iNrOfDatumParms;
	bool fCsyInpValid, fCsyOutValid, fCrdInpValid, fHeightInpValid;
	int RecomputeRewriteOutput();
	FieldString* fsResult;
	FieldString* fsHeightOut;
	String sFill64;
	String sFill38;
};

class _export FormTransformCoordinatesH_to_ctsXYZ: public FormInteractiveApplication
{
public:
  FormTransformCoordinatesH_to_ctsXYZ(CWnd* mw, const char* sPar);
private:  
  Coord crd;
  LatLon ll;
  String sCsyInput;
  CoordSystem csInput;
  int CsyInputCallBack(Event*);
  int CoordInputCallBack(Event*);
	int DDDCallBack(Event*);
	int HeightInCallBack(Event*);
	FieldGroup* fgLLDDD;
  FieldCoord *fldCrd;
  FieldLatLon *fldLLDMS;
	FieldCoord *fldLLDDD;
	FieldReal* frLatIn;
	FieldReal* frLonIn;
  FieldCoordSystem *fcsInput;//, *fcsOutput;
  StaticText *stCsyInput, *stResult;
	CheckBox* cbDDD;
	bool fDDD;
	//Coord crdLL;
	CheckBox* cbHeight;
	bool fHeight;
	FieldReal* frHeightIn;
	double rHeightIn;
	double rLatIn, rLonIn;
	CoordSystemViaLatLon* csvll;
	CoordSystemProjection* cspr;
	bool fProj;
	String	sEllipsoid;//, sEllipsoid2;
    StaticText    *stDatumName;
	CoordCTS ctsOut;
	FieldReal* frXCoordOut;
	FieldReal* frYCoordOut;
	FieldReal* frZCoordOut;
	bool fCsyInpValid, fCsyOutValid, fCrdInpValid, fHeightInpValid;
	int RecomputeRewriteOutput();
};

class _export FormTransformCoordinatesH_from_ctsXYZ: public FormInteractiveApplication
{
public:
  FormTransformCoordinatesH_from_ctsXYZ(CWnd* mw, const char* sPar);
private:
	double rX_In, rY_In, rZ_In;
	FieldReal* frXCoordIn;
	FieldReal* frYCoordIn;
	FieldReal* frZCoordIn;
  Coord crd;
  LatLon ll;
  String sCsyOutput;
  CoordSystem csOutput;
  //int CsyInputCallBack(Event*);
  int CsyOutputCallBack(Event*);
  int CoordInputCallBack(Event*);
//	int DDDCallBack(Event*);
//	int HeightInCallBack(Event*);
	FieldGroup* fgLLDDD;
  FieldCoord *fldCrd;
  FieldLatLon *fldLLDMS;
	FieldCoord *fldLLDDD;
//	FieldReal* frLatIn;
//	FieldReal* frLonIn;
  FieldCoordSystem *fcsOutput;//, *fcsInput;
  StaticText *stCsyInput, *stCsyOutput, *stResult, *stHeightOut;
	CheckBox* cbDDD;
	bool fDDD;
	CoordSystemViaLatLon* csvll;
	CoordSystemProjection* cspr;
	bool fProj;
	String	sEllipsoid;//, sEllipsoid1;
    StaticText    *stDatumName;
	bool fCsyOutValid, fCrdInpValid;
	int RecomputeRewriteOutput();
};

class _export FormTransformCoordinatesH_via_ctsXYZ: public FormInteractiveApplication
{
public:
  FormTransformCoordinatesH_via_ctsXYZ(CWnd* mw, const char* sPar);
private:  
  Coord crd;
  LatLon ll;
  double rX_In, rY_In, rZ_In;
  double rX_Out, rY_Out, rZ_Out;
  FieldReal* frXCoordIn;
  FieldReal* frYCoordIn;
  FieldReal* frZCoordIn;
  FieldReal* frXCoordOut;
  FieldReal* frYCoordOut;
  FieldReal* frZCoordOut;
  String sCsyInput, sCsyOutput;
  CoordSystem csInput, csOutput;
  int CsyInputCallBack(Event*);
  int CsyOutputCallBack(Event*);
  int CoordInputCallBack(Event*);
	int DDDCallBack(Event*);
	int HeightInCallBack(Event*);
	FieldGroup* fgLLDDD;
  FieldCoord *fldCrd;
  FieldLatLon *fldLLDMS;
	FieldCoord *fldLLDDD;
	FieldReal* frLatIn;
	FieldReal* frLonIn;
  FieldCoordSystem *fcsInput, *fcsOutput;
  StaticText *stCsyInput, *stCsyOutput;
	StaticText *stEllipsoidInput;
	StaticText *stEllipsoidOutput;
	CheckBox* cbDDD;
	bool fDDD;
	//Coord crdLL;
	CheckBox* cbHeight;
	bool fHeight;
	FieldReal* frHeightIn;
	String sOutpHeight;
	double rHeightIn;
	double rLatIn, rLonIn;
	double rHeightOut;
	CoordSystemViaLatLon* csvll1;
	CoordSystemViaLatLon* csvll2;
	CoordSystemProjection* cspr1;
	CoordSystemProjection* cspr2;
	bool fProj1;
	bool fProj2;
	Datum* datum1;
	Datum* datum2; 
	String	sEllipsoid1, sEllipsoid2;
	String	sDatum1, sDatum2;
	BursaWolfDatum* bwdat1;
  BursaWolfDatum* bwdat2;
  BadekasDatum* badat1;
  BadekasDatum* badat2;
  bool fBursaWolf1;
  bool fBursaWolf2;
  bool fBadekas1;
  bool fBadekas2;
  StaticText    *stDatumName1; 
  StaticText    *stDatumName2;
  int iNrOfDatumParms;
	bool fCsyInpValid, fCsyOutValid, fCrdInpValid, fHeightInpValid;
	int RecomputeRewriteOutput();
	FieldString* fsResult;
	FieldString* fsHeightOut;
	String sFill64;
	String sFill38;
};
#endif // FRMAPPIA_H




