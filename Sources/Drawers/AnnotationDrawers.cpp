#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h" 
#include "Drawers\SetDrawer.h" 
#include "Engine\Representation\Rpr.h"
#include "Engine\Representation\rprclass.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "drawers\linedrawer.h"
#include "drawers\pointdrawer.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Drawers\AnnotationDrawers.h"
#include "Engine\Domain\dmclass.h"
#include "Boxdrawer.h"
#include "DrawingColor.h"
#include "Drawers\GridDrawer.h"
#include "Engine\Base\Round.h"

using namespace ILWIS;


ILWIS::NewDrawer *createAnnotationDrawers(DrawerParameters *parms) {
	return new AnnotationDrawers(parms);
}

AnnotationDrawers::AnnotationDrawers(DrawerParameters *parms) : ComplexDrawer(parms, "AnnotationDrawers"){
	id = "AnnotationDrawers";
}

void AnnotationDrawers::prepare(PreparationParameters *pp) {
	vector<NewDrawer *> allDrawers;
	getDrawers(allDrawers);
	for(int i = 0; i < allDrawers.size(); ++i) {
		if ( allDrawers[i]) {
			allDrawers[i]->prepare(pp);
		}
	}
}

void AnnotationDrawers::refreshClassLegends() {
	vector<NewDrawer *> allDrawers;
	getDrawers(allDrawers);
	PreparationParameters pp(NewDrawer::ptRENDER|NewDrawer::ptRESTORE, 0);
	for(int i = 0; i < allDrawers.size(); ++i) {
		AnnotationDrawer *adrw = dynamic_cast<AnnotationClassLegendDrawer *>(allDrawers[i]);
		if ( adrw) {
			adrw->prepare(&pp);
		}
	}
}

//---------------------------------------------------------
AnnotationDrawer::AnnotationDrawer(DrawerParameters *parms, const String& name) : ComplexDrawer(parms,name), scale(1.0), dataDrawer(0) 
{
	ComplexDrawer *drw =  dynamic_cast<ComplexDrawer *>(parms->parent);
	if ( drw)
		dataDrawer = drw;
}

FileName AnnotationDrawer::associaltedFile() const {
	LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(dataDrawer);
	if ( ldrw) {
		SpatialDataDrawer *sddrw = dynamic_cast<SpatialDataDrawer *>(ldrw->getParentDrawer());
		if ( sddrw) {
			return sddrw->getObject()->fnObj;	
		}
	}
	return FileName();
}
String AnnotationDrawer::store(const FileName& fnView, const String& currentSection) const{
	ComplexDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Scale",fnView, scale);
	ObjectInfo::WriteElement(currentSection.c_str(),"Title",fnView, title);

	return currentSection;
}

void AnnotationDrawer::load(const FileName& fnView, const String& currentSection){
	ComplexDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"Scale",fnView, scale);
	ObjectInfo::ReadElement(currentSection.c_str(),"Title",fnView, title);
}

double AnnotationDrawer::getScale() const {
	return scale;
}

void AnnotationDrawer::setScale(double s) {
	scale= s;
}

void AnnotationDrawer::setTitle(const String& t){
	title = t;
}

String AnnotationDrawer::getTitle() const{
	return title;
}

void AnnotationDrawer::setParent(NewDrawer *drw){
	parentDrawer = drw;
}

//-------------------------------------------------------
AnnotationLegendDrawer::AnnotationLegendDrawer(DrawerParameters *parms, const String& name) : 
AnnotationDrawer(parms, name),
drawOutsideBox(false),
vertical(true),
bgColor(Color(255,255,255)),
useBackground(false),
columns(1),
includeName(true),
texts(0),
fontScale(1)
{
}

void AnnotationLegendDrawer::setOrientation(bool yesno) {
	vertical = yesno;
	if (vertical) {
		if ( !cbBox.fValid() ) {
			cbBox.MinX() = 0.02;
			cbBox.MinY() = 0.02;
		}
		cbBox.MaxX() = cbBox.MinX() + columns / 10.0;
		cbBox.MaxY() = cbBox.MinY() + columns / 3.0;
	} else {
		if ( !cbBox.fValid() ) {
			cbBox.MinX() = 0.02;
			cbBox.MinY() = 0.025;
		}
		cbBox.MaxX() = cbBox.MinX() + 1.0 / 3.0; // 0.3333
		cbBox.MaxY() = cbBox.MinY() + 0.1;
	}
}

int AnnotationLegendDrawer::noOfColumns() const {
	return columns;
}

void AnnotationLegendDrawer::setNoOfColumns(int n) {
	columns  = n;
}

bool AnnotationLegendDrawer::getIncludeName() const {
	return includeName;
}

void AnnotationLegendDrawer::setInlcudeName(bool yesno) {
	includeName = yesno;
}


bool AnnotationLegendDrawer::getOrientation() const{
	return vertical;
}

void AnnotationLegendDrawer::setDrawBorder(bool yesno){
	drawOutsideBox = yesno;
}

bool AnnotationLegendDrawer::getDrawBorder() const{
	return drawOutsideBox;
}

CoordBounds AnnotationLegendDrawer::getBox() const {
	return cbBox;
}

void AnnotationLegendDrawer::setBox(const CoordBounds& cb) {
	cbBox = cb;
}

double AnnotationLegendDrawer::getFontScale() const{
	return fontScale;
}
void AnnotationLegendDrawer::setFontScale(double v){
	if ( v > 0)
		fontScale = v;
}

void AnnotationLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationDrawer::prepare(pp);
	if ( (pp->type & NewDrawer::ptGEOMETRY) || (pp->type & NewDrawer::ptRESTORE)) {
		if ( pp->type & NewDrawer::ptGEOMETRY)
			setOrientation(vertical);
		DrawerParameters dp(getRootDrawer(), this);
		if ( texts != 0) {
			removeDrawer(texts->getId(), true);
		}
		texts = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dp);
		texts->setFont(new OpenGLText(getRootDrawer(),"arial.ttf",12 * fontScale,false));
		addPostDrawer(100,texts);
		SpatialDataDrawer *spdr = dataDrawer->isSet() ? static_cast<SpatialDataDrawer *>(dataDrawer) : static_cast<SpatialDataDrawer *>(dataDrawer->getParentDrawer());
		LayerDrawer *ldr = dataDrawer->isSet() ? dynamic_cast<LayerDrawer *>(dataDrawer->getDrawer(0)) : dynamic_cast<LayerDrawer *>(dataDrawer);
	
		BaseMapPtr *bmp = spdr->getBaseMap(0);
		objType = IOTYPE(bmp->fnObj);
		dm = ldr->useAttributeColumn() ? ldr->getAtttributeColumn()->dm() :  bmp->dm();
		fnName = spdr->getObject()->fnObj.sFile;
	}
	if ( pp->type & NewDrawer::ptRENDER || (pp->type & NewDrawer::ptRESTORE)) {
		DrawerParameters dp(getRootDrawer(), texts);
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(101,ComplexDrawer::dtPOST);
		if ( !txtdr) {
			txtdr = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			texts->addPostDrawer(101,txtdr);
		}
		//texts->setFont(new OpenGLText(getRootDrawer(),"arial.ttf",12 * fontScale,true));
		texts->getFont()->setHeight(12 * fontScale);
		if ( title == "")
			title = fnName.sFile;
		txtdr->setText(title);
	}
}

bool AnnotationLegendDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	getRootDrawer()->setZIndex(1 + getRootDrawer()->getZIndex());
	if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
		return true;
	bool is3D = getRootDrawer()->is3D();

	CoordBounds cbInner = cbArea;
	CoordBounds cbFar;

	if ( includeName) {
		if (is3D) // texts and lines at level 2
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 2) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 2) * 0.0005);
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(101,ComplexDrawer::dtPOST);
		if ( txtdr) {
			double h = txtdr->getHeight();
			txtdr->setCoord(Coord(cbInner.MinX(),cbInner.MaxY() + h * 0.8, 0));
			cbFar += txtdr->getTextExtent();
		}
	}
	CoordBounds cbBoxed = cbInner;
	if ( !cbFar.fUndef()) {
		cbBoxed += Coord(cbFar.width(), cbFar.height());
		cbBoxed.MaxY() += cbFar.height() * 2;
	}
	cbBoxed *= 1.05;
	if ( useBackground) {
		if (is3D) // background at level 0
			glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);
		glColor4d(bgColor.redP(), bgColor.greenP(), bgColor.blueP(), getAlpha());
		glBegin(GL_POLYGON);
		Coordinate c(cbBoxed.MinX(), cbBoxed.MinY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MinX(), cbBoxed.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MaxX(), cbBoxed.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MaxX(), cbBoxed.MinY());
		glVertex3d( c.x, c.y, 0);
		glEnd();
	}

	if ( drawOutsideBox) {
		if (is3D) // texts and lines at level 2
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 2) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 2) * 0.0005);
		glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(),getAlpha());
		glLineWidth(lproperties.thickness);
		glBegin(GL_LINE_STRIP);
		Coordinate c(cbBoxed.MinX(), cbBoxed.MinY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MinX(), cbBoxed.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MaxX(), cbBoxed.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MaxX(), cbBoxed.MinY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbBoxed.MinX(), cbBoxed.MinY());
		glVertex3d( c.x, c.y, 0);
		glEnd();
	}
	return true;
}

