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
// $Log: /ILWIS 3.0/ApplicationForms/frmpolap.cpp $
 * 
 * 21    3/19/03 4:36p Lichun
 * Changed Hor/Vert Continues  to Hor/Vert Continued in the drop-down
 * listbox Order. 
 * 
 * 20    2/05/03 12:13p Lichun
 * merge id-grid branch
 * 
 * 28    2/04/03 10:01a Lichun
 * Aligned the layout of Order and Corner for ID Grid. 
 * 
 * 27    2/04/03 9:07a Lichun
 * Adjusted labels for Origin Coordiante and Upper right corner. now it
 * reads: Origin Coordinate (Lat,Lon), Opposite Coordinate(Lat,Lon)
 * 
 * 26    1/10/03 5:50p Lichun
 * added help button
 * 
 * 25    10/04/02 2:09p Lichun
 * adjusted the defaults for grid width, height, number of rows and cols.
 * 
 * 24    9/26/02 12:22p Lichun
 * Adding a checkbox for direction point with default value 0. When it is
 * checked, the direction point must be entered, otherwise not needed.  
 * 
 * 23    9/13/02 10:53a Lichun
 * Adapted disenabled option to GrisSize and Nr. of Grids in the CallBack
 * function of ID grid, when one of them is unchecked. 
 * 
 * 22    9/06/02 4:39p Lichun
 * Added codes handling with right menu
 * 
 * 21    9/05/02 8:58a Lichun
 * Implemented form for ID grid map.
 * 
 * 19    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 18    23-02-01 2:05p Martin
 * fullname is now passed to the combo. prvents naming problems
 * 
 * 17    22/02/01 13:02 Willem
 * Disabled attribute selection of polygon map in PolygonMapTransform form
 * 
 * 16    19/02/01 10:47 Willem
 * Disabled attribute selection in segmentmaps for PolygonFromSegment
 * 
 * 15    16/02/01 16:14 Willem
 * - Map selection is now limited to Class/Id/Group/Bool/UniqueID
 * - Table and Map callbacks do not try to open a file with an empty
 * filename anymore
 * 
 * 14    8-02-01 17:02 Hendrikse
 * changed MapListerDomainType(".mpa", 0, false) to disable attrib choice
 * 
 * 13    19-12-00 16:12 Koolhoven
 * adapted FormPolygonMapFromSegment to include non-topological
 * polygonization
 * 
 * 12    14-11-00 9:58a Martin
 * added support for attrib table columns in the mapselectors
 * 
 * 11    8-11-00 17:24 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 10    17-10-00 14:36 Hendrikse
 * implementeded members and functions for densifying polygon coords, for
 * polygon maps with cartes coords and for polmaps with latlon coordsystem
 * 
 * 9     5-10-00 17:40 Koolhoven
 * with Transform operation ask Coordinate System below output map
 * 
 * 8     20-09-00 4:36p Martin
 * the input string for the transformpol form was not parsed completely
 * 
 * 7     18-07-00 11:54a Martin
 * extensions are now correct so a icon will show in the map nameeedit
 * 
 * 6     5/23/00 12:51p Wind
 * - accept  raster input maps with any domain in FormPolygonMapLabels
 * - accept  raster input maps with any domain in
 * FormPolygonMapFromSegment
 * 
 * 5     11-05-00 3:57p Martin
 * added 'attribute columns' nameedits
 * 
 * 4     28-02-00 11:56 Wind
 * adapted to changes in constructor of FieldColumn
 * 
 * 3     9/13/99 1:04p Wind
 * comments
 * 
 * 2     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
