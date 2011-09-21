#pragma once

#include <gl/gl.h>

class OpenGLFont {
public:
	OpenGLFont(const String& name, int height);
	~OpenGLFont();
	void renderText(const Coordinate& c, const String& text);
private:
	void loadFont(const String& name, int height) ;
    GLuint textures[128];
    GLuint displaylist;
    float h;

};