void AnnotationLegendDrawer::setText(const vector<String>& v, int count, const Coord& c) const {
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( count);
	if ( txt) {
		double h = txt->getHeight();
		double l = v[count].size() * h * 0.2;
		Coord crd = c;
		if ( !vertical)
			crd = Coord(c.x - l, c.y, c.z);

		txt->setCoord(crd);
		txt->setText(v[count]);
		txt->setActive(true);
	}
}

String AnnotationLegendDrawer::store(const FileName& fnView, const String& currentSection) const{
	AnnotationDrawer::store(fnView, currentSection);
	if ( dataDrawer) {
		SpatialDataDrawer *spdr = (SpatialDataDrawer *)(dataDrawer->getParentDrawer());
		BaseMapPtr *bmp = spdr->getBaseMap();
		ObjectInfo::WriteElement(currentSection.c_str(),"DrawerSource",fnView, bmp->fnObj);
	}
	ObjectInfo::WriteElement(currentSection.c_str(),"Location",fnView, cbBox);
	ObjectInfo::WriteElement(currentSection.c_str(),"OutsideBox",fnView, drawOutsideBox);
	ObjectInfo::WriteElement(currentSection.c_str(),"Vertical",fnView, vertical);
	ObjectInfo::WriteElement(currentSection.c_str(),"UseBackground",fnView, useBackground);
	ObjectInfo::WriteElement(currentSection.c_str(),"BackGroundColor",fnView, bgColor);
	ObjectInfo::WriteElement(currentSection.c_str(),"Columns",fnView, columns);
	ObjectInfo::WriteElement(currentSection.c_str(),"UseTitle",fnView, includeName);
	ObjectInfo::WriteElement(currentSection.c_str(),"FontScale",fnView, fontScale);
	lproperties.store(fnView,currentSection);

	return currentSection;
}

void AnnotationLegendDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	AnnotationDrawer::load(fnView, currentSection);
	FileName fnObj;
	ObjectInfo::ReadElement(currentSection.c_str(),"DrawerSource",fnView, fnObj);
	if ( fnObj.fValid()) {
		for(int i = 0; i < rootDrawer->getDrawerCount(); ++i) {
			SpatialDataDrawer *spdr = dynamic_cast<SpatialDataDrawer *>(rootDrawer->getDrawer(i));
			if ( spdr && !spdr->isSet()) {
				if ( spdr->getObject()->fnObj == fnObj) {
					LayerDrawer *ldr = dynamic_cast<LayerDrawer *>(spdr->getDrawer(0));
					if ( ldr)
						dataDrawer = ldr;
				}
			}
		}
	}

	ObjectInfo::ReadElement(currentSection.c_str(),"Location",fnView, cbBox);
	ObjectInfo::ReadElement(currentSection.c_str(),"OutsideBox",fnView, drawOutsideBox);
	ObjectInfo::ReadElement(currentSection.c_str(),"Vertical",fnView, vertical);
	ObjectInfo::ReadElement(currentSection.c_str(),"UseBackground",fnView, useBackground);
	ObjectInfo::ReadElement(currentSection.c_str(),"BackGroundColor",fnView, bgColor);
	ObjectInfo::ReadElement(currentSection.c_str(),"Columns",fnView, columns);
	ObjectInfo::ReadElement(currentSection.c_str(),"UseTitle",fnView, includeName);
	ObjectInfo::ReadElement(currentSection.c_str(),"FontScale",fnView, fontScale);
	lproperties.load(fnView,currentSection);
}

bool AnnotationLegendDrawer::getUseBackBackground() const {
	return useBackground;
}

void AnnotationLegendDrawer::setUseBackground(bool yesno){
	useBackground = yesno;
}

Color AnnotationLegendDrawer::getBackgroundColor() const {
	return bgColor;
}

void AnnotationLegendDrawer::setBackgroundColor(const Color& clr){
	bgColor = clr;
}
//------------------------------------------------------
ILWIS::NewDrawer *createAnnotationClassLegendDrawer(DrawerParameters *parms) {
	return new AnnotationClassLegendDrawer(parms);
}

AnnotationClassLegendDrawer::AnnotationClassLegendDrawer(DrawerParameters *parms) : AnnotationLegendDrawer(parms,"AnnotationClassLegendDrawer")
{
}

void AnnotationClassLegendDrawer::setActiveClasses(const vector<int>& rws) {
	raws.clear();
	LayerDrawer *ldr = dataDrawer->isSet() ? 
		dynamic_cast<LayerDrawer *>(dataDrawer->getDrawer(0)) :
		dynamic_cast<LayerDrawer *>(dataDrawer);
	DrawingColor dc(ldr);
	dc.setMultiColors(ldr->getDrawingColor()->multiColors(), false);
	dc.setColorSet(ldr->getDrawingColor()->colorSet());
	if (ldr->getDrawingColor()->multiColors() == 3)
		dc.copyClrRandomFrom(*(ldr->getDrawingColor()));
	for(int i=0; i < texts->getDrawerCount(); ++i) {
		texts->getDrawer(i)->setActive(false);
	}
	for(int i = 0; i < rws.size(); ++i) {
		long iRaw = rws[i]; //dm->pdc()->iKey(i+1);
		if ( iRaw == iUNDEF)
			continue;
		Color clr =dc.clrRaw(iRaw,ldr->getDrawMethod());
		raws.push_back(RawInfo(iRaw, clr));
		String txt = dm->pdc()->sValueByRaw(iRaw);
		texts->getDrawer(i)->setActive(true);
		((TextDrawer *)texts->getDrawer(i))->setText(txt);
	}
}

void AnnotationClassLegendDrawer::getActiveClasses(vector<int>& rws) const {
	rws.clear();
	for(int i = 0; i < raws.size(); ++i) {
		rws.push_back(raws[i].raw);
	}
}

void AnnotationClassLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationLegendDrawer::prepare(pp);
	LayerDrawer *ldr = dataDrawer->isSet() ? 
			dynamic_cast<LayerDrawer *>(dataDrawer->getDrawer(0)) : 
			dynamic_cast<LayerDrawer *>(dataDrawer);
	if ( pp->type & NewDrawer::ptRESTORE) {
		DrawingColor dc(ldr);
		dc.setMultiColors(ldr->getDrawingColor()->multiColors(), false);
		dc.setColorSet(ldr->getDrawingColor()->colorSet());
		if (ldr->getDrawingColor()->multiColors() == 3)
			dc.copyClrRandomFrom(*(ldr->getDrawingColor()));
		DrawerParameters dp(getRootDrawer(), texts);
		for(int i=0; i < raws.size(); ++i) {
			raws[i].clr = dc.clrRaw(raws[i].raw,ldr->getDrawMethod());
			String txt = dm->pdc()->sValueByRaw(raws[i].raw);
			TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			txtdr->setText(txt);
			texts->addDrawer(txtdr);
		}
	}
	if ( pp->type & NewDrawer::ptGEOMETRY) {
		columns = 1;
		DrawingColor dc(ldr);
		dc.setMultiColors(ldr->getDrawingColor()->multiColors(), false);
		dc.setColorSet(ldr->getDrawingColor()->colorSet());
		if (ldr->getDrawingColor()->multiColors() == 3)
			dc.copyClrRandomFrom(*(ldr->getDrawingColor()));
		DrawerParameters dp(getRootDrawer(), texts);
		raws.clear();
		for(int i = 0 ; i < dm->pdc()->iSize() ; ++i) {
			long iRaw = dm->pdc()->iKey(i+1);
			if ( iRaw == iUNDEF)
				continue;
			Color clr =dc.clrRaw(iRaw,ldr->getDrawMethod());
			String txt = dm->pdc()->sValueByRaw(iRaw);
			raws.push_back(RawInfo(iRaw,clr));
			TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			txtdr->setText(txt);
			texts->addDrawer(txtdr);
		}
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		if ( objType == IlwisObject::iotPOLYGONMAP) {
			hatches.clear();
			SpatialDataDrawer *spdr = dataDrawer->isSet() ? static_cast<SpatialDataDrawer *>(dataDrawer) : static_cast<SpatialDataDrawer *>(dataDrawer->getParentDrawer());
			LayerDrawer *ldr = dataDrawer->isSet() ? dynamic_cast<LayerDrawer *>(dataDrawer->getDrawer(0)) : dynamic_cast<LayerDrawer *>(dataDrawer);
			if (ldr->useRepresentation()) {
				Representation rpr = ldr->getRepresentation();
				if ( rpr.fValid() && rpr->prc()) {
					for(int i = 0 ; i < dm->pdc()->iSize() ; ++i) {
						long iRaw = dm->pdc()->iKey(i+1);
						if ( iRaw == iUNDEF)
							continue;
						long iRaw2 = ldr->useAttributeColumn() ? ldr->getAtttributeColumn()->iRaw(iRaw) : iRaw;
						double alpha = round(255.0 * rpr->prc()->rItemAlpha(iRaw2));
						String hatchName = rpr->prc()->sHatch(iRaw);
						if ( hatchName != sUNDEF) {
							const SVGLoader *loader = NewDrawer::getSvgLoader();
							SVGLoader::const_iterator cur = loader->find(hatchName);
							if ( cur == loader->end() || (*cur).second->getType() == IVGElement::ivgPOINT)
								return;
							const byte * hatch = (*cur).second->getHatch();
							const byte * hatchInverse = (*cur).second->getHatchInverse();
							Color backgroundColor = rpr->prc()->clrSecondRaw(iRaw2);
							long transparent = Color(-2); // in the old days this was the transparent value
							if (backgroundColor.iVal() == transparent) 
								backgroundColor = colorUNDEF;
							AnnotationClassAttributes attribs;
							attribs.alpha = alpha;
							attribs.hatch = hatch;
							attribs.hatchInverse = hatchInverse;
							attribs.backgroundColor = backgroundColor;
							hatches[iRaw] = attribs;
						}
					}
				}
			}
		}
	}

	if ( pp->type & NewDrawer::ptOFFSCREENSTART || pp->type & NewDrawer::ptOFFSCREENEND) {
		texts->prepare(pp);
	}
}

