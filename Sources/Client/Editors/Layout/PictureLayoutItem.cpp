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
// PictureLayoutItem.cpp: implementation of the PictureLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\PictureLayoutItem.h"
#include "Engine\Base\File\ElementMap.h"
#include "Headers\Hs\Layout.hs"
#include "Client\Mapwindow\ZoomableView.h"
#include "Headers\constant.h"
#include "Client\Editors\Layout\LayoutDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PictureLayoutItem::PictureLayoutItem(LayoutDoc* ld)
	: LayoutItem(ld)
{
	hMF = 0;
}


PictureLayoutItem::PictureLayoutItem(LayoutDoc* ld, HENHMETAFILE hnd)
	: LayoutItem(ld)
{
	hMF = CopyEnhMetaFile(hnd,NULL);
}

PictureLayoutItem::~PictureLayoutItem()
{
	if (hMF)
		DeleteEnhMetaFile(hMF);
}

void PictureLayoutItem::Serialize(CArchive& ar, const char* sSection)
{
	ElementContainer en;
	en.em = new ElementMap;
	if (ar.IsStoring()) {
		WriteElements(en, sSection);
		ObjectInfo::WriteElement(sSection, "Isotropic", en, m_fIsotropic); 	
		int iSize = GetEnhMetaFileBits(hMF,0,0);
		ObjectInfo::WriteElement(sSection, "MetaFileSize", en, iSize); 	
		en.em->SerializeSection(ar, sSection);
		BYTE* pBuf = new BYTE[iSize];
		GetEnhMetaFileBits(hMF,iSize,pBuf);
		ar.Write(pBuf,iSize);
		delete [] pBuf;
	}
	else {
		en.em->SerializeSection(ar, sSection);
		ReadElements(en, sSection);
		ObjectInfo::ReadElement(sSection, "Isotropic", en, m_fIsotropic); 	
		int iSize;
		ObjectInfo::ReadElement(sSection, "MetaFileSize", en, iSize); 	
		BYTE* pBuf = new BYTE[iSize];
		ar.Read(pBuf,iSize);
		if (hMF)
			DeleteEnhMetaFile(hMF);
		hMF = SetEnhMetaFileBits(iSize,pBuf);
		delete [] pBuf;
	}  
}

void PictureLayoutItem::ReadElements(ElementContainer& ec, const char* sSection)
{
	LayoutItem::ReadElements(ec, sSection);
}

void PictureLayoutItem::WriteElements(ElementContainer& ec, const char* sSection)
{
	LayoutItem::WriteElements(ec, sSection);
}

void PictureLayoutItem::OnDraw(CDC* cdc)
{
	if (0 == hMF) 
		return;
	CRect rct = rectPos();
	cdc->PlayMetaFile(hMF, &rct);
}

bool PictureLayoutItem::fConfigure()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(CWnd* p, bool* fIsotropic)
    : FormWithDest(p, TR("Edit Picture"))
    {
      new CheckBox(root, TR("&Isotropic"), fIsotropic);
      create();
    }
  };
  ConfigForm frm(ld->wndGetActiveView(), &m_fIsotropic);
  return frm.fOkClicked();
}

String PictureLayoutItem::sType() const
{
	return "Picture";
}

String PictureLayoutItem::sName() const
{
	return TR("Picture");
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
void PictureLayoutItem::OnContextMenu(CWnd* wnd, CPoint pt) 
{
	ZoomableView* zv = dynamic_cast<ZoomableView*>(wnd);
	CMenu men;
	men.CreatePopupMenu();
	if (zv) {
		pmadd(ID_NORMAL);
		pmadd(ID_ZOOMIN);
		pmadd(ID_ZOOMOUT);
		men.AppendMenu(MF_SEPARATOR);
	}
	pmadd(ID_EDIT_CLEAR);
  int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pt.x, pt.y, wnd);
	wnd->SendMessage(WM_COMMAND, iCmd);
}

bool PictureLayoutItem::fIsotropic() const
{
	return m_fIsotropic;
}

double PictureLayoutItem::rHeightFact() const
{
	ENHMETAHEADER emh;
	GetEnhMetaFileHeader(hMF, sizeof(emh), &emh);
	int iWidth = emh.rclBounds.right - emh.rclBounds.left;
	int iHeight = emh.rclBounds.bottom - emh.rclBounds.top;
	return double(iHeight) / iWidth;
}

bool PictureLayoutItem::fAddExtraClipboardItems()
{
	HENHMETAFILE hnd = CopyEnhMetaFile(hMF,NULL);
	SetClipboardData(CF_ENHMETAFILE, hnd);	
	return false;
}
									 
