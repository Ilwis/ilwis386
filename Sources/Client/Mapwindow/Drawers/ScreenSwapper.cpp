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
	csBitmap.Lock();
	if ( swapBitmap) {
		delete [] swapBitmap;
		swapBitmap = 0;
	}
	csBitmap.Unlock();
}

void ScreenSwapper::swapBufferToScreen(const CRect& rct){
	csBitmap.Lock();
	if (swapBitmap!=NULL) {
		glPixelTransferf(GL_MAP_COLOR, false);
		glDrawPixels(rct.Width(), rct.Height(),GL_RGBA,GL_FLOAT, swapBitmap);
		GLenum ret =  glGetError();
	}
	csBitmap.Unlock();
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
	swapBufferToScreen(rct);
	glDisable(GL_BLEND);
}

void ScreenSwapper::setBitmapRedraw(bool yesno) {
	useBitmapRedraw = yesno;
	if ( !yesno && swapBitmap) {
		csBitmap.Lock();
		delete [] swapBitmap;
		swapBitmap = 0;
		csBitmap.Unlock();
	}
}