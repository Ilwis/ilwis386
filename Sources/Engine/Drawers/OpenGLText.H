#pragma once

#include <gl/gl.h>
#include "Engine\Base\DataObjects\Color.h"
#include "headers\ftgl\ftgl.h"

namespace ILWIS {
	class RootDrawer;
}

class _export OpenGLText {
public:
	OpenGLText(ILWIS::RootDrawer *rootdrawer,const String& name, int height, bool fixed=false);
	~OpenGLText();
	void renderText(const Coordinate& c, const String& text);
	double getHeight() const;
private:
	//void loadFont(const String& name, int height) ;
	FTFont *font;
	Color color;
	int height;
	int actualHeight;
	bool fixedSize;
	double scale;


};