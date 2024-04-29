#include "Headers\toolspch.h"
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
, fBold(false)
, fItalic(false)
, color(Color(0,0,0))
{
	createFont();
	calcScale();
}

void OpenGLText::createFont() {
	FileName fn(name);
	fn.sFile += fBold ? (fItalic ? "bi" : "bd") : (fItalic ? "i" : "");
	if ( !fn.fExist()){
		TCHAR szPath[MAX_PATH];
		::SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath); 
		String file("%s\\fonts\\%S",szPath, name);
		fn = FileName(file);
		fn.sFile += fBold ? (fItalic ? "bi" : "bd") : (fItalic ? "i" : "");
    }
	//font = new FTTextureFont(fn.sFullPath().c_str());
	font = new FTPolygonFont(fn.sFullPath().c_str());
	font->FaceSize(fontHeight);
}

OpenGLText::~OpenGLText() {
	delete font;
}

void OpenGLText::setBoldItalic(bool bold, bool italic) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	fBold = bold;
	fItalic = italic;
	delete font;
	createFont();
}

bool OpenGLText::getBold() const {
	return fBold;
}

bool OpenGLText::getItalic() const {
	return fItalic;
}

void OpenGLText::calcScale() {
	double h = rootdrawer->getCoordBoundsZoom().height();
	double w = rootdrawer->getCoordBoundsZoom().width();
	if (h > w)
		scale = h / (double)rootdrawer->getViewPort().Row; // result is the meters/pixel or the degrees/pixel of the current zoom level
	else
		scale = w / (double)rootdrawer->getViewPort().Col; // result is the meters/pixel or the degrees/pixel of the current zoom level
}

void OpenGLText::prepare(ILWIS::PreparationParameters *pp) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
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

void OpenGLText::renderText(const ILWIS::NewDrawer::DrawLoop drawLoop, const Coordinate& c, const String& text) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	glPushMatrix();
	if ( !fixedSize && tempFont == 0) // tempfont != 0 means we are doing Edit/Copy and we want to keep the value of scale unaltered (otherwise the font that is Edit/Copied is too small / doesn't match the one that is seen on the screen)
		calcScale();
	glScaled(scale, scale, scale); // with this the GL space is temporarily expressed in pixels
	glColor4d(color.redP(), color.greenP(), color.blueP(), color.alphaP());
	if ((drawLoop == ILWIS::NewDrawer::drl2D && color.alpha() != 255) || (drawLoop == ILWIS::NewDrawer::drl3DTRANSPARENT)) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	} else
		glDisable(GL_BLEND);
	glEnable (GL_POLYGON_SMOOTH);
	glHint (GL_POLYGON_SMOOTH, GL_NICEST);
	if (faceUser && rootdrawer->is3D()) {
		glTranslated(horizontalShift + c.x / scale, verticalShift + c.y / scale, c.z);
		double rotX, rotY, rotZ;
		rootdrawer->getRotationAngles(rotX, rotY, rotZ);
		glRotatef(rotX,0,0,1); // Undo the rotation of RootDrawer
		glRotatef(rotY,1,0,0);
		FTPoint pStart(0, 0, 0);
		wchar_t wtext[512];
		//mbstowcs(wtext, text.c_str(), 512); // substituted by next statement because this did not work with PostgreSQL UTF-8 .. check if it still work with WFS (TODO)
		::MultiByteToWideChar( CP_ACP, 0, text.c_str(), text.length() + 1, wtext, 512 ); // note: "text.length() + 1" is intentional, documentation says "does not null-terminate an output string if the input string length is explicitly specified without a terminating null character"
		FTPoint pEnd =  font->Render(wtext,-1, pStart);
	} else {
		FTPoint pStart(horizontalShift + c.x / scale, verticalShift + c.y / scale, c.z);
		wchar_t wtext[512];
		//mbstowcs(wtext, text.c_str(), 512);
		::MultiByteToWideChar( CP_ACP, 0, text.c_str(), text.length() + 1, wtext, 512 ); // note: "text.length() + 1" is intentional, see above
		FTPoint pEnd =  font->Render(wtext,-1, pStart);
	}
	glDisable(GL_BLEND);
	glDisable(GL_POLYGON_SMOOTH);
	glPopMatrix();
}

void OpenGLText::setColor(const Color & clr) {
	color = clr;
}

Color OpenGLText::getColor() const {
	return color;
}

double OpenGLText::getHeight() const {
	if ( fontHeight != iUNDEF) {
		if ( !fixedSize && tempFont == 0) // tempfont != 0 means we are doing Edit/Copy and we want to keep the value of scale unaltered (otherwise the font that is Edit/Copied is too small / doesn't match the one that is seen on the screen)
			const_cast<OpenGLText*>(this)->calcScale(); // compute the newest scale, before using it
		return fontHeight * scale;
	}
	return 0;
}

void OpenGLText::setHeight(int h) {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	fontHeight = h ;
	font->FaceSize(fontHeight);
}

CoordBounds OpenGLText::getTextExtent(const String& txt) const {
	ILWISSingleLock sl(&csAccess, TRUE,SOURCE_LOCATION);
	if ( font) {
		//float x1,x2,y1,y2,z1,z2;
		//font->BBox(txt.c_str(), x1,y1,z1,x2,y2,z2);
		//FTBBox box = font->BBox(txt.c_str());
		wchar_t wtext[512];
		::MultiByteToWideChar( CP_ACP, 0, txt.c_str(), txt.length() + 1, wtext, 512 ); // note: "text.length() + 1" is intentional, see above
		FTBBox box = font->BBox(wtext);
		double x1 = box.Lower().X();
		double x2 = box.Upper().X();
		double y1 = box.Lower().Y();
		double y2 = box.Upper().Y();
		double z1 = box.Lower().Z();
		double z2 = box.Upper().Z();

		if ( !fixedSize && tempFont == 0) // tempfont != 0 means we are doing Edit/Copy and we want to keep the value of scale unaltered (otherwise the font that is Edit/Copied is too small / doesn't match the one that is seen on the screen)
			const_cast<OpenGLText*>(this)->calcScale(); // compute the newest scale, before using it
		return CoordBounds(Coord(x1*scale, y1*scale, z1*scale), Coord(x2 * scale, y2 * scale, z2 * scale));
	}
	return CoordBounds();
}

