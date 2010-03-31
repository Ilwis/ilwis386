
#include "Client\Headers\formelementspch.h"
#include "Picture.h"


#define HIMETRIC_INCH   2540    // HIMETRIC units per inch


////////////////////////////////////////////////////////////////
// CPicture implementation
//

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}

//////////////////
// Load from resource. Looks for "IMAGE" type.
//
BOOL CPicture::Load(HINSTANCE hInst, String name)
{
	// find resource in resource file
	//HBITMAP hb = ::LoadBitmap(hInst, name.scVal());
	BOOL ok = bm.LoadBitmapA(name.scVal());
	//BOOL ok = bm.Attach(hb);
	
	return TRUE;

}

//////////////////
// Load from path name.
//
BOOL CPicture::Load(unsigned char *buf, int len) {
	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
	if ( !hGlobal )
	{
		return FALSE;
	}

	unsigned char* lpBuffer = reinterpret_cast<unsigned char*> ( ::GlobalLock(hGlobal) );
	DWORD dwBytesRead = 0;

	for(int i = 0; i < len; ++i)
	{
		lpBuffer[i] = buf[i];
	}

	
	::GlobalUnlock(hGlobal);


	// don't delete memory on object's release
	IStream* pStream = NULL;
	if ( ::CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK )
	{
		::GlobalFree(hGlobal);
		return FALSE;
	}

	// create memory file and load it
	BOOL bRet = Load(pStream);

	::GlobalFree(hGlobal);

	return bRet;

}

BOOL CPicture::Load(LPCTSTR pszPathName)     
{
	HANDLE hFile = ::CreateFile(pszPathName, 
								FILE_READ_DATA,
								FILE_SHARE_READ,
								NULL, 
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if ( !hFile )
		return FALSE;

	DWORD len = ::GetFileSize( hFile, NULL); // only 32-bit of the actual file size is retained
	if (len == 0)
		return FALSE;

	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
	if ( !hGlobal )
	{
		::CloseHandle(hFile);
		return FALSE;
	}

	char* lpBuffer = reinterpret_cast<char*> ( ::GlobalLock(hGlobal) );
	DWORD dwBytesRead = 0;

	while ( ::ReadFile(hFile, lpBuffer, 4096, &dwBytesRead, NULL) )
	{
		lpBuffer += dwBytesRead;
		if (dwBytesRead == 0)
			break;
		dwBytesRead = 0;
	}

	::CloseHandle(hFile);

	
	::GlobalUnlock(hGlobal);


	// don't delete memory on object's release
	IStream* pStream = NULL;
	if ( ::CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK )
	{
		::GlobalFree(hGlobal);
		return FALSE;
	}

	// create memory file and load it
	BOOL bRet = Load(pStream);

	::GlobalFree(hGlobal);

	return bRet;
}

//////////////////
// Load from stream (IStream). This is the one that really does it: call
// OleLoadPicture to do the work.
//
BOOL CPicture::Load(IStream* pstm)
{
	Free();

	HRESULT hr = OleLoadPicture(pstm, 0, FALSE,
							IID_IPicture, (void**)&m_spIPicture);

	return hr == S_OK;	 

}

//////////////////
// Render to device context. Covert to HIMETRIC for IPicture.
//
// prcMFBounds : NULL if dc is not a metafile dc
//
BOOL CPicture::Render(HDC dc, RECT* rc, LPCRECT prcMFBounds) 
{
	if ( bm.GetSafeHandle() == 0) {

		if ( !m_spIPicture) 
			return FALSE;

		if ( !rc || (rc->right == rc->left && rc->bottom == rc->top) ) 
		{
			SIZE sz = GetImageSize(dc);
			rc->right = sz.cx;
			rc->bottom = sz.cy;
		}

		long hmWidth,hmHeight; // HIMETRIC units
		GetHIMETRICSize(hmWidth, hmHeight);

		m_spIPicture->Render(dc, 
							rc->left, rc->top, 
							rc->right - rc->left, rc->bottom - rc->top,
							0, hmHeight, hmWidth, -hmHeight, prcMFBounds);
	} else {
		CDC windowCDC;
		windowCDC.Attach(dc);
		CDC cdc;
		cdc.CreateCompatibleDC(&windowCDC);
		BITMAP bitMap;
		bm.GetBitmap(&bitMap);
		CBitmap *bmOld = cdc.SelectObject(&bm);
		CRect rct(rc);
		//windowCDC.BitBlt(0,0,bitMap.bmWidth,bitMap.bmHeight,&cdc,0,0,SRCCOPY);
		windowCDC.StretchBlt(0,0,rct.Width(),rct.Height(),&cdc,0,0,bitMap.bmWidth,bitMap.bmHeight, SRCCOPY);


		cdc.SelectObject(bmOld);
		windowCDC.Detach();
	}

	return TRUE;
}

//////////////////
// Get image size in pixels. Converts from HIMETRIC to device coords.
//
SIZE CPicture::GetImageSize(HDC dc) 
{
	SIZE sz = {0,0};
	if ( bm.GetSafeHandle()) {
		BITMAP bitmap;
		bm.GetBitmap(&bitmap);
		return CSize(bitmap.bmWidth, bitmap.bmHeight);
	} else {

		if (!m_spIPicture)
			return sz;
		
		LONG hmWidth, hmHeight; // HIMETRIC units
		m_spIPicture->get_Width(&hmWidth);
		m_spIPicture->get_Height(&hmHeight);

		sz.cx = hmWidth;
		sz.cy = hmHeight;

		if ( dc == NULL ) 
		{
			HDC dcscreen = ::GetWindowDC(NULL);

			SetHIMETRICtoDP(dcscreen,&sz); // convert to pixels
		} 
		else 
		{
			SetHIMETRICtoDP(dc,&sz);
		}
	}
	return sz;
}


void CPicture::SetHIMETRICtoDP(HDC hdc, SIZE* sz) 
{
	int nMapMode;
	if ( (nMapMode = ::GetMapMode(hdc)) < MM_ISOTROPIC && nMapMode != MM_TEXT)
	{
		// when using a constrained map mode, map against physical inch
		
		::SetMapMode(hdc,MM_HIMETRIC | MM_ISOTROPIC);
		POINT pt;
		pt.x = sz->cx;
		pt.y = sz->cy;
		::LPtoDP(hdc,&pt,1);
		sz->cx = pt.x;
		sz->cy = pt.y;
		::SetMapMode(hdc, nMapMode);
	}
	else
	{
		// map against logical inch for non-constrained mapping modes
		int cxPerInch, cyPerInch;
		cxPerInch = ::GetDeviceCaps(hdc,LOGPIXELSX);
		cyPerInch = ::GetDeviceCaps(hdc,LOGPIXELSY);
		sz->cx = MulDiv(sz->cx, cxPerInch, HIMETRIC_INCH);
		sz->cy = MulDiv(sz->cy, cyPerInch, HIMETRIC_INCH);
	}


	POINT pt;
	pt.x = sz->cx;
	pt.y = sz->cy;
	::DPtoLP(hdc,&pt,1);
	sz->cx = pt.x;
	sz->cy = pt.y;

}