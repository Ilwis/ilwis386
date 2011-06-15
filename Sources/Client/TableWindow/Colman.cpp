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
/* ColumnManageForm, ColumnSelector
   by Wim Koolhoven, may 1996
  (c) Ilwis System Development ITC
	Last change:  WK   14 Jul 97    9:02 am
*/  
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\COLMAN.H"
#include "Engine\Table\tblview.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblinfo.h"
//#include "Client\MainWindow\CommandCombo.h"
#include "Client\TableWindow\ColumnPropForm.h"
#include "Engine\Table\tblview.h"
#include "Headers\Htp\Ilwis.htp"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
//#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\TableWindow\TableDoc.h"
//#include "Client\TableWindow\TableWindow.h"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Mainwind.hs"
/*
extern "C" {
// from Undocumented Windows
DWORD FAR PASCAL DragObject(HWND,HWND,WORD,WORD,NPSTR,HANDLE);	// USER.464
BOOL FAR PASCAL DragDetect(HWND,LPPOINT);  // USER.465
}
*/
ColumnSelector::ColumnSelector(FormEntry* par, ColumnManageForm* frm, TablePtr* ptr)
  : BaseSelector(par),
    curArrow(Arrow),
    tbl(ptr), cmf(frm)
{
  style = WS_VSCROLL | LBS_WANTKEYBOARDINPUT;
  fDrag = false;
  cur = 0;
  psn->iMinWidth *= 1.5;
}

ColumnSelector::~ColumnSelector()
{
}

void ColumnSelector::create()
{
  BaseSelector::create();
  lb->setNotify(cmf, (NotifyProc)&ColumnManageForm::Select, Notify(LBN_SELCHANGE));
  lb->setNotify(cmf,(NotifyProc)&ColumnManageForm::Prop, Notify(LBN_DBLCLK));
// drag and drop still not implemented:
	lb->setNotify(this,(NotifyProc)&ColumnSelector::mouseButtonDown, WM_LBUTTONDOWN);
	lb->setNotify(this,(NotifyProc)&ColumnSelector::mouseButtonUp, WM_LBUTTONUP);
  lb->setNotify(cmf,(NotifyProc)&ColumnManageForm::OnKeyDown, WM_KEYDOWN);
    Fill();
  SendMessage(*lb,LB_SETTOPINDEX,0,0);
}

int ColumnSelector::idSelect()
{
  return lb->GetCurSel();
}

long ColumnSelector::DrawItem(DRAWITEMSTRUCT* dis)
{
  int id = dis->itemID;
  if (/*dis->itemAction & ODA_SELECT ||*/ id < 0)
    return 1;
  String s;
  s = sName(id);
  idw.DrawItem(dis,s,false,true,false);
  return 1;
}

void ColumnSelector::Fill()
{
  lb->ResetContent();
  for (int i = 0; i < tbl->iCols(); ++i) 
	{
		if ( tbl->col(i).ptr() == NULL)
			continue;
    String s = tbl->col(i)->sName();
		s &= ".clm";
    lb->AddString(s.scVal());
  }
}

int ColumnSelector::mouseButtonDown(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);  
	if (0 == ev)
		return 0;
	int iTop = SendMessage(*lb,LB_GETTOPINDEX,0,0);
  int iHeight = SendMessage(*lb,LB_GETITEMHEIGHT,0,0);
  idDrag = iTop + ev->pos().y / iHeight;
  if (idDrag < lb->GetCount()) {
    if (DragDetect(*lb,ev->pos())) {
			if (cur)
				delete cur;
			fDrag = true;
			cur = new zCursor("Clm16Cur");
    	::SetCapture(*lb);
			zRect rect;
			lb->GetWindowRect(rect);
			rect.top() -= 20;
			rect.bottom() += 20;
			ClipCursor((RECT*)&rect);
			::SetCursor((HCURSOR)*cur);
		}
	}
	return 0;
}

int ColumnSelector::mouseButtonUp(Event* Evt)
{
  MouseLBUpEvent *ev=dynamic_cast<MouseLBUpEvent *>(Evt);  
	if (0 == ev)
		return 0;
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
  int iTop = SendMessage(*lb,LB_GETTOPINDEX,0,0);
  int iHeight = SendMessage(*lb,LB_GETITEMHEIGHT,0,0);
  int id = iTop + ev->pos().y / iHeight;
  SendMessage(*lb,LB_SETSEL,0,MAKELPARAM(id,0));
  if (id != idDrag) {
    if (id < 0)
      id = 0;
    else if (id >= tbl->iCols())
      id = tbl->iCols() - 1;
    TableView* tvw = dynamic_cast<TableView*>(tbl);
    tvw->MoveCol(idDrag,id);
    Fill();
    PostMessage(*lb,LB_SETTOPINDEX,id,0);
    lb->SetSel(id);
  }
  return 0;
}

Column ColumnSelector::col(int id)
{
  return tbl->col(id);
}

String ColumnSelector::sName(int id)
{
  String s =  tbl->col(id)->sName();
	s &= ".clm";
  return s;
}

void ColumnSelector::StoreData()
{
}

