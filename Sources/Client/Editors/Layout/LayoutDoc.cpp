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
// LayoutDoc.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Map\LayoutObject.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Headers\constant.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Client\Base\IlwisDocTemplate.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\ScaleTextLayoutItem.h"
#include "Client\Editors\Layout\ScaleBarLayoutItem.h"
#include "Client\Editors\Layout\NorthArrowLayoutItem.h"
#include "Client\Editors\Layout\PictureLayoutItem.h"
#include "Client\Editors\Layout\MapBorderItem.h"
#include "Client\Editors\Layout\BitmapLayoutItem.h"
#include "Client\Editors\Layout\BoxLayoutItem.h"
#include <afxpriv.h>
#include "Headers\Hs\Layout.hs"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\flddat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LayoutDoc

IMPLEMENT_DYNCREATE(LayoutDoc, IlwisDocument)

BEGIN_MESSAGE_MAP(LayoutDoc, IlwisDocument)
	//{{AFX_MSG_MAP(LayoutDoc)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_ADD_MAPVIEW, OnAddMapView)
	ON_COMMAND(ID_ADD_TEXT, OnAddTextItem)
	ON_COMMAND(ID_ADD_PICTURE, OnAddPicture)
	ON_COMMAND(ID_ADD_BOX, OnAddBox)
	ON_COMMAND(ID_PAGEBORDER, OnPageBorder)
	ON_UPDATE_COMMAND_UI(ID_PAGEBORDER, OnUpdatePageBorder)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
dtViews is just used as DocTemplate to keep MFC happy
*/

LayoutDoc::LayoutDoc()
: dtViews(1, RUNTIME_CLASS(MapCompositionDoc), 
					   RUNTIME_CLASS(CFrameWnd), 
						 RUNTIME_CLASS(CView))
{
	fUseSerialize = true;
	szPaper = CSize(0,0);
	rectMargins = CRect(2000,2000,2000,2000);
	fPageBorder = false;
	// initialize current printer state
	m_hDevMode = NULL;
	m_hDevNames = NULL;

	if (0 == m_hDevMode) 
	{
		PAGESETUPDLG psd;
		memset(&psd, 0, sizeof(psd));
		psd.lStructSize = sizeof(psd);
		psd.Flags = PSD_MARGINS|PSD_INHUNDREDTHSOFMILLIMETERS|PSD_RETURNDEFAULT;
		psd.ptPaperSize = CPoint(szPaper.cx, szPaper.cy);
		psd.rtMargin = rectMargins;
		PageSetupDlg(&psd);
		szPaper = psd.ptPaperSize;
		rectMargins = psd.rtMargin;
		if (m_hDevMode != psd.hDevMode) {
			if (m_hDevMode != 0)
				GlobalFree(m_hDevMode);
			m_hDevMode = psd.hDevMode;
		}
		if (m_hDevNames != psd.hDevNames) {
			if (m_hDevNames != 0)
				GlobalFree(m_hDevNames);
			m_hDevNames = psd.hDevNames;
		}
	}	
}

LayoutDoc::~LayoutDoc()
{
// close all documents of dtViews and remove them from the list
// otherwise crashes will occur
	POSITION pos = dtViews.GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = dtViews.GetNextDoc(pos);
		dtViews.RemoveDocument(pDoc);
		pDoc->OnCloseDocument();
	}
	// free printer info
	if (m_hDevMode != NULL)	
		GlobalFree(m_hDevMode);
	if (m_hDevNames != NULL)
		GlobalFree(m_hDevNames);
}


BOOL LayoutDoc::OnNewDocument()
{
	if (!IlwisDocument::OnNewDocument())
		return FALSE;
	UpdateAllViews(0, hintITEM, 0);
	return TRUE;
}

