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
/* FieldMapList
   by Wim Koolhoven, october 1994
   (c) Ilwis System Development ITC
	Last change:  WK   22 Oct 98   10:20 am
*/
#include "Client\Headers\formelementspch.h"
#include "Headers\Hs\maplist.hs"
#include "Client\FormElements\fldlist.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\mask.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\ilwis.h"

FieldMapList::FieldMapList(FormEntry* parent, const String& sQuestion,
                           Parm *prm, bool fCreate)
: FieldDataTypeC(parent, sQuestion, prm, ".MPL",
		true, (NotifyProc)&FieldMapList::CreateMapList)
{
}

FieldMapList::FieldMapList(FormEntry* parent, const String& sQuestion,
                           String *sName, bool fCreate)
: FieldDataTypeC(parent, sQuestion, sName, ".MPL",
		true, (NotifyProc)&FieldMapList::CreateMapList)
{
  sNewName = *sName; 
}

int FieldMapList::CreateMapList(void*)
{
  FormCreateMapList form(frm()->wnd(), &sNewName);
  if (form.fOkClicked()) {
    FillDir();
    FileName fn(sNewName);
    SetVal(fn.sFile);
  }  
  return 0;
}

MapListSelector::MapListSelector(FormEntry* par)
  : BaseSelector(par)
{
  psn->iMinWidth = 350;
  psn->iMinHeight = psn->iHeight = 350;
  style = LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_MULTICOLUMN | 
          WS_HSCROLL | LBS_SORT;
}

MapListSelector::~MapListSelector()
{
}

void MapListSelector::FillArray(Array<FileName>& afn)
{
  int iNr = lb->GetCount();
  for (int i = 0; i < iNr; ++i)
    if (lb->GetSel(i))
    {
      CString s;
      lb->GetText(i, s);
			String ss(s);
      FileName fn(ss);
      afn &= fn;
    }
}

void MapListSelector::create()
{
  BaseSelector::create();
  lb->ResetContent();

	CFileFind finder;
	bool fWorking = finder.FindFile("*.MPR") != 0;
	while (fWorking)
	{
		fWorking = finder.FindNextFile() != 0;
		if (!finder.IsDirectory())
		{
			String sFile = String(finder.GetFileName());
    	lb->AddString(sFile.scVal());
		}
	}
	finder.Close();
}

String MapListSelector::sName(int id)
{
  CString s;;
  lb->GetText(id, s);
  return  String(s);
}

FormCreateMapList::FormCreateMapList(CWnd* wPar, String* sML)
: FormWithDest(wPar, SUITitleCreateMapList),
  sMapList(sML)
{
	iImg = IlwWinApp()->iImage(".mpl");

  sNewName = *sMapList;
  fml = new FieldDataTypeCreate(root, SUIMapList, &sNewName, ".MPL", true);
  fml->SetCallBack((NotifyProc)&FormCreateMapList::CallBackName);
  FieldString* fs = new FieldString(root, SUIDescription, &sDescr);
  fs->SetWidth(120);
  fmm = new FieldMultiMap(root, "", arr);
  fmm->SetIndependentPos();
  String sFill('*', 60);
  stRemark = new StaticText(root, sFill);
  stRemark->Align(fmm, AL_UNDER);
  stRemark->SetIndependentPos();
  SetMenHelpTopic("ilwismen\\create_a_map_list.htm");
  create();
}

int FormCreateMapList::exec()
{
  try {
    FormWithDest::exec();
    *sMapList = sNewName;
    FileName fn(*sMapList, ".mpl");
    int iSize = arr.iSize();
//    int iLow = arr.iLower();
    int iLow = 0;
    Array<FileName> afn(iSize);
    for (int i = 0; i < iSize; ++i)
      afn[i] = arr[iLow+i]->fnObj;
    MapList mpl(fn, afn);
    mpl->sDescription = sDescr;
    mpl->Store();
  }
  catch (ErrorObject& err) {
    err.Show();
  }  
  return 0;
}

