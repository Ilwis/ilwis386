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

bool DrawerContext::initOpenGL(CDC *dc, int m) {  
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
	int iFormat = ChoosePixelFormat( dc->m_hDC, &pfd );
	SetPixelFormat( dc->m_hDC, iFormat, &pfd );

	m_hdc = dc->m_hDC;
	m_hrc = wglCreateContext( m_hdc );    
	m_wnd = dc->GetWindow();

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

void DrawerContext::TakeContext()
{
	csOpenglContext.Lock();
	wglMakeCurrent(m_hdc, m_hrc);
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

	HGLRC hrc = ::wglGetCurrentContext();

	::wglMakeCurrent(NULL,  NULL);

	if (hrc)
		::wglDeleteContext(hrc);
	
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

void DrawerContext::setContext(HDC hdc, HGLRC hrc, int m) {
	m_hdc = hdc;
	m_hrc = hrc;
	mode = m | PFD_SUPPORT_OPENGL; // last one is mandatory as we use opengl, so I add it anyway, gdi is for convenience
}