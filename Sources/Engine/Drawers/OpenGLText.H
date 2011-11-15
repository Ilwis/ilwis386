#pragma once

#include <gl/gl.h>
#include "Engine\Base\DataObjects\Color.h"
#include "headers\ftgl\ftgl.h"

namespace ILWIS {
	class RootDrawer;
}

class _export OpenGLText {
public:
	OpenGLText(ILWIS::RootDrawer *rootdrawer,const String& name, int height, bool fixed = true, double horShift = 0, double verShift = 0);
	~OpenGLText();
	void renderText(const Coordinate& c, const String& text);
	void setColor(const Color & clr);
	double getHeight() const;
	CoordBounds getTextExtent(const String& txt) const;
private:
	//void loadFont(const String& name, int height) ;
	FTFont *font;
	Color color;
	int fontHeight;
	double horizontalShift;
	double verticalShift;
	bool fixedSize;
	double scale;
	ILWIS::RootDrawer *rootdrawer;


};