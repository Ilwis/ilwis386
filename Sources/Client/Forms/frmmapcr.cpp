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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapcr.cpp $
 * 
 * 25    16-06-05 13:45 Retsios
 * For assisting the form creation of the new application maplistcalc, now
 * initMapOut correctly labels the output as "Output Maplist" instead of
 * "Output Map Name" in case the output is a maplist. The boolean for
 * distinguishing between output map and output maplist must be set
 * beforehand. To preserve the original behavior, the exec function is
 * left intact (this function resets the boolean that indicates the output
 * type to "Map").
 * 
 * 24    7-09-01 18:41 Koolhoven
 * removed stImage in FormMapCreate, because it is no longer needed (image
 * is now just a normal domain with fixed range and precision), and it
 * disturbes the layout
 * 
 * 23    8-08-01 10:21 Koolhoven
 * SetDomainValueRangeStruct() on MapList now sets the member dvrsMaps in
 * MapListVirtual which is used during the freezing operation
 * 
 * 22    7-08-01 19:46 Koolhoven
 * added fOutMapList which can be set by the derived exec() function to
 * indicate that the output object should be a maplist instead
 * 
 * 21    13-11-00 18:08 Koolhoven
 * in case of an invalid domain SetRemarkOnBytesPerPixel() will now tell
 * so, instead of giving an uninitialized nr of bytes per pixel
 * 
 * 20    30-10-00 14:02 Koolhoven
 * In InitMapOut() FieldGeoRefC has now root as parent instead of fmc,
 * this ensures that the CheckData() is correctly called
 * 
 * 19    30/08/00 16:51 Willem
 * The valueRange for domainImage is now set to the proper values in the
 * create domain form (after clicking the domain create button)
 * 
 * 18    25/08/00 17:38 Willem
 * FormMapCreate now has a GeoRef member (used in Resample amongst others)
 * 
 * 17    25/08/00 14:54 Willem
 * The description of the GeoRef will now also be displayed in all raster
 * forms directly below the GeoRef field
 * 
 * 16    5-07-00 9:36 Koolhoven
 * Application Forms now use Show and Define buttons instead of OK
 * 
 * 15    2/02/00 15:28 Willem
 * Also pass the ValueRange in the application form to the domain create
 * form
 * 
 * 14    28-01-00 17:15 Hendrikse
 * changed in initRemark 50 to 60:   String sFill('x', 60);
 * 
 * 13    6-12-99 10:20 Wind
 * adpated to use of grTypes for GeoRefField
 * 
 * 12    30-11-99 10:30a Martin
 * added function for setting commandline
 * 
 * 11    22-11-99 3:12p Martin
 * put command on the commandline
 * 
 * 10    30-09-99 15:22 Koolhoven
 * Application Forms now use the correct icon
 * 
 * 9     9/13/99 12:35p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
 * 
 * 8     9/13/99 9:34a Wind
 * support of quoted file names
 * 
 * 7     9/08/99 12:10p Wind
 * 
 * 6     3-09-99 11:52a Martin
 * changed default string length
 * 
 * 5     5-03-99 12:16 Koolhoven
 * Corrected header comments
 * 
 * 4     3/04/99 10:16a Martin
// Revision 1.10  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.9  1997/09/10 16:17:06  Wim
// DomainCallBack() now also reports bytes per pixel as remark
// by calling SetRemarkOnBytesPerPixel()
//
// Revision 1.8  1997-09-10 16:02:13+02  Wim
// Only create a initRemark() in initMapOut(bool, dmTypes) when needed
//
// Revision 1.7  1997-09-10 10:30:04+02  Wim
// Only show fvr and stImage when the output domain can be a dmVALUE.
// Protected the callbacks against fvr=0 or stImage=0.
//
// Revision 1.6  1997-08-26 16:06:44+02  Wim
// Added initRemark()
//
// Revision 1.5  1997-08-20 21:25:12+02  Wim
// Enlarged stRemark
//
/* FormMapCreate
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   10 Sep 97    6:13 pm
*/
#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "Client\ilwis.h"
#include "Client\FormElements\flddom.h"

