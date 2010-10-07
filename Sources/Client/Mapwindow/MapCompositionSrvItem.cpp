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
// MapCompositionSrvItem.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// MapCompositionSrvItem

MapCompositionSrvItem::MapCompositionSrvItem(COleServerDoc* pServerDoc, BOOL bAutoDelete)
		: COleServerItem( pServerDoc, bAutoDelete)
{
	SetItemName("MapView");
	GetDataSource()->DelayRenderFileData(CF_DSPTEXT);

  hmodMsimg32 = LoadLibrary("msimg32.dll");
  if (hmodMsimg32 <= 0)
  {
    hmodMsimg32 = 0;
    alphablendfunc = 0;
  }
  else {
    alphablendfunc = (AlphaBlendFunc)GetProcAddress(hmodMsimg32, "AlphaBlend");
  }
	fViewPort = false;	
	MapCompositionDoc* mcd = GetDocument();	
	m_rcOffset = RowCol(0,0); //mcd->rcPrefOffset();
}

MapCompositionSrvItem::~MapCompositionSrvItem()
{
  FreeLibrary(hmodMsimg32);
}

BOOL MapCompositionSrvItem::OnDrawEx(CDC* pDC, DVASPECT nDrawAspect, CSize& rSize)
{
	ASSERT_VALID(pDC);
	ASSERT(AfxIsValidAddress(&rSize, sizeof(CSize)));
	if (nDrawAspect != DVASPECT_CONTENT)
		return FALSE;

	return OnDraw(pDC, rSize);
}
		
BOOL MapCompositionSrvItem::OnDraw(CDC* cdc, CSize&)
{
	// Called by the framework to render the OLE item into a metafile.
	// The metafile representation of the OLE item is used to display the 
	// item in the container application. If the container application was 
	// written with the Microsoft Foundation Class Library, the metafile is 
	// used by the Draw member function of the corresponding COleClientItem object.
	// There is no default implementation. You must override this function to draw
	// the item into the device context specified. 
//	MapCompositionDoc* mcd = GetDocument();
//	if (0 == mcd)
//		return FALSE;
//	if (!mcd->georef.fValid())
//		return FALSE;
//	if ( fViewPort == false)
//	{
//		MinMax mm =mcd->mmBounds();
//		RowCol rc = RowCol(mm.MaxRow() - m_rcOffset.Row, mm.MaxCol() - m_rcOffset.Col);
//		SetOffsetSize(m_rcOffset, rc);
//	}		
//
//	double rScale = mcd->rPrefScale();	
//	if (rScale < 0)
//		rScale = 10000;
//	double rPixSize = mcd->georef->rPixSize();
//	if (rPixSize < 0) // no pixelsize should not prevent drawing
//		rPixSize = 1;
//	double rFact = rPixSize / rScale * 10000; // 0.1 mm units
//
//	CSize size;
//	OnGetExtent(DVASPECT_CONTENT, size);
//	POSITION pos = mcd->GetFirstViewPosition();
//	CView* pFirstView = mcd->GetNextView( pos );
//	
//	// convert from 0.01 to 0.1 mm units
//	size.cx /= 10;
//	size.cy /= 10;
//	if (size.cx == 0 || size.cy == 0)
//		return FALSE;
//	MinMax mm2, mm = mcd->mmBounds();
//	mm.rcMin = rcOffset();
//	mm.MaxCol() = mm.MinCol() + (long)(size.cx / rFact) ;
//	mm.MaxRow() = mm.MinRow() + (long)(size.cy / rFact);
//	//mm.MaxCol() = min(mm2.MaxCol(), mm.MaxCol());
//	//mm.MaxRow() = min(mm2.MaxRow(), mm.MaxRow());
//
//	MinMax mmMf(RowCol(0,0), RowCol(mm.height(), mm.width()));
//  MetafilePositioner psnMf(mm,mcd->georef,1/rFact); 
//	CRect rect = psnMf.rectSize();
//
//	cdc->SetWindowOrg(rect.TopLeft());
//	cdc->SetWindowExt(rect.Size());
//
//Start:
//	int iState = mcd->iState();
//	bool fDummyDraw = false;
//
//	if (Color(255,255,255) != mcd->colBackground) // if not white
//	{
//		CPen penRect(PS_SOLID,1,mcd->colBackground);
//		CBrush brRect(mcd->colBackground);
//		CPen* penOld = cdc->SelectObject(&penRect);
//		CBrush* brOld = cdc->SelectObject(&brRect);
//		cdc->Rectangle(rect);
//		cdc->SelectObject(penOld);
//		cdc->SelectObject(brOld);
//	}
//	list<Drawer*>::iterator iter;
//  for (iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
//	{
//		Drawer* dr = *iter;
//		dr->Setup();
//	}
//  for (iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
//	{
//		if (iState != mcd->iState()) 
//			goto Start;
//		Drawer* dr = *iter;
//		MapDrawer *drwMap = dynamic_cast<MapDrawer *>(dr);			
//    if (0 != drwMap)
//    {
//			MinMax mmBand = mm;
//			CSize czMF = rect.Size();
//			const int iMAXBANDPIXELS = (long)3e6;
//			
//			long iMaxRowPerBand = (long)(iMAXBANDPIXELS / (double)mm.width());
//			
//			int iMaxBand = (long)(1.0 + mm.height() / iMaxRowPerBand);
//			mmBand.MaxRow() = min(mmBand.MaxRow(), mmBand.MinRow() + mm.height() / iMaxBand );			
//
//			
//			CDC dcBm;
//			CBitmap bm;
//			CClientDC dc(NULL);
//			bm.CreateCompatibleBitmap(&dc,mmBand.width(),mmBand.height());
//			dcBm.CreateCompatibleDC(&dc);
//			CBitmap* bmOld = dcBm.SelectObject(&bm);
//			CGdiObject* brOld = dcBm.SelectStockObject(WHITE_BRUSH);
//			CGdiObject* penOld = dcBm.SelectStockObject(WHITE_PEN);
//			int iHeight = mmBand.height();
//			int iMapRow = 0;
//			int iScreenLine = 0;
//			for(int iBand=0; iBand < iMaxBand; ++iBand)
//			{
//				BitmapPositioner psnBm(1,mmBand,mcd->georef);  						
//				CRect rctBand(0, 0,mmBand.width(),mmBand.height());
//	      dcBm.Rectangle(rctBand);
//				dr->draw(&dcBm, rctBand, &psnBm, &fDummyDraw);
//	 			cdc->SetStretchBltMode(COLORONCOLOR); // throw away superfluous rows/columns
//	 			cdc->StretchBlt(0, iScreenLine, czMF.cx, czMF.cy / iMaxBand, 
//	  					&dcBm, 0, 0, rctBand.right, rctBand.bottom, SRCCOPY);
//
//				mmBand.MinRow() = mmBand.MaxRow(); 
//				mmBand.MaxRow() = mmBand.MinRow() + iHeight;
//				iMapRow += iHeight;
//				iScreenLine += czMF.cy / iMaxBand;
//				
//			}
//
//      dcBm.SelectObject(penOld);
//      dcBm.SelectObject(brOld);
//      dcBm.SelectObject(bmOld);
//    }
//    else {
//			dr->draw(cdc, rect, &psnMf, &fDummyDraw);
//    }
//	}

	return TRUE;
}


