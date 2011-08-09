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
#include "Client\ilwis.h"
#include "Client\Base\Picture.h"
#include "Client\FormElements\FieldPicture.h"


FieldPicture::FieldPicture(FormEntry* par,  NotifyProc np, FormEntry* _alternativeCBHandler, bool _keepSize)
  : FormEntry(par,0, true), keepSize(_keepSize)
{
  //psn->iMinWidth = psn->iWidth = FLDNAMEWIDTH + 100;
  //psn->iMinHeight = psn->iHeight = 250;
  pb = 0;
  _npLeft = np;
  _npRight = NULL;
  alternativeCBH = _alternativeCBHandler;
}

FieldPicture::~FieldPicture()
{
}

void FieldPicture::show(int v) {
	if ( pb)
		pb->ShowWindow(v)  ;   
}

bool FieldPicture::Load(unsigned char *buf, int len) {
	bool ret =  picture.Load(buf,len) == TRUE;
	if ( pb)
		pb->Invalidate();
	return ret;
}

bool FieldPicture::Load(const FileName& fnPicture) {
	bool ret =  picture.Load(fnPicture.sFullPath().c_str()) == TRUE;
	if ( pb)
		pb->Invalidate();
	return ret;
}

bool FieldPicture::LoadResource(const String& resID) {
	BOOL ret = picture.Load(IlwWinApp()->m_hInstance, resID);
	if ( ret && keepSize) {
		CSize sz = picture.GetImageSize();
		psn->iMinWidth = psn->iWidth = sz.cx;
		psn->iMinHeight = psn->iHeight = sz.cy;
	}
	return ret == 1;
}

bool FieldPicture::fValid() {
	return true;
}
void FieldPicture::create() {
  zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iMinWidth, psn->iMinHeight - 1);
  pb = new ZappButton(this, _frm->wnd(), CRect(pntFld, dimFld), WS_TABSTOP | BS_ICON | BS_OWNERDRAW, 0, Id());
  if ( alternativeCBH) {
	  pb->setNotify(alternativeCBH, _npLeft, Notify(BN_CLICKED));
	  if ( _npRight) pb->setNotify(alternativeCBH, _npRight, Message(WM_RBUTTONUP));
  }
  else {
	pb->setNotify(_frm->wnd(), _npLeft, Notify(BN_CLICKED));
	if ( _npRight) pb->setNotify(_frm->wnd(), _npRight, Notify(WM_RBUTTONUP));
  }
  CreateChildren();
}

void FieldPicture::DrawItem(Event* ev)
{
	DrawItemEvent *dev = dynamic_cast<DrawItemEvent *>(ev);
	DRAWITEMSTRUCT *dis = reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);
//	OwnButtonSimple::DrawItem(dis);
	if ( keepSize) {
		CSize sz = picture.GetImageSize(dis->hDC);
		CRect rct(0,0,sz.cx,sz.cy);
		picture.Render(dis->hDC,&rct);
	}

	picture.Render(dis->hDC, &(dis->rcItem));
}

void FieldPicture::ClearData() {
   picture.Free();
   pb->Invalidate();
}