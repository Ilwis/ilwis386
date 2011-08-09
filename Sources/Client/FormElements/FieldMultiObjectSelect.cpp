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
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\FieldMultiObjectSelect.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Client\ilwis.h"


AttributeFileName::AttributeFileName(FileName& PFile) :
  FileName(PFile)
{
	sCol = PFile.sCol;
}
  
void FMOSItems::CleanUp()
{
	for(int i = 0 ; i < size(); ++i) 
		delete (*this)[i];
	resize(0);
}

FMOSItem::FMOSItem()
{
}

FMOSItem::FMOSItem(const FMOSItem& item)
{
	fnObject = item.fnItem();
}

FMOSItem::FMOSItem(const AttributeFileName& fn) :
	fnObject(fn)
{
}

bool FMOSItem::fEqual(FMOSItem *item)
{
	return fnObject == item->fnItem();
}

AttributeFileName FMOSItem::fnItem() const
{
	return fnObject;	
}

String FMOSItem::sItem() const
{
	return fnObject.sRelative();
}

FMOSItem* FMOSItem::Clone()
{
	return new FMOSItem(fnObject);
}

ObjectSelectorBase::ObjectSelectorBase(FormEntry* par) :
	BaseSelector(par),
    npSelection(0),
	cbh(0)
{
  psn->iMinWidth *= 2;
}

ObjectSelectorBase::~ObjectSelectorBase()
{
	for(int i = 0 ; i < iGetCount(); ++i)
		delete vfiItems[i];
}

FMOSItem* ObjectSelectorBase::fiGetItem(int id)
{
	if ( id >= 0 && id < vfiItems.size())
		return vfiItems[id];
	
	throw ErrorObject("Incorrect index in ObjectSelector used");
}

String ObjectSelectorBase::sName(int id)
{
	return fiGetItem(id)->sItem();
}


void ObjectSelectorBase::create()
{
  BaseSelector::create();
  lb->setNotify(this, (NotifyProc)&ObjectSelectorBase::DrawItem, WM_DRAWITEM);
  if ( npSelection != 0 )
  {
    NotificationEvent ne(lb->GetDlgCtrlID(), LBN_SELCHANGE, (LPARAM)lb->m_hWnd);
	lb->setNotify(cbh, npSelection, ne.message());
	lb->SetShowPopup(true);	
  }		
}

void ObjectSelectorBase::StoreData()
{
  // empty implementation!
}

void ObjectSelectorBase::ResetContent()
{
	lb->ResetContent();
	vfiItems.CleanUp();
}

void ObjectSelectorBase::SetSelectedItem(const FMOSItem& item)
{
  if ( item.sItem() == "")	
  {
	  lb->SetSel(-1, FALSE);
  }
  
  int id = lb->FindString(-1, item.sItem().c_str());
  if ( id < 0)
  {
	  lb->SetSel(-1, FALSE);
	  return;
  }		  
  
  lb->SetSel(id);
}

bool ObjectSelectorBase::fIsItemSelected(int id)
{
  return 0 != lb->GetSel(id); 
}

void ObjectSelectorBase::GetSelectedItems(vector<FMOSItem *>& buf)
{
  int iCnt = lb->GetSelCount(); 
  int* pi = new int[iCnt];
  iCnt = lb->GetSelItems(iCnt, pi);
  buf.resize(iCnt);
  for (int i = 0; i < iCnt; ++i)
    buf[i] = fiGetItem(pi[i])->Clone();
  delete [] pi;

}

void ObjectSelectorBase::SetSelectionFunction(CallBackHandler *cb,NotifyProc func)
{
	npSelection = func;
	cbh = cb;
}

void ObjectSelectorBase::AddItem(FMOSItem* item)
{
	FMOSItem *newitem = item->Clone();
	FMOSItems::iterator where = find(vfiItems.begin(), vfiItems.end(), newitem);
	int i = 0;
	for( ;i < vfiItems.size(); ++i)
	{
		if ( item->fEqual(vfiItems[i]))
			break;
	}
	
	if ( i >= vfiItems.size())
	{
		vfiItems.push_back(newitem);
		lb->AddString(newitem->sItem().c_str());
	}
	else
		delete newitem;
	delete item;
	item = 0;
}

void ObjectSelectorBase::ChangeItem(int iIndex, FMOSItem *item)
{
	if ( item == 0)
		return;
	
	if ( iIndex >= 0 && iIndex < iGetCount())
	{
		delete vfiItems[iIndex];
		vfiItems[iIndex] = item;
		lb->DeleteString(iIndex);
		lb->InsertString(iIndex, item->sItem().c_str());
	}
}

void ObjectSelectorBase::DeleteItem(int iIndex)
{
	if ( iIndex >= 0 && iIndex < iGetCount())
	{   
		FMOSItems::iterator here = vfiItems.begin();;
		for(int i = 0 ; i < iGetCount(); ++i, ++here )
			if ( i == iIndex) break;

		delete vfiItems[iIndex];
		vfiItems.erase(here);
		lb->DeleteString(iIndex);
	}		
}

int ObjectSelectorBase::iGetCount()
{
	return vfiItems.size();
}

InputObjectSelector::InputObjectSelector(FormEntry* par, ObjectLister *lister)
: ObjectSelectorBase(par),
  olLister(lister)
{
  style = LBS_SORT | LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_HASSTRINGS;
}