// MapCompositionSrvItem serialization

void MapCompositionSrvItem::Serialize(CArchive& ar)
{
	MapCompositionDoc* mcd = GetDocument();
	mcd->Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// MapCompositionSrvItem commands

BOOL MapCompositionSrvItem::OnGetExtent(DVASPECT nDrawAspect, CSize& szRes) 
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//if (nDrawAspect != DVASPECT_CONTENT)
	//	return FALSE;
	//if (m_sizeExtent.cx != 0) { // prevent scaling by container
	//	szRes = m_sizeExtent;
	//	return TRUE;
	//}
	//MapCompositionDoc* mcd = GetDocument();
	//double rScale = mcd->rPrefScale(); 
	//if (rScale < 0)
	//	rScale = 10000;
	//if (!mcd->georef.fValid())
	//	return FALSE;
	//double rPixSize = mcd->georef->rPixSize();
	//if (rPixSize < 0) // no pixelsize should not prevent drawing
	//	rPixSize = 1;
	//double rFact = rPixSize / rScale * 100000; // 0.01 mm units
	//RowCol rc = rcSize();
	//szRes.cx = (long)(rFact * rc.Col);
	//szRes.cy = (long)(rFact * rc.Row);
	return TRUE;
}

BOOL MapCompositionSrvItem::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	ASSERT(lpFormatEtc != NULL);
	//try
	//{
	//	if (lpFormatEtc->dwAspect != DVASPECT_CONTENT)
	//		return COleServerItem::OnRenderFileData(lpFormatEtc, pFile);
	//	switch (lpFormatEtc->cfFormat) 
	//	{
	//		case CF_TEXT:
	//		case CF_DSPTEXT:
	//		{
	//			CArchive ar(pFile, CArchive::store);
	//			MapCompositionDoc* mcd = GetDocument();
	//			ar.WriteString(mcd->GetTitle());
	//			ar.WriteString("\n\r");
	//			double rScale = mcd->rPrefScale(); 
	//			if (rScale > 0) {
	//				String sScale("Scale 1:%.f\n\r", rScale);
	//				ar.WriteString(sScale.scVal());
	//			}
	//			int i = 1;
	//		  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	//			{
	//				Drawer* dr = *iter;
	//				String sText("%i:\t%S\n\r", i++, dr->sTitle());
	//				ar.WriteString(sText.scVal());
	//			}
	//			ar << (BYTE)'\0';   
	//			return TRUE;
	//		}
	//		case CF_DIB:
	//		{
	//			// still incorrect
	//			MapCompositionDoc* mcd = GetDocument();
	//			MinMax mm = mcd->mmBounds();
	//			GeoRef grf = mcd->georef;
	//      int iScale = 0;
	//			if (!mcd->fRaster) {
	//				double rFact;
	//				rFact = max(mm.MaxCol(), mm.MaxRow()) / 2048.0;
	//				if (rFact > 1)
	//					iScale = (int)-floor(rFact);
	//			}
	//		  BitmapPositioner psnBm(iScale,mm,grf);  
	//			CRect rect(0,0,mm.MaxCol(),mm.MaxRow());
	//			CDC dc;
	//			dc.CreateCompatibleDC(0);
	//			CDC* dcBm = new CDC;
	//			CBitmap bm;
	//			bm.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	//			dcBm->CreateCompatibleDC(&dc);
	//			CBitmap* bmOld = dcBm->SelectObject(&bm);
	//			dcBm->SelectStockObject(WHITE_BRUSH);
	//			dcBm->SelectStockObject(WHITE_PEN);
	//			dcBm->Rectangle(rect);
	//			int iState = mcd->iState();
	//			for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	//			{
	//				if (iState != mcd->iState()) 
	//					break;
	//				Drawer* dr = *iter;
	//				if (iState != mcd->iState()) 
	//					break;
	//			}
	//			dcBm->SelectObject(bmOld);

	//			BITMAP bitmap;
	//			int iSize = bm.GetObject(sizeof(bitmap), &bitmap);
	//			pFile->Write(&bitmap, iSize);

	//			iSize = bitmap.bmWidth;
	//			char *cBuf = new char[iSize];
	//			iSize = bm.GetBitmapBits(iSize, cBuf);
	//			pFile->Write(cBuf,iSize);				
	//			delete [] cBuf;
	//			return TRUE;
	//		}
	//		default:
	//			return COleServerItem::OnRenderFileData(lpFormatEtc, pFile);
	//	}
	//}
	//catch (const ErrorObject&) {
	//	// do nothing
	//}
	//catch (CException* e) 
	//{
	//	e->Delete();
	//}
	return FALSE;
}