bool AnnotationClassLegendDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive())
		return false;

	bool is3D = getRootDrawer()->is3D();

	double maxw = 0;
	for(int i=0; i < raws.size(); ++i) {
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(i);
		double h = txtdr->getHeight() * 0.5;
		maxw = max(maxw, h * txtdr->getText().size());
	}

	CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
	if (cbMap.MinX() > cb.MinX())
		cb.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cb.MaxX())
		cb.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cb.MinY())
		cb.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cb.MaxY())
		cb.MaxY() = cbMap.MaxY();

	const int y_cells = 40;
	double cellHeight = cb.height() / (y_cells * 1.1);
	double cellWidth = cellHeight * 1.5;
	int rows = ceil(raws.size() / (double)columns);
	CoordBounds cbBoxRender (Coord(cb.cMin.x + cb.width() * cbBox.cMin.x, cb.cMin.y + cb.height() * cbBox.cMin.y),
		Coord(cb.cMin.x + cb.width() * cbBox.MinX() + (cellWidth + maxw) * columns, cb.cMin.y + cb.height() * cbBox.MinY() + rows * cb.height() / y_cells));

	glPushMatrix();
	glTranslated(cbBoxRender.MinX(), cbBoxRender.MinY(), 0);
	glScaled(scale, scale, 1);

	CoordBounds cbInner = CoordBounds(Coord(0,0), Coord(cbBoxRender.width(), cbBoxRender.height()));
	AnnotationLegendDrawer::draw(drawLoop, cbInner);
	drawPreDrawers(drawLoop, cbArea);
	if (drawLoop != drl3DTRANSPARENT) { // there are only opaque objects in the block; if a legend overlaps with layers, it should be drawn on-top, even if it contains transparent items
		if (is3D) // colored legend elements at level 1
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 1) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 1) * 0.0005);
		double shifty = cb.height() / y_cells;
		CoordBounds cbCell(Coord(0, cbBoxRender.height() - cellHeight),Coord(cellWidth, cbBoxRender.height()));
		for(int i=0; i < raws.size(); ++i) {
			if (objType == IlwisObject::iotRASMAP) {
				glColor3d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP());
				glBegin(GL_POLYGON);
				glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
				glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
				glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
				glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
				glEnd();
			} else if (objType == IlwisObject::iotPOLYGONMAP) {
				std::map<int, AnnotationClassAttributes>::const_iterator iter = hatches.find(raws[i].raw);
				if ( iter != hatches.end()){
					AnnotationClassAttributes attrib = (*iter).second;
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable(GL_BLEND);
					glColor3d(1,1,1);
					glBegin(GL_POLYGON);
					glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
					glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
					glEnd();
					if (is3D) // hatched legend elements at level 1.5
						glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 1.5) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 1.5) * 0.0005);
					glEnable(GL_POLYGON_STIPPLE);
					glPolygonStipple(attrib.hatch);
					glColor4d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP(), attrib.alpha * attrib.alpha / 65025.0);
					glBegin(GL_POLYGON);
					glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
					glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
					glEnd();
					glPolygonStipple(attrib.hatchInverse);
					Color backgroundColor = attrib.backgroundColor;
					glColor4f(backgroundColor.redP(), backgroundColor.greenP(), backgroundColor.blueP(), backgroundColor.alphaP() * attrib.alpha / 255.0);
					glBegin(GL_POLYGON);
					glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
					glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
					glEnd();
					glDisable(GL_POLYGON_STIPPLE);
					if (is3D) // reset to level 1
						glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 1) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 1) * 0.0005);
					glDisable(GL_BLEND);
				} else {
					glColor3d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP());
					glBegin(GL_POLYGON);
					glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
					glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
					glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
					glEnd();
				}
			} else if ( objType == IlwisObject::iotSEGMENTMAP) {
				glColor3d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP());
				glBegin(GL_LINES);
				glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
				glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
				glEnd();
			} else {
				glColor3d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP());
				glBegin(GL_POLYGON);
				double delta = cbCell.width() / 4;
				glVertex3d(cbCell.MinX() + delta, cbCell.MinY() + delta, 0);
				glVertex3d(cbCell.MinX() + delta, cbCell.MaxY() - delta, 0);
				glVertex3d(cbCell.MaxX() - delta, cbCell.MaxY() - delta, 0);
				glVertex3d(cbCell.MaxX() - delta, cbCell.MinY() + delta, 0);
				glEnd();
			}
			TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(i);
			txtdr->setCoord(Coord(cbCell.MinX() + cellWidth * 1.1, cbCell.MinY() + cbCell.height() / 3.0,0));
			cbCell.MinY() -= shifty;
			cbCell.MaxY() -= shifty;
			if ((i + 1) % rows == 0) {
				cbCell.MinY() = cbBoxRender.height() - cellHeight;
				cbCell.MaxY() = cbBoxRender.height();
				cbCell.MinX() += cellWidth + maxw;
				cbCell.MaxX() += cellWidth + maxw;
			}
		}
		if (is3D) // texts and lines at level 2
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 2) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 2) * 0.0005);
		cbCell = CoordBounds (Coord(0, cbBoxRender.height() - cellHeight),Coord(cellWidth, cbBoxRender.height()));
		for(int i=0; i < raws.size(); ++i) {
			glColor3d(0,0,0);
			glBegin(GL_LINE_STRIP);
				glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
				glVertex3d(cbCell.MinX(), cbCell.MaxY(), 0);
				glVertex3d(cbCell.MaxX(), cbCell.MaxY(), 0);
				glVertex3d(cbCell.MaxX(), cbCell.MinY(), 0);
				glVertex3d(cbCell.MinX(), cbCell.MinY(), 0);
			glEnd();
			cbCell.MinY() -= shifty;
			cbCell.MaxY() -= shifty;
			if ((i + 1) % rows == 0) {
				cbCell.MinY() = cbBoxRender.height() - cellHeight;
				cbCell.MaxY() = cbBoxRender.height();
				cbCell.MinX() += cellWidth + maxw;
				cbCell.MaxX() += cellWidth + maxw;
			}
		}
	}

	drawPostDrawers(drawLoop, cbArea); // the text elements

	glPopMatrix();
	if (is3D) // reset to level 0
		glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);

	getRootDrawer()->setZIndex(2 + getRootDrawer()->getZIndex()); // add two levels that this legend used

	return true;
}

String AnnotationClassLegendDrawer::store(const FileName& fnView, const String& parentSection) const {
	String currentSection = "AnnotationClassLegendDrawer::" + parentSection;
	AnnotationLegendDrawer::store(fnView, currentSection);
	String seq;
	for(int i=0; i<raws.size(); ++i) {
		if ( seq =="") {
			seq = String("%d", raws[i].raw);
		} else
			seq += ","+ String("%d", raws[i].raw);
	}
	ObjectInfo::WriteElement(currentSection.c_str(),"Raws",fnView, seq);

	return currentSection;
}

