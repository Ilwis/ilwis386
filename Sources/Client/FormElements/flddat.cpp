/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\engine.h"
#include "Client\FormElements\flddat.h"
#include "Client\FormElements\objlist.h"
#include "Client\ilwis.h"
#include "Headers\Hs\Userint.hs"

enum ObjectType { otMAPS = 0, otMPR, otMPA, otMPS, otMPP, otTBT,
                  otMPL, otIOC, otMPV, otILO, otATX, otGRH,
                  otDOM, otRPR, otGRF, otCSY,
                  otHIS, otHSA, otHSS, otHSP,
                  otSMS, otSTP, otSMC,
                  otTA2, otMAT, otFIL, otFUN, otISL,
                  otALL };

FieldDataTypeLarge::FieldDataTypeLarge(FormEntry* par,
                   String *psName, const String& sExt, bool fIcns)
: FormEntry(par,0,true),
  idw(false), fIcons(fIcns), ol(0),
  fOnlyEditable(false), fAlsoSystemDir(true), fromBaseMaps(false)
{
  st = 0;
  lbObject = 0;
  lbDir = 0;
  cbDrive = 0;
  _psName = psName;
	FileName fn(*psName);
  sName = fn.sFile;
  sDir = fn.sPath();
  SetCurrentDirectory(sDir.c_str());

  psn->iMinWidth = 2*FLDNAMEOUTPUTWIDTH + 20;
  psn->iMinHeight = psn->iHeight = 200;
  SetExt(sExt);
}

FieldDataTypeLarge::FieldDataTypeLarge(FormEntry* par,
                   String *psName, const String& sExt, ObjectLister* objl, bool fIcns)
: FormEntry(par,0,true),
  idw(false), fIcons(fIcns), ol(objl),
  fOnlyEditable(false), fAlsoSystemDir(true), fromBaseMaps(false)
{
  st = 0;
  lbObject = 0;
  lbDir = 0;
  cbDrive = 0;
  _psName = psName;
	FileName fn(*psName);
  sName = fn.sFile;
  sDir = fn.sPath();
  SetCurrentDirectory(sDir.c_str());

  psn->iMinWidth = 2*FLDNAMEOUTPUTWIDTH + 20;
  psn->iMinHeight = psn->iHeight = 200;
  SetExt(sExt);
}

FieldDataTypeLarge::~FieldDataTypeLarge()
{
  for (int i = 0; i < asExt.iSize(); ++i)
    delete asExt[i];
  delete st;
  delete lbObject;
  delete lbDir;
  delete cbDrive;
  delete ol;
}

void FieldDataTypeLarge::create()
{
	CPoint pnt = zPoint(psn->iPosX,psn->iPosY);
	zDimension dim = zDimension(psn->iWidth,psn->iHeight);
	zDimension dimSt = dim;
	dimSt.height() = YDIALOG(20);
	String sFill('x', 100);
	st = new CStatic();
	st->Create(sFill.sVal(), WS_CHILD, CRect(pnt, dimSt), frm()->wnd());
	st->SetFont(frm()->fnt);
	pnt.y += dimSt.height() + 1;
	bool fDirOnly = 0 == asExt.iSize();
	if (!fDirOnly)
		dim.width() = psn->iWidth / 2 - 10;
	dim.height() -= dimSt.height() + 1;
	DWORD styleCommon = LBS_NOTIFY | WS_VSCROLL | WS_HSCROLL | LBS_DISABLENOSCROLL |
		LBS_HASSTRINGS | LBS_SORT | WS_BORDER | WS_TABSTOP;
	DWORD style = styleCommon;
	if (fIcons)
		style |= LBS_OWNERDRAWFIXED;
	if (!fDirOnly)
	{
		lbObject = new OwnerDrawListBox(this, style, CRect(pnt, dim),frm()->wnd(), 1000);
		lbObject->SetFont(frm()->fnt);
		pnt.x += dim.width() + 20;
	}
	dim.height() -= YDIALOG(30);
	
	style = styleCommon | LBS_OWNERDRAWFIXED;
	lbDir = new OwnerDrawListBox(this, style, CRect(pnt, dim),frm()->wnd(), 1001);
	lbDir->SetFont(frm()->fnt);
	
	pnt.y += dim.height() + YDIALOG(4);
	dim.height() = YDIALOG(120);
	
	style = WS_TABSTOP | WS_VSCROLL | WS_GROUP | CBS_SORT |  CBS_AUTOHSCROLL | 
		      CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
	cbDrive = new OwnerDrawCB(this, style , CRect(pnt,dim),frm()->wnd(),1002);
	cbDrive->SetFont(frm()->fnt);
	
	if (!fDirOnly)
	{
		lbObject->setNotify(this,(NotifyProc)&FieldDataTypeLarge::DblClkObject, Notify(LBN_DBLCLK));
		if (_npChanged)
			lbObject->setNotify(_cb, _npChanged, Notify(CBN_SELCHANGE));
	}
	lbDir->setNotify(this,(NotifyProc)&FieldDataTypeLarge::DblClkDir, Notify(LBN_DBLCLK));
	cbDrive->setNotify(this, (NotifyProc)&FieldDataTypeLarge::CloseDrive, Notify(CBN_CLOSEUP));
	cbDrive->SendMessage(CB_SETEXTENDEDUI, TRUE, 0);
	FillDrive();		
	if (!fDirOnly)
	{
		FileName fn(*_psName);
		String sNam("%S%S", fn.sFile, fn.sExt);
		SelectExact(sNam);
	}
}

