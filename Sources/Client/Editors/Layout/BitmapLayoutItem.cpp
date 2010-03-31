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
// BitmapLayoutItem.cpp: implementation of the BitmapLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\BitmapLayoutItem.h"
#include "Engine\Base\File\ElementMap.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define WIDTHBYTES(i)   ((i+31)/32*4)

BitmapLayoutItem::BitmapLayoutItem(LayoutDoc* ld)
	: LayoutItem(ld)
{
}

BitmapLayoutItem::BitmapLayoutItem(LayoutDoc* ld, HBITMAP hnd)
	: LayoutItem(ld)
{
	m_fIsotropic = true;
	bm.Attach(CopyImage(hnd, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
}

BitmapLayoutItem::~BitmapLayoutItem()
{
}

void BitmapLayoutItem::OnDraw(CDC* cdc)
{
	CRect rct = rectPos();
/*	BITMAP bitmap;
	bm.GetObject(sizeof(bitmap),&bitmap);
	CDC dcBm;
	if (!dcBm.CreateCompatibleDC(cdc)) 
		dcBm.CreateCompatibleDC(0);
	CBitmap* bmOld = dcBm.SelectObject(&bm);
	cdc->StretchBlt(rct.left, rct.top, rct.Width(), rct.Height(),
		              &dcBm,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
	dcBm.SelectObject(bmOld);*/

  StretchDibBlt(*cdc, rct, SRCCOPY);

}

bool BitmapLayoutItem::fConfigure()
{
  class ConfigForm : public FormWithDest
  {
  public:
    ConfigForm(CWnd* p, bool* fIsotropic)
    : FormWithDest(p, SLOTitleBitmap)
    {
      new CheckBox(root, SLOUiIsotropic, fIsotropic);
      create();
    }
  };
  ConfigForm frm(ld->wndGetActiveView(), &m_fIsotropic);
  return frm.fOkClicked();
}

String BitmapLayoutItem::sType() const
{
	return "Bitmap";
}

String BitmapLayoutItem::sName() const
{
	return SLONameBitmap;
}

void BitmapLayoutItem::Serialize(CArchive& ar, const char* sSection)
{
	ElementContainer en;
	en.em = new ElementMap;
	if (ar.IsStoring()) {
		WriteElements(en, sSection);
		BITMAP bitmap;
		bm.GetObject(sizeof(bitmap), &bitmap);
		int iSize = bitmap.bmWidthBytes * bitmap.bmHeight;
		ObjectInfo::WriteElement(sSection, "Bitmap Width", en, bitmap.bmWidth); 	
		ObjectInfo::WriteElement(sSection, "Bitmap Height", en, bitmap.bmHeight); 	
		ObjectInfo::WriteElement(sSection, "Bitmap Planes", en, bitmap.bmPlanes); 	
		ObjectInfo::WriteElement(sSection, "Bitmap BitCount", en, bitmap.bmBitsPixel); 	
		ObjectInfo::WriteElement(sSection, "Isotropic", en, m_fIsotropic); 	
		ObjectInfo::WriteElement(sSection, "BitmapSize", en, iSize); 	
		en.em->SerializeSection(ar, sSection);
		BYTE* pBuf = new BYTE[iSize];
		bm.GetBitmapBits(iSize, pBuf);
		ar.Write(pBuf,iSize);
		delete [] pBuf;
	}
	else {
		en.em->SerializeSection(ar, sSection);
		ReadElements(en, sSection);
		int iWidth, iHeight, iPlanes, iBits;
		int iSize;
		ObjectInfo::ReadElement(sSection, "Bitmap Width", en, iWidth); 	
		ObjectInfo::ReadElement(sSection, "Bitmap Height", en, iHeight); 	
		ObjectInfo::ReadElement(sSection, "Bitmap Planes", en, iPlanes); 	
		ObjectInfo::ReadElement(sSection, "Bitmap BitCount", en, iBits); 	
		ObjectInfo::ReadElement(sSection, "Isotropic", en, m_fIsotropic); 	
		ObjectInfo::ReadElement(sSection, "BitmapSize", en, iSize); 	
		m_fIsotropic = true;
		BYTE* pBuf = new BYTE[iSize];
		ar.Read(pBuf,iSize);
		CWindowDC dc(CWnd::GetDesktopWindow());
		bm.CreateBitmap(iWidth, iHeight, iPlanes, iBits, 0);
		bm.SetBitmapBits(iSize, pBuf);
		delete [] pBuf;
	}  
}

void BitmapLayoutItem::ReadElements(ElementContainer& ec, const char* sSection)
{
	LayoutItem::ReadElements(ec, sSection);
}

void BitmapLayoutItem::WriteElements(ElementContainer& ec, const char* sSection)
{
	LayoutItem::WriteElements(ec, sSection);
}

bool BitmapLayoutItem::fAddExtraClipboardItems()
{
	HBITMAP hbm = (HBITMAP)CopyImage(bm, IMAGE_BITMAP, 0, 0, 0);
	SetClipboardData(CF_BITMAP,hbm);
	return true;
}
									 
bool BitmapLayoutItem::fIsotropic() const
{
	return m_fIsotropic;
}

double BitmapLayoutItem::rHeightFact() const
{
	// when bm not yet loaded return 1
	if (0 == bm.m_hObject)
		return 1;
	BITMAP bitmap;
	bm.GetObject(sizeof(bitmap),&bitmap);
	return double(bitmap.bmHeight) / bitmap.bmWidth;
}


// following code is from an example of the MSDN copied from the 2.23. Name of example is unknown
HANDLE BitmapLayoutItem::DibFromBitmap ( HBITMAP hb, HDC dc)
{ 
    BITMAP               bm; 
    BITMAPINFOHEADER     bi; 
    BITMAPINFOHEADER FAR *lpbi; 
    DWORD                dwLen; 
    HANDLE               hdib; 
    HANDLE               h; 
    HDC                  hdc;
    DWORD                biStyle=BI_RGB;

    HPALETTE hpal = NULL;
    SelectPalette(dc, hpal, true);
    if (!hb) 
        return NULL;
    bool fPalette = (GetDeviceCaps(dc, RASTERCAPS) & RC_PALETTE) != 0;
    int biBits= fPalette ? 8 : 24;
    if (hpal == NULL) 
        hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
 
    GetObject(hb,sizeof(bm),(LPSTR)&bm); 

    if (biBits == 0) 
        biBits =  bm.bmPlanes * bm.bmBitsPixel; 
 
    bi.biSize               = sizeof(BITMAPINFOHEADER); 
    bi.biWidth              = bm.bmWidth; 
    bi.biHeight             = bm.bmHeight; 
    bi.biPlanes             = 1; 
    bi.biBitCount           = biBits; 
    bi.biCompression        = biStyle; 
    bi.biSizeImage          = 0; 
    bi.biXPelsPerMeter      = 0; 
    bi.biYPelsPerMeter      = 0; 
    bi.biClrUsed            = 0; 
    bi.biClrImportant       = 0; 
 
    dwLen  = bi.biSize + PaletteSize(&bi); 
 
    hdc = GetDC(NULL); 
    hpal = SelectPalette(hdc,hpal,FALSE); 
         RealizePalette(hdc); 
 
    hdib = GlobalAlloc(GHND,dwLen); 
 
    if (!hdib){ 
        SelectPalette(hdc,hpal,FALSE); 
        ReleaseDC(NULL,hdc); 
        return NULL; 
    } 
 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
 
    *lpbi = bi; 
 
    GetDIBits(hdc, hb, 0L, (DWORD)bi.biHeight, 
        (LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);
 
    bi = *lpbi; 
    GlobalUnlock(hdib); 
 
    /* If the driver did not fill in the biSizeImage field, make one up */ 
    if (bi.biSizeImage == 0){ 
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight; 
 
        if (biStyle != BI_RGB) 
            bi.biSizeImage = (bi.biSizeImage * 3) / 2; 
    } 
 
    /*  realloc the buffer big enough to hold all the bits */ 
    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;
    h = (HANDLE)GlobalReAlloc(hdib,dwLen,0);
    if (h)
        hdib = h; 
    else{ 
        GlobalFree(hdib); 
        hdib = NULL; 
 
        SelectPalette(hdc,hpal,FALSE); 
        ReleaseDC(NULL,hdc); 
        return hdib; 
    } 
 
    /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the 
     *  bits this time 
     */ 
    lpbi = (LPBITMAPINFOHEADER )GlobalLock(hdib);
 
    if (GetDIBits( hdc, 
                   hb, 
                   0L,
                   (DWORD)bi.biHeight, 
                   (LPBYTE)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), 
                   (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS) == 0){ 
         GlobalUnlock(hdib); 
         hdib = NULL; 
         SelectPalette(hdc,hpal,FALSE); 
         ReleaseDC(NULL,hdc); 
         return NULL; 
    } 
 
    bi = *lpbi; 
    GlobalUnlock(hdib); 
 
    SelectPalette(hdc,hpal,FALSE); 
    ReleaseDC(NULL,hdc); 
    return hdib; 
}

WORD BitmapLayoutItem::PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

BOOL BitmapLayoutItem::StretchDibBlt (HDC hdc, zRect rct, LONG rop)
{
    LPBITMAPINFOHEADER lpbi; 
    LPSTR    pBuf; 
    BOOL     f; 

		HANDLE hbm = DibFromBitmap(bm, hdc);
		
    if (!hbm) return FALSE;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hbm);
 
    if (!lpbi) 
        return FALSE; 
 
    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi); 
 
    f = StretchDIBits ( hdc, 
            rct.left(), rct.top() ,
            rct.width(), rct.height() ,
            0, 0,
            lpbi->biWidth, lpbi->biHeight,
            pBuf, (LPBITMAPINFO)lpbi, 
            DIB_RGB_COLORS, 
            rop); 
 
    GlobalUnlock(hbm);
    return f; 
}

WORD BitmapLayoutItem::DibNumColors (VOID FAR * pv)
{
    int                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}





