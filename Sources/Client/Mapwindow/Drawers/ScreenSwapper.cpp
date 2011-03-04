#include "Headers\toolspch.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "Client\ilwis.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Client\Base\events.h"
#include "Client\Base\NfyMap.h"
#include "Client\Base\ZappTools.h"
#include "Client\Mapwindow\ZoomableView.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\ScreenSwapper.h"

using namespace ILWIS;

ScreenSwapper::ScreenSwapper() : swapBitmap(0), useBitmapRedraw(false){
}

ScreenSwapper::~ScreenSwapper() {
	if ( swapBitmap)
		delete [] swapBitmap;
}

void ScreenSwapper::swapBufferToScreen(const CRect& rct) const{
	if (swapBitmap!=NULL) {
		glPixelTransferf(GL_MAP_COLOR, false);
		glDrawPixels(rct.Width(), rct.Height(),GL_RGBA,GL_FLOAT, swapBitmap);
		GLenum ret =  glGetError();
	}
}

void ScreenSwapper::saveScreenBuffer(const CRect& rct)
{
	if ( swapBitmap == 0) {
		int width,height;
		width = rct.Width();
		height = rct.Height();
		if (swapBitmap==NULL) {
			swapBitmap = new float[width * height  * 4];
			glReadBuffer(GL_FRONT);
			glPixelTransferf(GL_MAP_COLOR, false);
			glReadPixels(rct.left,rct.top,width,height,GL_RGBA,GL_FLOAT,swapBitmap);
		}
	}
}

void ScreenSwapper::bitmapBufferRedraw(MapCompositionDoc *mdoc){
	if ( !useBitmapRedraw)
		return;

	CRect rct;
	mdoc->mpvGetView()->GetClientRect(&rct);
	saveScreenBuffer(rct);
	mdoc->rootDrawer->setupDraw();
	/*glViewport(0,0,pixArea.Col, pixArea.Row);
	setProjection(cbZoom);
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	glMatrixMode(GL_MODELVIEW); */
	glLoadIdentity();
	swapBufferToScreen(rct);
	//if ( selectionDrawer)
	//	selectionDrawer->draw();
	glDisable(GL_BLEND);
}

void ScreenSwapper::setBitmapRedraw(bool yesno) {
	useBitmapRedraw = yesno;
	if ( yesno==false && swapBitmap) {
		delete [] swapBitmap;
		swapBitmap = 0;
	}
}