BOOL LayoutDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	try {
		if (0 != lpszPathName) {
			FileName fn(lpszPathName);
			if (fn.sExt == ".mpv") {
				if (!OnNewDocument())
					return FALSE;
				MapCompositionDoc* mcd = new MapCompositionDoc;
				dtViews.AddDocument(mcd);
				if (!mcd->OnOpenDocument(lpszPathName))
					return FALSE;
				AddItem(new MapLayoutItem(this, mcd));
				return TRUE;
			}
			else if (fn.sExt != ".ilo")
				return FALSE;
		}
		DeleteContents();
		return IlwisDocument::OnOpenDocument(lpszPathName);
	}
	catch (ErrorObject& err)
	{
		err.Show();
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// LayoutDoc serialization

void LayoutDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		LayoutObject lo;
		ElementContainer& en = const_cast<ElementContainer&>(lo->fnObj);
		if (0 == en.em)
			en.em = new ElementMap;
		lo->Updated();
		lo->Store();
		en.em->SerializeSection(ar, "Ilwis");

    ObjectDependency objdep;
		list<LayoutItem*>::iterator iter;
	  for (iter = lli.begin(); iter != lli.end(); ++iter) 
		{
			LayoutItem* li = *iter;
      MapLayoutItem* mli = dynamic_cast<MapLayoutItem*>(li);
      if (mli) 
      {
      	FileName fnMapView = mli->mcd()->GetPathName();
        MapView mpv(fnMapView);
        objdep.Add(mpv);
      }
    }
    objdep.Store(lo.pointer());
		en.em->SerializeSection(ar, "ObjectDependency");

		ObjectInfo::WriteElement("Layout", "Paper Size", en, szPaper);
		ObjectInfo::WriteElement("Layout", "Paper Margins", en, rectMargins);
		ObjectInfo::WriteElement("Layout", "Page Border", en, fPageBorder);

		if (m_hDevMode) {
			LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock(m_hDevNames);
			LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(m_hDevMode);
			String sDriver = (LPCTSTR)lpDevNames + lpDevNames->wDriverOffset;
			ObjectInfo::WriteElement("Layout", "Driver", en, sDriver);
			String sDevice = (LPCTSTR)lpDevNames + lpDevNames->wDeviceOffset;
			ObjectInfo::WriteElement("Layout", "Device", en, sDevice);
			String sOutput = (LPCTSTR)lpDevNames + lpDevNames->wOutputOffset;
			ObjectInfo::WriteElement("Layout", "Output", en, sOutput);
			int iSizeDevMode = lpDevMode->dmSize + lpDevMode->dmDriverExtra;
			ObjectInfo::WriteElement("Layout", "DevMode Size", en, iSizeDevMode);
			ObjectInfo::WriteElement("Layout", "DevMode", en, (const char*)lpDevMode, iSizeDevMode);
			::GlobalUnlock(m_hDevNames);
			::GlobalUnlock(m_hDevMode);
		}

		long iItem = 0;
	  for (iter = lli.begin(); iter != lli.end(); ++iter) 
		{
			++iItem;
			LayoutItem* li = *iter;
			li->SetID(iItem);
			String sEntry("Item %i", iItem);
			ObjectInfo::WriteElement("Layout", sEntry.scVal(), en, li->sType());
			String sType = li->sType();
		}
		ObjectInfo::WriteElement("Layout", "Items", en, iItem);
		en.em->SerializeSection(ar, "Layout");
	
		iItem = 0;
	  for (iter = lli.begin(); iter != lli.end(); ++iter) 
		{
			++iItem;
			String sEntry("Item %i", iItem);
			LayoutItem* li = *iter;
			li->Serialize(ar, sEntry.scVal());
		}
	}
	else
	{
		ElementContainer en;
		en.em = new ElementMap;
		en.em->SerializeSection(ar, "Ilwis");
		en.em->SerializeSection(ar, "Layout");
		ObjectInfo::ReadElement("Layout", "Paper Size", en, szPaper);
		ObjectInfo::ReadElement("Layout", "Paper Margins", en, rectMargins);
		ObjectInfo::ReadElement("Layout", "Page Border", en, fPageBorder);

		int iSizeDevMode = 0;
		ObjectInfo::ReadElement("Layout", "DevMode Size", en, iSizeDevMode);
		if (iSizeDevMode > 0) {
			if (m_hDevMode != 0)
				GlobalFree(m_hDevMode);
			m_hDevMode = GlobalAlloc(GMEM_MOVEABLE, iSizeDevMode);
			LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(m_hDevMode);
			ObjectInfo::ReadElement("Layout", "DevMode", en, (char*)lpDevMode, iSizeDevMode);
			::GlobalUnlock(m_hDevMode);

			String sDriver;
			ObjectInfo::ReadElement("Layout", "Driver", en, sDriver);
			String sDevice;
			ObjectInfo::ReadElement("Layout", "Device", en, sDevice);
			String sOutput;
			ObjectInfo::ReadElement("Layout", "Output", en, sOutput);
			int iSizeDevNames = sizeof(DEVNAMES) + 4 + sDriver.length() + sDevice.length() + sOutput.length();
			m_hDevNames = GlobalAlloc(GMEM_MOVEABLE, iSizeDevNames);
			LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock(m_hDevNames);
			size_t iPos = sizeof(DEVNAMES);
			lpDevNames->wDriverOffset = iPos;
			char* p = (char*)lpDevNames + lpDevNames->wDriverOffset;
			strcpy(p, sDriver.scVal());
			iPos += strlen(p) + 1;
			lpDevNames->wDeviceOffset = iPos;
			p = (char*)lpDevNames + lpDevNames->wDeviceOffset;
			strcpy(p, sDevice.scVal());
			iPos += strlen(p) + 1;
			lpDevNames->wOutputOffset = iPos;
			p = (char*)lpDevNames + lpDevNames->wOutputOffset;
			strcpy(p, sOutput.scVal());
			iPos += strlen(p) + 1;
			lpDevNames->wDefault = iPos;
			p = (char*)lpDevNames + lpDevNames->wDefault;
			*p = 0;
			::GlobalUnlock(m_hDevNames);
		}

		long iItems = 0;
		ObjectInfo::ReadElement("Layout", "Items", en, iItems);
		for (int i = 1; i <= iItems; ++i) 
		{
			String sEntry("Item %i", i);
			String sType;
			LayoutItem* li = 0;
			ObjectInfo::ReadElement("Layout", sEntry.scVal(), en, sType);
			if (sType == "Text")
				li = new TextLayoutItem(this);
			else if (sType == "ScaleText")
				li = new ScaleTextLayoutItem(this);
			else if (sType == "ScaleBar")
				li = new ScaleBarLayoutItem(this);
			else if (sType == "NorthArrow")
				li = new NorthArrowLayoutItem(this);
			/*else if (sType == "Legend")
				li = new LegendLayoutItem(this);*/
			else if (sType == "MapView") {
				MapCompositionDoc* mcd = new MapCompositionDoc;
				dtViews.AddDocument(mcd);
				li = new MapLayoutItem(this, mcd);
			}
			else if (sType == "Picture")
				li = new PictureLayoutItem(this);
			else if (sType == "MapBorder")
				li = new MapBorderItem(this);
			else if (sType == "Bitmap")
				li = new BitmapLayoutItem(this);
			else if (sType == "Box")
				li = new BoxLayoutItem(this);
			if (0 == li) 
				continue;
			li->SetID(i);
			AddItem(li);
		}
		list<LayoutItem*>::iterator iter;
	  for (iter = lli.begin(); iter != lli.end(); ++iter) 
		{
			LayoutItem* li = *iter;
			String sEntry("Item %i", li->iID());
			li->Serialize(ar, sEntry.scVal());
		}
	}
}