int FormCreateMapList::CallBackName(Event*)
{
  fml->StoreData();
  FileName fn(sNewName, ".mpl");
  bool fOk = false;
  if (!fn.fValid())
    stRemark->SetVal(SUIErrInvalidMapListName);
  else if(File::fExist(fn))   
    stRemark->SetVal(SUIErrMapListExists);
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

MaskSelector::MaskSelector(FormEntry* par, Mask* msk)
  : BaseSelector(par)
{
  psn->iMinWidth = 300;
  psn->iMinHeight = psn->iHeight = 150;
  style = LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_MULTICOLUMN | 
          WS_HSCROLL | LBS_SORT;
  mask = msk;        
}

MaskSelector::~MaskSelector()
{
}

void MaskSelector::StoreData()
{
  int iNr = lb->GetCount();
  Array<String> asMaskParts;
  for (int i = 0; i < iNr; ++i)
    if (lb->GetSel(i))
    {
      CString s;
      lb->GetText(i,s);
      asMaskParts &= String(s);
    }
  if (asMaskParts.iSize() == iNr)  // all selected
    mask->SetMask("*");
  else
    mask->SetMaskParts(asMaskParts);
}

void MaskSelector::create()
{
  BaseSelector::create();
  lb->ResetContent();
  for (int i=0; i < mask->iMaskParts(); ++i) {
    lb->AddString(mask->sMaskPart(i).sVal());
    int id = lb->FindString(-1, mask->sMaskPart(i).sVal());
    lb->SetCurSel(id);
  }  
}

String MaskSelector::sName(int id)
{
  CString s;;
  lb->GetText(id, s);
  return  String(s);
}


/*extern "C" {
// from Undocumented Windows
DWORD FAR PASCAL DragObject(HWND,HWND,WORD,WORD,NPSTR,HANDLE);	// USER.464
BOOL FAR PASCAL DragDetect(HWND,LPPOINT);  // USER.465
}*/

GeneralMapSelector::GeneralMapSelector(FormEntry* par)
: BaseSelector(par)
{
  psn->iMinWidth *= 2;
}

String GeneralMapSelector::sName(int id)
{
  if (lb) 
  {
    CString s;
    lb->GetText(id, s);
    return String(s);
  }
  return "";  
}

void GeneralMapSelector::create()
{
  BaseSelector::create();
  lb->setNotify(this, (NotifyProc)&GeneralMapSelector::DrawItem, WM_DRAWITEM); 
}

void GeneralMapSelector::DrawItem(Event* ev)
{
  DrawItemEvent *dev=reinterpret_cast<DrawItemEvent *>(ev);
  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
  if (id < 0)
    return ;
  String str = sName(id);
  BOOL fSel = lb->GetSel(id);
  BOOL fUpNgbSel = FALSE;
  if (fSel && id > 0)
    fUpNgbSel = lb->GetSel(id) > 0;
  idw.DrawItem(dis,str,fSel,true,fUpNgbSel);
  return ;
}

void GeneralMapSelector::StoreData()
{
  // empty implementation!
}

void GeneralMapSelector::SetSel(int id, bool fSel)
{
  lb->SetSel(id);
}

void GeneralMapSelector::SetSel(const String& str, bool fSel)
{
  int id = lb->FindString(-1, str.scVal());
  lb->SetCurSel(id);
}

bool GeneralMapSelector::fSel(int id)
{
  return 0 != lb->GetSel(id); 
}

int GeneralMapSelector::iGetSelected(IntBuf& buf)
{
  int iCnt = lb->GetSelCount(); 
  int* pi = new int[iCnt];
  iCnt = lb->GetSelItems(iCnt, pi);
  buf.Size(iCnt);
  for (int i = 0; i < iCnt; ++i)
    buf[i] = pi[i];
  delete [] pi;
  return iCnt;
}

void GeneralMapSelector::RecalcHScrollSize()
{
	// Find the longest string in the list box.
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = lb->GetDC();
	CFont* fntOld = pDC->SelectObject(frm()->fnt);
	for (int i = 0; i < lb->GetCount(); ++i)
	{
		lb->GetText(i, str);
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}

	sz = pDC->GetTextExtent("x"); // extra char, because GetTextExtent underestimates
	pDC->SelectObject(fntOld);
	lb->ReleaseDC(pDC);
	
	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	lb->SetHorizontalExtent(dx + sz.cx);
}

MultiMapSelector::MultiMapSelector(FormEntry* par, const GeoRef& grf, const Domain& dom)
: GeneralMapSelector(par), gr(grf), dm(dom)
{
  style = LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_HSCROLL | WS_VSCROLL | LBS_HASSTRINGS;
}

void MultiMapSelector::create()
{
  GeneralMapSelector::create();
  lb->ResetContent();
  FilenameIter fi("*.mpr");
  while(fi.fValid())
  {
    String s=String(*fi).toLower();
    FileName fn(s);
    try {
      if (fOK(fn))
        lb->AddString(s.sVal());
    }
    catch (ErrorObject& err) {
      err.Show();
    }

    ++fi;
  }
	
	RecalcHScrollSize();
}

bool MultiMapSelector::fOK(const FileName& fn)
{
  if (dm.fValid()) {
    FileName fnDom;
    ObjectInfo::ReadElement("BaseMap", "Domain", fn, fnDom);
    if (fnDom != dm->fnObj) {
      if (fnDom.sExt == "") {
        Domain dom(fnDom);
        if (dom != dm)
          return false;
      }
      else
        return false;
    }
  }
  if (gr.fValid()) {
    FileName fnGrf;
    ObjectInfo::ReadElement("Map", "GeoRef", fn, fnGrf);
    if (fnGrf != gr->fnObj)
      return false;
  }
  return true;
}

MapSequenceSelector::MapSequenceSelector(FormEntry* par, Array<Map>& array)
: GeneralMapSelector(par),
  curArrow(Arrow), arr(array)
{
  style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_HSCROLL | WS_VSCROLL | LBS_HASSTRINGS;
  fDrag = false;
  cur = 0;
}

MapSequenceSelector::~MapSequenceSelector()
{
//  for (int i = lb->GetCount() - 1; i >= 0; --i)
//    Del(i);
}

void MapSequenceSelector::create()
{
  GeneralMapSelector::create();
  lb->ResetContent();
//  int iLow = arr.iLower();
  int iLow = 0;
  for (int i = 0; i < arr.iSize(); ++i) {
    int id = Add(arr[iLow+i]);
    SetSel(id, false);
  }
  lb->setNotify(this,(NotifyProc)&MapSequenceSelector::mouseButtonDown, WM_LBUTTONDOWN);
  lb->setNotify(this,(NotifyProc)&MapSequenceSelector::mouseButtonUp, WM_LBUTTONUP);
  lb->setNotify(this,(NotifyProc)&MapSequenceSelector::mouseMove, WM_MOUSEMOVE);
  lb->SendMessage(LB_SETCARETINDEX,0,0);
}

FormEntry* MapSequenceSelector::CheckData()
{
  if (0 == iMaps())
    return this;
  return 0;
}

void MapSequenceSelector::StoreData()
{
  int iNr = iMaps();
  arr.Resize(iNr);
//  int iLow = arr.iLower();
  int iLow = 0;
  for (int i = 0; i < iNr; ++i)
    arr[iLow+i] = map(i);
}

int MapSequenceSelector::mouseButtonDown(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);
  if (ev && !ev->fShiftPressed() && !ev->fCtrlPressed()) {
    if (lb->GetCount() == 0)
      return 0;
    int iTop = lb->GetTopIndex();
    int iHeight = lb->GetItemHeight(0); 
    CPoint p = ev->pos();
    idDrag = iTop + p.y / iHeight;
    if (idDrag < lb->GetCount()) 
    {
      if (DragDetect(lb->m_hWnd, p)) 
      {
        if (cur)
	        delete cur;
        fDrag = true;
        cur = new zCursor("Mpr16Cur");
        lb->SetCapture();
        CRect rect;
        lb->GetWindowRect(rect);
        rect.TopLeft().y -= 20;
        rect.BottomRight().y += 20;
        ClipCursor(&rect);
        ::SetCursor(*cur);
        return 1; // list box should not get mouse button down event
      }
      else
        lb->PostMessage(WM_LBUTTONUP, 0, Evt->lParm);
    }
  }
  return 0;
}

