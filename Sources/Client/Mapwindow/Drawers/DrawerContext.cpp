#include "Client\Headers\formelementspch.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"


using namespace ILWIS;

DrawerContext::DrawerContext(MapCompositionDoc *d)
: doc(d)
, maxTextureSize(128)
, glBusy(false)
, fUrgentRequestWaiting(false)
, fGLInitialized(false)
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
	// wglMakeCurrent( dc->m_hDC, mhRC );

	TakeContext(true);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.75,0.75,0.75,0.0);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	if (maxTextureSize > 512) // performance decreases from 1024 onwards on my computer
		maxTextureSize = 512;
	ReleaseContext();

	fGLInitialized = true;

	return true;
}

void DrawerContext::InvalidateWindow()
{
	if (m_wnd != 0 && IsWindow(m_wnd->m_hWnd))
		m_wnd->Invalidate(FALSE);
}

void DrawerContext::TakeContext(bool urgent)
{
	this->fUrgentRequestWaiting = urgent;
	if (urgent)
	{
		while(glBusy)
			Sleep(1);
		this->fUrgentRequestWaiting = false;
	}
	else
	{
		while (glBusy)
			Sleep(100);
	}

	glBusy = true;
	wglMakeCurrent(m_hdc, m_hrc);
}

void DrawerContext::ReleaseContext()
{
	wglMakeCurrent(NULL, NULL);
	glBusy = false;
}

DrawerContext::~DrawerContext() {
	clear();
}

void DrawerContext::clear() {

  //  if ( mhRC )
  //  {
  //      wglMakeCurrent( NULL, NULL );
  //      wglDeleteContext( mhRC );
		//mhRC = 0;
  //  }

	this->fUrgentRequestWaiting = true;
	while(glBusy)
		Sleep(100);

	HGLRC hrc = ::wglGetCurrentContext();

	::wglMakeCurrent(NULL,  NULL);

	if (hrc)
		::wglDeleteContext(hrc);
}

MapCompositionDoc *DrawerContext::getDocument() const {
	return doc;
}



