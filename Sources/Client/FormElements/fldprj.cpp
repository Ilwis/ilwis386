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
/* FieldProjection
   by Wim Koolhoven, october 1995
   (c) Ilwis System Development ITC
	Last change:  WK   12 Sep 97    5:24 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldprj.h"
#include "Client\ilwis.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

FieldItem::FieldItem(FormEntry* par, String *psName)
: FormEntry(par,0,true)
{
  lbObject = 0;
  sName = psName;
  psn->iMinWidth = 2*FLDNAMEWIDTH + 20;
  psn->iMinHeight = psn->iHeight = 200;
}

FieldItem::~FieldItem()
{
  delete lbObject;
}

void FieldItem::create()
{
  CPoint pnt = CPoint(psn->iPosX,psn->iPosY);
  zDimension dim = zDimension(psn->iWidth,psn->iHeight);
  DWORD style = LBS_NOTIFY | WS_VSCROLL | LBS_DISABLENOSCROLL |	LBS_HASSTRINGS | 
		            WS_BORDER | WS_TABSTOP;
  lbObject = new OwnerDrawListBox(this, style, CRect(pnt, dim), frm()->wnd(), 1000);   
  lbObject->SetFont(frm()->fnt);
  lbObject->setNotify(this,(NotifyProc)&FieldItem::DblClkObject, Notify(LBN_DBLCLK));
  if (_npChanged)
    lbObject->setNotify(_cb, _npChanged, Notify(LBN_SELCHANGE));
}

void FieldItem::show(int s)
{
  if (lbObject)
    lbObject->ShowWindow(s);
}

String FieldItem::sValue()
{
  int id = lbObject->GetCurSel();
	if (id == -1) return String("");
  CString s;
  lbObject->GetText(id, s);
  return String(s);
}

void FieldItem::StoreData()
{
  int id = lbObject->GetCurSel();
  CString s;
	if ( id == -1 ) return;
  lbObject->GetText(id, s);
  *sName = String(s);
  FormEntry::StoreData();
}

int FieldItem::DblClkObject(Event*)
{
  frm()->OnOK();
  return 1;
}

FieldProjection::FieldProjection(FormEntry* parent, String *psName)
: FieldItem(parent, psName)
{}

void FieldProjection::create()
{
  FieldItem::create();
  int iSize = 16000;
  char* sBuf = new char[iSize];
  String sPath = IlwWinApp()->Context()->sIlwDir();
  sPath &= "\\Resources\\Def\\projs.def";
  GetPrivateProfileString("Projections", NULL, "", sBuf, iSize, sPath.c_str());
  lbObject->AddString("Unknown");
  for (char* s = sBuf; *s; ++s) {
    lbObject->AddString(s);
    for (; *s; ++s);
  }
  delete sBuf;
  lbObject->SelectString(-1, (*sName).c_str());
}

FieldEllipsoid::FieldEllipsoid(FormEntry* parent, String *psName)
: FieldItem(parent, psName)
{
  psn->iMinWidth += 20;
}

void FieldEllipsoid::create()
{
  FieldItem::create();
  int iSize = 16000;
  char* sBuf = new char[iSize];
  String sPath = IlwWinApp()->Context()->sIlwDir();
  sPath &= "\\Resources\\Def\\ellips.def";
  GetPrivateProfileString("Ellipsoids", NULL, "", sBuf, iSize, sPath.c_str());
  lbObject->AddString("Sphere");
  for (char* s = sBuf; *s; ++s) {
    lbObject->AddString(s);
    for (; *s; ++s);
  }
  delete sBuf;
  lbObject->SelectString(-1, (*sName).c_str());
}

FieldDatum::FieldDatum(FormEntry* parent, String *psName)
: FieldItem(parent, psName) 
{
  psn->iMinHeight = psn->iHeight = 120;
}

void FieldDatum::create()
{
  FieldItem::create();
  //int iSize = 16000;
  //char* sBuf = new char[iSize];
  //String sPath = getEngine()->getContext()->sIlwDir();
  //sPath &= "\\Resources\\Def\\datum.def";
  //GetPrivateProfileString("Datums", NULL, "", sBuf, iSize, sPath.c_str());
	ILWIS::QueryResults results;

  String query("Select distinct name from Datums");
  getEngine()->pdb()->executeQuery(query, results);
  lbObject->AddString("Not Specified");
  for(int i = 0; i < results.size(); ++i) {
	  lbObject->AddString(results.get("name", i).c_str());
  }
  lbObject->SelectString(-1, (*sName).c_str());
}

FieldDatumArea::FieldDatumArea(FormEntry* parent, String *psName)
: FieldItem(parent, psName) 
{
  psn->iMinHeight = psn->iHeight = 80;
}

bool FieldDatumArea::fInit(const String& sDatum)
{
	lbObject->ResetContent();
	if ("" == sDatum) {
		Hide();
		return false;
	}
	ILWIS::QueryResults results;
	String query("Select area from Datums where name='%S'", sDatum);
	getEngine()->pdb()->executeQuery(query, results);
	if ( results.size() == 0) {
		Hide();
		return false;
	}

	for(int i = 0; i < results.size(); ++i) {
	  lbObject->AddString(results.get("area", i).c_str());
	}

	if ( lbObject->SelectString(-1, (*sName).c_str()) == -1)
		lbObject->SetCurSel(0);

	Show();
	return true;
}