// called because flag OLEMISC_RECOMPOSEONRESIZE is set on in Registry
BOOL MapCompositionSrvItem::OnSetExtent(DVASPECT nDrawAspect, const CSize& size) 
{
	if (nDrawAspect != DVASPECT_CONTENT)
		return FALSE;
	COleServerItem::OnSetExtent(nDrawAspect, size);
	NotifyChanged(nDrawAspect);
	return TRUE;
}

RowCol MapCompositionSrvItem::rcOffset() const
{
	if (fViewPort)
		return m_rcOffset;
	else {
		const MapCompositionDoc* mcd = const_cast<MapCompositionSrvItem*>(this)->GetDocument();
		return RowCol(0,0); //mcd->rcPrefOffset();
	}
}

RowCol MapCompositionSrvItem::rcSize() const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//if (fViewPort)
	//	return m_rcSize;
	//else {
	//	const MapCompositionDoc* mcd = const_cast<MapCompositionSrvItem*>(this)->GetDocument();
	//	MinMax mm = mcd->mmBounds();
	//	RowCol rc;
	//	rc.Row = mm.height();
	//	rc.Col = mm.width();

	//	return rc;
	//}
}

void MapCompositionSrvItem::ResetSize()
{
	fViewPort = false; // will reset size at next OnDraw (but will leave offset as it is!)
}

void MapCompositionSrvItem::ResetOffsetAndSize()
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//fViewPort = false; // will reset size at next OnDraw

	//MapCompositionDoc* mcd = GetDocument();
	//if (0 == mcd)
	//	return;
	//MinMax mm = mcd->mmBounds();

	//m_rcOffset.Row = mm.MinRow(); // reset offset
	//m_rcOffset.Col = mm.MinCol();
}

void MapCompositionSrvItem::SetOffsetSize(RowCol rcOffset, RowCol rcSize)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MapCompositionDoc* mcd = GetDocument();
	//if (0 == mcd)
	//	return;
	//MinMax mm = mcd->mmBounds();

	//// no offset outside mapview
	//if (rcOffset.Row < mm.MinRow())
	//	rcOffset.Row = mm.MinRow();
	//if (rcOffset.Col < mm.MinCol())
	//	rcOffset.Col = mm.MinCol();
	//if (rcOffset.Row >= mm.MaxRow()-1)
	//	rcOffset.Row = mm.MaxRow() - 2;
	//if (rcOffset.Col >= mm.MaxCol()-1)
	//	rcOffset.Col = mm.MaxCol() - 2;
	//// no size outside mapview
	//RowCol rc;
	//rc.Row = mm.height();
	//rc.Col = mm.width();
	//rc.Row -= rcOffset.Row;
	//rc.Col -= rcOffset.Col;
	//if (rcSize.Row < 2)
	//	rcSize.Row = 2;
	//if (rcSize.Col < 2)
	//	rcSize.Col = 2;
	//if (rcSize.Row > rc.Row)
	//	rcSize.Row = rc.Row;
	//if (rcSize.Col > rc.Col)
	//	rcSize.Col = rc.Col;

	m_rcOffset = rcOffset;
	m_rcSize = rcSize;
	fViewPort = true;
}