void FieldDataTypeLarge::useBaseMaps(bool yesno){
	fromBaseMaps = yesno;
}

// SelectExact will select the object with the filename sSearch (file.ext) by
// comparing it with the items in the list. The text stored in the listbox is used
// because this also contains the extension. The SelectString and SelectStringExact
// functions only look for the first match in the displayed strings, that do not
// contain the extension of the object.
void FieldDataTypeLarge::SelectExact(const String& sSearch)
{
	for (int i = 0; i < lbObject->GetCount(); i++)
	{
		CString s;
		lbObject->GetText(i, s);
		String sComp(s);
		if (fCIStrEqual(sSearch, sComp))
		{
			lbObject->SetCurSel(i);
			break;
		}
	}
}

void FieldDataTypeLarge::SetExt(const String& sExtensions)
{
	int i;
	for (i = 0; i < asExt.iSize(); ++i)
		delete asExt[i];
	asExt.Reset();
	int iNrExt = -1;
	for (i = 0; i < sExtensions.length(); i++)
	{
		if (sExtensions[i] == '.')
		{
			iNrExt++;
			asExt &= new String();
		}
		if (iNrExt >= 0)
			(*asExt[iNrExt]) &= sExtensions[i];
	}
	for (i = 0; i <= iNrExt; i++)
		asExt[i]->toLower();

	if (lbObject)
		Fill();
}

void FieldDataTypeLarge::CheckDrawItem(DRAWITEMSTRUCT* dis)
{
	if (dis->CtlID == 1000 || dis->CtlID == 1001 || dis->CtlID == 1002)
	{
		DrawItemEvent ev=DrawItemEvent(dis->CtlID, dis); 
		DrawItem(&ev);
	}
}

void FieldDataTypeLarge::DrawItem(Event *ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);
  int id = dis->itemID;
	if (id == -1 ) return;
  CString s;
  bool fSel;
  if (dis->CtlID == 1002) {
    cbDrive->GetLBText(id, s);
    fSel = cbDrive->GetCurSel() == id;
  }
  else {
    CListBox* lb = (dis->CtlID == 1000) ? lbObject : lbDir;
    lb->GetText(id, s);
    fSel = lb->GetCurSel() == id;
  }
}

void FieldDataTypeLarge::show(int s)
{
  if (st)
    st->ShowWindow(s);
  if (lbObject)
    lbObject->ShowWindow(s);
  if (lbDir)
    lbDir->ShowWindow(s);
  if (cbDrive)
    cbDrive->ShowWindow(s);
}