*/
// Revision 1.7  1998/10/08 14:45:01  Wim
// Transform has now callback to tell source coordsys type name
//
// Revision 1.6  1998-09-16 18:33:54+01  Wim
// 22beta2
//
// Revision 1.5  1998/02/24 08:45:18  martin
// Attrib maps may be in different directories.
//
// Revision 1.4  1997/09/01 13:03:57  Wim
// PolygonMap from SegmentMap: Labels default on.
//
// Revision 1.3  1997-08-13 12:20:44+02  Wim
// FieldDataType instead FieldMap in polfromras
//
// Revision 1.2  1997-08-13 10:18:28+02  Wim
// PolFromRas only allow id, class and bool maps
//
/* Form PolygonMap Applications
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    8 Oct 98    2:11 pm
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Applications\POLVIRT.H"
#include "Client\FormElements\fldcs.h"
#include "PolygonApplicationsUI\frmpolap.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Table\COLINFO.H"
#include "PolygonApplications\POLTRNSF.H"
#include "Headers\Hs\Coordsys.hs"

LRESULT Cmdattribpol(CWnd *wnd, const String& s)
{
	new FormAttributePolygonMap(wnd, s.scVal());
	return -1;
}

FormAttributePolygonMap::FormAttributePolygonMap(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitleAttribPolMap)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpa")
        if (sPolygonMap == "")
          sPolygonMap = fn.sFullNameQuoted(true);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  fldPolygonMap = new FieldPolygonMap(root, SAFUiPolMap, &sPolygonMap,
	                              new MapListerDomainType(".mpa", dmCLASS | dmIDENT | dmGROUP | dmBOOL | dmUNIQUEID));
  fldPolygonMap->SetCallBack((NotifyProc)&FormAttributePolygonMap::PolygonMapCallBack);
  fldTbl = new FieldTable(root, SAFUiTable, &sTbl);
  fldTbl->SetCallBack((NotifyProc)&FormAttributePolygonMap::TblCallBack);
  fldCol = new FieldColumn(root, SAFUiAttribute, Table(), &sCol,
    dmCLASS|dmIDENT|dmVALUE|dmBOOL|dmUNIQUEID);
  fldCol->SetCallBack((NotifyProc)&FormAttributePolygonMap::ColCallBack);
  stColRemark = new StaticText(root, String('x',50));
  stColRemark->SetIndependentPos();
  initPolygonMapOut(false);
  SetAppHelpTopic(htpPolygonMapAttribute);
  create();
}                    

int FormAttributePolygonMap::exec() 
{
  FormPolygonMapCreate::exec();
  String sExpr;
  bool fShort = false;
  FileName fn(sOutMap);
  FileName fnTbl(sTbl);
  FileName fnPolygonMap(sPolygonMap); 
  PolygonMap map(fnPolygonMap);
  if (map.fValid()) {
    if (map->fTblAtt()) {
      Table tbl = map->tblAtt();
      fShort = (tbl->fnObj == fnTbl);
    }  
  }    
  sPolygonMap = fnPolygonMap.sRelativeQuoted(false,fn.sPath());
  if (!fShort)
    sCol = String("%S.%S", sTbl, sCol);
  sExpr = String("PolygonMapAttribute(%S,%S)", 
                  sPolygonMap,sCol);
  execPolygonMapOut(sExpr);  
  return 0;
}

int FormAttributePolygonMap::PolygonMapCallBack(Event*)
{
	fldPolygonMap->StoreData();
	if (sPolygonMap.length() == 0)
		return 0;

	fldCol->FillWithColumns((TablePtr*)0);
	try
	{
		FileName fnMap(sPolygonMap); 
		PolygonMap map(fnMap);
		if (map.fValid())
		{
			fldTbl->SetDomain(map->dm()->sName());
			if (map->fTblAtt()) {
				fldTbl->SetVal(map->tblAtt()->fnObj.sFullPath());
			}
		}  
	}
	catch (ErrorObject&) {}
	return 0;
}

int FormAttributePolygonMap::TblCallBack(Event*)
{
	fldTbl->StoreData();
	if (sTbl.length() == 0)
		return 0;

	fldCol->FillWithColumns((TablePtr*)0);
	try
	{
		FileName fnTbl(sTbl); 
		fldCol->FillWithColumns(fnTbl);
		ColCallBack(0);
	}
	catch (ErrorObject&) {}
	return 0;
}

int FormAttributePolygonMap::ColCallBack(Event*)
{
  fldCol->StoreData();
  try {
    FileName fnTbl(sTbl); 
    ColumnInfo colinf(fnTbl, sCol);
    if (!colinf.fValid()) {
      stColRemark->SetVal("");
      return 0;
    }
    Domain dm = colinf.dm();
    String sRemark(dm->sTypeName());
    stColRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) {
    stColRemark->SetVal("");
  }
  return 0;
}

LRESULT Cmdlabelpol(CWnd *wnd, const String& s)
{
	new FormPolygonMapLabels(wnd, s.scVal());
	return -1;
}

FormPolygonMapLabels::FormPolygonMapLabels(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitlePolMapLabels)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if ((fn.sExt == "" || fn.sExt == ".mpa") && sPolygonMap == "")
        sPolygonMap = fn.sFullNameQuoted(false);
      else if ((fn.sExt == "" || fn.sExt == ".mpp") && sPointMap == "") 
        sPointMap = fn.sFullNameQuoted(false);
      else
        sOutMap = fn.sFullName(false);
    }
  }
  new FieldPolygonMap(root, SAFUiPolMap, &sPolygonMap);
  new FieldPointMap(root, SAFUiLabelPnts, &sPointMap, new MapListerDomainType(".mpp", 0/*dmVALUE|dmCLASS|dmIDENT|dmBOOL*/, true));
  initPolygonMapOut(false);
  SetAppHelpTopic(htpPolygonMapLabels);
  create();
}                    

