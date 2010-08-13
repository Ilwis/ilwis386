#pragma once

#include <gl/gl.h>
#include "Engine\Base\DataObjects\Color.h"
#include "headers\ftgl\ftgl.h"

class OpenGLText {
public:
	OpenGLText(const String& name, int height);
	~OpenGLText();
	void renderText(ILWIS::DrawerContext *context, const Coordinate& c, const String& text);
private:
	//void loadFont(const String& name, int height) ;
	FTFont *font;
	Color color;
	int height;
	int actualHeight;


};