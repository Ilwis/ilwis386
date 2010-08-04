#pragma once;

#include "Client\Mapwindow\Drawers\ComplexDrawer.h"

class OpenGLFont;

enum FontStyle { FS_ITALIC    = 0x0001, 
                 FS_UNDERLINE	= 0x0002,
                 FS_STRIKEOUT	= 0x0004 };

namespace ILWIS {

class _export TextSetDrawer : public ComplexDrawer {
public:
	TextSetDrawer(DrawerParameters *parms, const String& type);
	~TextSetDrawer();
	virtual void prepare(PreparationParameters *pp);
	OpenGLFont *getFont() const;
	void setFont(OpenGLFont *f);
	HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	void displayOptionsText(CWnd *parent);
	Color clrText;
	String sFaceName;
	short iFHeight;
	short iFWeight;
	short iFStyle;
	short iWidth;
	OpenGLFont *font;
};


class _export TextDrawer : public SimpleDrawer {
public:
	TextDrawer(DrawerParameters *parms, const String& type);
	TextDrawer(DrawerParameters *parms);
	~TextDrawer();
	virtual void prepare(PreparationParameters *pp);
	void draw(bool norecursion = false);
	void addDataSource(void *);
private:
	String text;
	Coordinate c;
};


}