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
/* $Log: /ILWIS 3.0/Main/Defform.cpp $
 * 
 * 22    26-10-01 13:59 Koolhoven
 * constructor of MapDefForm got extra parameter fBreakDep to indicate to
 * show a Calculate button instead of a Define button
 * 
 * 21    19-03-01 18:18 Koolhoven
 * MapDefForm::SetDefaults() now also uses extension of domain
 * 
 * 20    2/24/01 15:21 Retsios
 * Comment out unnecessary code to repair memory leak
 * 
 * 19    16-10-00 13:54 Koolhoven
 * by default the defform OK (enter) means Show
 * 
 * 18    10-10-00 15:54 Koolhoven
 * in CallBackDomainChange(): if no Domain set set stDomain on empty
 * string
 * 
 * 17    5-10-00 13:02 Koolhoven
 * MapDefForm will now show on definition time the options Show and Define
 * instead of only OK (define)
 * 
 * 16    14/09/00 11:53 Willem
 * The width of the description field is now set at a fixed length
 * 
 * 15    24/08/00 18:20 Willem
 * Display object name at the top in R/O FieldString to allow very long
 * filenames to be scrolled
 * 
 * 14    24/08/00 14:18 Willem
 * The name of the map is now displayed in a R/O FieldString 
 * 
 * 13    23/08/00 10:07 Willem
 * The StaticText used to display domain text has now more room for the
 * text
 * 
 * 12    17-07-00 3:00p Martin
 * ThreadLocasl set and removed
 * 
 * 11    11-07-00 8:14a Martin
 * illegal domains are not set as default choice
 * 
 * 10    25-01-00 13:11 Koolhoven
 * Command "editdef" in commadhandler
 * Button "Edit Definition" in PropertyForm
 * 
 * 9     14-01-00 17:58 Koolhoven
 * Ask for domain name with extension in DefForm
 * 
 * 8     29/11/99 15:31 Willem
 * The expression is now displayed in a R/O FieldString with the static
 * text 'Expression' in front
 * 
 * 7     26/11/99 12:14 Willem
 * Repaired the expression; it was improperly set to include "Expression:
 * ", causing any application to fail to run
 * 
 * 6     23/11/99 11:34 Willem
 * Map Expression is now displayed in a read-only FieldString with a
 * limited width. This allows long expressions to be scrolled and copied
 * to clipboard
 * 
 * 5     9-09-99 14:57 Koolhoven
 * 2.22 Change:
 * prevent too long text in MapDefForm for expression
 * 
 * 4     4-06-99 17:18 Koolhoven
 * 
 * 3     5/25/99 3:37p Martin
 * 
 * 2     5/19/99 3:32p Martin
 * changes for merging commandline dll with main dll
 * 
 * 1     5/19/99 1:02p Martin
 * 
 * 2     3/08/99 2:08p Martin
 * New filed added
 * 
 * 1     3/08/99 11:34a Martin
// Revision 1.8  1998/02/09 09:36:36  Wim
// Call ValueRangeCallBack(0) in DomainCallBack()
// this solves the problem that selecting "image" as domain did
// not change the remark about the number of bytes per pixel
//
// Revision 1.7  1997-09-17 15:17:48+01  Wim
// Check in feDefaultFocus() on domain bool
//
// Revision 1.6  1997-09-16 19:46:30+02  Wim
// Protected against frValueRange==0
//
// Revision 1.5  1997-09-10 18:49:14+02  Wim
// Log message
//
/* DefForm
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   17 Sep 97    4:14 pm
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\fentvalr.h"
#include "Applications\Raster\MAPCALC.H"
#include "Engine\Applications\POLVIRT.H"
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Domain\dm.h"
#include "Client\Forms\DEFFORM.H"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Forms\PropForm.h"
#include "Client\ilwis.h"

UINT ShowDefForm(LPVOID lpObjectStruct)
{
	::OleInitialize(NULL);
	CWinThread* thr = AfxGetThread();
	thr->m_pMainWnd = 0;

	ObjectStruct* os = (ObjectStruct*)lpObjectStruct;
	IlwisObject& obj = os->obj;
	FileName fn = obj->fnObj;
	IlwWinApp()->Context()->InitThreadLocalVars();
	IlwWinApp()->SetCurDir(fn.sPath());

	MapPtr* mpptr = dynamic_cast<MapPtr*>(obj.pointer());
	if (mpptr) {
		Map mp;
		mp.SetPointer(mpptr);
    MapDefForm frm(IlwWinApp()->GetMainWnd(), mp, false, false);
	}
	::OleUninitialize();
	delete os;
	IlwWinApp()->Context()->RemoveThreadLocalVars();
	return 0;
}

BEGIN_MESSAGE_MAP(ObjectDefForm, FormBaseDialog)
  ON_COMMAND(IDYES, OnShow)
  ON_COMMAND(IDNO, OnDefine)
END_MESSAGE_MAP()    

ObjectDefForm::ObjectDefForm(CWnd* wPar, const IlwisObject& obj, const String& sTitle)
  : FormWithDest(wPar, sTitle)
{
	//zFontInfo *info;
	//info = new zFontInfo(windsp());
	String s = obj->sTypeName();
	FieldString *fs = new FieldString(root, &s, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	zDimension dim = fs->Dim(s);
	fs->SetWidth(180);
	fs->SetIndependentPos();
	HideOnOk(true);
	fShow = true;
}

void ObjectDefForm::OnShow()
{
	fShow = true;
	OnOK();
}

void ObjectDefForm::OnDefine()
{
	fShow = false;
	OnOK();
}

MapDefForm::MapDefForm(CWnd* wPar, Map& _mp, bool fShowButtons, bool fBreakDep)
: ObjectDefForm(wPar, _mp, SMSTitleRasMapDef),
	mp(_mp), stDomain(0), fdm(0), fgr(0),
	stRemark(0), frValueRange(0)
{
	fsExpr = 0;
	sExpr = mp->sExpression();
	Domain dm = mp->dm();
	sDomain = dm->sName(true);
	if ( fCIStrEqual(sDomain, "String.dom")) // illegal domain of mp, intentionally, to force a domain chocie
		sDomain = "";
	vr = mp->dvrs().vr();
	FormEntry* feLast;
	if (mp->fExpressionChangeable()) {
		fSetDescFromExpr = sExpr == mp->sDescription;
		
		fsExpr = new FieldString(root, SMSUiExpr, &sExpr);
		fsExpr->SetWidth(120);
		fsExpr->SetIndependentPos();
		fdm = new FieldDomainC(root, SMSUiDomain, &sDomain,
			dmCLASS|dmIDENT|dmVALUE|dmGROUP|dmIMAGE|dmPICT|dmBOOL|dmBIT|dmCOLOR);
		fdm->SetCallBack((NotifyProc)&MapDefForm::CallBackDomainChange);
		String sFill('X', 50);
		stDomain = new StaticText(root, sFill);
		stDomain->SetIndependentPos();
		feLast = stDomain;
		PushButton* pb = new PushButton(root, SMSUiDefaults, 
			(NotifyProc)&MapDefForm::SetDefaults);
		pb->SetIndependentPos();
		pb->Align(fdm, AL_AFTER);
	}
	else {
		FormEntry* fs = new FieldString(root, SMSUiExpr, &sExpr, Domain(), true, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
		fs->SetWidth(120);
		fs->SetIndependentPos();
		StaticText *st;
		if (sDomain.length() > 0) {
			String sDomText = dm->sTypeName();
			st = new StaticText(root, sDomText);
			st->SetIndependentPos();
			if (sDomText != dm->sDescription) {
				st = new StaticText(root, dm->sDescription);
				st->SetIndependentPos();
			}
		}
		feLast = st;
	}  
	
	bool fValueRangeChange = fdm || mp->fValueRangeChangeable();
	if (0 == fdm)
		if (0 == dm->pdvi() && 0 == dm->pdvr())
			fValueRangeChange = false;
		if (fValueRangeChange) {
			if (0 == fdm) // to show/hide the correct ones
				feLast->SetCallBack((NotifyProc)&MapDefForm::CallBackDomainChange);
			if (mp->vr().fValid())
				vr = mp->vr();
			frValueRange = new FieldValueRange(root, SMSUiRange, &vr, fdm);
			frValueRange->SetCallBack((NotifyProc)&MapDefForm::ValueRangeCallBack);
			frValueRange->Align(feLast, AL_UNDER);  
			feLast = frValueRange;  
		}
		
	if (mp->fGeoRefChangeable()) {
		sGeoRef = mp->gr()->sName();
		fgr = new FieldGeoRefExisting(root, SMSUiGrf, &sGeoRef);
		fgr->SetCallBack((NotifyProc)&MapDefForm::CallBackGeoRefChange); 
		String sFill('X', 50);
		stGeoRef = new StaticText(root, sFill);
		stGeoRef->SetIndependentPos();
		feLast = stGeoRef;  
	}
	
	fsDesc = new FieldString(root, SMSUiDesc, &mp->sDescription);
	if (feLast != frValueRange)
		fsDesc->Align(feLast, AL_UNDER);
	fsDesc->SetWidth(120);
	fsDesc->SetIndependentPos();
	
	if (frValueRange) {  
		String sFill('X', 50);
		stRemark = new StaticText(root, sFill);
		stRemark->SetIndependentPos();
	}  
	
	SetMenHelpTopic(htpDefMap);

  if (fShowButtons) {
		fbs	|= fbsAPPLIC;
    if (fBreakDep)
  		fbs	|= fbsCALC;
  }
		
	create();
}

MapDefForm::~MapDefForm()
{
}

FormEntry* MapDefForm::feDefaultFocus()
{
  if (fdm || mp->fValueRangeChangeable()) {
    if (mp->vr().fValid() && 0!=frValueRange && 0==mp->dm()->pdbool())
      frValueRange->SetVal(mp->vr());
  }
  return fsDesc;
//  return FormBase::feDefaultFocus();
}
    
int MapDefForm::CallBackDomainChange(Event*)
{
  if (fdm) {
    fdm->StoreData();
    if (sDomain.length() == 0) {
      if (frValueRange)
        frValueRange->Hide();
			stDomain->SetVal("");
      return 0;
    }  
    FileName fn(sDomain, ".dom", false);
    Domain dm;
    try {
      dm = Domain(fn);
      if (dm.fValid())
        if (frValueRange)
          frValueRange->DomainCallBack(0);
    }
    catch (ErrorObject& err) {
      err.Show(SMSErrDomChange);
    }  
    if (!dm.fValid()) {
      stDomain->SetVal(SMSRemInvalidDomain);
      if (frValueRange)
        frValueRange->Hide();
      return 1;
    }
    stDomain->SetVal(dm->sDescription);
    ValueRangeCallBack(0);
  }
  else {  
    if (frValueRange)
      frValueRange->Show();
  }  
  return 0;
}

int MapDefForm::ValueRangeCallBack(Event*)
{
  if (0 == frValueRange)
    return 0;
  if (sDomain.length() == 0) {
    stRemark->SetVal("");
    return 0;
  }  
  FileName fn(sDomain);
  try { 
    frValueRange->StoreData();
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
        s = String(SAFRemBits_i.scVal(), iNr);
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
        s = String(SAFRemBytes_i.scVal(), iNr);
      }  
    }
    String sRemark(SAFRemMapPixUse_S.scVal(), s);
    stRemark->SetVal(sRemark);
  }
  catch (ErrorObject&) 
  {
    stRemark->SetVal(SAFRemInvalidDomain);
  }  
  return 0;
}
   
int MapDefForm::SetDefaults(Event*)
{
  fsExpr->StoreData();
  try {
    DomainValueRangeStruct dvrs = mp->dvrsDefault(sExpr);
    if (!dvrs.dm().fValid())
      return 0;
    sDomain = dvrs.dm()->sName(true);
    fdm->SetVal(sDomain);
    CallBackDomainChange(0);
    ValueRange vr = dvrs.vr();
    if (vr.fValid()) {
      frValueRange->SetVal(vr);
    }
  }
  catch (const ErrorObject& err) {
    err.Show();
  }
  if (fSetDescFromExpr)
    fsDesc->SetVal(sExpr);
  return 0;
}

int MapDefForm::CallBackGeoRefChange(Event*)
{
  fgr->StoreData();
  FileName fn(sGeoRef);
  fn.sExt = ".grf";
  String sRemark;
  GeoRef grf;
  try {
    grf = GeoRef(fn);
    sRemark = grf->sDescription;
  }
  catch (ErrorObject&) {
    sRemark = SMSRemInvalidGrf;
  }  
  stGeoRef->SetVal(sRemark);
  return 1;
}

int MapDefForm::exec()
{
  ObjectDefForm::exec();
  Domain dm;
  if (fdm) {
    FileName fnDom(sDomain);
    dm = Domain(fnDom);
  }
  if (!mp->fMergeDomainForCalc(dm, sExpr))
    return 0;
  String sOld = mp->sExpression();
  if (sOld != sExpr) {
    mp->Replace(sExpr);
  }
  if (dm.fValid())
    mp->SetDomainValueRangeStruct(dm);

  if (mp->fValueRangeChangeable()) {
    Domain dm = mp->dm();
    if (dm->pdbit() || (dm->fnObj.sFile == "bool")) 
      ; // do nothing
    else if (dm->pdvi()) {
      mp->SetValueRange(vr);
    }  
    else if (dm->pdvr())  {
      mp->SetValueRange(vr);
    }
  }    
    
  if (fgr) {
    try {
      GeoRef grNew(sGeoRef);
      mp->SetGeoRef(grNew);
      if (!grNew->fGeoRefNone())
        mp->SetSize(grNew->rcSize());
    }    
    catch (ErrorObject&) {
    }
  }
    
  mp->Store();
  return 0;
}    
  
SegmentMapDefForm::SegmentMapDefForm(CWnd* wPar, SegmentMap& _mp)
  : ObjectDefForm(wPar, _mp, SMSTitleSegMapDef),
  mp(_mp)
{
  String sExpr = mp->sExpression();
  Domain dm = mp->dm();
  String sDomain = dm->sName();
  String s = String(SMSUiExpr_s.scVal(), sExpr);
  StaticText* st = new StaticText(root, s);
  st->SetIndependentPos();
  String sDomText = dm->sTypeName();
  st = new StaticText(root, sDomText);
  st->SetIndependentPos();
  st = new StaticText(root, dm->sDescription);
  
  FieldString* fs = new FieldString(root, SMSUiDesc, &mp->sDescription);
  fs->Align(st, AL_UNDER);
  fs->SetWidth(120);
  fs->SetIndependentPos();
    
  SetMenHelpTopic(htpDefSegMap);
  create();
}
    
PolygonMapDefForm::PolygonMapDefForm(CWnd* wPar, PolygonMap& _mp)
  : ObjectDefForm(wPar, _mp, SMSTitlePolMapDef),
  mp(_mp)
{
  String sExpr = mp->sExpression();
  Domain dm = mp->dm();
  String sDomain = dm->sName();
  String s = String(SMSUiExpr_s.scVal(), sExpr);
  StaticText* st = new StaticText(root, s);
  st->SetIndependentPos();
  String sDomText = dm->sTypeName();
  st = new StaticText(root, sDomText);
  st->SetIndependentPos();
  st = new StaticText(root, dm->sDescription);
  
  FieldString* fs = new FieldString(root, SMSUiDesc, &mp->sDescription);
  fs->Align(st, AL_UNDER);
  fs->SetWidth(120);
  fs->SetIndependentPos();
    
  SetMenHelpTopic(htpDefPolMap);
  create();
}
    
PointMapDefForm::PointMapDefForm(CWnd* wPar, PointMap& _mp)
  : ObjectDefForm(wPar, _mp, SMSTitlePntMapDef),
  mp(_mp)
{
  String sExpr = mp->sExpression();
  Domain dm = mp->dm();
  String sDomain = dm->sName();
  String s = String(SMSUiExpr_s.scVal(), sExpr);
  StaticText* st = new StaticText(root, s);
  st->SetIndependentPos();
  String sDomText = dm->sTypeName();
  st = new StaticText(root, sDomText);
  st->SetIndependentPos();
  st = new StaticText(root, dm->sDescription);
  
  FieldString* fs = new FieldString(root, SMSUiDesc, &mp->sDescription);
  fs->Align(st, AL_UNDER);
  fs->SetWidth(120);
  fs->SetIndependentPos();
    
  SetMenHelpTopic(htpDefPntMap);
  create();
}
    
TableDefForm::TableDefForm(CWnd* wPar, const Table& tb)
  : ObjectDefForm(wPar, tb, SMSTitleTableDef),
  tbl(tb)
{
  String sExpr = tbl->sExpression();
  Domain dm = tbl->dm();
  String sDomain = dm->sName();
  String s = String(SMSUiExpr_s.scVal(), sExpr);
  StaticText* st = new StaticText(root, s);
  st->SetIndependentPos();
  String sDomText = dm->sTypeName();
  st = new StaticText(root, sDomText);
  st->SetIndependentPos();
  st = new StaticText(root, dm->sDescription);
  
  FieldString* fs = new FieldString(root, SMSUiDesc, &tbl->sDescription);
  fs->Align(st, AL_UNDER);
  fs->SetWidth(120);
  fs->SetIndependentPos();
    
  SetMenHelpTopic(htpDefTable);
  create();
}

/*
MatrixObjectDefForm::MatrixObjectDefForm(CWnd* wPar, MatrixObject& mt)
  : ObjectDefForm(wPar, mt, SMSTitleMatrixDef),
  mat(mt)
{
  String sExpr = mat->sExpression();
  Domain dm = mp->dm();
  String sDomain = dm->sName();
  s = String(SMSUiExpr_s, sExpr);
  st = new StaticText(root, s);
  st->SetIndependentPos();
  String sDomText = dm->sTypeName();
  st = new StaticText(root, sDomText);
  st->SetIndependentPos();
  st = new StaticText(root, dm->sDescription);
  
  FieldString* fs = new FieldString(root, SMSUiDesc, &mp->sDescription);
  fs->Align(st, AL_UNDER);
  fs->SetWidth(120);
  fs->SetIndependentPos();
    
  SetMenHelpTopic(htpDefPntMap);
  create();
}
*/
    