void AnnotationClassLegendDrawer::load(const FileName& fnView, const String& parentSection) {
	String currentSection = parentSection;
	AnnotationLegendDrawer::load(fnView, currentSection);
	String seq;
	ObjectInfo::ReadElement(currentSection.c_str(),"Raws",fnView, seq);
	Array<String> parts;
	Split(seq, parts, ",");
	raws.resize(parts.size());
	for(int i = 0; i<raws.size(); ++i) {
		raws[i].raw = parts[i].iVal();
	}
}
//-------------------------------------------------------
ILWIS::NewDrawer *createAnnotationValueLegendDrawer(DrawerParameters *parms) {
	return new AnnotationValueLegendDrawer(parms);
}

AnnotationValueLegendDrawer::AnnotationValueLegendDrawer(DrawerParameters *parms) : 
AnnotationLegendDrawer(parms,"AnnotationValueLegendDrawer"),
noTicks(5),
rstep(rUNDEF)
{
}

void AnnotationValueLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationLegendDrawer::prepare(pp);
	if ( (pp->type & NewDrawer::ptGEOMETRY) || (pp->type & NewDrawer::ptRESTORE)) {
		for(int i=0; i < 20 ; ++i) {
			DrawerParameters dp(getRootDrawer(), texts);
			TextDrawer *txt = (ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			txt->setActive(false);
			texts->addDrawer(txt);
		}
	}
	if ( pp->type & NewDrawer::ptGEOMETRY) {
		LayerDrawer *ldrw = dynamic_cast<LayerDrawer *>(getParentDrawer());
		SetDrawer *sdrw = dynamic_cast<SetDrawer *>(getParentDrawer());
		if ( sdrw) {
			vrr = sdrw->getStretchRangeReal();
		} else {
			vrr = ldrw->getStretchRangeReal(true);
		}
	}
	if ( pp->type & NewDrawer::ptOFFSCREENSTART || pp->type & NewDrawer::ptOFFSCREENEND) {
		texts->prepare(pp);
	}
}

bool AnnotationValueLegendDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{

	if ( !isActive())
		return false;

	// CoordBounds cbZoomExt = getRootDrawer()->getCoordBoundsZoomExt();
	//if ( !cbZoomExt.fContains(cbBox))
	//	return false;

	bool is3D = getRootDrawer()->is3D();

	CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
	if (cbMap.MinX() > cb.MinX())
		cb.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cb.MaxX())
		cb.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cb.MinY())
		cb.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cb.MaxY())
		cb.MaxY() = cbMap.MaxY();

	double frameWidth = vertical ? cbBox.width() * cb.height(): cbBox.width() * cb.width();
	double frameHeight = vertical ? cbBox.height() * cb.height() : cbBox.height() * cb.width();

	CoordBounds cbBoxRender (Coord(cb.cMin.x + cb.width() * cbBox.cMin.x, cb.cMin.y + cb.height() * cbBox.cMin.y),
		Coord(cb.cMin.x + cb.width() * cbBox.MinX() + frameWidth, cb.cMin.y + cb.height() * cbBox.MinY() + frameHeight));

	glPushMatrix();
	glTranslated(cbBoxRender.MinX(), cbBoxRender.MinY(), 0);
	glScaled(scale, scale, 1);

	double w = cbBoxRender.width();
	CoordBounds cbInner = CoordBounds(Coord(0,0), Coord(cbBoxRender.width(), cbBoxRender.height()));

	AnnotationLegendDrawer::draw(drawLoop, cbInner);
	drawPreDrawers(drawLoop, cbArea);

	double alpha = getAlpha();

	if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
		if (is3D) // colored legend elements at level 1
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 1) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 1) * 0.0005);
		double noOfRect = 100.0;
		DomainValueRangeStruct dvs;
		SpatialDataDrawer *spdr = dataDrawer->isSet() ? 
			static_cast<SpatialDataDrawer *>(dataDrawer) : 
			static_cast<SpatialDataDrawer *>(dataDrawer->getParentDrawer());

		LayerDrawer *ldr = dataDrawer->isSet() ? 
			dynamic_cast<LayerDrawer *>(dataDrawer->getDrawer(0)) : 
			dynamic_cast<LayerDrawer *>(dataDrawer);

		dvs = spdr->getBaseMap()->dvrs();
		if ( ldr && ldr->useAttributeColumn() && ldr->getAtttributeColumn().fValid()) {
			dvs = ldr->getAtttributeColumn()->dvrs();
		}
		vector<String> values = makeRange(dvs);
		RangeReal rr(dvs.rValue(values[0]), dvs.rValue(values[values.size()- 1]));
		if ( vertical) {
			drawVertical(cbInner, rr, 0, values, dvs);
		} else {
			drawHorizontal(cbInner, cbBoxRender, rr, 0, values, dvs);
		}
		if (is3D) // texts and lines at level 2
			glDepthRange(0.01 - (getRootDrawer()->getZIndex() + 2) * 0.0005, 1.0 - (getRootDrawer()->getZIndex() + 2) * 0.0005);
		glColor4f(0,0, 0, alpha );
		glBegin(GL_LINE_STRIP);
		Coordinate c(cbInner.MinX(), cbInner.MinY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbInner.MinX(), cbInner.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbInner.MaxX(), cbInner.MaxY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbInner.MaxX(), cbInner.MinY());
		glVertex3d( c.x, c.y, 0);
		c = Coordinate(cbInner.MinX(), cbInner.MinY());
		glVertex3d( c.x, c.y, 0);
		glEnd();
	}

	drawPostDrawers(drawLoop, cbArea); // the text elements
	glPopMatrix();
	if (is3D) // reset to level 0
		glDepthRange(0.01 - getRootDrawer()->getZIndex() * 0.0005, 1.0 - getRootDrawer()->getZIndex() * 0.0005);

	getRootDrawer()->setZIndex(2 + getRootDrawer()->getZIndex()); // add two levels that this legend used

	return true;
}

void AnnotationValueLegendDrawer::drawVertical(CoordBounds& cbInner, const RangeReal& rr, double z, const vector<String>& values, const DomainValueRangeStruct & dvs) const{
	int count = 1;
	DrawingColor dc(dataDrawer);
	cbInner.MaxX() = cbInner.MinX() + cbInner.width() / 3;
	double startx = cbInner.MinX();
	double starty = cbInner.MinY();
	double endx = startx + cbInner.width();
	double rStep = rr.rWidth() / 100.0;
	double rV = rr.rLo() + 0.5 * rStep;
	for(int i=0; i < texts->getDrawerCount(); ++i){
		if ( texts->getDrawer(i))
			texts->getDrawer(i)->setActive(false);
	}
	setText(values,0,Coord(endx + cbInner.width() / 3, starty  + cbInner.height() / 100.0,z));
	for(int i=0; i < 100; ++i) {
		Color clr = dc.clrVal(rV);
		glColor4f(clr.redP(),clr.greenP(), clr.blueP(), getAlpha() );
		double endy = starty + cbInner.height() / 100.0;
		glBegin(GL_POLYGON);
		glVertex3d(startx,starty,z);
		glVertex3d(startx,endy,z);
		glVertex3d(endx,endy,z);
		glVertex3d(endx,starty,z);
		glEnd();
		if ( count < values.size() && dvs.rValue(values[count]) <= rV) { 
			setText(values, count, Coord(endx + cbInner.width() / 3, starty,z));
			glColor4f(0,0,0,getAlpha());
			glBegin(GL_LINE_STRIP);
			glVertex3d(endx,endy,z);
			glVertex3d(endx + cbInner.width() / 10.0,endy,z);
			glEnd();
			++count;
		}
		starty = endy;
		rV += rStep;
	}
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( values.size()-1);
	double h = txt->getHeight() * 0.9;
	setText(values,values.size()-1,Coord(endx + cbInner.width() / 3, cbInner.height () - h/2.0,z));
}