int FormPolygonMapLabels::exec() 
{
  FormPolygonMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPolygonMap(sPolygonMap); 
  FileName fnPointMap(sPointMap); 
  sPolygonMap = fnPolygonMap.sRelativeQuoted(false,fn.sPath());
  sPointMap = fnPointMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("PolygonMapLabels(%S,%S)", 
                  sPolygonMap,sPointMap);
  execPolygonMapOut(sExpr);  
  return 0;
}

LRESULT Cmdsegpol(CWnd *wnd, const String& s)
{
	new FormPolygonMapFromSegment(wnd, s.scVal());
	return -1;
}

FormPolygonMapFromSegment::FormPolygonMapFromSegment(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitlePolMapFromSeg)
{
	sMask = "*";
	fMask = false;
	fTopology = true;
	fAutoCorrect = false;
	iOption = 0;
	if (sPar)
	{
		TextInput inp(sPar);
		TokenizerBase tokenizer(&inp);
		String sVal;
		for (;;)
		{
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			
			FileName fn(sVal);
			if ((fn.sExt == "" || fn.sExt == ".mps") && sSegmentMap == "")
				sSegmentMap = fn.sFullNameQuoted(false);
			else if (fn.sExt == ".mpp")
			{
				sLblPointMap = fn.sFullNameQuoted(false);
				iOption = 0;
			}
			else
				sOutMap = fn.sFullName(false);
		}
	}
	new FieldSegmentMap(root, SAFUiSegMap, &sSegmentMap, new MapListerDomainType(".mps", 0));
	CheckBox* cbMask = new CheckBox(root, SAFUiMask, &fMask);
	new FieldString(cbMask, "", &sMask);
	
	cbTopology = new CheckBox(root, SAFUiTopology, &fTopology);
	cbTopology->Align(cbMask, AL_UNDER);
	cbTopology->SetCallBack((NotifyProc)&FormPolygonMapFromSegment::CallBack);
	
	fgTop = new FieldGroup(root);
	fgTop->Align(cbTopology, AL_UNDER);
	RadioGroup* rgTop = new RadioGroup(fgTop, "", &iOption);
	RadioButton* rbLbl = new RadioButton(rgTop, SAFUiLabelPnts);
	new FieldPointMap(rbLbl, "", &sLblPointMap);
	new RadioButton(rgTop, SAFUiPolAutoIdent);
	CheckBox* cb = new CheckBox(fgTop, SAFUiAutoCorrection, &fAutoCorrect);
	cb->Align(rgTop, AL_UNDER);
	
	fgNonTop = new FieldGroup(root);
	fgNonTop->Align(cbTopology, AL_UNDER);
	RadioGroup* rgNonTop = new RadioGroup(fgNonTop, "", &iOption);
	new RadioButton(rgNonTop, SAFUiUseSegCode);
	rbLbl = new RadioButton(rgNonTop, SAFUiLabelPnts);
	new FieldPointMap(rbLbl, "", &sLblPointMap);
	new RadioButton(rgNonTop, SAFUiPolAutoIdent);
	
	initPolygonMapOut(false);
	SetAppHelpTopic(htpPolygonMapFromSegment);
	create();
}                    

int FormPolygonMapFromSegment::CallBack(Event*)
{
	cbTopology->StoreData();
	if (fTopology) {
		fgTop->Show();
		fgNonTop->Hide();
	}
	else {
		fgTop->Hide();
		fgNonTop->Show();
	}
	return 0;
}

int FormPolygonMapFromSegment::exec() 
{
  FormPolygonMapCreate::exec();
  if (!fMask)
    sMask = "";
  String sExpr;
  FileName fn(sOutMap);
  FileName fnSegmentMap(sSegmentMap); 
  sSegmentMap = fnSegmentMap.sRelativeQuoted(false,fn.sPath());
  String s1, s2, s3;
	if (fTopology) {
		s1 = String("PolygonMapFromSegment(%S,\"%S\"",sSegmentMap, sMask);
		if (0 == iOption) {
			FileName fnLbl(sLblPointMap);
			sLblPointMap = fnLbl.sRelativeQuoted(true,fn.sPath());
			s2 = String(",%S", sLblPointMap);
		}
		if (fAutoCorrect)
			s3 = ",auto)";
		else
			s3 = ")";
	}
	else {
		s1 = String("PolygonMapFromSegmentNonTopo(%S,\"%S\"",sSegmentMap, sMask);
		switch (iOption)
		{
			case 0:
				s2 = ",segments";
				break;
			case 1:
			{
				FileName fnLbl(sLblPointMap);
				sLblPointMap = fnLbl.sRelativeQuoted(true,fn.sPath());
				s2 = String(",%S", sLblPointMap);
			} break;
			case 2:
				s2 = ",unique";
				break;
		}
		s3 = ")";
	}
  sExpr = String("%S%S%S", s1, s2, s3);
  execPolygonMapOut(sExpr);
  return 0;
}