int MapSequenceSelector::mouseButtonUp(Event* Evt)
{
  MouseLBUpEvent *ev=reinterpret_cast<MouseLBUpEvent *>(Evt);
  if (ev) {
    if (!fDrag)
      return 0;
    fDrag = false;
    ::ReleaseCapture();
    ClipCursor(0);
    ::SetCursor((HCURSOR)curArrow);
    if (cur) {
      delete cur;
      cur = 0;
    }
    int iTop = lb->GetTopIndex();
    int iHeight = lb->GetItemHeight(0); 
    int id = iTop + ev->pos().y / iHeight;
    lb->SetSel(id);
    if (id != idDrag) {
      CString s;
      lb->GetText(idDrag, s);
      lb->DeleteString(idDrag);
      if (id < 0)
	      id = 0;
      if (id > lb->GetCount())
        lb->AddString(s);
      else
        lb->InsertString(id, s);
      lb->SetCaretIndex(id);
    }
  }
  return 0;
}

int MapSequenceSelector::mouseMove(Event*)
{
  if (fDrag)
    return 1;
  else
    return 0;
}

int MapSequenceSelector::idActive()
{
  if (lb->GetCount() == 0)
    return -1;
  return lb->GetCaretIndex(); 
}

int MapSequenceSelector::Add(const Map& map)
{
  String s = map->sName(true);
  return Add(s);
}

// Assumed: arr has at least one Map
bool MapSequenceSelector::fCheckDomains(const Map& mp, String& sErr) const
{ 
	if (arr[0]->dm() == Domain("none"))
	{
		sErr = SMPLErrDomainNoneNotAllowed;
		return false;
	}
	if (arr[0]->dm() != mp->dm())
	{
		sErr = String(SDATErrIncompatDomain_SS.sVal(), arr[0]->dm()->sName(true), mp->dm()->sName(true));
		return false;
	}

	return true;
}