void AnnotationValueLegendDrawer::drawHorizontal(CoordBounds& cbInner, const CoordBounds & cbBoxRender, const RangeReal& rr, double z, const vector<String>& values, const DomainValueRangeStruct & dvs) const{
	int count = 1;
	double shifty = 5.0;
	DrawingColor dc(dataDrawer);
	cbInner.MinY() = cbInner.MaxY() - cbBoxRender.height() / 2;
	double startx = cbInner.MinX();
	double starty = cbInner.MaxY();
	double endy = starty - cbInner.height();
	double rStep = rr.rWidth() / 100.0;
	double rV = rr.rLo() + 0.5 * rStep;

	for(int i=0; i < texts->getDrawerCount(); ++i){
		if ( texts->getDrawer(i))
			texts->getDrawer(i)->setActive(false);
	}
	setText(values,0,Coord(startx, endy - cbBoxRender.height() / shifty,z));
	for(int i=0; i < 100; ++i) {
		Color clr = dc.clrVal(rV);
		glColor4f(clr.redP(),clr.greenP(),clr.blueP(),getAlpha());
		double endx = startx + cbInner.width() / 100.0;
		glBegin(GL_POLYGON);
		glVertex3d(startx,starty,z);
		glVertex3d(startx,endy,z);
		glVertex3d(endx,endy,z);
		glVertex3d(endx,starty,z);
		glEnd();
		if ( dvs.rValue(values[count]) <= rV) { 
			setText(values, count, Coord( startx, endy - cbBoxRender.height() / shifty,z));
			glColor4f(0,0,0,getAlpha());
			glBegin(GL_LINE_STRIP);
			glVertex3d(endx,endy,z);
			glVertex3d(endx ,endy - cbBoxRender.height() / (shifty + 8),z);
			glEnd();
			++count;
		}
		startx = endx;
		rV += rStep;
	}
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( values.size()-1);
	//double h = txt->getHeight() * 0.9;
	//String s(values[values.size()-1]);
	CoordBounds cbTxt = txt->getTextExtent();
	setText(values,values.size()-1,Coord(cbInner.MaxX() - cbTxt.width() / 2.0, cbInner.MaxY( ) - cbInner.height() - cbBoxRender.height() / shifty,z));
}

vector<String> AnnotationValueLegendDrawer::makeRange(const DomainValueRangeStruct & dvs) const{
	vector<String> values;
	RangeReal rmd;
	double step = 1.0;
	bool fImage = dvs.dm()->pdi() || (( vrr.rLo() == 0 || vrr.rLo() == 1) && vrr.rHi() == 255);
	if ( fImage) {
		if ( rstep == rUNDEF) {
			step = 30;
			rmd = RangeReal(0,255);
			fImage = true;
		} else {
			step = rstep;
			rmd = RangeReal(vrr.rLo(), vrr.rHi());
		}
	} else{
		if ( rstep == rUNDEF)
			rmd = roundRange(vrr.rLo(), vrr.rHi(), step);
		else {
			rmd = vrr;
			step = rstep;
		}
	}

	int dec = -1;
	if ( rmd.rWidth() > 10)
		dec = 0;
	if ( (rmd.rWidth() / step) > 19) {
		step = rmd.rWidth() / 10;
	}
	for (double v = rmd.rLo(); v <= rmd.rHi(); v += step) {
		String sName;
		if ( fImage && v + step > 255) {
			sName = "255";
		} else
			sName = dvs.sValue(v, -1, dec);
		values.push_back(sName.sTrimSpaces());
	}	
	return values;
}

RangeReal AnnotationValueLegendDrawer::getRange() const{
	return vrr;
}
void AnnotationValueLegendDrawer::setRange(RangeReal& range){
	vrr = range;
}
double AnnotationValueLegendDrawer::getStep() const{
	return rstep;
}
void AnnotationValueLegendDrawer::setStep(double s){
	rstep = s;
}

void AnnotationValueLegendDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	AnnotationLegendDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"LegendSteps",fnView, rstep);
	ObjectInfo::ReadElement(currentSection.c_str(),"LegendRange",fnView, vrr);
}

String AnnotationValueLegendDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "LegendValueDrawer::" + parentSection;
	AnnotationLegendDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"LegendSteps",fnView, rstep);
	ObjectInfo::WriteElement(currentSection.c_str(),"LegendRange",fnView, vrr);

	return currentSection;


}
//----------------------------------------------------------
ILWIS::NewDrawer *createAnnotationBorderDrawer(DrawerParameters *parms) {
	return new AnnotationBorderDrawer(parms);
}

AnnotationBorderDrawer::AnnotationBorderDrawer(DrawerParameters *parms) : 
AnnotationDrawer(parms, "AnnotationBorderDrawer"),
borderBox(0), neatLine(true), ticks(true), step(1), numDigits(2){
	for(int i=0; i < 4; ++i)
		hasText.push_back(true);
	id = "AnnotationBorderDrawer";
}

AnnotationBorderDrawer::~AnnotationBorderDrawer() {
	delete borderBox;
}

bool AnnotationBorderDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	bool fBorderActive = isActive();
	bool fWhitespaceActive = getRootDrawer()->fWhitespace();
	bool fActive = fBorderActive || fWhitespaceActive;
	if (!fActive)
		return false;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	bool is3D = getRootDrawer()->is3D();
	
	CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
	CoordBounds cbInner = cbArea; // cbArea is cbZoom
	if (cbMap.MinX() > cbInner.MinX()) // limit cbArea by cbMap
		cbInner.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cbInner.MaxX())
		cbInner.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cbInner.MinY())
		cbInner.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cbInner.MaxY())
		cbInner.MaxY() = cbMap.MaxY();

	CoordBounds cbMapExt = getRootDrawer()->getMapCoordBoundsExt();
	CoordBounds cbOuter = getRootDrawer()->getCoordBoundsZoomExt();
	if (cbMapExt.MinX() > cbOuter.MinX()) // limit cbArea by cbMapExt
		cbOuter.MinX() = cbMapExt.MinX();
	if (cbMapExt.MaxX() < cbOuter.MaxX())
		cbOuter.MaxX() = cbMapExt.MaxX();
	if (cbMapExt.MinY() > cbOuter.MinY())
		cbOuter.MinY() = cbMapExt.MinY();
	if (cbMapExt.MaxY() < cbOuter.MaxY())
		cbOuter.MaxY() = cbMapExt.MaxY();

	borderBox->setBox(cbOuter, cbInner);
	// draw white space around
	borderBox->draw(drawLoop, cbArea);

	if (fBorderActive) {
		if ( hasText[0] )
			setText(cbInner,cbInner,AnnotationBorderDrawer::sLEFT, 0);
		if ( hasText[1] )
			setText(cbInner,cbInner,AnnotationBorderDrawer::sRIGHT, 0);
		if ( hasText[2] )
			setText(cbInner,cbInner,AnnotationBorderDrawer::sTOP, 0);
		if ( hasText[3] )
			setText(cbInner,cbInner,AnnotationBorderDrawer::sBOTTOM, 0);
		// draw texts
		AnnotationDrawer::draw(drawLoop, cbArea);
		if (drawLoop != drl3DTRANSPARENT) {
			if (neatLine) {
				// draw neat line
				glColor3d(0,0,0);
				glBegin(GL_LINE_STRIP);
				glVertex3d(cbInner.MinX(), cbInner.MinY(),0);
				glVertex3d(cbInner.MinX(), cbInner.MaxY(),0);
				glVertex3d(cbInner.MaxX(), cbInner.MaxY(),0);
				glVertex3d(cbInner.MaxX(), cbInner.MinY(),0);
				glVertex3d(cbInner.MinX(), cbInner.MinY(),0);
				glEnd();
			}
			if (ticks) {
				// draw ticks
				if ( hasText[0] )
					drawTicks(cbInner,cbInner,AnnotationBorderDrawer::sLEFT, 0);
				if ( hasText[1] )
					drawTicks(cbInner,cbInner,AnnotationBorderDrawer::sRIGHT, 0);
				if ( hasText[2] )
					drawTicks(cbInner,cbInner,AnnotationBorderDrawer::sTOP, 0);
				if ( hasText[3] )
					drawTicks(cbInner,cbInner,AnnotationBorderDrawer::sBOTTOM, 0);
			}
		} 
	}

	glDisable(GL_BLEND);

	return true;
}

struct ValFinder
{
	ValFinder(const RootDrawer* _rootDrawer, const Coord& coord, bool fXcoord, bool fRow, double rGoalVal) 
		: rootDrawer(_rootDrawer), crd(coord), fX(fXcoord), fFindRow(fRow), rGoal(rGoalVal)
		{}
	virtual double operator()(double rX)
	{
		if (fX)
			crd.x = rX;
		else
			crd.y = rX;
		return rCalc();
	}
	double rCalc() 
	{
		Coord crdRowCol (rootDrawer->glConv(crd));
		if (fFindRow) {
			rVal = crdRowCol.y;
			return crdRowCol.x - rGoal;
		}
		else {
			rVal = crdRowCol.x;
			return crdRowCol.y - rGoal;
		}
	}
	double rValue() const 
		{ return rVal; }
protected:
	bool fX, fFindRow;
	Coord crd;
	const RootDrawer * rootDrawer;
	double rGoal;
	double rVal;
};

// this iterative function can probably be optimized
double rFindNull(ValFinder& vf, double rDflt)
{
	double rX = rDflt;
	double rStep = 1e100;
	int iIters = 0;
	while (true) {
		double rY1 = vf(rX+1);
		double rY = vf(rX);
		double rDY = (rY1 - rY);
		if (abs(rDY) < 1e-100) // y does not depend on x ???
			return rUNDEF;
		double rDX = rY / rDY;
		if (abs(rDX) < 1e-6 || ++iIters > 10)
			return vf.rValue();
		if (abs(rDX) > abs(rStep) * 0.6) {
			rDX = sign(rDX) * abs(rStep) / 2;
			rStep *= 0.9;
		}
		else {
			rStep = rDX;
		}
		rX -= rDX;
	}
}