LRESULT Cmdtransfpol(CWnd *wnd, const String& s)
{
	new FormPolygonMapTransform(wnd, s.scVal());
	return -1;
}

FormPolygonMapTransform::FormPolygonMapTransform(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitlePolMapTransform)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
			if (fn.sExt == ".csy")
        sCoordSys = fn.sFullNameQuoted(false);
			else if (fn.sExt == "" || fn.sExt == ".mpa")
				if (sPolygonMap == "")
					 sPolygonMap = fn.sFullNameQuoted(false);
				else  
					 sOutMap = fn.sFullName(false);
    }
  }
  fldMap = new FieldPolygonMap(root, SAFUiPolMap, &sPolygonMap, new MapListerDomainType(".mpa", 0));
  fldMap->SetCallBack((NotifyProc)&FormPolygonMapTransform::MapCallBack);
  String sFill('x', 50);
  stCsy = new StaticText(root, sFill);
  stCsy->SetIndependentPos();
	fDensify = false;
	cbDensify = new CheckBox(root, SAFUiDensify, &fDensify);
	cbDensify->SetCallBack((NotifyProc)&FormPolygonMapTransform::DensifyCallBack);

	cbDensify->SetIndependentPos();

	fldDegMinSec = new FieldDMS(cbDensify, SAFUiDistance, &rDistance, 30, true);
	fldDegMinSec->Align(cbDensify, AL_UNDER);
	fldMeters = new FieldReal(cbDensify, SAFUiDistanceInMeters, &rDistance, ValueRange(0.001,1e12,0));
	fldMeters->Align(cbDensify, AL_UNDER);

	// do not call initPolygonMapOut(false) because also csy has to be asked
  fmc = new FieldPolygonMapCreate(root, SAFUiOutPolMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormPolygonMapCreate::OutPolygonMapCallBack);
  new FieldCoordSystemC(root, SAFUiCoordSys, &sCoordSys);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  
  SetAppHelpTopic(htpPolygonMapTransform);
  create();
}                    

int FormPolygonMapTransform::MapCallBack(Event*)
{
  fldMap->StoreData();
  try {
    FileName fnMap(sPolygonMap);
    PolygonMap map(fnMap);
    if (map.fValid()) {
      stCsy->SetVal(map->cs()->sTypeName());
			fFromLatLon = 0 != map->cs()->pcsLatLon();
			if (fDensify) 
			{
				rDistance = PolygonMapTransform::rDefaultDensifyDistance(map);
				if (fFromLatLon) 
				{
					fldDegMinSec->SetVal(rDistance);
					fldMeters->Hide();
					fldDegMinSec->Show();
				}
				else {
					fldMeters->SetVal(rDistance);
					fldMeters->Show();
					fldDegMinSec->Hide();
				}     
			}
			else
			{
				rDistance = 0;
				fldMeters->Hide();
				fldDegMinSec->Hide();
			}    
		}
    else
      stCsy->SetVal("");
  }
  catch (ErrorObject&) {
    stCsy->SetVal("");
		fldMeters->Hide();
		fldDegMinSec->Hide();
  }

  return 0;
}

int FormPolygonMapTransform::DensifyCallBack(Event*)
{
	cbDensify->StoreData();
  try {
		FileName fnMap(sPolygonMap); 
		PolygonMap map(fnMap);
		if (map.fValid()) {
			stCsy->SetVal(map->cs()->sTypeName());
			fFromLatLon = 0 != map->cs()->pcsLatLon();
			if (fDensify) 
			{
				rDistance = PolygonMapTransform::rDefaultDensifyDistance(map);
				if (fFromLatLon) 
				{
					fldDegMinSec->SetVal(rDistance);
					fldMeters->Hide();
					fldDegMinSec->Show();
				}
				else 
				{
					fldMeters->SetVal(rDistance);
					fldMeters->Show();
					fldDegMinSec->Hide();
				}     
			}
			else
			{
				rDistance = 0;
				fldMeters->Hide();
				fldDegMinSec->Hide();
			}
		}
		else {
			stCsy->SetVal("");
			rDistance = 0;
			fldMeters->Hide();
			fldDegMinSec->Hide();
		}
	}
  catch (ErrorObject&) {
    stCsy->SetVal("");
		rDistance = 0;
		fldMeters->Hide();
		fldDegMinSec->Hide();
  }
	return 0;
}

int FormPolygonMapTransform::exec() 
{
  FormPolygonMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPolygonMap(sPolygonMap); 
  FileName fnCoordSys(sCoordSys); 
  sPolygonMap = fnPolygonMap.sRelativeQuoted(false,fn.sPath());
  sCoordSys = fnCoordSys.sRelativeQuoted(false,fn.sPath());
  sExpr = String("PolygonMapTransform(%S,%S,%lf)", 
                  sPolygonMap,sCoordSys, rDistance);
  execPolygonMapOut(sExpr);  
  return 0;
}