void FieldDataTypeLarge::Fill()
{
	if (0 == lbObject)
		return;

	// Remember which object is currently selected in the list
	// After rereading the list this object is selected again
	// when it is still in the list of objects
	int iSelected = lbObject->GetCurSel();
	CString sSel;
	if (iSelected != LB_ERR)
	{
		int iLen = lbObject->GetTextLen(iSelected);
		lbObject->GetText(iSelected, sSel);
	}

	// Reread the object list
	lbObject->ResetContent();
	vector<String> paths;
	if ( fromBaseMaps) {
		int count = 0;
		FileName fnBase = getEngine()->getContext()->fnGetBasemapPath(count);
		while(fnBase.sDir != "") {
			String path = fnBase.sPath();
			path = path.substr(0,path.size() - 1);
			paths.push_back(path);
			fnBase = getEngine()->getContext()->fnGetBasemapPath(++count);
		}
	} 
	else
		paths.push_back(sDir);
	for (int i=0; i < asExt.iSize(); i++)
	{
		for(int k=0;k < paths.size(); ++k) {
			String sPath = paths[k];
			String sMaskExt = *asExt[i];
			sPath &= "\\*";
			sPath &= sMaskExt;

			CFileFind finder;
			bool fWorking = finder.FindFile(sPath.c_str()) != 0;
			while (fWorking)
			{
				fWorking = finder.FindNextFile() != 0;
				if (!finder.IsDirectory())
				{
					String sFile = finder.GetFileName();
					FileName fn(sFile);
					if (sMaskExt != ".*" && !fCIStrEqual(sMaskExt, fn.sExt))
						continue;
					fn.Dir(sDir);
					if (fOnlyEditable)
						if (access(fn.sFullName().c_str(),2)!=0 || ObjectInfo::fVirtual(fn))
							continue;
					if (ol)
						if (!ol->fOK(fn))
							continue;
    				lbObject->AddString(sFile.c_str());
				}
			}
			finder.Close();
		}

	}
	if (!fOnlyEditable && fAlsoSystemDir)
	{
		String sStdDir = IlwWinApp()->Context()->sStdDir();
		String sStdPath = sStdDir;
		sStdPath &= "\\*";
		for (int i=0; i < asExt.iSize(); i++)
		{
			String sPath = sStdPath;
			sPath &= *asExt[i];
			CFileFind finder;
			bool fWorking = finder.FindFile(sPath.c_str()) != 0;
			while (fWorking)
			{
				fWorking = finder.FindNextFile() != 0;
				if (!finder.IsDirectory())
				{
					String sFile = finder.GetFileName();
					FileName fn(sFile);
					fn.Dir(sStdDir);
					if (ol)
						if (!ol->fOK(fn))
							continue;
    				lbObject->AddString(sFile.c_str());
				}
			}
			finder.Close();
		}  
	}
	
	// Find the longest string in the list box.
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = lbObject->GetDC();
	CFont* fntOld = pDC->SelectObject(frm()->fnt);
	for (int i=0; i < lbObject->GetCount(); i++)
	{
		lbObject->GetText( i, str );
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	sz = pDC->GetTextExtent("x"); // extra char, because GetTextExtent underestimates
	pDC->SelectObject(fntOld);
	lbObject->ReleaseDC(pDC);
	
	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	lbObject->SetHorizontalExtent(dx + sz.cx);

	// Try to reselect to remembered object (if any). If this
	// fails disable any selection
	if (iSelected != LB_ERR && sSel.GetLength() > 0)
		SelectExact(String(sSel));
	else
		lbObject->SetCurSel(-1);
}

void FieldDataTypeLarge::FillDir()
{
  String s = TR("Directory ");
	String sTemp = sDir;
	sTemp.toLower();

  if (sTemp.length() > 60) 
	{
    s &= "...";
		s &= sTemp.sRight(60);
  }  
  else
    s &= sTemp;
  st->SetWindowText(s.c_str());

  SetCurrentDirectory(sDir.sVal());
  Fill();
  lbDir->ResetContent();

	int iColon = sDir.find(":\\");
	if (iColon != string::npos && iColon != sDir.length() - 2) // so current dir is not a root directory
   	lbDir->AddString("[..]");
	CFileFind dirfinder;
	bool fWorking = dirfinder.FindFile("*.*") != 0;
	while (fWorking)
	{
		fWorking = dirfinder.FindNextFile() != 0;
		if (dirfinder.IsDirectory() && !dirfinder.IsDots())
		{
			String sDir = String("[%s]", dirfinder.GetFileName());
    	lbDir->AddString(sDir.c_str());
		}
	}
	dirfinder.Close();

	// Find the longest string in the list box.
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = lbDir->GetDC();
	CFont* fntOld = pDC->SelectObject(frm()->fnt);
	for (int i=0; i < lbDir->GetCount(); i++)
	{
		lbDir->GetText( i, str );
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	sz.cx = 20; // h-size of an icon
	pDC->SelectObject(fntOld);
	lbDir->ReleaseDC(pDC);
	
	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	lbDir->SetHorizontalExtent(dx + sz.cx);

	lbDir->SetCurSel(0);
}

void FieldDataTypeLarge::FillDrive()
{
  FillDir();
  cbDrive->ResetContent();
  cbDrive->Dir(DRIVES|ONLY,"*.*");
  String sDrv = "[-x-]";
  sDrv[2] = sDir[0];
  cbDrive->SelectString(-1, sDrv.c_str());
}

FormEntry* FieldDataTypeLarge::CheckData()
{
  if (0 == lbObject)
    return 0;
  int id = lbObject->GetCurSel();
  if (id >= 0)
    return 0;
  else
    return this;
}

void FieldDataTypeLarge::StoreData()
{
  FileName fn = "nul";
  if (lbObject) {
    int id = lbObject->GetCurSel();
    if (id < 0) 
		{
			sName = String();
			*_psName = sName;
			return;
		}
    CString s;
    lbObject->GetText(id, s);
    fn = String(s);
  }
  if ( !fromBaseMaps)
	fn.Dir(sDir);
  else {
		String sStdDir = IlwWinApp()->Context()->sStdDir() + "\\Basemaps\\";
		fn.Dir(sStdDir);

  }

  sName = fn.sFullPath(true);
  sName.toLower();
  *_psName = sName;
  FormEntry::StoreData();
}

int FieldDataTypeLarge::DblClkObject(void *)
{
  frm()->OnOK();
  return 1;
}

int FieldDataTypeLarge::DblClkDir(void *)
{
  int id = lbDir->GetCurSel();
  CString s1;
  lbDir->GetText(id, s1);
  String s(s1);
  if (s == "")
    return 0;
  if (s[0] == '[') {
    if (s[1] == '-') {
      String sDrive;
      sDrive &= s[2];
      sDrive &= ':';
      SetCurrentDirectory(sDrive.c_str());
    }
    else {
      int sLen = s.length() - 2;
      for (int i = 0; i < sLen; i++)
        s[i] = s[i+1];
      s[sLen] = '\0';
      if (SetCurrentDirectory(s.c_str()) == 0)
        throw ErrorObject(TR("Invalid directory change"));
    }
    char sDummy[MAXPATH];
    if (0 == GetCurrentDirectory(MAX_PATH, sDummy))
      return 0;
    sDir = sDummy;
  }
  FillDir();

  *_psName = String();
  if (_npChanged)
    (_cb->*_npChanged)(0);

  return 0;
}

int FieldDataTypeLarge::CloseDrive(void *)
{
  int id = cbDrive->GetCurSel();
  CString s;
  cbDrive->GetLBText(id, s);
  if (s == "")
    return 0;
  if (s[0] == '[') {
    if (s[1] == '-') {
      String sDrive;
      sDrive &= s[2];
      sDrive &= ':';
      SetCurrentDirectory(sDrive.c_str());
    }
    char sDummy[MAX_PATH];
    if (0 == GetCurrentDirectory(MAX_PATH, sDummy))
      return 0;
    sDir = sDummy;
  }
  FillDrive();
  return 0;
}

FieldDataObject::FieldDataObject(FormEntry* par, String *psName)
: FieldGroup(par, true)
{
  new FieldBlank(this,0); // als deze regel er niet staat loopt de boel in de soep!!
  fdtl = new FieldDataTypeLarge(this, psName,
              ".MPR.MPA.MPS.MPP.MPV");
  iType = 0;
  fos = new FieldOneSelect(this, &iType);
  fos->psn->iMinWidth = fdtl->psn->iMinWidth;
  fos->Align(fdtl, AL_UNDER);
}

void FieldDataObject::create()
{
  fdtl->create();
  fos->create();
  OneSelectEdit* ose = fos->ose;
  // Keep exact same sequence as in enum ObjectType !!
  ose->AddString(TR("All Maps.ilw").c_str());
  ose->AddString(TR("Raster Map.mpr").c_str());
  ose->AddString(TR("Polygon Map.mpa").c_str());
  ose->AddString(TR("Segment Map.mps").c_str());
  ose->AddString(TR("Point Map.mpp").c_str());
  ose->AddString(TR("Table.tbt").c_str());
  ose->AddString(TR("Map List.mpl").c_str());
  ose->AddString(TR("Object Collection.ioc").c_str());
  ose->AddString(TR("Map View.mpv").c_str());
  ose->AddString(TR("Layout.ilo").c_str());
  ose->AddString(TR("Annotation Text.atx").c_str());
  ose->AddString(TR("Graph.grh").c_str());
  ose->AddString(TR("Domain.dom").c_str());
  ose->AddString(TR("Representation.rpr").c_str());
  ose->AddString(TR("GeoReference.grf").c_str());
  ose->AddString(TR("Coordinate System.csy").c_str());
  ose->AddString(TR("Histogram.his").c_str());
  ose->AddString(TR("Polygon Histogram.hsa").c_str());
  ose->AddString(TR("Segment Histogram.hss").c_str());
  ose->AddString(TR("Point Histogram.hsp").c_str());
  ose->AddString(TR("Sample Set.sms").c_str());
  ose->AddString(TR("Stereo Pair.stp").c_str());
  ose->AddString(TR("Criteria Tree.smc").c_str());
  ose->AddString(TR("2-Dimensional Table.ta2").c_str());
  ose->AddString(TR("Matrix.mat").c_str());
  ose->AddString(TR("Filter.fil").c_str());
  ose->AddString(TR("Function.fun").c_str());
  ose->AddString(TR("Script.isl").c_str());
  ose->AddString(TR("All ILWIS Objects.ilw").c_str());
  ose->SetCurSel(0);
  ose->setNotify(this, (NotifyProc)&FieldDataObject::TypeChanged, Notify(CBN_SELCHANGE));
}

int FieldDataObject::TypeChanged(void *)
{
  int id = fos->ose->GetCurSel();
  switch (ObjectType(id)) {
    case otMAPS: fdtl->SetExt(".MPR.MPA.MPS.MPP.MPV"); break;
    case otMPV: fdtl->SetExt(".MPV"); break;
    case otMPR: fdtl->SetExt(".MPR"); break;
    case otMPS: fdtl->SetExt(".MPS"); break;
    case otMPA: fdtl->SetExt(".MPA"); break;
    case otMPP: fdtl->SetExt(".MPP"); break;
    case otMPL: fdtl->SetExt(".MPL"); break;
    case otTBT: fdtl->SetExt(".TBT"); break;
    case otTA2: fdtl->SetExt(".TA2"); break;
    case otDOM: fdtl->SetExt(".DOM"); break;
    case otRPR: fdtl->SetExt(".RPR"); break;
    case otCSY: fdtl->SetExt(".CSY"); break;
    case otGRF: fdtl->SetExt(".GRF"); break;
    case otHIS: fdtl->SetExt(".HIS"); break;
    case otHSA: fdtl->SetExt(".HSA"); break;
    case otHSS: fdtl->SetExt(".HSS"); break;
    case otHSP: fdtl->SetExt(".HSP"); break;
    case otSMS: fdtl->SetExt(".SMS"); break;
    case otMAT: fdtl->SetExt(".MAT"); break;
    case otFUN: fdtl->SetExt(".FUN"); break;
    case otFIL: fdtl->SetExt(".FIL"); break;
    case otISL: fdtl->SetExt(".ISL"); break;
    case otATX: fdtl->SetExt(".ATX"); break;
    case otIOC: fdtl->SetExt(".IOC"); break;
    case otILO: fdtl->SetExt(".ILO"); break;
    case otGRH: fdtl->SetExt(".GRH"); break;
    case otSTP: fdtl->SetExt(".STP"); break;
    case otSMC: fdtl->SetExt(".SMC"); break;
    case otALL: fdtl->SetExt(".MPR.MPA.MPS.MPP.TBT.MPL.IOC.MPV.ILO.ATX.GRH.DOM.RPR.GRF.CSY.HIS.HSA.HSS.HSP.SMS.STP.SMC.TA2.MAT.FIL.FUN.ISL");
            break;
  }
  return 1;
}

DataObjectForm::DataObjectForm(CWnd* parent, const String& sTitle, String* sName, 
                const String& htp)
: FormWithDest(parent, sTitle)
{
  FieldDataObject* fdo = new FieldDataObject(root, sName);
  fdo->SetNoSystemDir();
  SetMenHelpTopic(htp);
  create();
}
  
DataObjectForm::DataObjectForm(CWnd* parent, const String& sTitle, String* sName, 
                 const String& sExt, const String& htp)
: FormWithDest(parent, sTitle)
{
  new FieldDataTypeLarge(root, sName, sExt);
  SetMenHelpTopic(help);
  create();
}          
