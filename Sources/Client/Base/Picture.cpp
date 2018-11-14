
#include "Client\Headers\formelementspch.h"
#include "Picture.h"
#include <gdiplus.h>

#define HIMETRIC_INCH   2540    // HIMETRIC units per inch


////////////////////////////////////////////////////////////////
// CPicture implementation
//

CPicture::CPicture()
: fResampleBicubic(false)
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
	//HBITMAP hb = ::LoadBitmap(hInst, name.c_str());
	BOOL ok = bm.LoadBitmapA(name.c_str());
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

BOOL CPicture::Load(LPCTSTR pszPathName, bool _fResampleBicubic)
{
	fResampleBicubic = _fResampleBicubic;
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

	while ( ::ReadFile(hFile, lpBuffer, min(4096,len), &dwBytesRead, NULL) )
	{
		lpBuffer += dwBytesRead;
		len -= dwBytesRead;
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

int CPicture::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num  = 0;
	UINT size = 0;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j=0; j<num; ++j) {
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

//////////////////
// Load from stream (IStream). This is the one that really does it: call
// OleLoadPicture to do the work.
//
BOOL CPicture::Load(IStream* pstm)
{
	Free();
	HRESULT hr = OleLoadPicture(pstm, 0, FALSE, IID_IPicture, (void**)&m_spIPicture);
	if (fResampleBicubic && (hr == S_OK)) { // keep some properties in case a bicubic-resample is needed at first render
		// Rewind the stream
		LARGE_INTEGER begin;
		begin.QuadPart = 0;
		pstm->Seek(begin, STREAM_SEEK_SET, NULL);
		// capture pixelFormat and pixelSize from pstm
		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pstm);
		pixelFormat = pBitmap->GetPixelFormat();
		paletteSize = pBitmap->GetPaletteSize();
		delete pBitmap; // release pBitmap
	}

	return hr == S_OK;	 
}

//////////////////
// Resample the image with high-quality bicubic to the new width and height
//
void CPicture::ResampleImageTo(LONG nWidth, LONG nHeight)
{
	HRESULT hr = S_FALSE;
	// capture m_spIPicture into stream
	SIZE sz = GetImageSize();
	UINT pixelSize = Gdiplus::GetPixelFormatSize(pixelFormat);
	DWORD len = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + int(sz.cy) * ((int(sz.cx * pixelSize / 8) / 4) + 1) * 4 + (Gdiplus::IsIndexedPixelFormat(pixelFormat) ? paletteSize : 0); // size in bytes of original bitmap
	HGLOBAL hGlobalOrig = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
	if (hGlobalOrig) {
		// don't delete memory on object's release
		IStream* pStreamOrig = NULL;
		if ( ::CreateStreamOnHGlobal(hGlobalOrig,FALSE,&pStreamOrig) == S_OK ) {
			m_spIPicture->SaveAsFile(pStreamOrig, FALSE, NULL);
			// rewind the stream
			LARGE_INTEGER begin;
			begin.QuadPart = 0;
			pStreamOrig->Seek(begin, STREAM_SEEK_SET, NULL);
			Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStreamOrig);
			Gdiplus::Bitmap* pBitmapOut = new Gdiplus::Bitmap(nWidth,nHeight,pixelFormat);
			if (pBitmapOut) {
				Gdiplus::Graphics * graphics = Gdiplus::Graphics::FromImage(pBitmapOut);
				if (graphics) {
					graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
					graphics->DrawImage(pBitmap,0,0,nWidth,nHeight); // draws pBitmap onto pBitmapOut
					len = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + int(nHeight) * ((int(nWidth * pixelSize / 8) / 4) + 1) * 4 + (Gdiplus::IsIndexedPixelFormat(pixelFormat) ? pBitmap->GetPaletteSize() : 0); // size of resampled bitmap
					HGLOBAL hGlobalOut = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
					if (hGlobalOut) {
						// don't delete memory on object's release
						IStream* pStreamOut = NULL;
						if ( ::CreateStreamOnHGlobal(hGlobalOut,FALSE,&pStreamOut) == S_OK ) {
							// Get the class identifier for the BMP encoder.
							CLSID bmpClsid;
							if (GetEncoderClsid(L"image/bmp", &bmpClsid) >= 0) {
								// Save pBitmapOut in the stream.
								Gdiplus::Status stat = pBitmapOut->Save(pStreamOut, &bmpClsid);
								if(stat == Gdiplus::Ok) {
									// Rewind the stream
									pStreamOut->Seek(begin, STREAM_SEEK_SET, NULL);
									// delete the old m_spIPicture
									Free();
									// Write the stream to m_spIPicture									
									hr = OleLoadPicture(pStreamOut, 0, FALSE, IID_IPicture, (void**)&m_spIPicture);
								}
							}						
						}
						::GlobalFree(hGlobalOut); // release pStreamOut
					}
					delete graphics; // release graphics
				}
			}
			delete pBitmapOut; // release pBitmapOut
			delete pBitmap; // release pBitmap
		}
		::GlobalFree(hGlobalOrig); // release pStreamOrig
	}
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

		if (fResampleBicubic) {
			SIZE sz = GetImageSize();
			if (sz.cx != rc->right || sz.cy != rc->bottom)
				ResampleImageTo(rc->right, rc->bottom); // this should happen only once, at first render (that is when the size is known of the corresponding FieldPicture formelement)
		}

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