LRESULT Cmdmaskpol(CWnd *wnd, const String& s)
{
	new FormPolygonMapMask(wnd, s.scVal());
	return -1;
}

FormPolygonMapMask::FormPolygonMapMask(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitleMaskPolMap)
{
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpa")
        if (sPolygonMap == "")
          sPolygonMap = fn.sFullNameQuoted(false);
        else  
          sOutMap = fn.sFullName(false);
    }
  }
  new FieldPolygonMap(root, SAFUiPolMap, &sPolygonMap, new MapListerDomainType(".mpa", 0, false));
  new FieldString(root, SAFUiMask, &sMask, Domain(), false);
  initPolygonMapOut(false);
  SetAppHelpTopic(htpPolygonMapMask);
  create();
}                    

int FormPolygonMapMask::exec() 
{
  FormPolygonMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnPolygonMap(sPolygonMap); 
  sPolygonMap = fnPolygonMap.sRelativeQuoted(false,fn.sPath());
  sExpr = String("PolygonMapMask(%S,\"%S\")", 
                  sPolygonMap, sMask);
  execPolygonMapOut(sExpr);  
  return 0;
}

LRESULT Cmdraspol(CWnd *wnd, const String& s)
{
	new FormPolygonMapFromRaster(wnd, s.scVal());
	return -1;
}

FormPolygonMapFromRaster::FormPolygonMapFromRaster(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTitlePolMapFromRas)
{
  fSmooth = true;
  iConnect = 1;
  if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if (fn.sExt == "" || fn.sExt == ".mpr") 
        sMap = fn.sFullNameQuoted(true);
      else
        sOutMap = fn.sFullName(true);
    }
  }
  new FieldDataType(root, SAFUiRasMap, &sMap,
                 new MapListerDomainType(".mpr", dmCLASS|dmIDENT|dmBOOL|dmUNIQUEID, true), true);
  RadioGroup* rg = new RadioGroup(root, SAFUiConnect, &iConnect);
  rg->SetIndependentPos();
  RadioButton* rb4 = new RadioButton(rg, SAFUi4Connected);
  rb4->Align(rg, AL_AFTER);
  RadioButton* rb8 = new RadioButton(rg, SAFUi8Connected);
  rb8->Align(rb4, AL_AFTER);
  new CheckBox(root, SAFUiSmoothLines, &fSmooth);
  initPolygonMapOut(false);
  SetAppHelpTopic(htpPolygonMapFromRas);
  create();
}                    

int FormPolygonMapFromRaster::exec() 
{
  FormPolygonMapCreate::exec();
  String sExpr;
  FileName fn(sOutMap);
  FileName fnMap(sMap); 
  sMap = fnMap.sRelativeQuoted(false,fn.sPath());
  String sConnect;
  switch (iConnect) {
    case 0: sConnect = "4"; break;
    case 1: sConnect = "8"; break;
  }
  sExpr = String("PolygonMapFromRas(%S,%S,%s)", 
            sMap, sConnect, fSmooth?"smooth":"nosmooth");
  execPolygonMapOut(sExpr);  
  return 0;
}

LRESULT Cmdidgrid(CWnd *wnd, const String& s)
{
	new FormPolygonMapGrid(wnd, s.scVal());
	return -1;
}