void LayoutDoc::DeleteContents()
{
	list<LayoutItem*>::iterator iter;
  for (iter = lli.begin(); iter != lli.end(); ++iter) 
	{
		LayoutItem* li = *iter;
		delete li;
	}
	lli.clear();
	UpdateAllViews(0);
}

void LayoutDoc::AddItem(LayoutItem* li)
{
	lli.push_back(li);
	MinMax mmPos = li->mmPosition();
	li->SetPosition(mmPos, -1);
	UpdateAllViews(NULL, hintITEM, li);
}

LayoutItem* LayoutDoc::liAddMapView(const String& sMapView)
{
	struct KeepDir
	{
		KeepDir(const String& sDir)
		{ 
			sOldDir = IlwWinApp()->sGetCurDir();
			IlwWinApp()->SetCurDir(sDir);
		}
		~KeepDir()
		{
			IlwWinApp()->SetCurDir(sOldDir);
		}
	private:
		String sOldDir;
	};

	try
	{
		MapCompositionDoc* mcd = new MapCompositionDoc;
		dtViews.AddDocument(mcd);
		FileName fn(sMapView);
		KeepDir kd(fn.sPath());
		if (!mcd->OnOpenDocument(sMapView.scVal()))
			return FALSE;
		LayoutItem* li = new MapLayoutItem(this, mcd);
		AddItem(li);
		return li;
	}
	catch(const ErrorObject&)
	{
		return 0;
	}
}	
/*
void LayoutDoc::SetPrinterDC(HGLOBAL hDevNames, HGLOBAL hDevMode)
{
	if (dcPrinter.m_hDC)
		::DeleteDC(dcPrinter.Detach());
	HDC hDC = AfxCreateDC(hDevNames, hDevMode);
	dcPrinter.Attach(hDC);
}
*/

