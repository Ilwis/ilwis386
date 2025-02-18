#pragma once

#include <gl/gl.h>
#include "Engine\Base\DataObjects\Color.h"
#include "headers\ftgl\ftgl.h"

namespace ILWIS {
	class RootDrawer;
	struct PreparationParameters;
}

class _export OpenGLText {
public:
	OpenGLText(ILWIS::RootDrawer *rootdrawer,const String& name, int height, bool fixed = true, double horShift = 0, double verShift = 0, bool orientedToUser = false);
	~OpenGLText();
	void renderText(const ILWIS::NewDrawer::DrawLoop drawLoop, const Coordinate& c, const String& text);
	void setColor(const Color & clr);
	Color getColor() const;
	double getHeight() const;
	CoordBounds getTextExtent(const String& txt) const;
	void createFont();
	void prepare(ILWIS::PreparationParameters *pp);
	void setHeight(int h);
	void setBoldItalic(bool bold, bool italic);
	bool getBold() const;
	bool getItalic() const;

private:
	void calcScale();
	FTFont *font;
	FTFont *tempFont;
	Color color;
	int fontHeight;
	double horizontalShift;
	double verticalShift;
	bool fixedSize;
	bool faceUser;
	double scale;
	bool fBold;
	bool fItalic;
	String name;
	ILWIS::RootDrawer *rootdrawer;
	mutable CCriticalSection csAccess;


};