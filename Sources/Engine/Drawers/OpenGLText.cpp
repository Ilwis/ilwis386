#include "Headers\toolspch.h"
#include <shfolder.h>
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\OpenGLText.h"


#define LIST_SIZE 96

OpenGLText::OpenGLText(ILWIS::RootDrawer *_rd,const String& _name, int height, bool fixed, double horShift, double verShift, bool orientedToUser)
: fontHeight(height)
, fixedSize(fixed)
, horizontalShift(horShift)
, verticalShift(verShift)
, faceUser(orientedToUser)
, font(0)
, rootdrawer(_rd)
, name(_name)
, tempFont(0)
{
	createFont();
	calcScale();
}

void OpenGLText::createFont() {
	FileName fn(name);
	if ( !fn.fExist()){
		TCHAR szPath[MAX_PATH];
		::SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath); 
		String file("%s\\fonts\\%S",szPath, name);
		fn = FileName(file);
    }
	//font = new FTTextureFont(fn.sFullPath().c_str());
	font = new FTPolygonFont(fn.sFullPath().c_str());
	font->FaceSize(fontHeight);
	color = Color(0,0,0);
}

OpenGLText::~OpenGLText() {
	delete font;
}

void OpenGLText::calcScale() {
	if (rootdrawer->getCoordBoundsZoom().height() > rootdrawer->getCoordBoundsZoom().width())
		scale = rootdrawer->getCoordBoundsZoom().height() / (double)rootdrawer->getViewPort().Row; // result is the meters/pixel or the degrees/pixel of the current zoom level
	else
		scale = rootdrawer->getCoordBoundsZoom().width() / (double)rootdrawer->getViewPort().Col; // result is the meters/pixel or the degrees/pixel of the current zoom level
}

void OpenGLText::prepare(ILWIS::PreparationParameters *pp) {
	if ( pp->type & ILWIS::NewDrawer::ptOFFSCREENSTART) {
		tempFont = font;
		createFont();
	}
	if (pp->type & ILWIS::NewDrawer::ptOFFSCREENEND) {
		if ( tempFont) {
			delete font;
			font = tempFont;
			tempFont = 0;
			//calcScale();
		}
	}
}

void OpenGLText::renderText(const Coordinate& c, const String& text) {
	glPushMatrix();
	if ( !fixedSize && tempFont == 0) { // tempfont == 0 means we are copying, don't mess with font scaling now. it's ok
		calcScale();
	}
	glScaled(scale, scale, scale); // with this the GL space is temporarily expressed in pixels
	glColor3d(color.redP(), color.greenP(), color.blueP());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable (GL_POLYGON_SMOOTH);
	glHint (GL_POLYGON_SMOOTH, GL_NICEST);
	if (faceUser && rootdrawer->is3D()) {
		glTranslated(horizontalShift + c.x / scale, verticalShift + c.y / scale, c.z);
		double rotX, rotY, rotZ;
		rootdrawer->getRotationAngles(rotX, rotY, rotZ);
		glRotatef(rotX,0,0,1); // Undo the rotation of RootDrawer
		glRotatef(rotY,1,0,0);
		FTPoint pStart(0, 0, 0);
		FTPoint pEnd =  font->Render(text.c_str(),text.size(), pStart);
	} else {
		FTPoint pStart(horizontalShift + c.x / scale, verticalShift + c.y / scale, c.z);
		FTPoint pEnd =  font->Render(text.c_str(),text.size(), pStart);
	}
	glDisable(GL_BLEND);
	glDisable(GL_POLYGON_SMOOTH);
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

CoordBounds OpenGLText::getTextExtent(const String& txt) const {
	if ( font) {
		//float x1,x2,y1,y2,z1,z2;
		//font->BBox(txt.c_str(), x1,y1,z1,x2,y2,z2);
		FTBBox box = font->BBox(txt.c_str());
		double x1 = box.Lower().X();
		double x2 = box.Upper().X();
		double y1 = box.Lower().Y();
		double y2 = box.Upper().Y();
		double z1 = box.Lower().Z();
		double z2 = box.Upper().Z();


		return CoordBounds(Coord(x1*scale, y1*scale, z1*scale), Coord(x2 * scale, y2 * scale, z2 * scale));
	}
	return CoordBounds();
}

