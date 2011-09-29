#pragma once;

#include "Engine\Drawers\ComplexDrawer.h"

enum FontStyle { FS_ITALIC    = 0x0001, 
                 FS_UNDERLINE	= 0x0002,
                 FS_STRIKEOUT	= 0x0004 };
class OpenGLText;
ILWIS::NewDrawer *createTextLayerDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createTextDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

class _export TextLayerDrawer : public ComplexDrawer {
public:
	TextLayerDrawer(DrawerParameters *parms);
	~TextLayerDrawer();
	virtual void prepare(PreparationParameters *pp);
	OpenGLText *getFont() const;
	void setFont(OpenGLText *f);
	//HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent);
private:
	void displayOptionsText(CWnd *parent);
	Color clrText;
	String sFaceName;
	short iFHeight;
	short iFWeight;
	short iFStyle;
	short iWidth;
	OpenGLText *font;
};


class _export TextDrawer : public SimpleDrawer {
public:
	TextDrawer(DrawerParameters *parms);
	~TextDrawer();
	virtual void prepare(PreparationParameters *pp);
	bool draw( const CoordBounds& cbArea=CoordBounds() ) const;
	void addDataSource(void *);
	void setCoord(const Coord& crd);
	void setText(const String& txt) { text = txt;}
	double getHeight() const;
	String getText() const { return text; }
private:
	String text;
	Coordinate c;
};


}