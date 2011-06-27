#include "Headers\toolspch.h"
#include <shfolder.h>
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\OpenGLText.h"


#define LIST_SIZE 96

OpenGLText::OpenGLText(ILWIS::RootDrawer *rootdrawer,const String& name, int h, bool fixed) : height(h), actualHeight(iUNDEF), fixedSize(fixed),font(0) {
	FileName fn(name);
	if ( !fn.fExist()){
		TCHAR szPath[MAX_PATH];
		::SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath); 
		String file("%s\\fonts\\%S",szPath, name);
		fn = FileName(file);
    }
	//font = new FTTextureFont(fn.sFullPath().scVal());
	font = new FTPolygonFont(fn.sFullPath().scVal());
	color = Color(0,0,0);
	scale = 0.18;
	double  ratio = ((double)height / 100.0)* rootdrawer->getCoordBoundsZoom().height();
	int h1 = ratio;
	if ( (h1 != actualHeight && !fixedSize) || actualHeight == iUNDEF) {
		actualHeight = h1;
		font->FaceSize(actualHeight);
	}
}

OpenGLText::~OpenGLText() {
	delete font;
}

void OpenGLText::renderText(const Coordinate& c, const String& text) {
	glPushMatrix();
	glScaled(scale, scale, 1);
	glColor3d(color.redP(), color.greenP(), color.blueP());
	glEnable (GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint (GL_POLYGON_SMOOTH, GL_DONT_CARE);
	font->Render(text.scVal(),text.size(),FTPoint(c.x / scale,c.y / scale,c.z));
	glPopMatrix();
}

double OpenGLText::getHeight() const{
	if ( actualHeight != iUNDEF)
		return actualHeight * scale;
	return 0;
}