InputObjectSelector::~InputObjectSelector()
{
	delete olLister;
}

void InputObjectSelector::create()
{
  ObjectSelectorBase::create();
  FillListBox();

}

void InputObjectSelector::FillListBox()
{
  ResetContent();
  FilenameIter fi("*.mpr");
  while(fi.fValid())
  {
    String s=String(*fi).toLower();
    AttributeFileName fn(s);
    try {
      if (fOK(fn))
	  {
		FMOSItem *fm = new FMOSItem(s);
        AddItem(fm);
	  }			
    }
    catch (ErrorObject& err) {
      err.Show();
    }

    ++fi;
  }	
}

void InputObjectSelector::SetObjectLister(ObjectLister *ol)
{
	if ( olLister)
	{
		delete olLister;
	}
	olLister = ol;
	FillListBox();
}

bool InputObjectSelector::fOK(const AttributeFileName& fn)
{
	if (olLister)
		return olLister->fOK(fn);
	return true; // if no object lister is set, all files are acceptable.
}

SelectedObjectsSelector::SelectedObjectsSelector(FormEntry* par, vector<AttributeFileName>& selected) :
 ObjectSelectorBase(par),
	 arrSelected(selected),
 fnt(0)
{
	if (arrSelected.size() > 0 && arrSelected.size() <= 9)
		style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | LBS_HASSTRINGS;
	else
		style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_HASSTRINGS;
}

SelectedObjectsSelector::~SelectedObjectsSelector()
{
}

void SelectedObjectsSelector::create()
{
  ObjectSelectorBase::create();
  lb->ResetContent();
  if ( fnt != 0 )
	lb->SetFont(fnt);

  int iLow = 0;
  for (int i = 0; i < arrSelected.size(); ++i) 
  {
    AddItem(new FMOSItem(arrSelected[iLow+i]));
  }
  lb->SendMessage(LB_SETCARETINDEX,0,0);
}

void SelectedObjectsSelector::StoreData()
{
  int iNr = iGetCount();
  arrSelected.resize(iNr);

  int iLow = 0;
  for (int i = 0; i < iNr; ++i)
    arrSelected[iLow+i] = fiGetItem((i))->fnItem();
}

void SelectedObjectsSelector::SetFont(CFont *font)
{
	fnt = font;
}

FieldMultiObjectSelectBase::FieldMultiObjectSelectBase(FormEntry* par, const String& sQuestion, ObjectLister *lister, vector<AttributeFileName>& lfnSelected ) :
  FieldGroup(par, true),
  mosInput(0)
  
{
 /* sts = 0;
  if ("" != sQuestion) {
      }
*/
	if ( lfnSelected.size() == 0 )  
	{
		fgAll = new FieldGroup(this);	  
		sts = new StaticTextSimple(fgAll, sQuestion);
		sts->SetIndependentPos();
		fgLeft = new FieldGroup(this);
		fgMiddle = new FieldGroup(this);
		fgRight = new FieldGroup(this);
		fgRight->Align(fgMiddle, AL_AFTER);
		fgMiddle->Align(fgLeft, AL_AFTER);
		
		fb = new FieldBlank(fgMiddle, 2);
		pbAdd = new PushButton(this, " > ", (NotifyProc)&FieldMultiObjectSelectBase::Add, true);
		pbAdd->Align(fb, AL_UNDER);
		pbDel = new PushButton(this, " < ", (NotifyProc)&FieldMultiObjectSelectBase::Del, true);
		pbDel->Align(pbAdd, AL_UNDER);
		pbAdd->SetWidth(30);
		pbDel->SetWidth(30);
	}
	else
	{
		fgRight = new FieldGroup(this);
	}		

}

void FieldMultiObjectSelectBase::create()
{
  FieldGroup::create();
}

String FieldMultiObjectSelectBase::sGetText()
{
	if (mosInput)
		return mosInput->sGetText() + "\n" + ossSelected->sGetText();
	else
		return ossSelected->sGetText();
}

int FieldMultiObjectSelectBase::Add(Event*)
{
	if ( mosInput == 0 )
		return 0;
	
	ossSelected->SetSelectedItem(FMOSItem(""));
	FMOSItems buf;

	mosInput->GetSelectedItems(buf);
	int id = -1;

	for (int i = 0; i < buf.size(); ++i)
	{
		ossSelected->AddItem(buf[i]);		
		mosInput->SetSelectedItem(*buf[i]);
	}
	DoCallBack();
	return 0;
}

int FieldMultiObjectSelectBase::Del(Event*)
{
	int iNr = ossSelected->iGetCount();
	for (int i = iNr - 1; i >= 0; --i) 
	{
		if (ossSelected->fIsItemSelected(i)) 
		{
		  ossSelected->DeleteItem(i);
		}
	}
	DoCallBack();
	return 0;
}

void FieldMultiObjectSelectBase::SetObjectLister(ObjectLister *lister)
{
	if (lister && mosInput != 0)
		mosInput->SetObjectLister(lister);
}

FieldMultiObjectSelect::FieldMultiObjectSelect(FormEntry* par, const String& sQuestion, ObjectLister *lister, vector<AttributeFileName>& arrSelected) :
	FieldMultiObjectSelectBase(par, sQuestion, lister, arrSelected)
{
  mosInput = new InputObjectSelector(this, lister);		
  ossSelected = new SelectedObjectsSelector(this, arrSelected);
}
