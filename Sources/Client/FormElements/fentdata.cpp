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
/* Form entries for data
// by Wim Koolhoven, aug. 1993
// changed by Jelle Wind, dec. 1993
// (c) Computer Department ITC

// Base class for data object form entries
	Last change:  WK   24 Sep 97   12:51 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\nameedit.h"
#include "Client\FormElements\objlist.h"

//--- [ FieldDataTypeSimple ]--------------------------------------------------------------------------
FieldDataTypeSimple::FieldDataTypeSimple(FormEntry* p, Parm *prm, const String& sExt, bool fExt)
  : FormEntry(p, prm), _sExt(sExt), _fExt(fExt)
{
	_sName = prm->sVal();
	_psName = 0;

	ol = new ObjectExtensionLister(0, sExt);

	Init();
};

FieldDataTypeSimple::FieldDataTypeSimple(FormEntry* p, String *psName, const String& sExt, bool fExt, ObjectLister *obl)
  : FormEntry(p), _sExt(sExt), _fExt(fExt)
{
	_sName = *psName;
	_psName = psName;

	ol = !obl ? new ObjectExtensionLister(0, sExt) : obl;

	Init();
};

FieldDataTypeSimple::FieldDataTypeSimple(FormEntry* p,Parm *prm, ObjectLister* objl, bool fExt)
  : FormEntry(p, prm), ol(objl), _fExt(fExt)
{
	_sExt = ol->sFileExt();
	_sName = prm->sVal();
	_psName = 0;

	Init();
};

FieldDataTypeSimple::FieldDataTypeSimple(FormEntry* p, String *psName, ObjectLister* objl, bool fExt)
  : FormEntry(p), ol(objl), _fExt(fExt)
{
	_sExt = ol->sFileExt();
	_psName = psName;
	_sName = *psName;

	Init();
};

void FieldDataTypeSimple::Init()
{
	if (_sName.length() > 0)
	{
		FileName fn(_sName);
		int iN = count(_sExt.begin(), _sExt.end(), '.');
		if ( iN == 1)
			fn.sExt = _sExt;
		else if (fn.sExt.length() == 0)
		{
			unsigned int i = 1;
			while (i < _sExt.length() && _sExt[i] != '.') i++;
			if (i != 1)
				fn.sExt = _sExt.sSub(0, i);
		}
		fn.sExt.toLower();
		if (0 != _sName.strchrQuoted('\\'))
			_sName = fn.sFullName();
		else
			_sName = String("%S%S", fn.sFile, fn.sExt);
	}

	setHelpItem(htpUiDataType);
	fAllowEmpty = false;
	zDimension dim = Dim("gh");
	psn->iHeight = dim.cy + 9;
	psn->iMinWidth = FLDNAMEOUTPUTWIDTH ;
	ne = 0;
}

FieldDataTypeSimple::~FieldDataTypeSimple()
{
	delete ne;
}

String FieldDataTypeSimple::sGetExtentsions()
{
	return _sExt;
}

void FieldDataTypeSimple::SetObjLister(ObjectLister* objl)
{
  CString s;
	if (ne) {
		int id = ne->GetCurSel();
		if (id >= 0)
			ne->GetLBText(id, s);
	}
  String sVal(s);
  if (ne)
    ne->SetObjLister(objl);
  else  
    delete ol;
  ol = objl;
	if (ne)
		SetVal(sVal);
}

void FieldDataTypeSimple::create()
{
  zPoint pntFld;
  pntFld.x = psn->iPosX;
  pntFld.y = psn->iPosY;
  ne = new NameEdit(this, _frm, pntFld, Id(), _sName,
                      true, ol, _fExt, psn->iMinWidth);

  ne->SetFont(_frm->fnt);
  CreateChildren();
}

void FieldDataTypeSimple::show(int sw)
{
  if (ne)
    ne->ShowWindow(sw);
}

void FieldDataTypeSimple::SetVal(const String& sVal)
{ 
  if (0 == ne)
    return;
  ne->SetVal(sVal);
  int iSel = ne->GetCurSel();
  if ((iSel < 0) && (sVal.length() > 0)) {
    ne->AddString(sVal.scVal());
    ne->BaseNameEdit::SetVal(sVal);
  }
  if (_npChanged)
    (_cb->*_npChanged)(0);
}

String FieldDataTypeSimple::sGetText()
{
  if (!fShow())
    return String();
  if (ne)
    return ne->sName();
  return String();
}

FormEntry* FieldDataTypeSimple::CheckData()
{
  if (!fShow() || ne==0) return 0;
  int iRes = ne->CheckData();
  if (!fAllowEmpty && (ne->sName() == ""))
    return this;
  return iRes ? FormEntry::CheckData() : this;
}

void FieldDataTypeSimple::StoreData()
{
  if (fShow()&&ne) {
    if (_psName != 0)
		{
			*_psName = ne->sName();
		}	
    else
			 _prm->Replace(ne->sName());
  }
  FormEntry::StoreData();
}


void FieldDataTypeSimple::SetFocus()
{
#ifdef ZAPP
  if (ne)
    ne->SetFocus();
#endif
  _frm->ilwapp->setHelpItem(htp());
}

void FieldDataTypeSimple::Enable() 
{ 
  if (ne) ne->EnableWindow(TRUE); 
}
void FieldDataTypeSimple::Disable() 
{ 
  if (ne) ne->EnableWindow(FALSE); 
}

void FieldDataTypeSimple::DrawItem(DRAWITEMSTRUCT* dis)
{
  if (ne)
    ne->DrawItem(dis);
}

String FieldDataTypeSimple::sName() 
{ 
	if ( ne)
		return ne->sName(); 
	
	return "";
} 

void FieldDataTypeSimple::FillDir() 
{
	if ( ne )
		ne->FillDir(); 
}

//---[ FieldDataType ]------------------------------------------------------------------------------
FieldDataType::FieldDataType(FormEntry* p, const String& sQuestion,
                     Parm *prm, const String& sExt, bool fExt)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldDataTypeSimple(this, prm, sExt, fExt);
  if (st)
  {
    fld->Align(st, AL_AFTER);
	st->psn->iHeight = fld->psn->iHeight;
	psn->iHeight = fld->psn->iHeight;
  }
  setHelpItem(htpUiDataType);
}

FieldDataType::FieldDataType(FormEntry* p, const String& sQuestion,
                     String *psName, const String& sExt, bool fExt, ObjectLister *obl)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldDataTypeSimple(this, psName, sExt, fExt, obl);
  if (st)
  {
    fld->Align(st, AL_AFTER);
	st->psn->iHeight = fld->psn->iHeight;
	psn->iHeight = fld->psn->iHeight;
  }
};

FieldDataType::FieldDataType(FormEntry* p, const String& sQuestion,
                     Parm *prm, ObjectLister* ol, bool fExt)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldDataTypeSimple(this, prm, ol, fExt);
  if (st)
  {
    fld->Align(st, AL_AFTER);
	st->psn->iHeight = fld->psn->iHeight;
	psn->iHeight = fld->psn->iHeight;
  }
  setHelpItem(htpUiDataType);
}

FieldDataType::FieldDataType(FormEntry* p, const String& sQuestion,
                     String *psName, ObjectLister* ol, bool fExt)
  : FormEntry(p, 0, true)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldDataTypeSimple(this, psName, ol, fExt);
  if (st)
  {
    fld->Align(st, AL_AFTER);
	st->psn->iHeight = fld->psn->iHeight;
	psn->iHeight = fld->psn->iHeight;
  }
};


FieldDataType::~FieldDataType()
{ } // children are deleted by FormEntry

void FieldDataType::SetWidth(short iWidth)
{
  fld->SetWidth(iWidth);
}

String FieldDataType::sGetExtentsions()
{
	return fld->sGetExtentsions();
}

void FieldDataType::enable() { 
  fld->Enable(); 
  if (st) st->Enable(); 
}
void FieldDataType::disable() {
  fld->Disable(); 
  if (st) st->Disable(); 
}
void FieldDataType::DisableUnset() { 
  SetVal("");
  if (fld->ne)
    fld->ne->SetCurSel(-1); 
  disable(); 
}    

String FieldDataType::sGetText()
{
    if ( !fShow() ) 
		return String();

    String s1;
    if (st) 
		s1 = st->sGetText();
	if (s1.length() > 0)
		s1 &= String(":\t%S", fld->sGetText());
	else
		s1 = fld->sGetText();

    return s1;
}

void FieldDataType::GetAttribParts(String& sValue, String& sM, String& sC, String& sT)
{
	if ( sValue.find(".") != -1) // this is a fully qualified map|attrib|column
	{
			Array<String> arParts;
			Split(sValue, arParts, ".");
			sM = arParts[0] + "." + arParts[1];
			sC = arParts[arParts.iSize() - 1].sHead(".");
	}
		else
			sM = sValue;
}


//--- [ FieldDataC ]------------------------------------------------------------------------------------
void FieldDataTypeC::init(NotifyProc npCreate)
{
	if ( fld )
	{
	  fld->psn->SetBound(0,-1,0,0);
		pbCreate = new OwnButtonSimple(this, "CreateBut",
				 npCreate, true, false);
		pbCreate->Align(fld, AL_AFTER);
		pbCreate->SetIndependentPos();
	}		
}

void FieldDataTypeC::create() 
{ 
  // glue create button to field before it
  pbCreate->psn->iPosX = fld->psn->iPosX+fld->psn->iMinWidth;
  CreateChildren();
}

void FieldDataTypeC::SetVal(const String& sVal)
{
	if ( fld )
	{
		fld->SetVal(sVal);
		if ( fld->ne )
			fld->ne->ResetObjectTreeCtrl();
	}		
}


//---- [ FieldDataTypeCreate ]-------------------------------------------------------------------------
FieldDataTypeCreate::FieldDataTypeCreate(FormEntry* p, const String& sQuestion, Parm *prm,
                      const String& sExt, bool fExt)
  : FormEntry(p, prm, true), _sExt(sExt)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  _fExt = fExt;
  _sName = prm->sVal();
  _psName = 0;
  fld = new FieldStringSimple(this, &_sName, Domain(), false);
  fld->psn->iMinWidth = FLDNAMEOUTPUTWIDTH;
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiDataType);
}

FieldDataTypeCreate::FieldDataTypeCreate(FormEntry* p, const String& sQuestion,
                      String *psName, const String& sExt, bool fExt)
  : FormEntry(p, 0, true), _sExt(sExt)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  _fExt = fExt;
  _psName = psName;
  _sName = *psName;
  fld = new FieldStringSimple(this, &_sName, Domain(), false);
  fld->psn->iMinWidth = FLDNAMEOUTPUTWIDTH;
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiDataType);
}

FieldDataTypeCreate::~FieldDataTypeCreate()
{
}

void FieldDataTypeCreate::SetWidth(short iWidth)
{
  fld->SetWidth(iWidth);
}



void FieldDataTypeCreate::SetVal(const String& sVal)
{
  fld->SetVal(sVal);
}

FormEntry* FieldDataTypeCreate::CheckData()
{
  if (!fShow()) return 0;
  FormEntry* fe = fld->CheckData();
  if (fe) return fe;
  FileName fn(fld->sVal(), _sExt);
  if (!fn.fValid())
    return this;
  return 0;
}

String FieldDataTypeCreate::sGetText()
{
	if ( !fShow() ) 
		return String();

    String s1;
    if (st) 
		s1 = st->sGetText();

	if (s1.length() > 0)
		s1 &= String(":\t%S", fld->sGetText());
	else
		s1 = fld->sGetText();

    return s1;

}

const String FieldDataTypeCreate::sName()
{
  return fld->sVal();
}

void FieldDataTypeCreate::StoreData()
{
  if (fShow()) {
    String s = fld->sVal(); 
		if ("" != s) {
			FileName fn(s.sQuote(), _sExt);
			_sName = fn.sFullNameQuoted(_fExt); 
			if (_psName != 0)
				*_psName = _sName;
			else
				_prm->Replace(_sName);
		}
  }
  FormEntry::StoreData();
}

//-----[ FieldMapFromMapLis ]-------------------------------------------------------------------------
FieldMapFromMapList::FieldMapFromMapList(FormEntry* p, const String& sQuestion,
              const MapList& ml, int* iMapNr)
  : FormEntry(p, 0, true),
    mpl(ml), iNr(iMapNr)
{
  if (sQuestion.length() != 0)
    st = new StaticTextSimple(this, sQuestion);
  else
    st = 0;
  fld = new FieldOneSelect(this, 0);
	fld->SetWidth(FLDNAMEWIDTH);
  if (st)
    fld->Align(st, AL_AFTER);
  setHelpItem(htpUiColumn);
};

void FieldMapFromMapList::create()
{
  CreateChildren();
  if (mpl.fValid())
    FillWithMaps(mpl);
  *iNr -= mpl->iLower();
  fld->ose->SetCurSel(*iNr);
}

FormEntry* FieldMapFromMapList::CheckData()
{
  if (fShow()) {
    int id = fld->ose->GetCurSel();
    if (id < 0)
      return this;
  }
  return FormEntry::CheckData();
}

void FieldMapFromMapList::StoreData()
{
  CheckData();
  if (fShow()) 
    *iNr = fld->ose->GetCurSel()+mpl->iLower();
  FormEntry::StoreData(); // do not use FieldOneSelect::StoreData!
}

void FieldMapFromMapList::FillWithMaps(const MapList& ml)
{
  mpl = ml;
  String sMap;
  fld->ose->ResetContent();
  if (mpl.fValid()) {
    for (int i = mpl->iLower(); i <= mpl->iUpper(); ++i) {
      sMap = mpl->map(i)->sName(true);
      fld->ose->AddString(sMap.sVal());
    }
  }
  fld->ose->SetCurSel(0);
}

void FieldMapFromMapList::SetFocus()
{
  fld->SetFocus();
  _frm->ilwapp->setHelpItem(htp());
}




