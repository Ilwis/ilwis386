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
// $Log: /ILWIS 3.0/FormElements/fldmap.cpp $
 * 
 * 13    26-10-00 17:50 Koolhoven
 * hour glass cursor in create map form exec
 * 
 * 12    5-09-00 17:05 Koolhoven
 * default domain for Create Pnt and Seg Map is again not filled in
 * (instead of UniqueID, because that requires a more complete
 * implementation)
 * 
 * 11    3-08-00 14:24 Koolhoven
 * fUpdateCatalog flag for attributetable of pnt/segmap is switched off
 * 
 * 10    27-07-00 15:16 Koolhoven
 * Added UniqueID option to Create PntMap and SegMap forms
 * 
 * 9     27-07-00 13:01 Koolhoven
 * improved layout of Create Vector Map forms
 * 
 * 8     17-05-00 14:35 Koolhoven
 * Added icon in create forms
 * 
 * 7     6-05-00 10:53 Hendrikse
 * In 2nd constructor of FormCreateVectormap distiction is now made
 * between fFromLatLon yes or no to make the input fields of coordbounds.
 * They will not appear both anymore in case of creating a map from inside
 * a mapwindow.
 * 
 * 6     3/24/00 3:11p Hendrikse
 * changed CallBack()  into CallBackXY();
 * added FormCreateVectorMap::exec
 * and calls to it in order to stoe modified Latlons in CreateForm
 * 
 * 5     3/23/00 1:05p Hendrikse
 * implemented  CallBack for wrong LatLon bounds
 * in FormCreateVectorMap 
 * 
 * 4     8/12/99 10:29 Willem
 * Changed FileName asignments (added extension in constructor)
 * 
 * 3     9/13/99 12:33p Wind
 * comments
 * 
 * 2     9/13/99 10:21a Wind
 * adapted to quoted file names
