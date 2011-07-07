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
	int iFormat = ChoosePixelFormat( hdc, &pfd );
	SetPixelFormat( hdc, iFormat, &pfd );

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
