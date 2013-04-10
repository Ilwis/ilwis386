#include "Headers\toolspch.h"
#include "Engine\Drawers\DrawerContext.h"

using namespace ILWIS;

DrawerContext::DrawerContext() :
maxTextureSize(128)
, maxPaletteSize(256)
, fGLInitialized(false)
, m_hdc(0)
, m_hrc(0)
, m_wnd(0)
, palette(0)
, mode(mDRAWTOWINDOW | mUSEDOUBLEBUFFER)
{
}

bool DrawerContext::initOpenGL(HDC hdc, CWnd * wnd, int m) {  
	if ( (m & mFORCEINIT) == 0) {
		if (fGLInitialized)
			return false;// no init needed, already done
	}
	mode = m & ~mFORCEINIT; // strip this one off, need not to be saved

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL;
	if ( m & mDRAWTOWINDOW )
		pfd.dwFlags |= PFD_DRAW_TO_WINDOW ;
	if ( m & mUSEDOUBLEBUFFER)
		pfd.dwFlags |= PFD_DOUBLEBUFFER;
	if ( m & mDRAWTOBITMAP)
		pfd.dwFlags |=  PFD_DRAW_TO_BITMAP ;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	if (mode & mSOFTWARERENDERER) {
		PIXELFORMATDESCRIPTOR pfd2 = pfd;
		int iNr = DescribePixelFormat( hdc, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd2);
		pfd2 = pfd;
		int iFormat = ChoosePixelFormat( hdc, &pfd2 ); // check what is the reference format we have to compete against; this is most likely a hardware accelerated format
		int bestColorBits = 0;
		int bestDepthBits = 0;
		for (int i = 1; i <= iNr; ++i) {
			pfd2 = pfd;
			DescribePixelFormat( hdc, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd2 );
			if ((pfd2.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED) && (pfd2.dwFlags & PFD_SUPPORT_OPENGL) && (pfd2.iPixelType == PFD_TYPE_RGBA) && (pfd2.cColorBits >= 8) && (pfd2.cDepthBits >= 8) && (pfd2.iLayerType == PFD_MAIN_PLANE)) {
				if (m & mDRAWTOWINDOW) {
					if (!(pfd2.dwFlags & PFD_DRAW_TO_WINDOW))
						continue;
				}
				if (m & mUSEDOUBLEBUFFER) {
					if (!(pfd2.dwFlags & PFD_DOUBLEBUFFER))
						continue;
				}
				if (m & mDRAWTOBITMAP) {
					if (!(pfd2.dwFlags & PFD_DRAW_TO_BITMAP))
						continue;
				}
				if (pfd2.cColorBits > bestColorBits) {
					bestColorBits = pfd2.cColorBits;
					bestDepthBits = pfd2.cDepthBits;
					iFormat = i;
				} else if (pfd2.cColorBits == bestColorBits && pfd2.cDepthBits > bestDepthBits) {
					bestDepthBits = pfd2.cDepthBits;
					iFormat = i;
				}
			}
		}
		pfd2 = pfd;
		DescribePixelFormat( hdc, iFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd2 );
		SetPixelFormat( hdc, iFormat, &pfd2 );
	} else {
		int iFormat = ChoosePixelFormat( hdc, &pfd );
		SetPixelFormat( hdc, iFormat, &pfd );
	}

	m_hdc = hdc;
	m_hrc = wglCreateContext( m_hdc );    
	m_wnd = wnd;

	TakeContext();
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.75,0.75,0.75,0.0);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	if (maxTextureSize > 512) // performance decreases from 1024 onwards on my computer
		maxTextureSize = 512;
	glGetIntegerv(GL_MAX_PIXEL_MAP_TABLE, &maxPaletteSize);
	if (maxPaletteSize > 65536)
		maxPaletteSize = 65536;
	ReleaseContext();

	fGLInitialized = true;

	return true;
}

void DrawerContext::doDraw()
{
	if (m_wnd != 0 && IsWindow(m_wnd->m_hWnd))
		m_wnd->Invalidate(FALSE);
}

bool DrawerContext::TakeContext()
{
	csOpenglContext.Lock();
	if ( m_hdc == 0) {
		csOpenglContext.Unlock();
		return false;
	}
	wglMakeCurrent(m_hdc, m_hrc);
	return true;
}

void DrawerContext::ReleaseContext()
{
	wglMakeCurrent(NULL, NULL);
	csOpenglContext.Unlock();
}

DrawerContext::~DrawerContext() {
	clear();
}

void DrawerContext::clear() {
	csOpenglContext.Lock();
	wglMakeCurrent(m_hdc, m_hrc);

	for (map<IVGAttributes*, GLuint>::iterator it = SVGSymbolDisplayListAreas.begin(); it != SVGSymbolDisplayListAreas.end(); ++it) {
		if (it->second != 0)
			glDeleteLists(it->second, 1);
	}

	for (map<IVGAttributes*, GLuint>::iterator it = SVGSymbolDisplayListContours.begin(); it != SVGSymbolDisplayListContours.end(); ++it) {
		if (it->second != 0)
			glDeleteLists(it->second, 1);
	}

	::wglMakeCurrent(NULL,  NULL);

	if (m_hrc)
		::wglDeleteContext(m_hrc);
	
	m_hdc = 0;
	m_hrc = 0;
	m_wnd = 0;

	csOpenglContext.Unlock();
}

void DrawerContext::swapBuffers() const{
	if ( mode & mUSEDOUBLEBUFFER)
		SwapBuffers(m_hdc);
}

void DrawerContext::setActivePalette(const Palette * palette) {
	this->palette = palette;
}

bool DrawerContext::isActivePalette(const Palette * palette) const {
	return this->palette == palette;
}
