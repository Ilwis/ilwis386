



#pragma once




//////////////////
// Picture object--encapsulates IPicture
//
class CPicture 
{

public:
	CPicture();
	~CPicture();

	// Load frm various sosurces
	BOOL Load(HINSTANCE hInst, String name);
	BOOL Load(LPCTSTR pszPathName, bool fResampleBicubic);
	BOOL Load(IStream* pstm);
	BOOL Load(unsigned char *buf, int len);

	// render to device context
	BOOL Render(HDC dc, RECT* rc, LPCRECT prcMFBounds=NULL) ;

	SIZE GetImageSize(HDC dc=NULL) ;

	operator IPicture*() {
		return m_spIPicture;
	}

	void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy)  {
		if ( !m_spIPicture)
			return;

		cx = cy = 0;
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Width(&cx);
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Height(&cy);
	}

	void Free() {
		if (m_spIPicture) {
			m_spIPicture.Release();
		}
	}

protected:

	void SetHIMETRICtoDP(HDC hdc, SIZE* sz);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	void ResampleImageTo(LONG nWidth, LONG nHeight);
	bool fResampleBicubic;

	CComQIPtr<IPicture> m_spIPicture; // ATL smart pointer to IPicture
	HRESULT m_hr; // last error code
	CBitmap bm;
	INT pixelFormat; // keep the pixelformat and paletteSize for creating compatible bitmaps
	INT paletteSize;
};