LayoutItem* LayoutDoc::liFindID(int iID) const
{
	list<LayoutItem*>::const_iterator iter;
  for (iter = lli.begin(); iter != lli.end(); ++iter) 
	{
		LayoutItem* li = *iter;
		if (li == 0)
			continue;
		if (li->iID()	== iID)
			return li;
	}
	return 0;
}

void LayoutDoc::OnAddTextItem()
{
	LayoutItem* li = new TextLayoutItem(this);
	if (li->fConfigure())
		AddItem(li);
	else
		delete li;
}

void LayoutDoc::OnEditPaste()
{
  wndGetActiveView()->OpenClipboard();
	LayoutItem* li = 0;

	UINT iClpFmt = EnumClipboardFormats(0);
	for (; 0 != iClpFmt && 0 == li; iClpFmt = EnumClipboardFormats(iClpFmt)) 
	{
		switch (iClpFmt)
		{
			case CF_ENHMETAFILE: {
				HENHMETAFILE hnd = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
				li = new PictureLayoutItem(this, hnd);
				break; }
			case CF_BITMAP: {
				HBITMAP hnd = (HBITMAP)GetClipboardData(CF_BITMAP);
				li = new BitmapLayoutItem(this, hnd);
				break; }
			case CF_TEXT: {
				HANDLE hnd = GetClipboardData(CF_TEXT);
				char* pc = (char*)GlobalLock(hnd);
				li = new TextLayoutItem(this, pc);
				GlobalUnlock(hnd);
				break; }
		}
	}
	CloseClipboard();
	if (li) {
		AddItem(li);
	}
}

void LayoutDoc::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL fPaste = IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		            IsClipboardFormatAvailable(CF_BITMAP) ||
		            IsClipboardFormatAvailable(CF_TEXT);
	pCmdUI->Enable(fPaste);
}

void LayoutDoc::OnAddMapView()
{
	class AddMapViewForm: public FormWithDest
  {
  public:
    AddMapViewForm(CWnd* parent, String* sName)
    : FormWithDest(parent, SLOTitleAddMapView)
    {
			new FieldDataTypeLarge(root, sName, ".mpv");
	    SetMenHelpTopic("ilwismen\\layout_editor_insert_map_view.htm");
      create();
    }
  };
  String sMapView;
  AddMapViewForm frm(wndGetActiveView(), &sMapView);
	if (frm.fOkClicked()) {
		liAddMapView(sMapView);	
	}
}

HENHMETAFILE LayoutDoc::ReadMetaFile(const char* str)	const
{
	HMETAFILE hm = GetMetaFile(str);
	int iSize = 0;
	byte* pBuf = 0;
	if (0 != hm) {
		iSize = GetMetaFileBitsEx(hm,0,NULL);
		pBuf = new byte[iSize];
		GetMetaFileBitsEx(hm,iSize,pBuf);
		DeleteMetaFile(hm);
	}
	else {
		ifstream file(str, ios::binary | ios::in /* | ios::nocreate*/);
		file.seekg(22);
		METAHEADER mh;
		file.read((char*)&mh,sizeof(mh));
		iSize = 2 * mh.mtSize;
		pBuf = new byte[iSize];
		file.seekg(22);
		file.read((char*)pBuf, iSize);
		file.close();
	}
	CClientDC dc(wndGetActiveView());
	HENHMETAFILE hMF = SetWinMetaFileBits(iSize,pBuf,dc.GetSafeHdc(),NULL);
	delete [] pBuf;

	return hMF;
}