Coordinate ptBorderX(const CoordBounds & cb, RootDrawer * rootDrawer, AnnotationBorderDrawer::Side side, double rX)
{
	double centerX = cb.MinX() + cb.width() / 2.0;
	double centerY = cb.MinY() + cb.height() / 2.0;
	Coord crdCenter = rootDrawer->glToWorld(Coord(centerX, centerY));
	crdCenter.x = rX;
	double r;

	switch (side)
	{
		case AnnotationBorderDrawer::sTOP: 
			r = rFindNull(ValFinder(rootDrawer,crdCenter,false,false,cb.MaxY()), crdCenter.y);
			return Coordinate(r, cb.MaxY());
		case AnnotationBorderDrawer::sBOTTOM:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,false,false,cb.MinY()), crdCenter.y);
			return Coordinate(r, cb.MinY());
		case AnnotationBorderDrawer::sLEFT:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,false,true,cb.MinX()), crdCenter.y);
			return Coordinate(cb.MinX(), r);
		case AnnotationBorderDrawer::sRIGHT:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,false,true,cb.MaxX()), crdCenter.y);
			return Coordinate(cb.MaxX(), r);
	}
	return crdUNDEF;
}

Coordinate ptBorderY(const CoordBounds & cb, RootDrawer * rootDrawer, AnnotationBorderDrawer::Side side, double rY)
{
	double centerX = cb.MinX() + cb.width() / 2.0;
	double centerY = cb.MinY() + cb.height() / 2.0;
	Coord crdCenter = rootDrawer->glToWorld(Coord(centerX, centerY));
	crdCenter.y = rY;
	double r;

	switch (side)
	{
		case AnnotationBorderDrawer::sTOP: 
			r = rFindNull(ValFinder(rootDrawer,crdCenter,true,false,cb.MaxY()), crdCenter.x);
			return Coordinate(r, cb.MaxY());
		case AnnotationBorderDrawer::sBOTTOM:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,true,false,cb.MinY()), crdCenter.x);
			return Coordinate(r, cb.MinY());
		case AnnotationBorderDrawer::sLEFT:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,true,true,cb.MinX()), crdCenter.x);
			return Coordinate(cb.MinX(), r);
		case AnnotationBorderDrawer::sRIGHT:
			r = rFindNull(ValFinder(rootDrawer,crdCenter,true,true,cb.MaxX()), crdCenter.x);
			return Coordinate(cb.MaxX(), r);
	}
	return crdUNDEF;
}

void AnnotationBorderDrawer::setText(const CoordBounds & cb, const CoordBounds & cbArea, AnnotationBorderDrawer::Side side, double z) const{
	if ( side == sLEFT || side == sRIGHT) {
		for(int i = 0; i < ypos.size(); ++i) {
			TextDrawer *txtdrw = const_cast<AnnotationBorderDrawer *>(this)->getTextDrawer(i,side);
			CoordBounds cbText = txtdrw->getTextExtent();
			double x;
			if (side == sLEFT)
				x = cb.cMin.x - cbText.width() - cbArea.width() * 0.01;
			else // sRIGHT
				x = cb.cMax.x + cbArea.width() * 0.01;
			double y = ptBorderY(cb, getRootDrawer(), side, ypos[i]).y;			
			txtdrw->setCoord(Coord(x, y - cbText.height() / 2, z));
			txtdrw->setActive(cb.MaxY() >= y && y >= cb.MinY());
		}
	} else {
		if ( side == sTOP || side == sBOTTOM) {
			for(int i = 0; i < xpos.size(); ++i) {
				TextDrawer *txtdrw = const_cast<AnnotationBorderDrawer *>(this)->getTextDrawer(i,side);
				CoordBounds cbText = txtdrw->getTextExtent();
				double y;
				if (side == sTOP)
					y = cb.cMax.y + cbArea.height() * 0.01;
				else // sBOTTOM
					y = cb.cMin.y - cbText.height() - cbArea.height() * 0.01;
				double x = ptBorderX(cb, getRootDrawer(), side, xpos[i]).x;
				txtdrw->setCoord(Coord(x - cbText.width() / 2, y, z));
				txtdrw->setActive(cb.MaxX() >= x && x >= cb.MinX());
			}
		}
	}
}

void AnnotationBorderDrawer::drawTicks(const CoordBounds & cb, const CoordBounds & cbArea, AnnotationBorderDrawer::Side side, double z) const{
	if (side == sLEFT || side == sRIGHT) {
		for(int i = 0; i < ypos.size(); ++i) {
			double y = ptBorderY(cb, getRootDrawer(), side, ypos[i]).y;
			if (cb.MaxY() >= y && y >= cb.MinY()) {
				double x;
				double x1;
				if (side == sLEFT) {
					x = cb.cMin.x;
					x1 = x - cbArea.width() * 0.005;
				} else { // sRIGHT
					x = cb.cMax.x;
					x1 = x + cbArea.width() * 0.005;
				}
				glBegin(GL_LINE_STRIP);
				glVertex3d(x, y, z);
				glVertex3d(x1, y, z);
				glEnd();
			}
		}
	} else if (side == sTOP || side == sBOTTOM) {
		for(int i = 0; i < xpos.size(); ++i) {
			double x = ptBorderX(cb, getRootDrawer(), side, xpos[i]).x;
			if (cb.MaxX() >= x && x >= cb.MinX()) {
				double y;
				double y1;
				if (side == sTOP) {
					y = cb.cMax.y;
					y1 = y + cbArea.height() * 0.005;
				} else { // sBOTTOM
					y = cb.cMin.y;
					y1 = y - cbArea.height() * 0.005;
				}
				glBegin(GL_LINE_STRIP);
				glVertex3d(x, y, z);
				glVertex3d(x, y1, z);
				glEnd();
			}
		}
	}
}

int AnnotationBorderDrawer::getNumberOfDigits() const{
	return numDigits;
}

void AnnotationBorderDrawer::setNumberOfDigits(int num){
	numDigits = num;
}

String AnnotationBorderDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "AnnotationBorderDrawer::" + parentSection;
	AnnotationDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Steps",fnView, step);
	ObjectInfo::WriteElement(currentSection.c_str(),"Neatline",fnView, neatLine);
	ObjectInfo::WriteElement(currentSection.c_str(),"Ticks",fnView, ticks);
	ObjectInfo::WriteElement(currentSection.c_str(),"digits",fnView, numDigits);

	return currentSection;
}

void AnnotationBorderDrawer::load(const FileName& fnView, const String& parentSection){
		String currentSection = parentSection;
		AnnotationDrawer::load(fnView, currentSection);

		ComplexDrawer *annotations = (ComplexDrawer *)getParentDrawer();

		vector<NewDrawer *> allDrawers;
		annotations->getDrawers(allDrawers);
		for(int i = 0; i < allDrawers.size(); ++i) {
			GridDrawer *gdrw = dynamic_cast<GridDrawer *>(allDrawers[i]);
			if ( gdrw)
				dataDrawer = gdrw; 
		}

		ObjectInfo::ReadElement(currentSection.c_str(),"Steps",fnView, step);
		ObjectInfo::ReadElement(currentSection.c_str(),"Neatline",fnView, neatLine);
		ObjectInfo::ReadElement(currentSection.c_str(),"Ticks",fnView, ticks);
		ObjectInfo::ReadElement(currentSection.c_str(),"digits",fnView, numDigits);	
}

void AnnotationBorderDrawer::prepare(PreparationParameters *pp){
	AnnotationDrawer::prepare(pp);
	if (  pp->type & RootDrawer::ptGEOMETRY || pp->type & RootDrawer::ptRESTORE){
		calcLocations();
		isLatLon = getRootDrawer()->getCoordinateSystem()->pcsLatLon() != 0;
		if ( !borderBox) {
			DrawerParameters dp(getRootDrawer(),this);
			borderBox = new BoxDrawer(&dp);
			borderBox->setAlpha(1);
			borderBox->setDrawColor(Color(255,255,255));
			texts = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dp);
			OpenGLText * font = new OpenGLText(getRootDrawer(),"arial.ttf",12,false);
			texts->setFont(font);
			getRootDrawer()->setAnnotationFont(font);
			addDrawer(texts);
		}
		texts->clear();
		DrawerParameters dp(getRootDrawer(),texts);
		for(int i = 0 ; i < xpos.size() * 2; ++i) { 
			TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			texts->addDrawer(txtdr);

		}
		for(int i = 0 ; i < ypos.size() * 2; ++i) { 
			TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			texts->addDrawer(txtdr);
		}

		for(int i = 0; i < ypos.size(); ++i) {
			String txt =  isLatLon ? String("%.*f",numDigits, ypos[i]) : String("%d", (long)ypos[i]);
			TextDrawer *txtdrw = getTextDrawer(i,sLEFT);
			txtdrw->setText(txt);
			txtdrw = getTextDrawer(i,sRIGHT);
			txtdrw->setText(txt);
		}

		for(int i = 0; i < xpos.size(); ++i) {
			String txt =  isLatLon ? String("%.*f",numDigits, xpos[i]) : String("%d", (long)xpos[i]);
			TextDrawer *txtdrw = getTextDrawer(i,sTOP);
			txtdrw->setText(txt);
			txtdrw = getTextDrawer(i,sBOTTOM);
			txtdrw->setText(txt);
		}
	}
	if ( pp->type & NewDrawer::ptOFFSCREENSTART || pp->type & NewDrawer::ptOFFSCREENEND) {
		texts->prepare(pp);
	}
}