FormPolygonMapGrid::FormPolygonMapGrid(CWnd* mw, const char* sPar)
	: FormPolygonMapCreate(mw, SAFTitlePolygonMapGrid),
	m_fOrderFilled(false),m_fCornerFilled(false),m_fLatLon(false)
{
	if (sPar) {
    TextInput inp(sPar);
    TokenizerBase tokenizer(&inp);
    String sVal;
    for (;;) {
      Token tok = tokenizer.tokGet();
      sVal = tok.sVal();
      if (sVal == "")
        break;
      FileName fn(sVal);
      if ((fn.sExt == ".csy" || fn.sExt == "") && m_sCSY == "")
        m_sCSY = fn.sFullNameQuoted(true);
      else
        sOutMap = fn.sFullName(true);
    }
  }

	m_iChkRowColValue = 1;
	m_iChkSizeValue = 1;
	m_iChkDirXY = 0;
	m_iChkDirLL = 0;
	
  m_fldCSY = new FieldCoordSystemC(root, SAFUiCoordSys, &m_sCSY);
  m_fldCSY->SetCallBack((NotifyProc)&FormPolygonMapGrid::ChangeCSY);

	m_fldOrigin = new FieldCoord(root, SAFUiOriginCoord, &m_crdOrigin);

	FieldGroup *fg = new FieldGroup(root);
	fg->Align(m_fldCSY,AL_UNDER);
	m_stOriCoordLL = new StaticText(fg, SAFUiOriginCoordLL);
	m_fldOriLat = new FieldLat(fg, "", &m_llOri.Lat);
	m_fldOriLat->Align(m_stOriCoordLL,AL_AFTER);
	StaticText *stEmpty = new StaticText(fg, "");
	stEmpty->Align(m_stOriCoordLL,AL_UNDER); 
	m_fldOriLon = new FieldLon(fg, "", &m_llOri.Lon);
	m_fldOriLon->Align(m_fldOriLat,AL_UNDER); 
	
	m_chkGridSize = new CheckBox(root, SAFUiGridSize, &m_iChkSizeValue);
	m_chkGridSize->Align(stEmpty, AL_UNDER); 
	m_chkGridSize->SetCallBack((NotifyProc)&FormPolygonMapGrid::ChangeGridProperties);

	ValueRange vr1(0.0001, 1e300, 1e-6);
	m_rWidth = rUNDEF;
	m_rHeight = rUNDEF;
	FieldReal *fldWidth = new FieldReal(m_chkGridSize, "", &m_rWidth, vr1);
	FieldReal *fldHeight = new FieldReal(m_chkGridSize, "", &m_rHeight, vr1);
	fldWidth->Align(m_chkGridSize, AL_AFTER);
	fldWidth->SetWidth(45);
	fldWidth->SetIndependentPos();
	fldHeight->Align(fldWidth, AL_AFTER);
	fldHeight->SetWidth(45);

	m_chkRowCol = new CheckBox(root, SAFUiNrVerticalHorizontalGrids, &m_iChkRowColValue);
	m_chkRowCol->SetCallBack((NotifyProc)&FormPolygonMapGrid::ChangeGridProperties);
	m_chkRowCol->Align(m_chkGridSize, AL_UNDER);
	ValueRange vr2(1, 1e300, 1e-6);
	m_iRows = iUNDEF;
	m_iCols = iUNDEF;
	FieldInt *fldRows = new FieldInt(m_chkRowCol, "", &m_iRows, vr2);
	FieldInt *fldCols = new FieldInt(m_chkRowCol, "", &m_iCols, vr2);
	fldRows->Align(m_chkRowCol, AL_AFTER);
	fldRows->SetWidth(45);
	fldRows->SetIndependentPos();
	fldCols->Align(fldRows, AL_AFTER);
	fldCols->SetWidth(45);

	m_fldCrdUR =  new FieldCoord(root, String("%S (X,Y)", SAFUiOppositeCoord), &m_crdUR);
	m_fldCrdUR->Align(m_chkRowCol,AL_UNDER);
	
	m_stURLL = new StaticText(root, SAFUiOppositeCoordLL);
	m_stURLL->Align(m_chkRowCol,AL_UNDER);
	m_fldURLat = new FieldLat(root, "", &m_llUR.Lat);
	m_fldURLat->Align(fldRows,AL_UNDER);
	StaticText *stEmpty1 = new StaticText(root, "");
	stEmpty1->Align(m_stURLL,AL_UNDER); 
	m_fldURLon = new FieldLon(root, "", &m_llUR.Lon);
	m_fldURLon->Align(m_fldURLat,AL_UNDER); 

	m_chkDirXY = new CheckBox(root, String("%S (X,Y)", SAFUiDirectionPoint), &m_iChkDirXY);
	m_chkDirXY->Align(stEmpty1,AL_UNDER); 
	m_chkDirLL = new CheckBox(root, String("%S (Lat,Lon)", SAFUiDirectionPoint), &m_iChkDirLL);
	m_chkDirLL->Align(stEmpty1,AL_UNDER);
	m_chkDirLL->Hide(); 
	
	m_fldCrdP = new FieldCoord(m_chkDirXY, "", &m_crdP);
	m_fldCrdP->Align(m_chkDirXY,AL_AFTER);
	m_fldCrdP->SetIndependentPos();
	StaticText *st1 = new StaticText(root, "");
	st1->Align(m_chkDirXY,AL_UNDER); 

	m_fldDPLat = new FieldLat(m_chkDirLL, "", &m_llDP.Lat);
	m_fldDPLat->Align(m_chkDirLL,AL_AFTER);
	m_fldDPLat->SetIndependentPos(); 
	m_fldDPLon = new FieldLon(m_chkDirLL, "", &m_llDP.Lon);
	m_fldDPLon->Align(st1,AL_AFTER); 
	m_fldDPLon->SetIndependentPos(); 

  FieldGroup *fgOrder = new FieldGroup(root);
	fgOrder->Align(st1,AL_UNDER);
  fgOrder->SetIndependentPos();
	FieldGroup *fgCorner = new FieldGroup(root);
	fgCorner->Align(fgOrder,AL_AFTER);
	fgCorner->SetIndependentPos();
	
	StaticText *stOrder = new StaticText(fgOrder, SAFUiOrder);
	m_fldOrder = new FieldOneSelectTextOnly(fgOrder, &m_sOrder, false);
	m_fldOrder->SetCallBack((NotifyProc)&FormPolygonMapGrid::FillOrder);
	m_fldOrder->SetWidth(53);
	m_fldOrder->Align(stOrder, AL_AFTER);
	StaticText *stSC = new StaticText(fgCorner, SAFUiStartCorner);
	//FieldBlank *fb = new FieldBlank(fgOrder, 0); // Used to force proper alignment
	m_fldCorner = new FieldOneSelectTextOnly(fgCorner, &m_sStartCorner, false);
	m_fldCorner->SetCallBack((NotifyProc)&FormPolygonMapGrid::FillStartCorner);
	m_fldCorner->SetWidth(50);
	m_fldCorner->Align(stSC, AL_AFTER);
	
	ValueRange vr3(0, 1e300, 1e-6);
	m_iStartNr = 1;
	m_iSteps = 1;
	FieldInt *fldStareNr = new FieldInt(fgOrder, SAFUiStartNumber, &m_iStartNr, vr3);
	fldStareNr->Align(fgOrder,AL_UNDER);
	fldStareNr->SetWidth(52.9);
	FieldInt *fldSteps = new FieldInt(fgCorner, SAFUiSteps, &m_iSteps, vr3);
	fldSteps->Align(fgCorner,AL_UNDER);
	fldSteps->SetWidth(49.9);
		
	FieldString *fldPrefix = new FieldString(fgOrder, SAFUiPrefix, &m_sPrefix);
	fldPrefix->Align(fldStareNr,AL_UNDER);
	fldPrefix->SetWidth(52.9);
	
	FieldString *fldPostfix = new FieldString(fgCorner, SAFUiPostfix, &m_sPostfix);
	fldPostfix->Align(fldSteps,AL_UNDER);
	fldPostfix->SetWidth(49.9);
	
	m_iTable = 0;
	m_iLabels = 0;
	CheckBox *chkTable = new CheckBox(root, SAFUiTable, &m_iTable);
	chkTable->Align(fgOrder,AL_UNDER);
	FieldDataType *ftbl = new FieldDataType(chkTable, "", &m_sTbl, new TableListerDomainType(dmCLASS|dmIDENT|dmUNIQUEID), false);
  ftbl->Align(chkTable,AL_AFTER);
	CheckBox *chkLabels = new CheckBox(root, SAFUiOutputLabelPoints, &m_iLabels);
	chkLabels->Align(chkTable,AL_UNDER);
	
	initPolygonMapOut(false);
  SetAppHelpTopic(htpPolygonMapGrid);
  create();
}