*/
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/10 16:54:12  Wim
// FormCreateMap now tells about bytes per pixel
//
/* FieldMap
   by Wim Koolhoven, november 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   10 Sep 97    6:38 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldmap.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Appforms.hs"
#include "Headers\Hs\Coordsys.hs"
#include "Client\ilwis.h"

FormCreateBaseMap::FormCreateBaseMap(CWnd* wPar, const String& sTitle,
  const String& sext, String* smap, const String& sDom)
  : FormWithDest(wPar, sTitle),
  sExt(sext),
  sMap(smap),
  sDomain(sDom),
  vr(0,100,1)
{
	iImg = IlwWinApp()->iImage(sExt);

  if (sMap)
    sNewName = *sMap;
  fmc = new FieldDataTypeCreate(root, SMSUiMapName, &sNewName, sExt, false);
  fmc->SetCallBack((NotifyProc)&FormCreateBaseMap::CallBackName);
  initDescr();
}

int FormCreateBaseMap::CallBackName(Event*)
{
  fmc->StoreData();
  FileName fn(sNewName, sExt);
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(SAFErrInvalidMapName);
  else if(File::fExist(fn))   
    stRemark->SetVal(SAFErrMapExists);
  else {
    fOk = true;  
    stRemark->SetVal("");
  }
  if (fOk)
    EnableOK();
  else    
    DisableOK();
  return 0;
}

int FormCreateBaseMap::DomainCallBack(Event*)
{

  fdc->StoreData();
  fvr->Hide();
  if (sDomain[0]) {
    FileName fn(sDomain);
    try { 
      Domain dm(fn);
      ValueRange vr(dm);
      if (vr.fValid()) {
        fvr->SetVal(vr);
        fvr->DomainCallBack(0);
//        fvr->Show();
      }
    }
    catch (ErrorObject&) {
    }
  }
  return 0;
}

void FormCreateBaseMap::initDescr()
{
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
}

void FormCreateBaseMap::initDomain(long dmTypes)
{
  fdc = new FieldDomainC(root, SAFUiDomain, &sDomain, dmTypes);
  fdc->SetCallBack((NotifyProc)&FormCreateBaseMap::DomainCallBack);
	fvr = new FieldValueRange(root, SAFUIRange, &vr, fdc);
  fvr->Align(fdc, AL_UNDER);

  String sFill('*', 40);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
}

int FormCreateBaseMap::exec()
{
  FormWithDest::exec();
  FileName fn(sNewName, sExt);
  sNewName = fn.sFullPathQuoted(true);
  if (sMap)
    *sMap = sNewName;
  return 0;
}

FormCreateMap::FormCreateMap(CWnd* wPar, String* smap, 
                const String& sGrf, const String& sDom)
: FormCreateBaseMap(wPar, SAFTitleCreateMap, ".mpr", smap, sDom),
  sGeoRef(sGrf)
{
  new FieldGeoRefC(root, SAFUiGeoRef, &sGeoRef);
  initDomain(dmCLASS|dmIDENT|dmVALUE|dmIMAGE|dmBOOL|dmBIT);
  fdc->SetCallBack((NotifyProc)&FormCreateMap::DomainCallBack);
  fvr->SetCallBack((NotifyProc)&FormCreateMap::ValueRangeCallBack);

  SetMenHelpTopic(htpCreateMap);
  create();
}  

int FormCreateMap::exec()
{
	CWaitCursor wc;
  FormCreateBaseMap::exec();
  GeoRef grf(sGeoRef);
  if (grf->rcSize().Row < 2 || grf->rcSize().Col < 2) {
    MessageBox(SAFErrWrongSizeGeoref.scVal(), SAFTitleCreateMap.scVal(), MB_OK | MB_ICONEXCLAMATION);
    return 0;
  }
  Domain dm(sDomain);
  DomainValueRangeStruct dvrs(dm,vr);

  Map map(sNewName, grf, grf->rcSize(), dvrs);
  map->sDescription = sDescr;
  map->FillWithUndef();

  return 0;
}

int FormCreateMap::DomainCallBack(Event*)
{
  FormCreateBaseMap::DomainCallBack(0);
  SetRemarkOnBytesPerPixel();
  return 0;
}

int FormCreateMap::ValueRangeCallBack(Event*)
{
  fvr->StoreData();
  SetRemarkOnBytesPerPixel();
  return 0;
}

void FormCreateMap::SetRemarkOnBytesPerPixel()
{
  if (0 == stRemark)
    return;
  if (sDomain.length() == 0) {
    stRemark->SetVal("");
    return;
  }  
  FileName fn(sDomain);
  try { 
    Domain dm(fn);
    DomainValueRangeStruct dvrs(dm, vr);
    String s;
    StoreType st = dvrs.st();
    int iNr;
    if (st < stBYTE) {
      if (st == stBIT)
        s = SAFRem1bit;
      else {  
        switch (st) {
          case stDUET: iNr = 2; break;
          case stNIBBLE: iNr = 4; break;
        }
        s = String(SAFRemBits_i.sVal(), iNr);
      }  
    }
    else {
      if (st == stBYTE)
        s = SAFRem1byte;
      else {  
        switch (st) {
          case stINT: iNr = 2; break;
          case stLONG: iNr = 4; break;
          case stREAL: iNr = 8; break;
          case stCRD: iNr = 16; break;
        }
        s = String(SAFRemBytes_i.sVal(), iNr);
      }  
    }
    String sRemark(SAFRemMapPixUse_S.scVal(), s);
    stRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) 
  {
    stRemark->SetVal(SAFRemInvalidDomain);
  }  
}


FormCreateVectorMap::FormCreateVectorMap(CWnd* wPar,
    const String& sTitle, const String& sExt, String* sMap,
    const String& sCsy, const String& sDom)
  : FormCreateBaseMap(wPar, sTitle, sExt, sMap, sDom),
  sCsys(sCsy)
{
  fcsc = new FieldCoordSystemC(root, SMSUiCoordSys, &sCsys);
  fcsc->SetCallBack((NotifyProc)&FormCreateVectorMap::CSysCallBack);

  fgCoord = new FieldGroup(root);
  fldCrdMin = new FieldCoord(fgCoord, SMSUiMinXY, &cb.cMin);
  fldCrdMin->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackXY);
  fldCrdMax = new FieldCoord(fgCoord, SMSUiMaxXY, &cb.cMax);
  fldCrdMax->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackXY);

  fgLatLon = new FieldGroup(root);
	fgLatLon->Align(fcsc, AL_UNDER);
  fldMinLat = new FieldLat(fgLatLon, SCSUiMinLat, &llMin.Lat);
  fldMinLat->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
  fldMinLon = new FieldLon(fgLatLon, SCSUiMinLon, &llMin.Lon);
//  fldMinLon->Align(fldMinLat, AL_AFTER);
  fldMinLon->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
  fldMaxLat = new FieldLat(fgLatLon, SCSUiMaxLat, &llMax.Lat);
//  fldMaxLat->Align(fldMinLon, AL_UNDER);
  fldMaxLat->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
  fldMaxLon = new FieldLon(fgLatLon, SCSUiMaxLon, &llMax.Lon);
//  fldMaxLon->Align(fldMaxLat, AL_AFTER);
  fldMaxLon->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);

//	sDomain = "UniqueID"; -- leave default empty for the time being 5/9/00
  initDomain(dmCLASS|dmIDENT|dmVALUE|dmBOOL|dmUNIQUEID);
}

FormCreateVectorMap::FormCreateVectorMap(CWnd* wPar,
    const String& sTitle, const String& sExt, String* sMap,
    const String& sCsy, const CoordBounds& crdbnds, const String& sDom)
  : FormCreateBaseMap(wPar, sTitle, sExt, sMap, sDom),
  sCsys(sCsy), cb(crdbnds)
{
  StaticText* st = new StaticText(root, SMSUiCoordSys);
  StaticText* st2 = new StaticText(root, sCsys);
  st2->Align(st, AL_AFTER);
  
  FieldGroup *fg = new FieldGroup(root);
	fg->Align(st, AL_UNDER);
	CoordSystem cs(sCsys);
	if (cs.fValid()) {
	  bool fFromLatLon = cs->pcsLatLon() != 0;
		if (!fFromLatLon) {
			fldCrdMin = new FieldCoord(fg, SMSUiMinXY, &cb.cMin);
			fldCrdMin->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackXY);
			fldCrdMax = new FieldCoord(fg, SMSUiMaxXY, &cb.cMax);
			fldCrdMax->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackXY);
		}
		else {
		  fg->SetIndependentPos();
			fldMinLat = new FieldLat(fg, SCSUiMinLat, &llMin.Lat);
			fldMinLat->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
			fldMinLon = new FieldLon(fg, SCSUiMinLon, &llMin.Lon);
		//  fldMinLon->Align(fldMinLat, AL_AFTER);
			fldMinLon->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
			fldMaxLat = new FieldLat(fg, SCSUiMaxLat, &llMax.Lat);
			fldMaxLat->Align(fldMinLon, AL_UNDER);
			fldMaxLat->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
			fldMaxLon = new FieldLon(fg, SCSUiMaxLon, &llMax.Lon);
		//  fldMaxLon->Align(fldMaxLat, AL_AFTER);
			fldMaxLon->SetCallBack((NotifyProc)&FormCreateVectorMap::CallBackLatLon);
		}
	}
	sDomain = "UniqueID";
  initDomain(dmCLASS|dmIDENT|dmVALUE|dmBOOL|dmUNIQUEID);
}

int FormCreateVectorMap::CSysCallBack(Event*)
{
	fcsc->StoreData();
	if (sCsys == "")
	{
		fgLatLon->Hide();
		fgCoord->Hide();
		return 0;
	}
	try 
	{
		CoordSystem cs(sCsys);
		if (cs.fValid()) {
			bool fFromLatLon = cs->pcsLatLon() != 0;
			Coord cMin = cs->cb.cMin;
			Coord cMax = cs->cb.cMax;
			if ((cs->cb.cMin.x >= cs->cb.cMax.x) ||
				(cs->cb.cMin.y >= cs->cb.cMax.y)) 
			{
				//cb.cMin = cb.cMax = Coord();
				cMin = cMax = Coord();
			}  
			if (!fFromLatLon) 
			{
				fldCrdMin->SetVal(cMin);
				fldCrdMax->SetVal(cMax);
				fgCoord->Show();
				fgLatLon->Hide();
			}
			else {
				LatLon laloMin, laloMax;
				laloMin.Lat = cMin.y; 
				laloMax.Lat = cMax.y;
				laloMin.Lon = cMin.x; 
				laloMax.Lon = cMax.x;
				fldMinLat->SetVal(laloMin.Lat);
				fldMaxLat->SetVal(laloMax.Lat);
				fldMinLon->SetVal(laloMin.Lon);
				fldMaxLon->SetVal(laloMax.Lon);
				fgCoord->Hide();
				fgLatLon->Show();
			}
		}
	}
	catch (ErrorObject&) {}  
	return 0;  
}

int FormCreateVectorMap::CallBackXY(Event*) 
{
  fldCrdMin->StoreData();
  fldCrdMax->StoreData();
  Coord cMin = fldCrdMin->crdVal();
  Coord cMax = fldCrdMax->crdVal();
  if (cMin.x < cMax.x && cMin.y < cMax.y)
    EnableOK();
  else
    DisableOK();  
  return 0;  
}

int FormCreateVectorMap::CallBackLatLon(Event*) 
{
  //LatLon llMin, llMax;
	fldMinLat->StoreData();
	fldMinLon->StoreData();
	fldMaxLat->StoreData();
	fldMaxLon->StoreData();
  llMin.Lat = fldMinLat->rVal();
  llMin.Lon = fldMinLon->rVal();
  llMax.Lat = fldMaxLat->rVal();
  llMax.Lon = fldMaxLon->rVal();
  if (llMin.Lat < llMax.Lat && llMin.Lon < llMax.Lon)
    EnableOK();
  else
    DisableOK();
  return 0;  
}

int FormCreateVectorMap::exec()
{
  FormCreateBaseMap::exec();
  CoordSystem cs(sCsys);
  if (cs->pcsLatLon() != 0)
  {
    cb.cMin.x = llMin.Lon;
		cb.cMax.x = llMax.Lon;
		cb.cMin.y = llMin.Lat;
		cb.cMax.y = llMax.Lat;
  }
  return 0;
}

FormCreateSeg::FormCreateSeg(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreateSegMap,
  ".mps", sMap, sCsy, sDomain)
{
  SetMenHelpTopic(htpCreateSegmentMap);
  create();
}

FormCreateSeg::FormCreateSeg(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreateSegMap,
  ".mps", sMap, sCsy, cb, sDomain)
{
  SetMenHelpTopic(htpCreateSegmentMap);
  create();
}

int FormCreateSeg::exec()
{
  FormCreateVectorMap::exec();
  CoordSystem cs(sCsys);
  FileName fn(sNewName);
	Domain dm;
	FileName fnDom(sDomain);
	if ("uniqueid" == fnDom.sFile.toLower()) 
		dm = Domain(fn, 0, dmtUNIQUEID, "seg");
	else
		dm =  Domain(fnDom);
  DomainValueRangeStruct dvrs(dm,vr);
  SegmentMap map(fn, cs, cb, dvrs);
	if ("uniqueid" == fnDom.sFile.toLower()) { 
	  FileName fnAtt(fn, ".tbt", true);
		Table	tblAtt = Table(fnAtt, dm);
		Column col = Column(tblAtt, "Name", Domain("String"));
		map->SetAttributeTable(tblAtt);
		tblAtt->fUpdateCatalog = false;
	}
  map->sDescription = sDescr;
  map->Store();
  return 0;
}

FormCreatePol::FormCreatePol(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreatePolMap,
  ".mpa", sMap, sCsy, sDomain)
{
  SetMenHelpTopic(htpCreatePolygonMap);
  create();
}

FormCreatePol::FormCreatePol(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreatePolMap,
  ".mpa", sMap, sCsy, cb, sDomain)
{
  SetMenHelpTopic(htpCreatePolygonMap);
  create();
}

int FormCreatePol::exec()
{
  FormCreateVectorMap::exec();
  CoordSystem cs(sCsys);
  Domain dm(sDomain);
  DomainValueRangeStruct dvrs(dm,vr);
  FileName fn(sNewName);
  PolygonMap map(fn, cs, cb, dvrs);
  map->sDescription = sDescr;
  map->Store();
  return 0;
}

FormCreatePnt::FormCreatePnt(CWnd* wPar, String* sMap,
    const String& sCsy, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreatePntMap,
  ".mpp", sMap, sCsy, sDomain)
{
  SetMenHelpTopic(htpCreatePointMap);
  create();
}

FormCreatePnt::FormCreatePnt(CWnd* wPar, String* sMap,
    const String& sCsy, const CoordBounds& cb, const String& sDomain)
: FormCreateVectorMap(wPar, SMSTitleCreatePntMap,
  ".mpp", sMap, sCsy, cb, sDomain)
{
  SetMenHelpTopic(htpCreatePointMap);
  create();
}

int FormCreatePnt::exec()
{
  FormCreateVectorMap::exec();
  CoordSystem cs(sCsys);
  FileName fn(sNewName);
	FileName fnDom(sDomain);
	Domain dm;
	if ("uniqueid" == fnDom.sFile.toLower()) 
		dm = Domain(fn, 0, dmtUNIQUEID, "pnt");
	else
		dm =  Domain(fnDom);
  DomainValueRangeStruct dvrs(dm,vr);
  PointMap map(fn, cs, cb, dvrs);
	if ("uniqueid" == fnDom.sFile.toLower()) {
	  FileName fnAtt(fn, ".tbt", true);
		Table	tblAtt = Table(fnAtt, dm);
		Column col = Column(tblAtt, "Name", Domain("String"));
		map->SetAttributeTable(tblAtt);
		tblAtt->fUpdateCatalog = false;
	}
  map->sDescription = sDescr;
  map->Store();
  return 0;
}