TextDrawer *AnnotationBorderDrawer::getTextDrawer(int index, Side side) {
	int offset = 0;
	if ( side == AnnotationBorderDrawer::sRIGHT)
		offset = ypos.size();
	if ( side == AnnotationBorderDrawer::sTOP)
		offset = ypos.size() *2;
	if ( side == AnnotationBorderDrawer::sBOTTOM)
		offset = ypos.size() * 2  + xpos.size();
	if ( offset + index < texts->getDrawerCount())
		return (TextDrawer *)texts->getDrawer(offset + index);
	return 0;
}

void AnnotationBorderDrawer::calcLocations() {
	ypos.clear();
	xpos.clear();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
	CoordBounds cbMapWorld;
	cbMapWorld += getRootDrawer()->glToWorld(cbMap.cMin);
	cbMapWorld += getRootDrawer()->glToWorld(cbMap.cMax);
	cbMapWorld += getRootDrawer()->glToWorld(Coord(cbMap.cMin.x, cbMap.cMax.y));
	cbMapWorld += getRootDrawer()->glToWorld(Coord(cbMap.cMax.x, cbMap.cMin.y));
	Coord cMin = cbMapWorld.cMin;
	Coord cMax = cbMapWorld.cMax;
	GridDrawer *gdr = dynamic_cast<GridDrawer *>(dataDrawer);
	if ( !gdr)
		return;
	double rDistance = gdr->getGridSpacing();
	if ( rDistance == rUNDEF)
		rDistance = rRound((cMax.x - cMin.x) / 7);
	int count = 0;
	for (double x = ceil(cMin.x / rDistance) * rDistance; x < cMax.x ; x += rDistance, ++count)
	{
		if ( count % step == 0)
			xpos.push_back(x);
	}
	count = 0;
	for (double y = ceil(cMin.y / rDistance) * rDistance; y < cMax.y ; y += rDistance, ++count)
	{
		if ( count % step == 0)
			ypos.push_back(y);
	}
}

bool AnnotationBorderDrawer::hasNeatLine() const{
	return neatLine;
}

bool AnnotationBorderDrawer::hasTicks() const{
	return ticks;
}

int AnnotationBorderDrawer::getStep() const{
	return step;
}

void AnnotationBorderDrawer::setHasNeatLine(bool yesno){
	neatLine = yesno;
}

void AnnotationBorderDrawer::setHasTicks(bool yesno){
	ticks = yesno;
}

void AnnotationBorderDrawer::setStep(int st){
	if ( st > 0)
		step = st;
}
//------------------------------------------------
ILWIS::NewDrawer *createAnnotationScaleBarDrawer(DrawerParameters *parms) {
	return new AnnotationScaleBarDrawer(parms);
}

AnnotationScaleBarDrawer::AnnotationScaleBarDrawer(DrawerParameters *parms) : 
AnnotationDrawer(parms, "AnnotationScaleBarDrawer"),
size(rUNDEF), ticks(5), texts(0), unit("meters"), km(false), line(false), divideFirstInterval(true), multiLabels(false)
{
	CoordSystem csy = getRootDrawer()->getCoordinateSystem();
	if (csy.fValid() && csy->pcsLatLon())
		unit = "degrees";
	CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
	if (cbMap.MinX() > cb.MinX())
		cb.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cb.MaxX())
		cb.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cb.MinY())
		cb.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cb.MaxY())
		cb.MaxY() = cbMap.MaxY();

	CoordBounds cbProject (getRootDrawer()->glToWorld(cb.cMin), getRootDrawer()->glToWorld(cb.cMax));

	size = max(1.0, rRound(cbProject.width() * 0.2 / ticks));
	double totSize = ticks * size / cbProject.width();
	height = 0.01;
	begin.y = 0.95;
	begin.x = 0.5 - totSize / 2.0;
}

void AnnotationScaleBarDrawer::prepare(PreparationParameters *pp){
	AnnotationDrawer::prepare(pp);
	if (  pp->type & RootDrawer::ptGEOMETRY || pp->type & RootDrawer::ptRESTORE){
		if (!texts) {
			DrawerParameters dp(getRootDrawer(),this);
			texts = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dp);
			texts->setFont(new OpenGLText(getRootDrawer(),"arial.ttf",12,false));
			addPostDrawer(100,texts);
		}
		texts->clear();
		for(int i=0; i <= ticks; ++i) {
			DrawerParameters dp(getRootDrawer(),texts);
			TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp);
			texts->addDrawer(txtdr);
		}
	}
	if ( pp->type & NewDrawer::ptOFFSCREENSTART || pp->type & NewDrawer::ptOFFSCREENEND) {
		texts->prepare(pp);
	}
}

bool AnnotationScaleBarDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive())
		return false;

	bool is3D = getRootDrawer()->is3D(); 

	CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
	if (cbMap.MinX() > cb.MinX())
		cb.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cb.MaxX())
		cb.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cb.MinY())
		cb.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cb.MaxY())
		cb.MaxY() = cbMap.MaxY();
	CoordBounds cbProject (getRootDrawer()->glToWorld(cb.cMin), getRootDrawer()->glToWorld(cb.cMax));
	Coord crd (cb.MinX() + cb.width() * begin.x, cb.MinY() + cb.height() * begin.y);

	double tickSize = size * cb.width() / cbProject.width();
	double totSize = ticks * tickSize;
	if (totSize > cb.width() / 2.0 || totSize < cb.width() / 15.0) { // too big or too small: recompute size
		const_cast<AnnotationScaleBarDrawer*>(this)->size = max(1.0, rRound(cbProject.width() * 0.2 / ticks));
		tickSize = size * cb.width() / cbProject.width();
		totSize = ticks * tickSize;
	}

	glPushMatrix();
	glTranslated(crd.x, crd.y, 0);

	glColor3d(0,0,0);
	drawPreDrawers(drawLoop, cbArea);

	if (drawLoop != drl3DTRANSPARENT) { // there are only opaque objects in the block
		double start = 0;
		if (line) {
			glBegin(GL_LINES);
			glVertex3d(0, 0, 0);
			glVertex3d(totSize, 0, 0);
			for(int i = 0; i <= ticks; ++i) {
				if (divideFirstInterval && i == 0) { // the first interval should be divided into 5 small parts
					double startD = start;
					double tickSizeD = tickSize / 5.0;
					for (int j = 0; j < 5; ++j) {
						glVertex3d(startD, 0, 0);
						glVertex3d(startD, -height * cb.height(), 0);
						startD += tickSizeD;
					}
				} else {
					glVertex3d(start, 0, 0);
					glVertex3d(start, -height * cb.height(), 0);
				}
				start += tickSize;
			}
			glEnd();
		} else { // blocked
			bool white = false; // start with black
			glBegin(GL_QUADS);
			for(int i = 0; i < ticks; ++i) {
				if (divideFirstInterval && i == 0) { // the first interval should be divided into 5 small parts
					double startD = start;
					double tickSizeD = tickSize / 5.0;
					for (int j = 0; j < 5; ++j) {
						if (white)
							glColor3d(255,255,255);
						else
							glColor3d(0,0,0);
						glVertex3d(startD, 0, 0);
						glVertex3d(startD, -height * cb.height(), 0);
						glVertex3d(startD + tickSizeD, -height * cb.height(), 0);
						glVertex3d(startD + tickSizeD, 0, 0);
						white = !white;
						startD += tickSizeD;
					}
				} else {
					if (white)
						glColor3d(255,255,255);
					else
						glColor3d(0,0,0);
					glVertex3d(start, 0, 0);
					glVertex3d(start, -height * cb.height(), 0);
					glVertex3d(start + tickSize, -height * cb.height(), 0);
					glVertex3d(start + tickSize, 0, 0);
					white = !white;
				}
				start += tickSize;
			}
			glEnd();
			glColor3d(0,0,0);
			glBegin(GL_LINE_STRIP);
			glVertex3d(0, 0, 0);
			glVertex3d(totSize, 0, 0);
			glVertex3d(totSize, -height * cb.height(), 0);
			glVertex3d(0, -height * cb.height(), 0); // the left vertical line is never drawn (not needed)
			glEnd();
		}
		for(int i = 0; i <= ticks; ++i) {
			if (i > 0 && !multiLabels)
				i = ticks;
			TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(i);
			if ( txtdr) {
				String s = km ? String("%d",(long)(i * size / 1000.0)) : String("%d",(long)size * i);
				txtdr->setText(s);
				double h = txtdr->getHeight();
				h += height * cb.height();
				double xShift = h * s.size() / 6.0;
				double x = i * tickSize - xShift;
				if ( i == ticks)
					s += " " + unit;
				txtdr->setText(s);
				txtdr->setCoord(Coord(x, -h,0));
			}
		}
	}

	drawPostDrawers(drawLoop, cbArea);

	glPopMatrix();

	return true;
}