// Assumed: arr has at least one Map
bool MapSequenceSelector::fCheckGeoRefs(const Map& mp, String& sErr) const
{
	if (arr[0]->gr()->fGeoRefNone()) // still should have same size
	{
		if (arr[0]->gr()->rcSize() != mp->gr()->rcSize())
		{
			sErr = String(SDATErrIncompatMapSize_SS.sVal(), arr[0]->gr()->sName(true), mp->gr()->sName(true));
			return false;
		}
	}                 
	else
	{
		if (arr[0]->gr() != mp->gr())
		{
			sErr = String(SDATErrIncompatGeoRef_SS.sVal(), arr[0]->gr()->sName(true),  mp->gr()->sName(true));
			return false;
		}
	}

	return true;
}                   

int MapSequenceSelector::Add(const String& s)
{
	StoreData();
	if (arr.size() > 0)
	{
		String sErr;
		Map mp(s);
		if (!fCheckGeoRefs(mp, sErr))
		{
			IlwWinApp()->GetMainWnd()->MessageBox(sErr.scVal(), SDATErrMapListError.scVal(), MB_OK | MB_ICONSTOP);
			return -1;
		}

		if (!fCheckDomains(mp, sErr))
		{
			IlwWinApp()->GetMainWnd()->MessageBox(sErr.scVal(), SDATErrMapListError.scVal(), MB_OK | MB_ICONSTOP);
			return -1;
		}
	}
	
	int iNr = lb->GetCount();
	int id;
	for (id = 0; id < iNr; ++id)
		if (s == sName(id))
		{
			lb->SetSel(id);
			return id;
		}
	id = lb->AddString(s.scVal());
	lb->SetSel(id);
	lb->SetCaretIndex(id);
	RecalcHScrollSize();
	return id;
}

void MapSequenceSelector::Del(int id)
{
  if (id >= 0) {
    lb->DeleteString(id);
		RecalcHScrollSize();
  }
}

int MapSequenceSelector::Del(const Map& map)
{
  String sName = map->sName(true);
  char* s = sName.sVal();
  int id = lb->FindStringExact(-1, s);
  if (id >= 0)
	{
    lb->DeleteString(id);
		RecalcHScrollSize();
	}
  return id;
}

int MapSequenceSelector::iMaps() const
{
  return lb->GetCount();
}

Map MapSequenceSelector::map(int id)
{
  String str = sName(id);
  Map map(str);
  return map;
}

String MapSequenceSelector::sGetText()
{
  String s;
  int iNr = lb->GetCount();
  for (int i = 0; i < iNr; ++i) {
    s &= sName(i);
    s &= "\r\n";
  }
  return s;
}

FieldMultiMap::FieldMultiMap(FormEntry* par, const String& sQuestion, Array<Map>& arr)
: FieldGroup(par, true)
{
  StaticTextSimple* sts = 0;
  if ("" != sQuestion) {
    sts = new StaticTextSimple(this, sQuestion);
    sts->SetIndependentPos();
  }
  Domain dm;
  GeoRef grf;
  if (arr.iSize() > 0) {
//    Map map = arr[arr.iLower()];
    Map map = arr[0];
    dm = map->dm();
    grf = map->gr();
  }
  mcs = new MultiMapSelector(this, grf, dm);
  if (sts)
    mcs->Align(sts, AL_UNDER);
  FieldBlank* fb = new FieldBlank(this, 2);
  fb->Align(mcs, AL_AFTER);
  PushButton* pbAdd = new PushButton(this, " > ", (NotifyProc)&FieldMultiMap::Add, true);
  PushButton* pbDel = new PushButton(this, " < ", (NotifyProc)&FieldMultiMap::Del, true);
  pbAdd->SetWidth(30);
  pbDel->SetWidth(30);
  cacs = new MapSequenceSelector(this, arr);
  cacs->Align(fb, AL_AFTER);
}

void FieldMultiMap::create()
{
  FieldGroup::create();
}

int FieldMultiMap::Add(Event*)
{
	cacs->SetSel(-1,false);
	IntBuf buf;
	int iNr = mcs->iGetSelected(buf);
	for (int i = 0; i < iNr; ++i)
	{
		String s = mcs->sName(buf[i]);
		mcs->SetSel(s, false);
		cacs->Add(s);
	}
	return 0;
}

int FieldMultiMap::Del(Event*)
{
  int iNr = cacs->iMaps();
  for (int i = iNr - 1; i >= 0; --i) {
    if (cacs->fSel(i)) {
      String s = cacs->sName(i);
      cacs->Del(i);
      mcs->SetSel(s, true);
    }
  }
  return 0;
}

String FieldMultiMap::sGetText()
{
  return cacs->sGetText();
}





