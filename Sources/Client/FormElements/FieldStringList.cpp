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
 *************************************
 **************************/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FieldStringList.h"

FieldStringList::FieldStringList(FormEntry* par, String *_selected, vector<String> _content)
: GeneralMapSelector(par), selected(_selected), content(_content)
{
  style = LBS_SORT | LBS_HASSTRINGS;
}

void FieldStringList::create()
{
  GeneralMapSelector::create();
  resetContent(content);
  RecalcHScrollSize();
}

bool FieldStringList::fValid() {
	return lb != 0 && lb->m_hWnd != 0;
}

void FieldStringList::resetContent(vector<String> _content) {
  content = _content;
  lb->ResetContent();
  for(vector<String>::iterator cur = content.begin(); cur != content.end(); ++cur) 
  {
    String s= *cur;
    lb->AddString(s.sVal());
  }
}

int FieldStringList::iGetSelected(IntBuf& buf)
{
  int index = lb->GetCurSel();
  if ( index == LB_ERR)
	  return 0;

  buf.Size(1);
  buf[0] = index;
	
  return 1   ;
}