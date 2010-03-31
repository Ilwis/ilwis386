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
// ScaleBarSrvItem.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\ScaleBarSrvItem.h"
//#include <olestd.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ScaleBarSrvItem::ScaleBarSrvItem(COleServerDoc* pServerDoc, BOOL bAutoDelete)
		: COleServerItem( pServerDoc, bAutoDelete)
{
	SetItemName("ScaleBar");
	GetDataSource()->DelayRenderFileData(CF_TEXT);
}

ScaleBarSrvItem::~ScaleBarSrvItem()
{
}


BOOL ScaleBarSrvItem::OnDraw(CDC* pDC, CSize& rSize)
{
	MapCompositionDoc* mcd = GetDocument();
	double rScale = mcd->rPrefScale();
	String sScale("1:%.f", rScale);
	CString str = sScale.scVal();
	CRect rect(0,0,6000,1000);
	rSize = rect.Size();
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0, 0);
	pDC->SetWindowExt(rSize);
	pDC->SetViewportExt(rSize);  // Note: only affects the m_hAttribDC

	CFont fnt;
	fnt.CreateFont(-800, 0, 0, 0, FW_BOLD, 0,0,0,0,0,0,0,0, "Arial");
	CFont* fntOld = pDC->SelectObject(&fnt);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(0,0,str);
	pDC->SelectObject(fntOld);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// ScaleBarSrvItem serialization

void ScaleBarSrvItem::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL ScaleBarSrvItem::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	ASSERT(lpFormatEtc != NULL);
	try
	{
		if (lpFormatEtc->dwAspect != DVASPECT_CONTENT)
			return COleServerItem::OnRenderFileData(lpFormatEtc, pFile);
		switch (lpFormatEtc->cfFormat) 
		{
			case CF_TEXT:
			case CF_DSPTEXT:
			{
				CArchive ar(pFile, CArchive::store);
				MapCompositionDoc* mcd = GetDocument();
				double rScale = mcd->rPrefScale(); 
				String sScale("Scale 1:%.f\n\r", rScale);
				ar.WriteString(sScale.scVal());
				ar << (BYTE)'\0';   
				return TRUE;
			}
			default:
				return COleServerItem::OnRenderFileData(lpFormatEtc, pFile);
		}
	}
	catch (const ErrorObject&) {
		// do nothing
	}
	catch (CException* e) 
	{
		e->Delete();
	}
	return FALSE;
}

COleDataSource* ScaleBarSrvItem::OnGetClipboardData(BOOL bIncludeLink,
	LPPOINT lpOffset, LPSIZE lpSize)
{
	return COleServerItem::OnGetClipboardData(bIncludeLink, lpOffset, lpSize);

	ASSERT_VALID(this);

	COleDataSource* pDataSource = new COleDataSource;
	TRY
	{
		//ASSERT(lpOffset == NULL ||
		//	AfxIsValidAddress(lpOffset, sizeof(POINT), FALSE));
		//STGMEDIUM stgMedium;
		///* - exclude embedding
		//// add CF_EMBEDDEDOBJECT by creating memory storage copy of the object
		//GetEmbedSourceData(&stgMedium);
		//pDataSource->CacheData((CLIPFORMAT)_oleData.cfEmbedSource, &stgMedium);
		//*/

		//// add CF_OBJECTDESCRIPTOR
		//CLIPFORMAT cfOBJECTDESCRIPTOR = (CLIPFORMAT)::RegisterClipboardFormat(CF_OBJECTDESCRIPTOR);
		//GetObjectDescriptorData(lpOffset, lpSize, &stgMedium);
		//pDataSource->CacheData(cfOBJECTDESCRIPTOR, &stgMedium);
		//// add any presentation entries/conversion formats that the item
		////  can produce.

		//CLIPFORMAT cfLINKSOURCE = (CLIPFORMAT)::RegisterClipboardFormat(CF_LINKSOURCE);
		//CLIPFORMAT cfLINKSOURCEDESCRIPTOR = (CLIPFORMAT)::RegisterClipboardFormat(CF_LINKSRCDESCRIPTOR);

		//// add CF_LINKSOURCE if supporting links to pseudo objects
		//if (bIncludeLink && GetLinkSourceData(&stgMedium))
		//{
		//	pDataSource->CacheData(cfLINKSOURCE, &stgMedium);

		//	// add CF_LINKSOURCEDESCRIPTOR
		//	GetObjectDescriptorData(lpOffset, lpSize, &stgMedium);
		//	pDataSource->CacheData(cfLINKSOURCEDESCRIPTOR,
		//		&stgMedium);
		//}
		//AddOtherClipboardData(pDataSource);
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
	}
	END_CATCH_ALL

		ASSERT_VALID(pDataSource);
	return pDataSource;
}