ColumnManageForm::ColumnManageForm(TableDoc* tdoc, TableView* view)
: FormWithDest(tdoc->wndGetActiveView(), STBTitleColManagement, false/* no cancel button*/ ),
  tvw(view), tbldoc(tdoc)
{
  cs = new ColumnSelector(root,this,tvw);
  String sFill('x', 50);
  stTypeName = new StaticText(root, sFill);
  stTypeName->SetIndependentPos();
  stTypeName->psn->SetBound(0,0,0,0);
  stDescr = new StaticText(root, sFill);
  stDescr->SetIndependentPos();
  stDescr->psn->SetBound(0,0,0,0);
  stRemark = new StaticText(root, sFill);
  stRemark->SetIndependentPos();

  pbAdd = new PushButton(root, STBUiAddColumn, (NotifyProc)&ColumnManageForm::Add);
  pbAdd->SetIndependentPos();
  pbProp = new PushButton(root, STBUiProperties, (NotifyProc)&ColumnManageForm::Prop);
  pbProp->Align(pbAdd, AL_AFTER);
  pbProp->SetIndependentPos();
  pbRemove = new PushButton(root, STBUiDelete, (NotifyProc)&ColumnManageForm::Remove);
  pbRemove->Align(pbAdd, AL_UNDER);
  pbRemove->SetIndependentPos();
  pbMakeUpToDate = new PushButton(root, STBUiMakeUpToDate, (NotifyProc)&ColumnManageForm::MakeUpToDate);
  pbMakeUpToDate->Align(pbRemove, AL_AFTER);
  pbMakeUpToDate->SetIndependentPos();
  StaticText* st = new StaticText(root, STBRemDragToOrder);
  st->Align(pbRemove, AL_UNDER);
  st->SetIndependentPos();
  st->SetCallBack((NotifyProc)&ColumnManageForm::Select);
	
  SetMenHelpTopic("ilwismen\\column_management.htm");
  create();
}

int ColumnManageForm::OnKeyDown( Event* ev) 
{
	int nKey = ev->wParm;
   if (nKey == VK_INSERT) {
      Add(0);
      return -2;
    }
    else if (nKey == VK_DELETE) {
      Remove(0);
      return -2;
    }
  return -1;
}

int ColumnManageForm::Select(Event* Evt)
{
  int id = cs->idSelect(); 
  if (id < 0) {
    pbProp->disable();
    pbRemove->disable();
    stTypeName->SetVal("");
    stDescr->SetVal("");
    stRemark->SetVal("");
    pbMakeUpToDate->Hide();
  }
  else {
    pbProp->enable();
    pbRemove->enable();
    Column col = cs->col(id);
    stTypeName->SetVal(col->sTypeName());
    stDescr->SetVal(col->sDescription);
    bool fMakeUpToDate = false;
    if (col->fOwnedByTable())
      stRemark->SetVal(STBRemColIsTblOwned);
    else if (col->fDependent()) {
      ObjectTime timNewest = 0;
      String sObjName, s;
      col->GetNewestDependentObject(sObjName, timNewest);
      switch (timNewest) {
        case 0:
          s = SMSRemObjectIsUpToDate;
          break;
        case -1:
          s = String("%S: %S", SMSRemObjectMissing, sObjName);
          break;
        default: {
          s = String("%S: %S (%S)", SMSRemObjectIsNotUpToDate, sObjName, timNewest.sDateTime());
          fMakeUpToDate = true;
        }
      }
      stRemark->SetVal(s);
    }
    else if (col->fReadOnly())
      stRemark->SetVal(STBRemColIsReadOnly);
    else
      stRemark->SetVal(col->objtime.sDateTime());
    if (fMakeUpToDate)
      pbMakeUpToDate->Show();
    else
      pbMakeUpToDate->Hide();
  }
  return 1;
}

int ColumnManageForm::Prop(Event* Evt)
{
  int id = cs->idSelect(); //Evt->wParam;
  if (id < 0) 
    return 1;
  int iDummy = -1;
  ColumnPropForm frm(this, tvw->cv(id), &iDummy);
  return 1;
}

int ColumnManageForm::Add(Event* Evt)
{
  tbldoc->OnAddColumn();
  cs->Fill();
  Select(0);
  return 0;
}

int ColumnManageForm::Remove(Event* Evt)
{
  int id = cs->idSelect();
  if (id < 0) 
    return 1;
	if (tbldoc->fDelColumn(id)) {
    cs->Fill();
    Select(0);
		}
  return 0;
}

int ColumnManageForm::MakeUpToDate(Event* Evt)
{
  int id = cs->idSelect();
  if (id < 0) 
    return 1;
  Column col = cs->col(id);
  String s1(SMSMsgIsNotUpToDate_S.scVal(), col->sTypeName());
  String s("%S\n%S", s1, SMSMsgRecalcMakeUpToDate);
  if (IDYES == MessageBox(s.scVal(), SMSMsgCheckUpToDate.scVal(), MB_YESNO|MB_ICONINFORMATION)) {
    col->MakeUpToDate();
    Select(0);
  }
  return 0;
}