int FormPolygonMapGrid::ChangeCSY(Event *)
{
	m_fldCSY->StoreData();
	
	if (File::fExist(FileName(m_sCSY, ".csy")))
	{
		CoordSystem csy(m_sCSY);
		m_fLatLon = csy->pcsLatLon() == 0 ? false : true;

	}
	else
		m_fLatLon = false;

	if (! m_fLatLon)
	{
			m_stOriCoordLL->Hide(); 
			m_stURLL->Hide();
			m_fldOriLat->Hide();
			m_fldOriLon->Hide();
			m_fldURLat->Hide();
			m_fldURLon->Hide();
			m_fldDPLat->Hide();
			m_fldDPLon->Hide();
			m_fldOrigin->Show();
			
			if (m_iChkSizeValue == 1 && m_iChkRowColValue == 1)
			  m_fldCrdUR->Hide();
			else
				m_fldCrdUR->Show();
			m_chkDirXY->Show();
			m_chkDirLL->Hide();
			
	}	
	else
	{
			m_fldOrigin->Hide();
			m_fldCrdUR->Hide();
			m_fldCrdP->Hide();
			m_stOriCoordLL->Show();
			m_stURLL->Show();
			m_fldOriLat->Show();
			m_fldOriLon->Show();
			if (m_iChkSizeValue == 1 && m_iChkRowColValue == 1)
			{
					m_fldURLat->Hide();
					m_fldURLon->Hide();
			}	
			else
			{
					m_fldURLat->Show();
					m_fldURLon->Show();
			}
			m_chkDirLL->Show();
			m_chkDirXY->Hide();
	}	

	return 0;
}

int FormPolygonMapGrid::FillOrder(Event *)
{
	if (m_fOrderFilled)
		return 0;

	m_fldOrder->AddString("Horizontal");
	m_fldOrder->AddString("Horizontal Continued");
	m_fldOrder->AddString("Vertical");
	m_fldOrder->AddString("Vertical Continued");
	m_fldOrder->SelectItem("Horizontal");
	m_fOrderFilled = true;

	return 1;
}

