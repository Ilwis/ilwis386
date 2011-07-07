#include "Headers\toolspch.h"
#include <shfolder.h>
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\OpenGLText.h"


#define LIST_SIZE 96

OpenGLText::OpenGLText(ILWIS::RootDrawer *rootdrawer,const String& name, int height, bool fixed, double horShift, double verShift)
: fontHeight(height)
, fixedSize(fixed)
, horizontalShift(horShift)
, verticalShift(verShift)
, font(0)
{
	FileName fn(name);
	if ( !fn.fExist()){
		TCHAR szPath[MAX_PATH];
		::SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath); 
		String file("%s\\fonts\\%S",szPath, name);
		fn = FileName(file);
    }
	//font = new FTTextureFont(fn.sFullPath().scVal());
	font = new FTPolygonFont(fn.sFullPath().scVal());
	font->FaceSize(fontHeight);
	color = Color(0,0,0);
	if (fixedSize) {
		if (rootdrawer->getCoordBoundsZoom().height() > rootdrawer->getCoordBoundsZoom().width())
			scale = rootdrawer->getCoordBoundsZoom().height() / (double)rootdrawer->getViewPort().Row; // result is the meters/pixel or the degrees/pixel of the current zoom level
		else
			scale = rootdrawer->getCoordBoundsZoom().width() / (double)rootdrawer->getViewPort().Col; // result is the meters/pixel or the degrees/pixel of the current zoom level
	}
	else {
		if (rootdrawer->getMapCoordBounds().height() > rootdrawer->getMapCoordBounds().width())
			scale = rootdrawer->getMapCoordBounds().height() / (double)rootdrawer->getViewPort().Row; // result is the meters/pixel or the degrees/pixel of the image
		else
			scale = rootdrawer->getMapCoordBounds().width() / (double)rootdrawer->getViewPort().Col; // result is the meters/pixel or the degrees/pixel of the image
	}
}

OpenGLText::~OpenGLText() {
	delete font;
}

void OpenGLText::renderText(const Coordinate& c, const String& text) {
	glPushMatrix();
	glScaled(scale, scale, 1); // with this the GL space is temporarily expressed in pixels
	glColor3d(color.redP(), color.greenP(), color.blueP());
	glEnable (GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint (GL_POLYGON_SMOOTH, GL_DONT_CARE);
	font->Render(text.scVal(),text.size(),FTPoint(horizontalShift + c.x / scale, verticalShift + c.y / scale, c.z));
	glPopMatrix();
}

void OpenGLText::setColor(const Color & clr) {
	color = clr;
}

double OpenGLText::getHeight() const{
	if ( fontHeight != iUNDEF)
		return fontHeight * scale;
	return 0;
}

