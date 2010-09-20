#include "Client\Headers\formelementspch.h"
#include <shfolder.h>
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Base\OpenGLText.h"


#define LIST_SIZE 96

OpenGLText::OpenGLText(const String& name, int h) : height(h), actualHeight(iUNDEF) {
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
}

OpenGLText::~OpenGLText() {
	delete font;
}

void OpenGLText::renderText(ILWIS::RootDrawer *rootdrawer,const Coordinate& c, const String& text) {
	int h = ((double)height / 100.0)* rootdrawer->getCoordBoundsZoom().height();
	if ( h != actualHeight) {
		actualHeight = h;
		font->FaceSize(400);
	}

	glColor3d(color.redP(), color.greenP(), color.blueP());
	glEnable (GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint (GL_POLYGON_SMOOTH, GL_DONT_CARE);
	font->Render(text.scVal(),text.size(),FTPoint(c.x,c.y,c.z));
}