int FormPolygonMapGrid::FillStartCorner(Event *)
{
	if (m_fCornerFilled)
		return 0;

	m_fldCorner->AddString("Lower Left");
	m_fldCorner->AddString("Lower Right");
	m_fldCorner->AddString("Upper Left");
	m_fldCorner->AddString("Upper Right");
	m_fldCorner->SelectItem("Lower Left");

	m_fCornerFilled = true;

	return 1;
}

int FormPolygonMapGrid::ChangeGridProperties(Event*)
{
	m_chkGridSize->StoreData();
	m_chkRowCol->StoreData();

	if (m_iChkSizeValue == 1 && m_iChkRowColValue == 1)
	{
		if (!m_fLatLon)
			m_fldCrdUR->Hide();
		else
		{
			m_fldURLat->Hide();
			m_fldURLon->Hide();
		}
		m_chkRowCol->enable(); 
		m_chkGridSize->enable();
	}	
	else if (m_iChkSizeValue == 0 || m_iChkRowColValue == 0)
	{
		if (!m_fLatLon)
			m_fldCrdUR->Show();
		else
		{
			m_fldURLat->Show();
			m_fldURLon->Show();
		}
		if (m_iChkSizeValue == 0)
			m_chkRowCol->disable();  
		else if (m_iChkRowColValue == 0)
			m_chkGridSize->disable(); 

	}	
	return 1;
}

int FormPolygonMapGrid::exec() 
{
	FormPolygonMapCreate::exec();

	String sLabels,sOdLbl,sScLbl;
	if (fCIStrEqual(m_sOrder, "Horizontal"))
		sOdLbl = "H";
	else if (fCIStrEqual(m_sOrder, "Horizontal Continued"))
		sOdLbl = "HC";

	else if (fCIStrEqual(m_sOrder, "Vertical"))
		sOdLbl = "V";
	else
		sOdLbl = "VC";

	if (fCIStrEqual(m_sStartCorner, "Lower Left"))
		sScLbl= "LL";
	else if (fCIStrEqual(m_sStartCorner, "Lower Right"))
		sScLbl = "LR";

	else if (fCIStrEqual(m_sStartCorner, "Upper Left"))
		sScLbl = "UL";
	else
		sScLbl = "UR";

	if (m_iLabels == 1)
		sLabels = "LabelPoints";
	else
		sLabels = "NoLabelPoints";

	String sOriginX,sOriginY;
	String sURX,sURY,sDPX,sDPY;
	if (! m_fLatLon)
	{
			sOriginX = String("%g",m_crdOrigin.x);
			sOriginY = String("%g",m_crdOrigin.y);
			if ((m_crdUR.x != rUNDEF) && (m_crdUR.y != rUNDEF))
			{
					sURX = String("%g",m_crdUR.x); 
					sURY = String("%g",m_crdUR.y); 
			}
			if (m_iChkDirXY == 1)
			{
					sDPX = String("%g",m_crdP.x); 
					sDPY = String("%g",m_crdP.y); 
			}
	}
	else
	{
			sOriginX = String("%g",m_llOri.Lon);
			sOriginY = String("%g",m_llOri.Lat);
			if ((m_llUR.Lon != rUNDEF) && (m_llUR.Lat != rUNDEF))
			{
					sURX = String("%g",m_llUR.Lon); 
					sURY = String("%g",m_llUR.Lat);
			}

			if (m_iChkDirLL == 1)
			{
					sDPX = String("%g",m_llDP.Lon); 
					sDPY = String("%g",m_llDP.Lat);
			}
	}	
	String sWidth, sHeight,sRows,sCols;
	if (m_iChkSizeValue )
	{
		 sWidth = String("%g",m_rWidth);
		 sHeight = String("%g",m_rHeight);
	}
	if (m_iChkRowColValue )
	{
		 sRows = String("%li",m_iRows);
		 sCols = String("%li",m_iCols);
	}
	//m_sPrefix = 
	//m_sPostfix = String("'%S'", m_sPostfix);
	FileName fn(sOutMap);
  FileName fnCS(m_sCSY);
	m_sCSY = fnCS.sRelativeQuoted(false,fn.sPath());
	if (m_sTbl.length()  != 0)
	{
			FileName fnTbl(m_sTbl);
			m_sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());		
	}	
  
	String sExpr = String("PolygonMapGrid(%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%li,%li,%S,%S)", 
		              m_sCSY,
									sOriginX,sOriginY,
									sWidth, sHeight,
									sRows, sCols,
									sURX,sURY,
									sDPX,sDPY,
									m_sTbl,sLabels,sOdLbl,sScLbl,m_iStartNr,m_iSteps,m_sPrefix,m_sPostfix);
  execPolygonMapOut(sExpr);  
	
	return 0;
}
