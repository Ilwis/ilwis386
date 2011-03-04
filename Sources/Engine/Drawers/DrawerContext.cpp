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
{
}

bool DrawerContext::initOpenGL(CDC *dc) {  
	if (fGLInitialized)
		return false;// no init needed, already done

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
				  PFD_DOUBLEBUFFER;
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
	SwapBuffers(m_hdc);
}