Coord AnnotationScaleBarDrawer::getBegin(){
	return begin;
}

void AnnotationScaleBarDrawer::setBegin(const Coord& b){
	begin = b;
}

double AnnotationScaleBarDrawer::getSize() const{
	return size;
}

void AnnotationScaleBarDrawer::setSize(double w){
	size = w;
}

String AnnotationScaleBarDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "AnnotationBorderDrawer::" + parentSection;
	AnnotationDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Size",fnView, size);
	ObjectInfo::WriteElement(currentSection.c_str(),"Begin",fnView, begin);
	ObjectInfo::WriteElement(currentSection.c_str(),"Height",fnView, height);
	ObjectInfo::WriteElement(currentSection.c_str(),"Ticks",fnView, ticks);
	ObjectInfo::WriteElement(currentSection.c_str(),"Unit",fnView, unit);
	ObjectInfo::WriteElement(currentSection.c_str(),"UseKm",fnView, km);
	return currentSection;
}

void AnnotationScaleBarDrawer::load(const FileName& fnView, const String& parentSection){
	String currentSection = parentSection;
	AnnotationDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(currentSection.c_str(),"Size",fnView, size);
	ObjectInfo::ReadElement(currentSection.c_str(),"Begin",fnView, begin);
	ObjectInfo::ReadElement(currentSection.c_str(),"Height",fnView, height);
	ObjectInfo::ReadElement(currentSection.c_str(),"Ticks",fnView, ticks);
	ObjectInfo::ReadElement(currentSection.c_str(),"Unit",fnView, unit);
	ObjectInfo::ReadElement(currentSection.c_str(),"UseKm",fnView, km);
}

String AnnotationScaleBarDrawer::getUnit() const{
	return unit;
}

void AnnotationScaleBarDrawer::setUnit(const String& _unit){
	unit = _unit;
}

int AnnotationScaleBarDrawer::getTicks() const{
	return ticks;
}

void AnnotationScaleBarDrawer::setTicks(int t){
	if ( t > 1)
		ticks = t;
	else
		ticks = 2;
}

bool AnnotationScaleBarDrawer::getKm() const{
	return km;
}

void AnnotationScaleBarDrawer::setKm(bool k){
	km = k;
}

bool AnnotationScaleBarDrawer::getLine() const{
	return line;
}

void AnnotationScaleBarDrawer::setLine(bool l){
	line = l;
}

bool AnnotationScaleBarDrawer::getDivideFirstInterval() const{
	return divideFirstInterval;
}

void AnnotationScaleBarDrawer::setDivideFirstInterval(bool d){
	divideFirstInterval = d;
}

bool AnnotationScaleBarDrawer::getMultiLabels() const{
	return multiLabels;
}

void AnnotationScaleBarDrawer::setMultiLabels(bool l){
	multiLabels = l;
}

//------------------------------------------------
ILWIS::NewDrawer *createAnnotationNorthArrowDrawer(DrawerParameters *parms) {
	return new AnnotationNorthArrowDrawer(parms);
}

Coord AnnotationNorthArrowDrawer::getBegin(){
	return begin;
}
void AnnotationNorthArrowDrawer::setBegin(const Coord& b){
	begin = b;
	if ( arrow)
		arrow->setCoord(b);
}

void AnnotationNorthArrowDrawer::setScale(double s){
	scale = s;
	if ( arrow) {
		PointProperties *prop = (PointProperties *)arrow->getProperties();
		prop->scale = scale;
	}
}
void AnnotationNorthArrowDrawer::setArrowType(const String& type){
	northArrowType = type;
	if (arrow) {
		PointProperties *prop = (PointProperties *)arrow->getProperties();
		prop->symbol = type;
		PreparationParameters pp(NewDrawer::ptRENDER);
		pp.props.symbolType = type;
		arrow->prepare(&pp);
	}
}

String AnnotationNorthArrowDrawer::getArrowType() const{
	return northArrowType;
}

String AnnotationNorthArrowDrawer::store(const FileName& fnView, const String& parentSection) const{
	String currentSection = "AnnotationNorthArrowDrawer::" + parentSection;
	AnnotationDrawer::store(fnView, currentSection);
	ObjectInfo::WriteElement(currentSection.c_str(),"Begin",fnView, begin);

	return currentSection;
}

void AnnotationNorthArrowDrawer::load(const FileName& fnView, const String& parentSection){
		String currentSection = parentSection;
		AnnotationDrawer::load(fnView, parentSection);
		ObjectInfo::ReadElement(currentSection.c_str(),"Begin",fnView, begin);
}

AnnotationNorthArrowDrawer::AnnotationNorthArrowDrawer(DrawerParameters *parms)
: AnnotationDrawer(parms, "AnnotationNorthArrowDrawer")
, arrow(0)
{
	CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
	CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
	if (cbMap.MinX() > cb.MinX())
		cb.MinX() = cbMap.MinX();
	if (cbMap.MaxX() < cb.MaxX())
		cb.MaxX() = cbMap.MaxX();
	if (cbMap.MinY() > cb.MinY())
		cb.MinY() = cbMap.MinY();
	if (cbMap.MaxY() < cb.MaxY())
		cb.MaxY() = cbMap.MaxY();
	begin.y = 0.9;
	begin.x = 0.9;
	scale = 2.5;
}

AnnotationNorthArrowDrawer::~AnnotationNorthArrowDrawer()
{
	if (arrow)
		delete arrow;
}

void AnnotationNorthArrowDrawer::prepare(PreparationParameters *pp){
	AnnotationDrawer::prepare(pp);
	if (  pp->type & RootDrawer::ptGEOMETRY || pp->type & RootDrawer::ptRESTORE){
		if (!arrow) {
			DrawerParameters dp(getRootDrawer(),this);
			arrow = new PointDrawer(&dp);
			PointProperties *prop = (PointProperties *)arrow->getProperties();
			prop->symbol = "Arrow";
			prop->drawColor = Color(0,0,0);
			prop->scale = scale;
			PreparationParameters pp(NewDrawer::ptRENDER);
			pp.props.symbolType = "Arrow";
			pp.props.symbolSize = scale * 100;
			arrow->prepare(&pp);
		}
	}
}

bool AnnotationNorthArrowDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive())
		return false;
	GeoRef gr = getRootDrawer()->getGeoReference();
	double angle = 0;
	if ( gr.fValid() && !gr->fNorthOriented()) {
		RowCol rc = gr->rcSize();
		rc.Row /= 2;
		rc.Col /= 2;
		Coord c1 = gr->cConv(rc);
		rc.Row += 1;
		Coord c2 = gr->cConv(rc);
		double rDX = c2.x - c1.x;
		double rDY = c2.y - c1.y;
		double at = atan2(rDX, -rDY);
		if (at == 0 && rDY < 0)
			at = M_PI_2;
		angle = at * 180.0 / M_PI;
	}
	if (arrow) {
		PointProperties *prop = (PointProperties *)arrow->getProperties();
		prop->angle = angle;
		CoordBounds cb = getRootDrawer()->getCoordBoundsZoomExt();
		CoordBounds cbMap = getRootDrawer()->getMapCoordBoundsExt();
		if (cbMap.MinX() > cb.MinX())
			cb.MinX() = cbMap.MinX();
		if (cbMap.MaxX() < cb.MaxX())
			cb.MaxX() = cbMap.MaxX();
		if (cbMap.MinY() > cb.MinY())
			cb.MinY() = cbMap.MinY();
		if (cbMap.MaxY() < cb.MaxY())
			cb.MaxY() = cbMap.MaxY();
		Coord crd (cb.MinX() + cb.width() * begin.x, cb.MinY() + cb.height() * begin.y);
		arrow->setCoord(crd);
		arrow->draw(drawLoop, getRootDrawer()->getCoordBoundsZoomExt());
	}

	return true;
}