LayoutItem* LayoutDoc::liAddPicture(const String& sPicture)
{
	FileName fn(sPicture);
	HENHMETAFILE hMF = 0;
	LayoutItem* li = 0;
	if (fn.sExt == ".emf")
		hMF = GetEnhMetaFile(sPicture.scVal());
	else if (fn.sExt == ".wmf") 
		hMF = ReadMetaFile(sPicture.scVal());
	if (hMF) {
		li = new PictureLayoutItem(this, hMF);
		DeleteEnhMetaFile(hMF);
	}
	if (fn.sExt == ".bmp") {
		HBITMAP hbm = (HBITMAP)LoadImage(NULL,sPicture.scVal(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		li = new BitmapLayoutItem(this, hbm);
		DeleteObject(hbm);
	}
	if (li)
		AddItem(li);
	return li;
}

void LayoutDoc::OnAddPicture()
{
	class AddPictureForm: public FormWithDest
  {
  public:
    AddPictureForm(CWnd* parent, String* sName)
    : FormWithDest(parent, SLOTitleAddPicture)
    {
      new FieldDataTypeLarge(root, sName, ".emf.wmf.bmp", true);
	    SetMenHelpTopic("ilwismen\\layout_editor_insert_bitmap_picture.htm");
      create();
    }
  };
  String sPicture;
  AddPictureForm frm(wndGetActiveView(), &sPicture);
	if (frm.fOkClicked()) 
		liAddPicture(sPicture);
}

void LayoutDoc::OnAddBox()
{
	LayoutItem* li = new BoxLayoutItem(this);
	if (li->fConfigure())
		AddItem(li);
	else
		delete li;
}

void LayoutDoc::RemoveItem(LayoutItem* li)
{
	lli.remove(li);
start:
	for (list<LayoutItem*>::iterator iter = lli.begin(); iter != lli.end(); ++iter) 
	{
		LayoutItem* item = *iter;
		if (item->fDependsOn(li)) {
			RemoveItem(item);
			goto start;
		}
	}
	delete li;
	UpdateAllViews(0, hintITEM, 0);
	SetModifiedFlag();
}

void LayoutDoc::OnPageBorder()
{
	fPageBorder = !fPageBorder;
	UpdateAllViews(0, hintPAGEBORDER);
}

void LayoutDoc::OnUpdatePageBorder(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(fPageBorder?1:0);
}

void LayoutDoc::OnFileNew()
{
	if (!SaveModified())
		return;
	OnNewDocument();
}

void LayoutDoc::OnFileOpen()
{
	if (!SaveModified())
		return;
	class OpenForm: public FormWithDest
  {
  public:
    OpenForm(CWnd* parent, String* sName)
    : FormWithDest(parent, SLOTitleOpenLayout)
    {
			new FieldDataTypeLarge(root, sName, ".ilo");
			SetMenHelpTopic("ilwismen\\layout_editor_open_layout.htm");
      create();
    }
  };
  String sLayout;
  OpenForm frm(wndGetActiveView(), &sLayout);
	if (frm.fOkClicked()) {
		OnOpenDocument(sLayout.scVal());
		SetModifiedFlag(FALSE);
	}
}

zIcon LayoutDoc::icon() const
{
	return zIcon("LayoutIcon");
}

void LayoutDoc::CalcBitmapSize(int iResDpi, int& iWidth, int& iHeight, int& iFileSize)
{
	double rPixPer001MM = iResDpi / 2540.0; // 25.4 mm/inch
	iWidth = rounding(szPaper.cx * rPixPer001MM);
	iHeight = rounding(szPaper.cy * rPixPer001MM);
	int iImgSize = ((((iWidth * 24) + 31) & ~31) >> 3) * iHeight;
	iFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + iImgSize;
}

CSize LayoutDoc::szGetPrintableSize() const
{
	return CSize(szPaper.cx - rectMargins.left -  rectMargins.right, szPaper.cy - rectMargins.top - rectMargins.bottom);
}

bool LayoutDoc::fIsEmpty() const
{
	return lli.size() == 0;
}