FormMapCreate::FormMapCreate(CWnd* mwin, const String& sTitle)
: FormGeneralApplication(mwin, sTitle), fgr(0), fdc(0), fvr(0),
  mw(mwin), 
  vr(0,100,1),
  stRemark(0),
	fOutMapList(false)
{
	int iImg = IlwWinApp()->iImage("ExeMap16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
}                    


void FormMapCreate::initMapOut(bool fAskGeoRef, bool fAskDomain)
{
  initMapOut((long)(fAskGeoRef ? 0xFFFFFFFF : 0L), (long)(fAskDomain ? 0xFFFFFFFF : 0L));
}

void FormMapCreate::initMapOut(bool fAskGeoRef, long dmTypes)
{
  initMapOut((long)(fAskGeoRef ? 0xFFFFFFFF : 0L), dmTypes);
}

void FormMapCreate::initMapOutGeoRef3D()
{
  fmc = new FieldMapCreate(root, SAFUiOutRasMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
  FormEntry* fe = fmc;
  fe = new FieldGeoRef3DC(fmc, SAFUiGeoRef, &sGeoRef);
  fe->Align(fmc, AL_UNDER);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->Align(fe, AL_UNDER);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
}

void FormMapCreate::initMapOut(long grTypes, long dmTypes)
{
	if (fOutMapList)
		fmc = new FieldMapCreate(root, "&Output MapList", &sOutMap);
	else
		fmc = new FieldMapCreate(root, SAFUiOutRasMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
  FormEntry* fe = fmc;
  if (0 != grTypes) {
    if (-1 == grTypes)
      grTypes = grALL | grNONONE | grNO3D;
    fgr = new FieldGeoRefC(root, SAFUiGeoRef, &sGeoRef, grTypes);  
	fgr->SetCallBack((NotifyProc)&FormMapCreate::ChangeGeoRef);
    fgr->Align(fmc, AL_UNDER);
	String s('X', 50);
	stGRDesc = new StaticText(root, s);
	stGRDesc->SetVal(String());
	stGRDesc->SetIndependentPos();
	
    fe = stGRDesc;
  }  
  if (0 != dmTypes) {
    if (-1 == dmTypes)
      dmTypes = dmCLASS|dmIDENT|dmVALUE|dmIMAGE|dmPICT|dmCOLOR|dmBOOL|dmBIT;
    fdc = new FieldDomainC(root, SAFUiDomain, &sDomain, dmTypes);
    fdc->Align(fe, AL_UNDER);
    fe = fdc;
    if (dmTypes & dmVALUE) {
      fdc->SetCallBack((NotifyProc)&FormMapCreate::DomainCallBack);
      fvr = new FieldValueRange(root, SAFUiValRange, &vr, fdc);
      fvr->SetCallBack((NotifyProc)&FormMapCreate::ValueRangeCallBack);
      fvr->Align(fdc, AL_UNDER);
      fe = fvr;
    }
  }  
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->Align(fe, AL_UNDER);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  if (dmTypes & dmVALUE)
    initRemark();
}

void FormMapCreate::initMapOutValRange(bool fAskGeoRef)
{
  fmc = new FieldMapCreate(root, SAFUiOutRasMap, &sOutMap);
  fmc->SetCallBack((NotifyProc)&FormMapCreate::OutMapCallBack);
  if (fAskGeoRef) {
    fgr = new FieldGeoRefC(root, SAFUiGeoRef, &sGeoRef);  
	fgr->SetCallBack((NotifyProc)&FormMapCreate::ChangeGeoRef);
    fgr->Align(fmc, AL_UNDER);
	String s('X', 50);
	stGRDesc = new StaticText(root, s);
	stGRDesc->SetVal(String());
	stGRDesc->SetIndependentPos();
  }  
  else {
    FieldBlank* fb = new FieldBlank(root, 0);
    fb->Align(fmc, AL_UNDER);
  }
  fvr = new FieldValueRange(root, SAFUiValRange, &vr, 0);
  fvr->SetCallBack((NotifyProc)&FormMapCreate::ValueRangeCallBack);
  StaticText* st = new StaticText(root, SAFUiDescription);
  st->psn->SetBound(0,0,0,0);
  FieldString* fs = new FieldString(root, "", &sDescr);
  fs->SetWidth(120);
  fs->SetIndependentPos();
  initRemark();
}

int FormMapCreate::OutMapCallBack(Event*)
{
  fmc->StoreData();
  // later check on existance etc.  
  return 0;
}

int FormMapCreate::ChangeGeoRef(Event*)
{
	fgr->StoreData();
	if (sGeoRef.length() > 0)
	{
		m_gr = GeoRef(sGeoRef);
		stGRDesc->SetVal(m_gr->sGetDescription());
	}
	
	return 0;
}

int FormMapCreate::DomainCallBack(Event*)   
{
  fdc->StoreData();
  if (sDomain.length() == 0) {
    if (fvr)
      fvr->Hide();
    SetRemarkOnBytesPerPixel();
    return 0;
  }  
  FileName fn(sDomain);
  Domain dm;
  try { 
    dm = Domain(fn); 
    if (dm.fValid() && fvr)
      fvr->DomainCallBack(0);
  }    
  catch (const ErrorObject&) {
    SetRemarkOnBytesPerPixel();
    return 0;
  }
  SetRemarkOnBytesPerPixel();
  return 0;
}

int FormMapCreate::ValueRangeCallBack(Event*)
{
  if (0 == fvr)
    return 0;
  fvr->StoreData();
	if (fdc != 0)
		fdc->SetValueRange(vr);
  SetRemarkOnBytesPerPixel();
  return 0;
}

void FormMapCreate::SetRemarkOnBytesPerPixel()
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
					default:
				    stRemark->SetVal(SAFRemInvalidDomain);
						return;
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
}
   
int FormMapCreate::exec()
{
  FormGeneralApplication::exec();
  FileName fn(sOutMap);
  FileName fnDomain(sDomain); 
  sDomain = fnDomain.sRelativeQuoted(true,fn.sPath());
  FileName fnGeoRef(sGeoRef); 
  sGeoRef = fnGeoRef.sRelativeQuoted(true,fn.sPath());
  fOutMapList = false;
  return 1;
}

void FormMapCreate::execMapOut(const String& sExpr)
{
  FileName fn(sOutMap);
  fn.sExt = ".mpr";
  if (fOutMapList)
    fn.sExt = ".mpl";
  if (fn.fExist()) {
    String sErr(SAFMsgAlreadyExistsOverwrite_S.scVal(), fn.sFullPath(true));
    int iRet=mw->MessageBox(sErr.scVal(), SAFMsgAlreadyExists.scVal(), MB_YESNO|MB_ICONEXCLAMATION);
    if (iRet != IDYES)
      return;
  }
  String sCmd = fn.sShortNameQuoted();
  DomainValueRangeStruct dvs;
  if (sDomain[0]) {
    try {
      FileName fnDomain(sDomain, ".dom", false);
      Domain dm(fnDomain);
      dvs = DomainValueRangeStruct(dm, vr);
    }
    catch (ErrorObject& err) {
      err.Show();
    }
    if (!fOutMapList)
      if (dvs.vr().fValid())
        sCmd &= String("{dom=%S;vr=%S}", sDomain, dvs.vr()->sRange());
      else
        sCmd &= String("{dom=%S}", sDomain);
  }
  sCmd &= " = ";
  sCmd &= sExpr;
  
  SetCommand(sCmd);
  String sExec;
  try {
    if (!fOutMapList)
    {
      Map mpOut(fn, sExpr);
      if (!mpOut.fValid())
        return;
      if (sDomain[0]) {
        Domain dm = dvs.dm();
        mpOut->fMergeDomainForCalc(dm, sExpr);
        mpOut->SetDomainValueRangeStruct(dvs);
      }
      if (sDescr[0])
        mpOut->sDescription = sDescr;
      mpOut->Store(); // destructor of mpOut could be called later than opening map
      if (fShow) {
        sExec = "show " + mpOut->sNameQuoted(true);
        IlwWinApp()->Execute(sExec);
      }
    }
    else 
    {
      MapList mplOut(fn, sExpr);
      if (!mplOut.fValid())
        return;
      mplOut->SetDomainValueRangeStruct(dvs);
      if (sDescr[0])
        mplOut->sDescription = sDescr;
      mplOut->Store(); // destructor of mplOut could be called later than opening map
      if (fShow) {
        sExec = "show " + mplOut->sNameQuoted(true);
        IlwWinApp()->Execute(sExec);
      }
    }
  }
  catch (ErrorObject& err) {
    err.Show();
    return;
  }  
}

void FormMapCreate::SetDefaultValueRange(const ValueRange& vr)
{
  if (vr.fValid() && fvr)
    fvr->SetVal(vr);
}

void FormMapCreate::initRemark()
{
  if (0 != stRemark)
    return;
  String sFill('x', 60);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();
}
